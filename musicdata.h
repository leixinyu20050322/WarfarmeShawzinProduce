#include"linkedlist.h"
#include"musicmsg.h"
#include<fstream>
#include<iostream>
#include<sstream>
#include<stack>

#ifndef LIB_MUSICDATA_H
#define LIB_MUSICDATA_H

using namespace std;
namespace musiclib {
	class musicList;
	stack<musicList*>trash;
	class musicList: public linkedlist<dat>{
		public:
		bool p0[comType],tmpTag[comType][2];
		//stack<musicList*>trash;
		void emptyTrash(){
			musicList* now=nullptr;
			while(!trash.empty()){
				now=trash.top();
				trash.pop();
				if(!now){
					continue;
				}
				now->remove();
			}
			return;
		}
		void trashRedundant(bool dir=1){
			if((dir&&is_Last())||(!dir&&is_Head()))return ;
			chkSingleGap(dir);
			emptyTrash();
		}
		void reset_mark(){
			for(int i=0;i<comType;i++){
				p0[i]=0;
			}
		}
		musicList(dat v,musicList* ot=nullptr):linkedlist<dat>(v){
			for(int i=0;i<comType;i++){
				p0[i]=0;
			}
		}
		musicList* getLogicElementById(int id,bool first=1){
			bool suc=0;
			if(id<=0)return nullptr;
			if(first){
				for(int i=0;i<comType;i++){
					p0[i]=0;
				}
			}
			reset_mark();
			musicList* now=static_cast<musicList*>(get_Head());
			while(now&&id){
				if(now->val.isVoice()){
					id--;
					if(id==0){
						suc=1;
						break;
					}
				}else if(now->val.isSpecialMessage()){
					p0[now->val.getCommandGroup()]=1-val.get_Half();
				}
				if(now->is_Last()){
					suc=0;
					break;
				}
				now=static_cast<musicList*>(now->nxt);
			}
			if(suc)return now;
			else return nullptr;
		}
		void remove_prep(bool remove_other=1){
		    if(val.isVoice()){
		        while(pre&&pre->val.isSpecialMessage()){
		            pre->moveTo(this);
		        }
		    }
		    trash.push(this);
		    return ;
		}
		void recoverNextDelay(){
			if(!nxt)return ;
			nxt->val.delay+=val.delay;
			return ;
		}
		void chkSingleGap(bool dir=1){
			if(!nxt)return ;
			musicList* rec[comType][2];
			for(int i=0;i<comType;i++){
				rec[i][0]=rec[i][1]=nullptr;
			}
			musicList* lnxt;
			if(dir)lnxt=getLogicNext();
			else lnxt=getLogicPrev();
			if((dir&&(lnxt||nxt))||(!dir&&(lnxt||pre))){
				musicList* tar=static_cast<musicList*>(dir?nxt:pre);
				while(tar!=lnxt){
					if(tar->val.isSpecialMessage()){
						int group=tar->val.getCommandGroup(),half=tar->val.get_Half();
						if(rec[group][half]&&rec[group][1-half]){
							trash.push(rec[group][0]);
							trash.push(tar);
							if(half)tar->recoverNextDelay();
						}else if(rec[group][half]){
							trash.push(tar);
							if(half)tar->recoverNextDelay();
						}else{
							rec[group][half]=tar;
						}
					}
					if(dir){
						if(tar->is_Last())break;
						tar=static_cast<musicList*>(tar->nxt);
					}else{
						if(tar->is_Head())break;
						tar=static_cast<musicList*>(tar->pre);
					}
				}
			}	
			//cout<<"gap checked, trash counts "<<trash.size()<<endl;
			return ;
		}
		void removeById(int id){
			musicList* itm=getLogicElementById(id);
			if(itm){
				itm->remove_prep();
				itm->trashRedundant();
			}
			return ;
		}
		musicList* insertAfter_fromFile(string &path,int logic_id){
			musicList* now=getLogicElementById(logic_id,1);
			return insertFromFile(path,p0,now);
		}
		static musicList* insertFromFile(string &path,bool cont[comType],musicList* st=nullptr){
			ifstream ifs(path.c_str());
			if(!ifs){
				return nullptr;
			}
			musicList* now=st;
			musicList* rec[comType];
			bool p[comType];
			for(int i=0;i<comType;i++){
				rec[i]=nullptr;
				p[i]=0;
			}
			musicList* cur=nullptr;
			double delay_;
			string comm;
			while(ifs>>delay_>>comm){
				//cout<<"reads "<<delay_<<" "<<comm<<endl;
				bool suc=0;
				int code;
				for(int i=0;!suc&&i<comType;i++){
					if(comm==command_pairs[i][0]){
						suc=1;
						code=0-2*i-1;
						p[i]=1;
					}else if(comm==command_pairs[i][1]){
						suc=1;
						code=0-2*i-2;
						p[i]=0;
					}
				}
				if(!suc){
					code=comm[0]-'0';
					if(p[1])code=minor[code-1];
					else code=major[code-1];
					if(p[0])code+=keysPerGroup;
				}
				musicList* cur;
				if(now){
					cur=static_cast<musicList*>(now->insert_val((dat){delay_,code},1));
					//cout<<"inserting "<<now->nxt->val<<" after "<<now->val<<endl;
				}
				else{
					cur=new musicList((dat){delay_,code});
				}
				now=cur;
				//cout<<"unit : "<<now->val<<" inserted."<<endl;
			}
			//cout<<"done, final unit : "<<cur->val<<endl;
			return static_cast<musicList*>(now->get_Head());
		}
		musicList* getLogicNext_(){
			if(val.isVoice())return this;
			else{
				tmpTag[val.getCommandGroup()][val.get_Half()]=1;
				return static_cast<musicList*>(nxt)->getLogicNext();
			}
		}
		musicList* getLogicNext(){
			if(is_Last()||nxt->val.msgcode==0)return nullptr;
			return static_cast<musicList*>(nxt)->getLogicNext_();
		}
		musicList* getLogicPrev_(){
			if(val.isVoice())return this;
			else{
				tmpTag[val.getCommandGroup()][val.get_Half()]=1;
				return static_cast<musicList*>(pre)->getLogicPrev();
			}
		}
		musicList* getLogicPrev(){
			if(is_Head()||pre->val.msgcode==0)return nullptr;
			return static_cast<musicList*>(pre)->getLogicPrev_();
		}
		void resetTag(){
			for(int i=0;i<comType;i++){
				tmpTag[i][0]=tmpTag[i][1]=0;
			}return ;
		}
		double chkLogicPrev(double totTime){
			double split_=totTime/(2*comType);
			int cnt=0;
			resetTag();
			musicList *lprev=getLogicPrev();
			if(lprev){
				//cout<<"Got previous:"<<lprev->val.msgcode<<" prev of "<<val.msgcode<<endl;
				/*cout<<"tags:"<<endl;
				for(int i=0;i<comType;i++){
					cout<<i<<" : "<<tmpTag[i][0]<<' '<<tmpTag[i][1]<<endl;
				}*/
				if(!tmpTag[1][0]&&val.isMinor()&&!lprev->val.isMinor()){
					//cout<<"implement insert code -3"<<endl;
					insert_val((dat){split_,-3});
					cnt++;
				}else if(!tmpTag[1][1]&&!val.isMinor()&&lprev->val.isMinor()){
					insert_val((dat){split_,-4});
					cnt++;
				}else if(val.isMinor()==lprev->val.isMinor()){
					musicList *st=this;
					while(st!=lprev){
						st=static_cast<musicList*>(st->pre);
						//if(st)cout<<"moving place for "<<st->val.delay<<" "<<st->val.msgcode<<endl;
						if(!st||st==lprev)break;
						if(st->val.getCommandGroup()==1)st->moveTo(this);
					}
				}
				if(!tmpTag[0][0]&&val.isUpper()&&!lprev->val.isUpper()){
					//cout<<"implement insert code -3"<<endl;
					insert_val((dat){split_,-1});
					cnt++;
				}else if(!tmpTag[0][1]&&!val.isUpper()&&lprev->val.isUpper()){
					insert_val((dat){split_,-2});
					cnt++;
				}else if(val.isUpper()==lprev->val.isUpper()){
					musicList *st=this;
					while(st!=lprev){
						st=static_cast<musicList*>(st->pre);
						if(!st||st==lprev)break;
						if(st->val.getCommandGroup()==0)st->moveTo(this);
					}
				}
			}else{
				if(!tmpTag[1][0]&&val.isMinor()){
					insert_val((dat){split_,-3});
					cnt++;
				}
				if(!tmpTag[0][0]&&val.isUpper()){
					insert_val((dat){split_,-1});
					cnt++;
				}
			}
			return split_*cnt;
		}
		double chkLogicNext(double totTime){
			double split_=totTime/(2*comType);
			int cnt=0;
			musicList *lnxt=getLogicNext();
			if(lnxt){
				if(!tmpTag[1][1]&&val.isMinor()&&!lnxt->val.isMinor()){
					insert_val((dat){split_,-4},1);
					cnt++;
				}else if(!tmpTag[1][0]&&!val.isMinor()&&lnxt->val.isMinor()){
					insert_val((dat){split_,-3},1);
					cnt++;
				}
				if(!tmpTag[0][1]&&val.isUpper()&&!lnxt->val.isUpper()){
					insert_val((dat){split_,-2},1);
					cnt++;
				}else if(!tmpTag[0][0]&&!val.isUpper()&&lnxt->val.isUpper()){
					insert_val((dat){split_,-1},1);
					cnt++;
				}
			}
			return split_*cnt;
		}
		static musicList* insertSingle(musicList* now,double t,string comm,bool cont[comType],bool after=0){
			int code=getCodeFromCommand(comm,cont);
			musicList* cur;
			if(now)cur=static_cast<musicList*>(now->insert_val((dat){t,code},after));
			else{
				cur=new musicList((dat){t,code});
			}
			double timetot;
			//cout<<"check delay..."<<endl;
			musicList* logicPrev=cur->getLogicPrev();
			timetot=cur->val.delay;
			cur->val.delay-=cur->chkLogicPrev(timetot);
			//cout<<"previous checked."<<endl;
			musicList* logicNxt=cur->getLogicNext();
			if(logicNxt){
				timetot=logicNxt->val.delay;
				logicNxt->val.delay-=cur->chkLogicNext(timetot);
			}
			//cout<<"after checked."<<endl;
			cur->trashRedundant();
			cur->trashRedundant(0);
			//cout<<"trash emptied!"<<endl;
			return static_cast<musicList*>(cur->get_Head());
		}
		musicList* insertSingleBefore(int id,double t,string comm){
			musicList* now=getLogicElementById(id,1);
			if(now)cout<<"Got element:"<<now->val<<", inserting before it."<<endl;
			if(now)return static_cast<musicList*>(insertSingle(now,t,comm,p0));
			else return static_cast<musicList*>(insertSingle(static_cast<musicList*>(get_Last()),t,comm,p0,1));
		}
		
		void queryAll() {
			int logicLineCnt=0;
		    cout << "[";
		    bool first = true;
		    musicList *p = static_cast<musicList*>(get_Head());
		    while (p) {
		    	bool addLogic=0;
		    	if(p->val.msgcode>0)logicLineCnt++;
		    	else{
		    		addLogic=1;
				}
		        if (!first) cout << ",";
		        first = false;
		        cout << "{\"t\":" << (int)p->val.delay
		             << ",\"cmd\":\"" << p->val.getCommand() << "\""
		             << ",\"logicId\":" << logicLineCnt+addLogic << "}";
		        if(p->is_Last()){
		        	break;
				}
		        p = static_cast<musicList*>(p->nxt);
		    }
		    cout << "]\n";
		}
		void saveFile(const string &path) {
		    ofstream fo(path);
		    if (!fo) { cout << "{\"error\":\"write failed\"}\n"; return; }
		    musicList *p = static_cast<musicList*>(get_Head());
		    while (p) {
		        fo << (int)p->val.delay << " " << p->val.getCommand() << "\n";
		        if(p->is_Last())break;
		        p = static_cast<musicList*>(p->nxt);
		    }
		    cout << "{\"status\":\"saved\"}\n";
		}
		~musicList(){}
	};
}

#endif