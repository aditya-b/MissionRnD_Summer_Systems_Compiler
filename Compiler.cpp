// Compiler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdlib.h>
#include<string.h>

struct symbol_table_node
{
	char symbol[3];
	int size;
	int address;
	int type;
	void *value;
};


struct intermediate_code_node
{
	int line;
	int opcode;
	int *params;
};

struct stack_node
{
	int address;
	int params;
};

struct label_node
{
	char label[5];
	int address;
};

char* get_label(label_node *label_table, int length, int address)
{
	int i;
	for (i = 0; i < length; i++)
	{
		if (label_table[i].address == address)
			return label_table[i].label;
	}
	return NULL;
}

int get_label_address(label_node *label_table, int length, char *label)
{
	int i;
	for (i = 0; i < length; i++)
	{
		if (!strcmp(label_table[i].label, label))
			return label_table[i].address;
	}
	return -1;
}
int get_symbol(symbol_table_node *symbol_table, char *symbol,int length)
{
	int i = 0;
	int s = 0;
	char *sym = symbol;
	if (symbol[strlen(symbol) - 1] == ']')
	{
		char symbl[3];
		while (symbol[i] != '[')
			symbl[i] = symbol[i++];
		symbl[i] = '\0';
		i++;
		while (symbol[i] != ']')
			s = s * 10 + (symbol[i++] - 48);
		sym = symbl;
	}
	for (i = 0; i < length; i++)
		if (!strcmp(symbol_table[i].symbol, sym))
			return (symbol_table[i].address+s);
	return -1;
}

int get_opcode(char opcodes[][6], const char *op)
{
	int i = 0;
	for (i = 0; i < 14; i++)
		if (!strcmp(opcodes[i], op))
			return i + 1;
	return 0;
}

void add_to_symbol_table(const char *symbol, int address, int type, int size, symbol_table_node **symbol_table, int *length, int value)
{
	int x = *length;
	*symbol_table = (symbol_table_node*)realloc(*symbol_table, sizeof(symbol_table_node)*(x + 1));
	strcpy((*symbol_table)[x].symbol, symbol);
	(*symbol_table)[x].address = address;
	(*symbol_table)[x].size = size;
	(*symbol_table)[x].type = type;
	(*symbol_table)[x].value = NULL;
	if (value != 1111111)
	{
		(*symbol_table)[x].value = malloc(1); 
		*((int*)(*symbol_table)[x].value) = value;
	}
	*length = *length + 1;
}

void add_to_intermediate_table(int line, int opcode, int *params, intermediate_code_node **intermediate_code,int *length,int param_length)
{
	int x = *length;
	*intermediate_code = (intermediate_code_node*)realloc(*intermediate_code, sizeof(intermediate_code_node)*(x + 1));
	int i = 0;
	(*intermediate_code)[x].line = line;
	(*intermediate_code)[x].opcode = opcode;
	(*intermediate_code)[x].params = (int*)malloc(sizeof(int)*param_length);
	for (i = 0; i < param_length; i++)
	{
		(*intermediate_code)[x].params[i] = params[i];
	}
	*length = *length + 1;
}


