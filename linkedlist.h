#include<iostream>

#ifndef LIB_LINKEDLIST2_H
#define LIB_LINKEDLIST2_H

using namespace std;
namespace musiclib { 
	template<typename T>
	struct linkedlist{
		T val;
		linkedlist<T>*pre,*nxt;
		linkedlist<T>(T v):val(v){
			pre=nxt=this;
		}
		bool is_Head(){
			return pre==this;
		}bool is_Last(){
			return nxt==this;
		}void make_Head(){
			pre=this;
		}void make_Last(){
			nxt=this;
		}
		linkedlist<T>*get_Head(){
			//cout<<"getting head at "<<val<<endl;
			linkedlist<T>*now=this;
			while(!now->is_Head()){
				now=now->pre;
			}
			return now;
		}linkedlist<T>*get_Last(){
			linkedlist<T>*now=this;
			while(!now->is_Last()){
				now=now->nxt;
			}
			return now;
		}
		linkedlist<T>*insert_val(T val,bool after=0){
			linkedlist<T>*tar=new linkedlist<T>(val);
			insert(tar,after);
			return tar;
		}
		void insert(linkedlist<T>*tar,bool after=0){
			//cout<<"insert "<<tar->val<<" before "<<val<<endl;
			if(!tar)return;
			if(after){
				if(!is_Last()){
					tar->nxt=nxt;
					nxt->pre=tar;
				}
				nxt=tar;
				tar->pre=this;
			}else{
				if(!is_Head()){
					tar->pre=pre;
					pre->nxt=tar;
				}
				pre=tar;
				tar->nxt=this;
			}
		}
		linkedlist<T>*remove(bool dir=0,int cnt=0){//not null -> has node
			if(is_Head()&&is_Last())return nullptr;
			linkedlist<T>*ret=nullptr;
			if(!is_Last()){
				nxt->pre=pre;
				ret=nxt;
			}
			else{
				pre->make_Last();
				ret=pre;
			}
			if(!is_Head()){
				pre->nxt=nxt;
				ret=pre;
			}
			else{
				nxt->make_Head();
				ret=nxt;
			}
			if(cnt){
				if(dir){
					return nxt->remove(dir,cnt-1);
				}else{
					return pre->remove(dir,cnt-1);
				}
			}else return ret;
		}
		void moveTo(linkedlist<T>*nPos){
			if(!nPos)return ;
			remove();
			nPos->insert(this,1);
		}
		~linkedlist(){
			while(!is_Last())nxt->remove();
			remove();
		}
	};
}
#endif