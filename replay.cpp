// replay.cpp  —— 先方向↓ 数字↓ 数字↑ 方向↑  一气呵成
#include "interception.h"
#include "Tools.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <atomic>

using USHORT = unsigned short;

int curTextLine=0,btmTextLine;
int selection=0;
struct songCode{
	string name;
	string filename;
	double speed;	
};
vector<songCode>songs;
vector<int>songLines;
void insertSongs(){
	songs.push_back((songCode){"盗墓笔记·十年人间","DaoMuBiJiShiNianRenJian.txt",0.6});
	songs.push_back((songCode){"Aurora","Aurora.txt",0.125});
	return ;
}

// ---------- 扫描码 ----------
const USHORT scLeft = 75, scDown = 80, scRight = 77, scSpace = 57;
USHORT scNum(char c) { return 0x01 + (c - '0'); }   // 1→0x02 ...

// ---------- 发键助手 ----------
UINT KeyboardId,MouseId;
InterceptionDevice findLiveKeyboard(InterceptionContext ctx)
{
    InterceptionStroke stroke;
    for (int i = 1; i <= 10; ++i) {
        if (!interception_is_keyboard(i)) continue;
        // 非阻塞 receive，超时 1 ms
        if (interception_receive(ctx, i, &stroke, 1) > 0) return i;
    }
    return 0;   // 没找到
}
void sendKey(InterceptionContext ctx, USHORT code, bool down,bool arr=false) {
    InterceptionKeyStroke k{};
    k.code = code;
    if(arr)k.state = down ? 2 : 3;
    else k.state = down ? 0 : 1;// 0=down  1=up
    k.information = 0;
    interception_send(ctx, KeyboardId, (InterceptionStroke*)&k, 1);
}
InterceptionContext ctx;
void init(){
	system("chcp 65001");
	toolInit();
    insertSongs();
	return ;
}
wchar_t hwid[500];
void songInit(){
	ctx = interception_create_context();
    if (!ctx) { cerr << "driver not loaded\n"; return ; }
    interception_set_filter(ctx, interception_is_keyboard,
                            INTERCEPTION_FILTER_KEY_ALL);
    KeyboardId = interception_wait(ctx);
	/*for (int i = 1; i <= 20; ++i) {
	    if (interception_is_keyboard(i)) {          // 1-10 必为键盘
	        KeyboardId=i;
	    }
	    else if (interception_is_mouse(i)) {        // 11-20 必为鼠标
	        MouseId=i;2
	    }
	}*/
	moveTo(1,btmTextLine+1);
	cout<<"Get Devide Id: Keyboard:"<<KeyboardId<<" & Mouse:"<<MouseId<<endl;
    return ;
}
atomic<bool> endPlaying{true};          // 原子标志，防止竞态