void generate_ICODE(int ln_no,int im_no,int opcode,char *line,symbol_table_node **symbol_table, int *symbol_table_length, intermediate_code_node **intermediate_code, int *intermediate_code_length, char opcodes[][6], int s, label_node **label_table,int *label_table_length)
{
	int i = s, x = 0;
	char op[6];
	op[0] = '\0';
	int *params;
	int y, a;
	char symbol[6];
	switch (opcode)
	{
	case 1:
		params = (int*)malloc(sizeof(int) * 2);
		x = 0;
		y = 0;
		while (x < 2)
		{
			y = 0;
			symbol[0] = '\0';
			while (line[i] != ' ' && line[i] != '\n' && line[i] != '\0' && line[i] != EOF)
				symbol[y++] = line[i++];
			i++;
			symbol[y] = '\0';
			a = get_symbol(*symbol_table, symbol, *symbol_table_length);
			if (a == -1)
			{
				a = symbol[0] - 'A';
				a = a > 7 ? -20 : -a-1;
			}
			if (a == -20)
			{
				printf("Error: Symbol not found at line %d.\n", ln_no);
				return;
			}
			params[x] = a;
			x++;
		}
		add_to_intermediate_table(im_no, opcode, params, intermediate_code, intermediate_code_length, 2);
		break;
	case 2:
	case 3:
	case 4:
		params = (int*)malloc(sizeof(int) * 3);
		x = 0;
		y = 0;
		while (x < 3)
		{
			y = 0;
			symbol[0] = '\0';
			while (line[i] != ' ' && line[i] != '\n' && line[i] != '\0' && line[i] != EOF)
				symbol[y++] = line[i++];
			i++;
			symbol[y] = '\0';
			a = get_symbol(*symbol_table, symbol, *symbol_table_length);
			if (a == -1)
			{
				a = symbol[0] - 'A';
				a = a > 7 ? -20 : -a-1;
			}
			if (a == -20)
			{
				printf("Error: Symbol not found at line %d.\n", ln_no);
				return;
			}
			params[x] = a;
			x++;
		}
		add_to_intermediate_table(im_no, opcode, params, intermediate_code, intermediate_code_length, 3);
		break;
	case 5:
		params = (int*)malloc(sizeof(int));
		symbol[0] = '\0';
		x = 0;
		while (line[i] != '\0'&&line[i] != '\n')
			symbol[x++] = line[i++];
		symbol[x]='\0';
		params[0] = get_label_address(*label_table, *label_table_length, symbol);
		add_to_intermediate_table(im_no, opcode, params, intermediate_code, intermediate_code_length, 1);
		break;
	case 12:
	case 13:
		params = (int*)malloc(sizeof(int));
		symbol[0] = '\0';
		x = 0;
		while (line[i] != '\0'&&line[i] != '\n')
			symbol[x++] = line[i++];
		symbol[x] = '\0';
		a = get_symbol(*symbol_table, symbol, *symbol_table_length);
		if (a == -1)
		{
			a = symbol[0] - 'A';
			a = a > 7 ? -20 : -a-1;
		}
		if (a == -20)
		{
			printf("Error: Symbol not found at line %d.\n", ln_no);
			return;
		}
		params[0] = a;
		add_to_intermediate_table(im_no, opcode, params, intermediate_code, intermediate_code_length, 1);
		break;
	}
}
void evaluate_if(FILE *fp,int *ln_no, int *im_no, int opcode, char *line, symbol_table_node **symbol_table, int *symbol_table_length, intermediate_code_node **intermediate_code, int *intermediate_code_length, char opcodes[][6], int s, label_node **label_table, int *label_table_length,stack_node **stack,int *top)
{
	int i = s, x = 0;
	char op[6];
	op[0] = '\0';
	char symbol[6];
	symbol[0] = '\0';
	char lines[30];
	int *params=(int*)malloc(sizeof(int)*4);
	int y=0,a=0;
	while (line[i] != '\n'&&line[i] != '\0')
	{
		x = 0;
		while (line[i] != ' ' && line[i] != '\n'&&line[i] != '\0')
			symbol[x++] = line[i++];
		symbol[x] = '\0';
		a = get_symbol(*symbol_table, symbol, *symbol_table_length);
		if (a == -1)
		{
			a = symbol[0] - 'A';
			a = a > 7 ? -20 : -a - 1;
		}
		if (a == -20)
		{
			printf("Error: Symbol not found at line %d.\n", *ln_no);
			return;
		}
		params[y++] = a;
		x = 0;
		i++;
		while (line[i] != ' ' && line[i] != '\n'&&line[i] != '\0')
			op[x++] = line[i++];
		op[x] = '\0';
		int opcode_comparison = get_opcode(opcodes, op);
		x = 0;
		i++;
		while (line[i] != ' ' && line[i] != '\n'&&line[i] != '\0')
			symbol[x++] = line[i++];
		symbol[x] = '\0';
		a = get_symbol(*symbol_table, symbol, *symbol_table_length);
		if (a == -1)
		{
			a = symbol[0] - 'A';
			a = a > 7 ? -20 : -a - 1;
		}
		if (a == -20)
		{
			printf("Error: Symbol not found at line %d.\n", ln_no);
			return;
		}
		params[y++] = a;
		params[y++] = opcode_comparison;
		params[y] = *ln_no + 1;
		add_to_intermediate_table(*im_no, opcode, params, intermediate_code, intermediate_code_length, 4);
		*top += 1;
		*stack = (stack_node*)realloc(*stack, sizeof(stack_node)*(*top + 1));
		(*stack)[*top].address = *intermediate_code_length;
		(*stack)[*top].params = 4;
		*im_no += 1;
		*ln_no += 1;
		while (fgets(lines,30,fp))
		{
			if (!strstr(lines, "IF") && !strstr(lines,"ELSE") && !strstr(lines,"ENDIF"))
			{
				x = 0;
				i = 0;
				symbol[0] = '\0';
				while (lines[i] != '\n'&&lines[i] != '\0'&&lines[i] != ' ')
					symbol[i] = lines[i++];
				symbol[i] = '\0';
				i++;
				generate_ICODE(*ln_no, *im_no, get_opcode(opcodes, symbol), lines, symbol_table, symbol_table_length, intermediate_code, intermediate_code_length, opcodes, i, label_table, label_table_length);
				*im_no += 1;
			}
			else if (strstr(lines, "IF") && !strstr(lines,"ENDIF"))
			{
				i = 0;
				while (lines[i++] != ' ');
				evaluate_if(fp, ln_no, im_no, 6, lines, symbol_table, symbol_table_length, intermediate_code, intermediate_code_length, opcodes, i, label_table, label_table_length, stack, top);

			}
			else if (strstr(lines, "ELSE"))
			{
				i = 0;
				int *p = (int*)malloc(sizeof(int));
				p[0] = 0;
				add_to_intermediate_table(*im_no, 5, p, intermediate_code, intermediate_code_length, 1);
				*top += 1;
				*stack = (stack_node*)realloc(*stack, sizeof(stack_node)*(*top + 1));
				(*stack)[*top].address = *intermediate_code_length;
				(*stack)[*top].params = 1;
			}
			else
			{
				int flag = 2;
				*top += 1;
				*stack = (stack_node*)realloc(*stack, sizeof(stack_node)*(*top + 1));
				(*stack)[*top].address = *intermediate_code_length;
				while (flag && *top > 0)
				{
					int add = (*stack)[*top].address;
					*top -= 1;
					(*intermediate_code)[(*stack)[*top].address - 1].params[(*stack)[*top].params - 1] = add + 1;
					*ln_no += 1;
					flag--;
				}
				*top -= 1;
				return;
			}
			*ln_no += 1;
		}
	}
}

