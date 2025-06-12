//wx - wx Word eXtraction -
//Copyright(c) Daisuke Okanohara 2002-2003. All rights reserved.
#include "wxExtract.hpp"

class compare_point{
public:
	int operator()(const wordInfo& w1, const wordInfo& w2) const{
		return w1.point > w2.point;
	}
};
class compare_r_point{
public:
	int operator()(const wordInfo& w1, const wordInfo& w2) const{
		return w2.point > w1.point;
	}
};
// rcount順にsort
class compare_rcount{
public:
	int operator()(const wordInfo& w1, const wordInfo& w2) const{
		return w1.rcount > w2.rcount;
	}
};
inline int wxEncoder::culcPoint(int width,int length, int fe, int le){
	return(int)(log(width) * length * 1024);
}
void wxEncoder::extract(){
	//数える際重複分を効率よく含められるように後ろに最初を複製
	for(int i = 0; i < N_EXT - 1; i++) buf[fileSize+i] = buf[i];
	int count[65536]={0}; //2文字分布数えsort用頻度表
	// 2文字分布を数える
	for(int i = 0; i < fileSize; i++)
		count[buf[i]<<8|buf[i+1]]++, pos[i] = i;
	// 累積を分布数えsort用に入力
	for(int i = 0;++i < 65536;) count[i] += count[i-1];
	// Radixsortを行う
	{
//		SystemTime timeCount;
		for(int k = N_EXT; k > 0;){
			k -= 2;
			for(int i = fileSize, p;i;)
				p=pos[--i], work[--count[buf[p+k]<<8|buf[p+k+1]]] = p;
			k -= 2;
			for(int i = 0, p; i < fileSize;)
				p=work[i++], pos[count[buf[p+k]<<8|buf[p+k+1]]++] = p;
		}
//		printf( "sort: %s (sec.)\n", timeCount.str_Get() ) ;
	}
	vector<int> bStart(N_EXT+1); //前回一致した所の開始位置を保持。+1なのは N_EXT一致がbStart[N_EXT]に入るから
	int ds;
	// 単語を抽出
	{
//		SystemTime timeCount;
		int pre_nm = 0; //前回の前の文字との一致数
		int guardPos = fileSize+N_EXT-1;
		pos[fileSize] = guardPos; //最後のぎりぎり
		buf[guardPos] = (byte)(buf[pos[fileSize-1]]+1);

		for(int i = 0; i < fileSize; i++){
			int nm = nextMatch(i); //次の部分列との一致数を計算

			if(nm > pre_nm) //前回より多く一致
				for(int j = nm; j > pre_nm;) bStart[j--] = i; //登録用に今の位置を記録
			else if(nm < pre_nm) //前回より少なく一致
				for(int j = nm; j++ < pre_nm;)
					addWord(bStart[j],i+1,j);//順次登録していく。登録内容は first <= 範囲 < last length:長さ　を登録する
			else if(nm==0 && pre_nm==0)
				addWord(i,i+1,1); //1文字しか出現しないものは特別に登録
			pre_nm = nm;
		}
		ds=dic.size();
//		printf( "addW: %s (sec.) dicSize:%d\n", timeCount.str_Get() ,ds) ;
	}
	// 評価値逆順にsort
	sort(dic.begin(),dic.end(),compare_point());
	// 割り当て
	{
//		SystemTime timeCount;
		allocate2();
		for(int k = 2;k--;allocate2()){
			// point再計算
			for(int i = ds;i--;)
				dic[i].point = culcPoint(dic[i].rcount,dic[i].length,dic[i].fe,dic[i].le);
			sort(dic.begin(),dic.end(),compare_point());
		}
//		printf( "allo: %s (sec.)\n", timeCount.str_Get() ) ;
	}
	sort(dic.begin(),dic.end(),compare_rcount());
	{
		int i = 0;
		for(; i < ds && dic[i].rcount; i++);
		kindN = i;
	}
	{
		// DEBUG　割り当ての様子を出力
		showDic();
		for(int i = 0; i < fileSize; i++){
			if(work[i] > 0) putc('_',stderr);
			else if(work[i] == 0) throw "alloc error";
			putc(buf[i],stderr);
		}
	}
}
inline int wxEncoder::nextMatch(int p){
	int i=0, fst = pos[p], snd = pos[p+1];

	for(; i < N_EXT && buf[fst+i] == buf[snd+i]; i++);
	return i;
}
inline void wxEncoder::addWord(int first,int last,int length){
	int width = last-first, fe = 0, le = 0;

	if(length != 1){
		if(width<=widthBorder) return;
		int i = 256;
		// 初期化
		for(;i--;) fcount[i] = lcount[i] = 0;
		// entropy検査
		for(i = first+!first; i < last; i++){
			fcount[buf[pos[i]-1]]++;
			lcount[buf[pos[i]+length]]++;
		}
		// entropyを計算
		for(i = 256;i;){
			int c = fcount[--i] * 4096;
			if(c >= width) fe += entropyTable[c/width-1]; //-1はentropyTable が 0から4095までだから
			c = lcount[i] * 4096;
			if(c >= width) le += entropyTable[c/width-1];
		}
		if(fe+le < 3000) return;
	/*
		int i = first+!pos[first];
		bool fFlag = false, lFlag = false;
		byte fb = buf[pos[i]-1], lb = buf[pos[i]+length];

		for(;++i < last;){
			if(buf[pos[i]-1] != fb) fFlag = true;
			if(buf[pos[i]+length] != lb) lFlag = true;
			if(fFlag && lFlag) break;
		}
		if(i == last) return;
		*/
	}
	wordInfo tmp(first,last,length,culcPoint(width,length,fe,le),dic.size()+1,fe,le); //+1なのは1から番号開始のため。0は特別なflag
	dic.push_back(tmp);
	return;
}
void wxEncoder::showDic(){
	vector<int> count(N_EXT);
	int i = dic.size(), s=i;
	for(;i--;)if(dic[i].rcount) count[dic[i].length-1] += dic[i].rcount;
	for(;++i < N_EXT;) fprintf(stderr,"%2d %u\n",i+1,count[i]);
	fprintf(stderr,"---contents of dic ---\n");
 	fprintf(stderr,"wordN:%d kindN:%d size of dic:%d\n",wordN,kindN,dic.size());
 	fprintf(stderr,"  count  length   point  number  rcount\n");
 				  //XXXXXXX XXXXXXX XXXXXXX XXXXXXX XXXXXXX
 	for(i = 0; i < s; i++){
 		wordInfo tmp = dic[i];
 		fprintf(stderr,"%7u %7u %7u %7u %7u ",tmp.last-tmp.first,tmp.length,tmp.point,tmp.number,tmp.rcount);
		dicWord(i);
 		fputc('\n',stderr);
 	}
}
void wxEncoder::dicWord(int n){
	for(int i = 0, l=dic[n].length; i < l; i++)
		fputc(buf[pos[dic[n].first]+i],stderr);
}
inline int wxEncoder::occupy(int n){
	wordInfo& w=dic[n];
	int rcount=0; //登録された数
	int first=w.first, last=w.last, length=w.length, wiPos=w.number;
	for(int i = first; i < last; i++){
		int p = pos[i];
		// 空いているか検査. ここは高速化可能
		if(p+length > fileSize) continue;
		int j = 0;
		for(; j < length && !work[p+j]; j++);
		if(j != length) continue; //占有されていた
		//占有flagを入れる
		work[p] = wiPos; //開始位置
		for(j = 0; j < length; j++) work[p+j] = -wiPos; //開始でない場所には負の登録番号をいれておく
		rcount++;
	}
	return rcount;
}
int wxEncoder::culcFileSize(){
	double conFileSize = 0; //file内容の圧縮後の尺
	double dicFileSize = 0; //辞書内容の圧縮後の尺
	// 圧縮後file尺を予測する
	double totalLog2 = log(wordN) / LOG2;
	for(int i = dic.size();i;){
		int width = dic[--i].rcount;
		if(width){
			double log2width = log(width) / LOG2;
			double wordBits  = totalLog2-log2width;
			conFileSize += wordBits * width;
			dicFileSize += dic[i].length+3; //長さ1byte,確率2byte
		}
	}
	conFileSize /= 8; //bit計算をbyteに直す
	printf("%u %u %u %8f\n",(int)conFileSize,(int)dicFileSize,(int)(conFileSize+dicFileSize),(double)(conFileSize+dicFileSize) * 100 / fileSize);
	fprintf(stderr,"%u %u %u %8f\n",(int)conFileSize,(int)dicFileSize,(int)(conFileSize+dicFileSize),(double)(conFileSize+dicFileSize) * 100 / fileSize);
	return(int)(conFileSize+dicFileSize);
}
int wxEncoder::allocate(){
	int localKindN = 0, localWordN = 0, i = fileSize, dicSize = dic.size();
	for(;i;) work[--i] = 0;
	// 割り当て
	for(;i<dicSize;i++){
		if(dic[i].stop){
			dic[i].rcount = 0;
			continue;
		}
		int rcount = occupy(i);
		dic[i].rcount = rcount;
		if(rcount > 0){
			localWordN += rcount;
			localKindN++;
		}
	}
	kindN = localKindN;
	wordN = localWordN;

	return culcFileSize();
}
int wxEncoder::allocate2(){
	//それぞれのmaxを決める
	int i = fileSize, ds=dic.size();
	for(;i;) work[--i] = 0;//workを初期化する
	vector<int> numberTable(ds+1);
	for(;i<ds;){
		wordInfo& w=dic[i];
		int j=dic[i].first, last=w.last, number=w.number, length=w.length, p;
		numberTable[number] = i++;
		for(w.rcount=0;j < last; j++)
			if(work[p=pos[j]] == 0 && p+length <= fileSize)
				work[p] = number;
	}
	wordN = 0; //抽出された結果の単語数
	// 頭から順番に見ていく
	for(int i = 0; i < fileSize; i++){
		int number = work[i];
		if(number){
			int length = dic[numberTable[number]].length;
			dic[numberTable[number]].rcount++;
			wordN++;
			if(length+i > fileSize) printf("error");
			for(;--length;) work[++i] =-number;
		}else throw "error!";
	}
	return culcFileSize();
}