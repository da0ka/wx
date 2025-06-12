//wx - wx Word eXtraction -
//Copyright(c) Daisuke Okanohara 2002-2003. All rights reserved.
#include"wxExtract.hpp"

void wxDecoder::decode(){
	int kindN, wordN;

	fread(&kindN,sizeof(int),1,infp);
	fread(&wordN,sizeof(int),1,infp);

	printf("kindN:%d wordN:%d",kindN,wordN);

	fprintf(stderr,"%u\n",kindN);
	vector< vector<byte> > dic(kindN);

	for(int i = 0; i < kindN; i++){
		unsigned char length = fgetc(infp);
		dic[i].resize(length);
		for(int j = 0; j < length; j++)
			fputc(dic[i][j] = fgetc(infp),stderr);
		fputc('\n',stderr);
	}
	//---: 入力----------
	vector<uint> freq(kindN), cumFreq(kindN + 1);
	{
		short tmp;
		for(int i = 0; i < kindN; i++){
			fread(&tmp,sizeof(short),1,infp);
			freq[i] = (int)tmp;
			cumFreq[i + 1] = freq[i] + cumFreq[i];
		}
	}
//	SystemTime timeCount;
	{
		rangeDecoder dec(infp);
		for(int i = 0; i < wordN; i++){
			uint cur = dec.getCharcterShift(freq,cumFreq,kindN,16);
			for(int j = 0, l=dic[cur].size(); j < l; j++)
				fputc(dic[cur][j],outfp);
		}
	}
//	printf("decode: %s (sec.)\n", timeCount.str_Get());
}