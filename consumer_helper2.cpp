#include"musicdata.h"
using namespace std;
using namespace musiclib;
int main() {
	musicList* hd;
	string cmd;
	bool cont[comType];
	hd=nullptr;
	//cout<<hd->val<<endl;
    ios::sync_with_stdio(false);
    string op;
    while (cin>>op) {
        if (op == "quit") break;
        //lock_guard<mutex> lk(mtx);
        if (op == "load") {
        	int aft_id;
        	cin>>cmd>>aft_id;
        	if(!hd){
        		for(int i=0;i<comType;i++)cont[i]=0;
        		hd=musicList::insertFromFile(cmd,cont);
			}
            else hd=hd->insertAfter_fromFile(cmd,aft_id);
            cout << "{\"status\":\"loaded\"}\n";
        } else if (op == "save") {
            string path;
			cin >> path;
            hd->saveFile(path);
            cout << "{\"status\":\"saved\"}\n";
        } else if (op == "query") {
            hd->queryAll();
        } else if (op == "insert") {
            int t,bef_id;
            cin >>bef_id>> t>>cmd;
			//cmd = cmd.substr(1);
			if(!hd){
				for(int i=0;i<comType;i++)cont[i]=0;
				hd=musicList::insertSingle(nullptr,t,cmd,cont);
			}
            else hd=hd->insertSingleBefore(bef_id,t,cmd);
			cout << "{\"status\":\"inserted\"}\n";
        } else if (op == "delete"||op == "remove") {
            int t; cin>>t;
            hd->removeById(t);
            cout << "{\"status\":\"deleted\"}\n";
        } else {
            cout << "{\"error\":\"unknown cmd\"}\n";
        }
        if(hd)hd=static_cast<musicList*>(hd->get_Head());
        cout.flush();
    }
    return 0;
}
/*
load Aurora.txt 0
insert 3 50000 3#~
insert 4 50000 2#~
insert 5 50000 1#~
delete 2
query
*/