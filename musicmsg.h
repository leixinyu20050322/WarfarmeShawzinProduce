#include<iostream>
#include"constants.h"
#include"Tools.h"

#ifndef LIB_MUSICMSG_H
#define LIB_MUSICMSG_H

using namespace std;
namespace musiclib {
	struct dat{
		double delay;
		int msgcode;
		int get_Half(){
			if(msgcode>=0)return -1;
			return (-1-msgcode)&1;
		}
		int getCommandGroup(){
			return (-1-msgcode)>>1;
		}
		bool isUpper(){
			return msgcode>keysPerGroup;
		}
		bool isMinor(){
			for(int i=0;i<mainKeysPerGroup;i++){
				if(minor[i]!=0&&(msgcode-1)%keysPerGroup+1==minor[i])return 1;
			}
			return 0;
		}
		bool isVoice(){
			return msgcode>0&&msgcode<=20;
		}
		bool isSpecialMessage(){
			return msgcode>=0-2*comType&&msgcode<0;
		}
		string getCommand(){
			if(isVoice()){
				//cout<<"getting voice: "<<key_map[(msgcode-1)%12]<<" by code "<<msgcode<<endl;
				return toString(key_map[(msgcode-1)%12]);
			}else if(isSpecialMessage()){
				int id=-1-msgcode;
				return command_pairs[id>>1][id&1];
			}else{
				return "";
			}
		}
	};
	inline ostream& operator <<(ostream &os,const dat &d){
		os<<"("<<d.delay<<","<<d.msgcode<<")";
		return os;
	}
	int getCodeFromCommand(string &comm,bool cont[comType]){
		bool p[comType];
		for(int i=0;i<comType;i++)p[i]=cont[i];
		bool suc=0;
		int code=0;
		for(int i=0;!suc&&i<comType;i++){
			if(comm==command_pairs[i][0]){
				code=0-2*i-1;
				suc=1;
			}else if(comm==command_pairs[i][1]){
				code=0-2*i-2;
				suc=1;
			}
		}
		if(!suc){
			code=comm[0]-'0';
			for(int i=1;i<comm.size();i++){
				if(comm[i]=='#')p[1]=1;
				else if(comm[i]=='~')p[0]=1;
			}
			if(p[1])code=minor[code-1];
			else code=major[code-1];
			if(p[0])code+=keysPerGroup;
		}
		return code;
	}
}
#endif