void generate_intermediate_code(FILE *fp, symbol_table_node **symbol_table, int *symbol_table_length, intermediate_code_node **intermediate_code, int *intermediate_code_length,char opcodes[][6],label_node **label_table,int *label_table_length, stack_node **stack, int *top)
{
	char line[30];
	int address = 0;
	char symbol[6];
	int flag = 1;
	int ln_no = 0;
	int im_no = 1;
	while (fgets(line, 30, fp))
	{
		int type = line[0] == 'D' ? 1 : 0;
		int size = 0;
		int value = 1111111; 
		if (!strstr(line, "START:") && flag)/*Get the declarations and add them to symbol table*/
		{
			int i = 0, x = 0;
			while (line[i++] != ' ');
			while (line[i] != '\n' && line[i] != '\0' && line[i] != '=')
			{
				while (line[i] != '[' && line[i] != '\0' && line[i] != '=' && line[i] != '\n')
					symbol[x++] = line[i++];
				symbol[x] = '\0';
				if (line[i] == '[')
				{
					i++;
					while (line[i] != ']'&&line[i] != '\0' && line[i] != '=' && line[i] != '\n')
						size = size * 10 + (line[i++] - 48);
					i++;
				}
			}
			if (line[i] == '=')
			{
				value = 0;
				i++;
				while (line[i] != '\n'&&line[i] != '\0')
					value = value * 10 + (line[i++] - 48);
			}
			if (size == 0)
				size = 1;
			if (get_symbol(*symbol_table, symbol, *symbol_table_length) == -1)
				add_to_symbol_table(symbol, address, type, size, symbol_table, symbol_table_length, value);
			address += size;
		}
		else if(flag)/*Neglect START: line in code*/
			flag = 0;
		else/*Add to intermediate code*/
		{
			int i = 0, x = 0;
			char op[6];
			op[0] = '\0';
			int *params;
			int y, a;
			while (line[i] != '\n'&&line[i] != '\0' && line[i]!=EOF)
			{
				while (line[i] != ' '&&line[i]!='\0'&&line[i]!='\n' && line[i] != EOF)
					op[i] = line[i++];
				op[i] = '\0';
				i++;
				int opcode = get_opcode(opcodes, op);
				if (!opcode && op[i - 2] != ':')
					printf("Error: Invalid operation at line %d\n",ln_no);
				else if (op[i - 2] == ':')
				{
					fgets(line, 30, fp);
					ln_no++;
					char label[4];
					op[i - 2] = '\0';
					strcpy(label, op);
					i = 0;
					while (line[i] != ' '&&line[i] != '\0'&&line[i] != '\n' && line[i] != EOF)
						op[i] = line[i++];
					op[i] = '\0';
					i++;
					opcode = get_opcode(opcodes, op);
					*label_table = (label_node*)malloc(sizeof(label_node)*(*label_table_length + 1));
					strcpy((*label_table)[*label_table_length].label, label);
					(*label_table)[*label_table_length].address = *intermediate_code_length+1;
					*label_table_length += 1;
					if (opcode != 6)
						generate_ICODE(ln_no, im_no, opcode, line, symbol_table, symbol_table_length, intermediate_code, intermediate_code_length, opcodes, i, label_table, label_table_length);
					else
						evaluate_if(fp, &ln_no, &im_no, opcode, line, symbol_table, symbol_table_length, intermediate_code, intermediate_code_length, opcodes, i, label_table, label_table_length, stack, top);
					im_no+=1;
					break;
				}
				else if (opcode == 6)
				{
					evaluate_if(fp,&ln_no, &im_no, opcode, line, symbol_table, symbol_table_length, intermediate_code, intermediate_code_length, opcodes, i, label_table, label_table_length,stack,top);
					break;
				}
				else if (opcode == 14)
				{
					*top += 1;
					*stack = (stack_node*)realloc(*stack, sizeof(stack_node)*(*top + 1));
					(*stack)[*top].address = *intermediate_code_length;
					while (*top > 0)
					{
						int add = (*stack)[*top].address + 1;
						*top -= 1;
						(*intermediate_code)[(*stack)[*top].address-1].params[(*stack)[*top].params - 1] = add;
					}
					break;
				}
				else
				{
					generate_ICODE(ln_no, im_no, opcode, line, symbol_table, symbol_table_length, intermediate_code, intermediate_code_length, opcodes, i,label_table,label_table_length);
					im_no++;
					break;
				}
			}
		}
		ln_no++;
	}
}
void execute_intermediate_code(intermediate_code_node *intermediate_code, int intermediate_code_length, symbol_table_node *symbol_table, int symbol_table_length)
{
	void *memory[1024];
	int i;
	for (i = 0; i < symbol_table_length;i++)
	{
		int add = symbol_table[i].address;
		void *val = symbol_table[i].value;
		if (add<0)
			symbol_table[i].value = memory + 1023 + add;
		else
			symbol_table[i].value = memory + add;
		if (val != NULL)
			*((int*)symbol_table[i].value) = *((int*)val);
	}
	for (i = 0; i < intermediate_code_length;)
	{
		int opcode = intermediate_code[i].opcode;
		int *params = intermediate_code[i].params;
		void *a, *b, *c,*d;
		switch (opcode)
		{
			case 1:
				a = params[0] < 0 ? memory + 1023 +params[0]: memory +params[0];
				b = params[1] < 0 ? memory + 1023 +params[1]: memory +params[1];
				*((int*)a) = *((int*)b);
				i++;
				break;
			case 2:
				a = params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
				b = params[1] < 0 ? memory + 1023 + params[1] : memory + params[1];
				c = params[2] < 0 ? memory + 1023 + params[2] : memory + params[2];
				*((int*)a) = *((int*)b) + *((int*)c);
				i++;
				break;
			case 3:
				a = params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
				b = params[1] < 0 ? memory + 1023 + params[1] : memory + params[1];
				c = params[2] < 0 ? memory + 1023 + params[2] : memory + params[2];
				*((int*)a) = *((int*)b) - *((int*)c);
				i++;
				break;
			case 4:
				a = params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
				b = params[1] < 0 ? memory + 1023 + params[1] : memory + params[1];
				c = params[2] < 0 ? memory + 1023 + params[2] : memory + params[2];
				*((int*)a) = *((int*)b) * *((int*)c);
				i++;
				break;
			case 5:
				i = params[0]-1;
				break;
			case 6:
				switch (params[2])
				{
					case 7: 
						a = params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
						b = params[1] < 0 ? memory + 1023 + params[1] : memory + params[1];
						if (*((int*)a) == *((int*)b))
							i++;
						else
							i = params[3]-1;
						break;
					case 8:
						a = params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
						b = params[1] < 0 ? memory + 1023 + params[1] : memory + params[1];
						if (*((int*)a) < *((int*)b))
							i++;
						else
							i = params[3]-1;
						break;
					case 9:
						a = params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
						b = params[1] < 0 ? memory + 1023 + params[1] : memory + params[1];
						if (*((int*)a) > *((int*)b))
							i++;
						else
							i = params[3]-1;
						break;
					case 10:
						a = params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
						b = params[1] < 0 ? memory + 1023 + params[1] : memory + params[1];
						if (*((int*)a) <= *((int*)b))
							i++;
						else
							i = params[3]-1;
						break;
					case 11:
						a = params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
						b = params[1] < 0 ? memory + 1023 + params[1] : memory + params[1];
						if (*((int*)a) >= *((int*)b))
							i++;
						else
							i = params[3]-1;
						break;
				}
				break;
			case 12:
				a= params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
				printf("%d\n", *((int*)a));
				i++;
				break;
			case 13:
				a = params[0] < 0 ? memory + 1023 + params[0] : memory + params[0];
				int x;
				scanf("%d", &x);
				*((int*)a) = x;
				i++;
				break;
		}
	}
}
void flush_intermediate_code_to_file(intermediate_code_node *intermediate_code, int intermediate_code_length)
{
	int i, j;
	FILE *fp = fopen("IM_CODE.txt", "w");
	int n=0,opcode;
	for (i = 0; i < intermediate_code_length; i++)
	{
		opcode = intermediate_code[i].opcode;
		fprintf_s(fp, "%d ", opcode);
		switch (opcode)
		{
		case 1:n = 2; break;
		case 2:
		case 3:
		case 4:n = 3; break;
		case 5:n = 1; break;
		case 6:n = 4; break;
		case 12:
		case 13:n = 1; break;
		default:n = 0;
		}
		for (j = 0; j < n; j++)
			fprintf_s(fp, "%d ", intermediate_code[i].params[j]);
		fprintf_s(fp, "\b\n");
	}
	fclose(fp);
	printf("Intermediate code flushed to file 'IM_CODE.txt'\n");
}

