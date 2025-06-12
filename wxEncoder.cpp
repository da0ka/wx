// wx - wx Word eXtraction -
// Copyright(c) Daisuke Okanohara 2002-2003. All rights reserved
#include"wxExtract.hpp"

void wxEncoder::encode(){
	printf("start encoding kindN:%d wordN:%d\n",kindN,wordN);
	// 辞書情報の出力
	int mapsize = 0;
	fwrite(&kindN,sizeof(int),1,outfp);
	fwrite(&wordN,sizeof(int),1,outfp);

	vector<uint> freq(kindN), cumFreq(kindN+1);
	vector<int> numberTable(dic.size()+1);

	for(int i = 0; i < kindN; i++){
		int length = dic[i].length, p = pos[dic[i].first];
		fputc(length,outfp); //長さを出力
		for(int k = 0; k < length; k++) fputc(buf[p+k],outfp);
		mapsize += length+3;
		freq[i] = dic[i].rcount;
	}
	printf("mapsize:%d\n",mapsize);
	// 変換表を作る
	for(int i=dic.size();i--;) numberTable[dic[i].number] = i;
	// 正規化
	{
		int tmpWordN = 0, dicmax = 65536;	//合計をdicmaxでまとめる
		for(int i = 0; i < kindN; i++)
			tmpWordN += freq[i] = (freq[i]*dicmax+wordN-1) / wordN;	//規格化。　+wordN - 1 は　freq[a] >= 1 を0にしない処理
		int sub = tmpWordN - dicmax; //多い分
		for(int k = 0; sub > 0 ; k++){
			if(freq[k] == 1 || k == kindN){
				k = 0;continue;//巻き戻し
			}
			freq[k]--;
			sub--;
		}
	}
	// 確率設定及び出力
	for(int i = 0; i < kindN; i++){
		cumFreq[i+1] = cumFreq[i] + freq[i];
		short tmp = (short)(freq[i]);
		fwrite(&tmp,sizeof(short),1,outfp);
	}
	{
		rangeEncoder enc(outfp);
		for(int i = 0; i < fileSize; i++){
			int cur = work[i];
			if(cur > 0){
				int n = numberTable[cur];
				enc.encodeshift(cumFreq[n],freq[n],16);
			}
		}
	}
	{
		int outFileSize = ftell(outfp);
		printf("results:%d %f\n",outFileSize,(double)((double)outFileSize * 100 / (double)fileSize));
	}
	//D workの内容を出力
	for(int i = 0; i < fileSize; i++)
		if(work[i] == 0) printf("zero:%u\n",i);
}