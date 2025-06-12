//wx - word extraction -
//Copyright(c) Daisuke Okanohara 2002 - 2003. All rights reserved.
#include<iostream>
#include<string>
#include<math.h>
#include"wxExtract.hpp"
using namespace std;

int main (int argc, char* argv[]){
	if(argc != 3){ // argcの数を調べる
		cout << "wx 1.01 05/04 Copyright (C) 2002 - 2003 Daisuke Okanohara(VZV05226@nifty.com)\nusage: we (e/d) InFileName\n";
		return -1;
	}
	try{ //主処理
		string sv(argv[1]);
		if(sv == "e"){ //encode
			string debugfpName(argv[2]); //debug fileの名前
			debugfpName += "_edg.txt";
			freopen(debugfpName.c_str(),"wb",stderr); //debug結果出力
			wxEncoder e(argv[2]);
			e.extract();
			e.encode();
		}else if(sv == "d"){ //decode
			string debugfpName(argv[2]); //debug fileの名前
			debugfpName += "_ddg.txt";
			freopen(debugfpName.c_str(),"wb",stderr);	//debug結果出力

			wxDecoder d(argv[2]);
			d.decode();
		}else throw "argv is error. \nusage: we (e/d) InFileName\n";//不正
	}
	catch(const char* errorMessage){ //---: エラーがあった----------
		cout << "error:" << errorMessage << endl;
		return -1;
	}
	return 0;
}