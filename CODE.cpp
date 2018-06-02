#include<stdio.h>
#include<stdlib.h>

#define E 0 
int main()
{
register int AX,BX,CX,DX,EX,FX,GX,HX,IX;
int B;
int A;
int C[4];
int D;
scanf("%d",&AX);
scanf("%d",&BX);
A=AX;
B=BX;
CX=AX+BX;
X:
scanf("%d",&AX);
DX=AX-BX;
printf("%d\n",DX);
printf("%d\n",CX);
if(CX == DX)
{
C[0]=CX;
printf("%d\n",C[0]);
goto X;
}
else{
C[1]=DX;
printf("%d\n",C[1]);
goto X;
}
printf("%d\n",E);
return 0;
}