void flush_symbol_table_to_file(symbol_table_node *symbol_table, int symbol_table_length)
{
	int i;
	FILE *fp = fopen("SYM_TAB.txt", "w");
	for (i = 0; i < symbol_table_length; i++)
	{
		fprintf_s(fp,"%s %d %d %d ", symbol_table[i].symbol, symbol_table[i].address, symbol_table[i].size, symbol_table[i].type);
		if (symbol_table[i].value != NULL)
			fprintf_s(fp,"%d", *((int*)symbol_table[i].value));
		fprintf(fp,"\n");
	}
	fclose(fp);
	printf("Symbol table flushed to file 'SYM_TAB.txt'\n");
}
char* get_symbol_by_address(int address, symbol_table_node *symbol_table, int symbol_table_length)
{
	int i = 0;
	for (i = 0; i < symbol_table_length; i++)
	{
		if (address >= 0 && address < symbol_table[i].address + symbol_table[i].size)
		{
			if (symbol_table[i].size != 1)
			{
				char *symbol=(char*)malloc(sizeof(char)*10);
				int x = 0;
				while (symbol_table[i].symbol[x])
					symbol[x] = symbol_table[i].symbol[x++];
				int offset = address - symbol_table[i].address ;
				if (offset == 0)
				{
					symbol[x++] = '[';
					symbol[x++] = '0';
					symbol[x++] = ']';
					symbol[x] = '\0';
					return symbol;
				}
				symbol[x++] = '[';
				while (offset != 0)
				{
					symbol[x++] = offset + 48;
					offset /= 10;
				}
				symbol[x++] = ']';
				symbol[x] = '\0';
				return symbol;
			}
			return symbol_table[i].symbol;
		}
	}
	switch(-address - 1)
	{
	case 0:
		return (char*)"AX";
	case 1:
		return (char*)"BX";
	case 2:
		return (char*)"CX";
	case 3:
		return (char*)"DX";
	case 4:
		return (char*)"EX";
	case 5:
		return (char*)"FX";
	case 6:
		return (char*)"GX";
	case 7:
		return (char*)"HX";
	}
}