void songExit(){
	interception_destroy_context(ctx);
	//cout<<"Exit confirmed."<<endl;
	//system("pause");
}
void playMusicAtMajor(){
	ifstream in(songs[selection].filename.c_str());
	double ms;
	string demand;
	bool shift = false, space = false;
	while (in >> ms >> demand) {
		if(endPlaying)break;
	    ms=ms*songs[selection].speed;
	    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
	    this_thread::sleep_for(chrono::milliseconds(static_cast<long>(ms)));
		USHORT dir = 0, num = 0;
	    if (demand == "up_key") { shift = true;}
	    else if(demand == "down_key"){
	        shift = false;
		}else if(demand == "lower_half"){
			space = true;
		}else if(demand == "recover_half"){
			space = false;
		}
		else{
		    // ----------- 映射表 -----------
		    switch (demand[0]) {
		    	case '1': num = scNum('1'); if (shift) { dir = scDown; num = scNum('2'); } break;
			    case '2': num = scNum('2'); if (shift) { dir = scDown; num = scNum('3'); } break;
			    case '3': num = scNum('3'); if (shift) { dir = scRight; num = scNum('1'); } break;
			    case '4': dir = scLeft; num = scNum('1'); if (shift) { dir = scRight; num = scNum('2'); } break;
			    case '5': dir = scLeft; num = scNum('2'); if (shift) { dir = scRight; num = scNum('3'); } break;
			    case '6': dir = scLeft; num = scNum('3'); break;
			    case '7': dir = scDown; num = scNum('1'); break;
		    }
		}
		// ======== 顺序：方向↓ 数字↓ 数字↑ 方向↑ ========
	    if(space) sendKey(ctx,scSpace,true,false);
	    else sendKey(ctx,scSpace,false,false);
	    if (dir) sendKey(ctx, dir, true,true);   // 方向按下
	    //else sendKey(ctx, dir, false,true);
	    sendKey(ctx, num, true);            // 数字按下
	    sendKey(ctx, num, false);           // 数字抬起
	    if (dir) sendKey(ctx, dir, false,true);  // 方向抬起
	    //if(space) sendKey(ctx,scSpace,false,true);
	}
	songExit();
	return ;
}
void outputNextText(int col,bool changeLine,string msg){
	curTextLine++;
	if(changeLine)curTextLine++;
	moveTo(col,curTextLine);
	cout<<msg<<endl;
	return ;
}
void reOutputNextText(int nLine,int col,string msg=""){
	curTextLine=nLine;
	moveTo(col,nLine);
	cout<<msg<<endl;
	return ;
}
void output_interface(){
	system("cls");
	SetCursor_(0);
	outputNextText(2,false,"Welcome to Warframe DJ channel, please channel your Shawzin to mode [Major Keys]");
	outputNextText(2,true," - Choose songs from the following list:");
	for(int i=0;i<songs.size();i++){
		if(i==selection){
			setColor(10,0);
		}else{
			setColor(7,0);
		}
		songLines.push_back(curTextLine+1);
		outputNextText(2,false,"  "+toString(i+1)+" - "+songs[i].name);
	}
	setColor(7,0);
	outputNextText(2,true,"[NOTE] - operate method -");
	outputNextText(2,false," - W : change to previous one ");
	outputNextText(2,false," - S : change to next one ");
	outputNextText(2,false," - T : play this one ");
	btmTextLine=curTextLine;
	return ;
}
void changeSelection(int nSelection){
	if(nSelection<0||nSelection>=songs.size()||nSelection==selection)return ;
	string oStr="  "+toString(selection+1)+" - "+songs[selection].name
		,nStr="  "+toString(nSelection+1)+" - "+songs[nSelection].name;
	//outputNextText(1,true,"changing to "+toString(nSelection));
	moveTo(2,songLines[selection]);
	setColor(7,0);
	//puts(oStr.c_str());
	cout<<oStr<<endl;
	moveTo(2,songLines[nSelection]);
	setColor(10,0);
	cout<<nStr<<endl;
	selection=nSelection;
	setColor(7,0);
	return ;
}

char ch;
unique_ptr<thread> recTh;          // 智能指针托管

void stopAndWait() {
    endPlaying = true;
    if (recTh && recTh->joinable()) {
        recTh->join();
        recTh.reset();                       // 线程对象销毁
    }
    //system("pause");
}
void startPlaying() {
	stopAndWait(); // 先确保旧线程结束
    endPlaying = false;
    songInit();
    recTh = make_unique<thread>(playMusicAtMajor);
    moveTo(1,btmTextLine+2);
    cout<<"Currently playing: "<<songs[selection].name;
}
int main() {
    init();
    output_interface();
	while(1){
		if(_kbhit()){
			ch=getch();
			if(ch>='a'||ch<='z')ch-=('a'-'A');
			if(ch=='S'){
				changeSelection(selection+1);
			}else if(ch=='W'){
				changeSelection(selection-1);
			}else if(ch=='T'){
				startPlaying();
			}else if(ch=='Q'){
				break;
			}
		}
	}
	songExit();
	return 0;
	    
}