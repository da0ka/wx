#ifndef _WX_HPP_
#define _WX_HPP_
/*
	wx - wx Word eXtraction -
	Copyright(c) Daisuke Okanohara 2002-2003. All rights reserved.
*/
#include<stdio.h>
#include<string>
#include<vector>
#include<algorithm>
#include<math.h>
#include"SystemTime.cxx"
#include"entropyTable.cpp"
#include"rangeCoder.hpp"

using namespace std;
using namespace CXL;

#define N_EXT 16	//抽出する単語の最長

#define LOG2 0.69314710

typedef unsigned char  byte;	//抽出するdata. ここでは1byte
typedef unsigned short word;	//ここでは2byte

typedef vector<byte>::iterator bi;
typedef vector<int>::iterator  ii;

typedef unsigned int uint; //rangeCoder用に使う

using namespace std;

struct wordInfo{ // 単語情報を保持する
	int first;		//単語の最初の位置. pos[first]で指定
	int last;		//単語の最後の位置. 範囲は　first <= i < last
	int length;		//単語の長さ
	int point;		//評価値
	int number;		//固有の登録番号 移動した後や、sortした後も同じ
	int rcount;		//実際の出現回数
	int stop;		//wordNet等によってstopされているかどうか
	int fe, le;	// 評価関数用. 保留

	// constructor
	wordInfo(int inFirst,int inLast,int inLength, int inPoint,int inNumber, int inFe, int inLe):
		first(inFirst),
		last(inLast),
		length(inLength),
		number(inNumber),
		point(inPoint),
		rcount(0),
		stop(0),
		fe(inFe),
		le(inLe){}
	~wordInfo(){}
};

class wxEncoder{
private:
	FILE *infp, *outfp;	//入出力file

	int fileSize;
	int widthBorder;	//単語候補としての最低許容出現回数

	vector<byte> buf;	//入力data
	vector<int> pos;	//位置
	vector<int> work;	//作業用領域
	vector<vector<int>> pointTable;
	vector<wordInfo> dic; //単語辞書
	vector<int> fcount, lcount;

	int kindN; //登録された単語の種類数
	int wordN; //抽出された結果の単語数

	inline int nextMatch(int);
	inline void addWord(int,int,int);
	void showDic();	//dicの内容を表示させる
	void dicWord(int);	//引数の番号のdicの単語実体を表示
	inline int occupy(int);
	int culcFileSize();
	int allocate();
	int allocate2();
	inline int culcPoint(int,int,int,int);
public:
wxEncoder(const char* fileName){
	infp = fopen(fileName,"rb");	//入力fileをopen
	if(!infp) throw ("cannot open %s",fileName);
	{
		string outfpName(fileName);	//出力file名を作成
		outfpName += ".wx";
		outfp = fopen(outfpName.c_str(),"wb");
		if(!outfp) throw ("cannot open %s",outfpName.c_str());
	}
	// file尺取得
	fseek(infp,0,SEEK_END);
	fileSize = ftell(infp);
	rewind(infp);

	// file尺から単語候補の最低許容出現回数を設定
	widthBorder = fileSize / 100000; //ここは正確な数は不明
	if(widthBorder<2) widthBorder=2;
	// buffer取得
	buf.resize(fileSize + N_EXT); // 最後の1つは番人用
	pos.resize(fileSize + 1); // +1は番人用
	work.resize(fileSize);
	fcount.resize(256);
	lcount.resize(256);

	// Data入力
	for(int i = 0; i < fileSize;) buf[i++] = fgetc(infp);
	// file内容を出力
	printf("%s %dbytes\n",fileName,fileSize);
}
~wxEncoder(){
	if(infp) fclose(infp);
	if(outfp) fclose(outfp);
}
void extract();
void encode();
};
class wxDecoder{
private:
	FILE *infp, *outfp; //入出力file
public:
	wxDecoder(const char* inFileName){ infp = fopen(inFileName,"rb");
	if(!infp) throw "infp open error";
	string outFileName(inFileName);

	// .weがあるかどうかを検査
	if((outFileName.substr(outFileName.length() - 3)) != ".wx")
		throw ("input file name doesn't have .wx");
	outFileName += ".tes";	//D　本来ならweをとるが、元のfileと区別がつかなくなるのでtesをつける。
	outfp = fopen(outFileName.c_str() ,"wb");
	if(!outfp) throw "outfp open error";
}
~wxDecoder(){
	if(infp) fclose(infp);
	if(outfp) fclose(outfp);
}
// 復元
void decode(); //we_decoder.cpp
};
#endif