void flush_c_code(FILE *fp, intermediate_code_node *intermediate_code, int intermediate_code_length, symbol_table_node *symbol_table, int symbol_table_length, int *i, int opcode, stack_node **stack, int *top, label_node *label_table, int label_table_length)
{
	if (*top >= 0)
	{
		if (*i == (*stack)[*top].address)
		{
			fprintf_s(fp,"}\n");
			*top -= 1;
		}
	}
	char op[3];
	char *label;
	switch (opcode)
	{
	case 1:
		fprintf_s(fp, "%s=%s;\n", get_symbol_by_address(intermediate_code[*i].params[0], symbol_table, symbol_table_length), get_symbol_by_address(intermediate_code[*i].params[1], symbol_table, symbol_table_length));
		break;
	case 2:
		fprintf_s(fp, "%s=%s+%s;\n", get_symbol_by_address(intermediate_code[*i].params[0], symbol_table, symbol_table_length), get_symbol_by_address(intermediate_code[*i].params[1], symbol_table, symbol_table_length), get_symbol_by_address(intermediate_code[*i].params[2], symbol_table, symbol_table_length));
		break;
	case 3:
		fprintf_s(fp, "%s=%s-%s;\n", get_symbol_by_address(intermediate_code[*i].params[0], symbol_table, symbol_table_length), get_symbol_by_address(intermediate_code[*i].params[1], symbol_table, symbol_table_length), get_symbol_by_address(intermediate_code[*i].params[2], symbol_table, symbol_table_length));
		break;
	case 4:
		fprintf_s(fp, "%s=%s*%s;\n", get_symbol_by_address(intermediate_code[*i].params[0], symbol_table, symbol_table_length), get_symbol_by_address(intermediate_code[*i].params[1], symbol_table, symbol_table_length), get_symbol_by_address(intermediate_code[*i].params[2], symbol_table, symbol_table_length));
		break;
	case 5:
		label = get_label(label_table, label_table_length, intermediate_code[*i].params[0]);
		if (label != NULL)
			fprintf(fp, "goto %s;\n", label);
		else
		{
			fprintf(fp, "}\nelse{\n");
			*top += 1;
			*stack = (stack_node*)realloc(*stack, sizeof(stack_node)*(*top + 1));
			(*stack)[*top].address = intermediate_code[*i].params[0]-1;
		}
		break;
	case 6:
		switch (intermediate_code[*i].params[2])
		{
		case 7:strcpy_s(op, "=="); break;
		case 8:strcpy_s(op, "<"); break;
		case 9:strcpy_s(op, ">"); break;
		case 10:strcpy_s(op, "<="); break;
		case 11:strcpy_s(op, ">="); break;
		}
		fprintf(fp, "if(%s %s %s)\n{\n", get_symbol_by_address(intermediate_code[*i].params[0], symbol_table, symbol_table_length), op, get_symbol_by_address(intermediate_code[*i].params[1], symbol_table, symbol_table_length));
		break;
	case 12:
		fputs("printf(\"%d\\n\",", fp);
		fputs(get_symbol_by_address(intermediate_code[*i].params[0], symbol_table, symbol_table_length), fp);
		fputs(");\n", fp);
		break;
	case 13:
		fputs("scanf(\"%d\",&", fp);
		fputs(get_symbol_by_address(intermediate_code[*i].params[0], symbol_table, symbol_table_length), fp);
		fputs(");\n", fp);
		break;
	default:return;
	}
	return;
}

