#include<string>

#ifndef LIB_CONSTANTS_H
#define LIB_CONSTANTS_H

using namespace std;
const int keysPerGroup=12,mainKeysPerGroup=7;
const int key_map[keysPerGroup]={1,2,2,3,3,4,5,5,6,6,7,7};
const int major[mainKeysPerGroup]={1,3,5,6,8,10,12};
const int minor[mainKeysPerGroup]={0,2,4,0,7,9,11};
const int comType=2;
const string command_pairs[comType][2]={
	{"up_key","down_key"},
	{"lower_half","recover_half"}
};
bool pair_comm(string a,string b){
	for(int i=0;i<comType;i++){
		if(a==command_pairs[i][0]&&b==command_pairs[i][1])return 1;
	}
	return 0;
}

#endif