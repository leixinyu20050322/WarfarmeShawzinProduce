#include<stdio.h>
#include<conio.h>
#include<windows.h>
#include<stdlib.h>
#include<string>
#define ll long long

using namespace std;

#define f_in FOREGROUND_INTENSITY
#define f_blue FOREGROUND_BLUE
#define f_green FOREGROUND_GREEN
#define f_red FOREGROUND_RED

/*

0=black
1=blue
2=green
3=blue(lake)
4=red
5=purple
6=yellow
7=white
8=grey
9=light_blue 
10=light_green
11=light_aqua
12=light_red
13=light purple
14=light_yellow
15=enlighted_white*/

COORD co;
HANDLE a;
CONSOLE_CURSOR_INFO c;
void toolInit(){
	a=GetStdHandle(STD_OUTPUT_HANDLE);
}
void SetCursor_(bool k){
	GetConsoleCursorInfo(a,&c);
	c.bVisible=k;
	SetConsoleCursorInfo(a,&c);
	return ;
}
void moveTo(int x,int y){
	//COORD co;
	co.X=x;
	co.Y=y;
	//HANDLE a=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(a,co);
	return ;
}
void vanish_(int x,int y){
	moveTo(x,y);
	printf(" ");
	return ;
}
WORD wcolor;
void setColor(int forgc,int backc){
	wcolor=((backc&0x0F)<<4)+(forgc&0x0F);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),wcolor);
	return ;
}
string toString(int x){
	string ret;
	while(x){
		ret=(char)('0'+x%10)+ret;
		x/=10;
	}
	return ret;
}
ll qpow(ll x,ll y){
	ll ans=1ll,base=x;
	while(y){
		if(y&1){
			ans=ans*base;
		}
		y>>=1;
		base=base*base;
	}
	return ans;
}