void generate_c_code(intermediate_code_node *intermediate_code, int intermediate_code_length, symbol_table_node *symbol_table, int symbol_table_length, label_node *label_table, int label_table_length, stack_node **stack, int *top)
{
	FILE *fp = fopen("CODE.cpp", "w");
	int i, j;
	fprintf_s(fp, "#include<stdio.h>\n#include<stdlib.h>\n\n");
	for (i = 0; i < symbol_table_length; i++)
	{
		if(!symbol_table[i].type)
			fprintf_s(fp, "#define %s %d \n", symbol_table[i].symbol, *((int*)symbol_table[i].value));
	}
	fprintf_s(fp, "int main()\n{\nregister int AX,BX,CX,DX,EX,FX,GX,HX,IX;\n");
	for (i = 0; i < symbol_table_length; i++)
	{
		if (symbol_table[i].type)
		{
			fprintf_s(fp, "int %s", symbol_table[i].symbol);
			if (symbol_table[i].size != 1)
				fprintf_s(fp, "[%d]", symbol_table[i].size);
			fprintf_s(fp, ";\n");
		}
	}
	j = -1;
	for (i = 0; i < intermediate_code_length; i++)
	{
		int opcode = intermediate_code[i].opcode;
		char *label = get_label(label_table, label_table_length, intermediate_code[i].line);
		if (label != NULL)
			fprintf_s(fp, "%s:\n", label);
		flush_c_code(fp, intermediate_code, intermediate_code_length, symbol_table, symbol_table_length, &i, opcode, stack, top, label_table, label_table_length);

	}
	fprintf_s(fp, "return 0;\n}");
	fclose(fp);
	printf("C Code successfully generated!\n");
}

