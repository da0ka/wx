#ifndef __rangecoder_cpp__
#define __rangecoder_cpp__
/*
	wx - wx Word eXtraction -
	Copyright(c) Daisuke Okanohara 2002-2003. All rights reserved.
*/
#include<stdio.h>
#include<iostream>
#include<vector>

typedef unsigned char uc;
typedef unsigned int  uint;

using namespace std;

#define TOP (1U << 24)
#define BOT (1U << 16)

class rangeEncoder{
private:
	uint low;		//幅の下
	uint range;		//幅の大きさ
	uint passed;	//出力した数
	FILE* outfp;	//出力するfile

	//!!--次の行はDEBUG用-------!!
	uint count;

private: void pc(uc c){
	fputc(c,outfp);
	passed++;
}
public: uint getPassed(){
	return passed;
}
public: rangeEncoder(FILE* o): outfp(o){
	low = passed = count = 0;
	range = 0xFFFFFFFF;//1番大きい数
}
public: ~rangeEncoder(){
	for(int i = 4;i--;low <<= 8) pc(low >> 24);
}
public: void encode(uint cumFreq, uint freq, uint totFreq){
	range /= totFreq;
	low += range * cumFreq;

	for(range *= freq;(low ^ (low + range)) < TOP || range < BOT && (range=-low&BOT-1);range <<= 8)
		pc(low >> 24), low   <<= 8;
}
public: void encodeshift(uint cumFreq, uint freq, uint totShift){
	range >>= totShift;
	low += range * cumFreq;

	for(range *= freq;(low ^ (low + range)) < TOP || range < BOT && (range=-low&BOT-1);range <<= 8)
		pc(low >> 24), low <<= 8;
}
public: void encodeNumber (uint number, uint maxnumber){
	encode(number,1,maxnumber);
}
//!!--次の行はDEBUG用-------!!
public: void valueDebug(){
	fprintf(stderr,"%8u,%15u,%15u\n",count, low,range);
	count++;
}
};

class rangeDecoder{
private:
	uint low, code, range;
	FILE* infp;
	uint count;//DEBUG用

private:
	uc  gc(){ return fgetc(infp);}
public:
rangeDecoder(FILE* in) : infp(in){
	low = code = count = 0;
	range = 0xFFFFFFFF;
	for(int i = 4;i--;)code = code << 8 | gc();
}
uint getfreqshift(uint totshift){
	range >>= totshift;
	uint tmp = (code - low) / range;
	if(tmp >= (1 << totshift)) printf("totfreq error");
	return tmp;
}
uint getfreq(uint totFreq){
	range /= totFreq;
	uint tmp = (code - low) / range;
	if(tmp >= totFreq) printf("totfreq error");
	return tmp;
}
uint getCharcter(vector<uint>& freq,vector<uint>& cumFreq,int n){
	range /= cumFreq[n];
	uint tmp = (code - low) / range;
	if(tmp >= cumFreq[n]) printf("totfreq error");
	int i = 0, j = n;

	while(i < j){
		int k = i + j>>1;
		if(cumFreq[k + 1] <= tmp) i = k + 1;
		else j = k;
	}
	low += range * cumFreq[i];

	for(range *= freq[i];(low ^ (low + range)) < TOP || range < BOT && (range=-low&BOT-1);range <<= 8)
		code = code << 8 | gc(), low <<= 8;
	return i;
}
uint getCharcterShift(vector<uint>& freq,vector<uint>& cumFreq,int n, int totShift){
	range >>= totShift;
	uint tmp = (code - low) / range;
	if(tmp >= (1 << totShift)) printf("totfreq error");
	int i = 0, j = n;

	while(i < j){
		int k = i + j>>1;
		if(cumFreq[k + 1] <= tmp) i = k + 1;
		else j = k;
	}
	low += range * cumFreq[i];
	for(range *= freq[i];(low ^ (low + range)) < TOP || range < BOT && (range=-low&BOT-1);range <<= 8)
		code = code << 8 | gc(), low <<= 8;
	return i;
}
uint decodeNumber(uint maxNumber){
	range /= maxNumber;
	uint tmp = (code - low) / range;

	if(tmp >= maxNumber) printf("totfreq error");

	for(low += range * tmp;(low ^ (low + range)) < TOP || range < BOT && (range=-low&BOT-1);range <<= 8)
		code = code << 8 | gc(), low <<= 8;
	return tmp;
}
void decode(uint cumfreq, uint freq){
	low += cumfreq * range;

	for(range *= freq;(low ^ (low + range)) < TOP || range < BOT && (range=-low&BOT-1);range <<= 8)
		code = code << 8 | gc(), low   <<= 8;
}
void valueDebug(){
	fprintf(stderr,"%8u,%15u,%15u\n",count, low,range);
	count++;
}
};
#endif