int main()
{
	symbol_table_node *symbol_table = (symbol_table_node*)malloc(sizeof(symbol_table_node));
	intermediate_code_node *intermediate_code = (intermediate_code_node*)malloc(sizeof(intermediate_code_node));
	label_node *label_table = (label_node*)malloc(sizeof(label_node));
	stack_node *stack = (stack_node*)malloc(sizeof(stack_node));
	int top = -1;
	char opcodes[][6] = { "MOV","ADD","SUB","MUL","JMP","IF","EQ","LT","GT","LTEQ","GTEQ","PRINT","READ","ENDIF" };
	int symbol_table_length = 0, intermediate_code_length = 0, label_table_length=0, i;
	void *registers = malloc(8);
	FILE *fp;
	char filename[30];
	printf("Enter File Name: ");
	gets_s(filename);
	fp = fopen(filename, "r");
	if (fp == NULL)
		printf("Error: No such file found!");
	else
		generate_intermediate_code(fp, &symbol_table, &symbol_table_length, &intermediate_code, &intermediate_code_length,opcodes,&label_table,&label_table_length,&stack,&top);
	for (i = 0; i < symbol_table_length; i++)
	{
		printf("%s %d %d %d ", symbol_table[i].symbol, symbol_table[i].address, symbol_table[i].size, symbol_table[i].type);
		if(symbol_table[i].value!=NULL)
			printf("%d", *((int*)symbol_table[i].value));
		printf("\n");
	}
	printf("\n\n");
	for (i = 0; i < intermediate_code_length; i++)
	{
		printf("%d %d ", intermediate_code[i].line, intermediate_code[i].opcode);
		int n;
		switch (intermediate_code[i].opcode)
		{
		case 1:n = 2; break;
		case 2:
		case 3:
		case 4:n = 3; break;
		case 5:n = 1; break;
		case 6:n = 4; break;
		case 12:
		case 13:n = 1; break;
		default:n = 0;
		}
		for (int j = 0; j < n; j++)
			printf("%d ", intermediate_code[i].params[j]);
		printf("\n");
	}
	//execute_intermediate_code(intermediate_code, intermediate_code_length, symbol_table, symbol_table_length);
	flush_intermediate_code_to_file(intermediate_code, intermediate_code_length);
	flush_symbol_table_to_file(symbol_table, symbol_table_length);
	generate_c_code(intermediate_code, intermediate_code_length, symbol_table, symbol_table_length,label_table,label_table_length,&stack,&top);
	return 0;
}