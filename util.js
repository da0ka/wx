const culcPoint=(w,l,fe,le)=>w?0|Math.log(w)*l*1024:1<<31,entropyTable=function(s,S,a,c,d,b,i,p,w){for(;--i||(b=a.charCodeAt(d++),i=a.charCodeAt(8+d));)S[S(b++)]=c++;for(a=i='';c=s.charAt(i++);)if((d=S[c])<92)a+=S(b+d);else if(d<1976){if(p=(w=a.slice(-28)).substring(d=(c=d-92)%28,c=d+c/28+2))for(;w.length<c;)w+=p;a+=w.substring(c,d)}else d<2068?b=12353:d&&(b=12449),a+=c;for(s=[i=0];i<4095;)s[++i]=s[i-1]+a.charCodeAt(i)-64;return s}('@HGFEEDEDCCBA@?@',String.fromCharCode,'        ?\u0758[]',0,0);
let N_EXT=16;

function nextMatch(p,P,B){
	for(var i=0,a=P[p],b=P[p+1];i<N_EXT&&B[a+i]===B[b+i];)i++;
	return i
}
function addWord(a,z,l,B,C,P,D){
	let w=z-a,fe=0,le=0;
	if(l^1){
		if(w<=D.widthBorder)return;
		let i=a||1,c;
		//entropy検査
		for(;i<z;C[B[c+l]+256]++)c=P[i++],C[B[c-1]]++;
		//entropyを計算
		for(i=256;i;C[i]=C[i+256]=0){
			c=C[--i]<<12;
			if(c>=w)fe+=entropyTable[c/w-1|0];//-1はentropyTable が 0から4095までだから
			c=C[i+256]<<12;
			if(c>=w)le+=entropyTable[c/w-1|0];
		}
		if(fe+le<3000)return
	}
	D.push({first:a,last:z,length:l,point:culcPoint(w,l,fe,le),number:D.length+1,fe,le,hit:0,stop:0})//+1なのは1から番号開始のため。0は特別なflag
}
function dicWord(n,D,B,P,O){
	for(let i=0,l=D[n].length,p=P[D[n].first];i<l;)
		O[O.p++]=B[p+i++]
}
function occupy(n,D,W,P){
	for(var{first,last,length,number}=D[n],w=W.length,p,rc=0,i=first;i<last;){
		p=P[i++];
		//空いているか検査.ここは高速化可能
		if(p+length>w)continue;
		let j=0;
		for(;j<length&&!W[p+j];)j++;
		if(j<length)continue;//占有されていた
		//占有flagを入れる
		rc++;W[p]=number;//開始位置
		for(j=0;j<length;)W[p+j++]=-number//開始でない場所には負の登録番号をいれておく
	}return rc
}
function culcFileSize(D){
	let fs=0,ds=0;//file内容の圧縮後の尺, 辞書内容の圧縮後の尺
	//圧縮後file尺を予測する
	const LOG2=Math.LN2,sumlog2=Math.log(D.wordN)/LOG2;
	for(let i=D.length,c;i;)
		if(c=D[--i].hit)
			fs+=c*=sumlog2-Math.log(c)/LOG2,
			ds+=D[i].length+3;//仮に長さ1byte,確率2byte
	return fs/8+ds>>>0
}
function allocate(D,W,P){
	let kn=0,wn=0,i=W.length,ds=D.length;
	for(;i--;)W[i]=0;
	//割り当て
	for(;++i<ds;){
		if(D[i].stop){D[i].hit=0;continue}
		let c=D[i].hit=occupy(i,D,W,P);
		if(c>0)wn+=c,kn++;
	}
	D.kindN=kn;D.wordN=wn;
	//return culcFileSize(D)
}
function allocate2(D,W,P){
	//それぞれのmaxを決める
	for(var i=W.length,w=i,l=D.length,n=0,n2i=new Uint32Array(l+1);i;)W[--i]=0;
	for(;i<l;){
		let{first,last,number,length}=D[i],j=first,p;
		for(D[n2i[number]=i++].hit=0;j<last;)
			if(!W[p=P[j++]]&&p+length<=w)W[p]=number
	}
	//頭から順番に見ていく
	for(i=D.wordN=0;i<w;n++){
		let d=D[n2i[l=W[i++]]];
		d.hit++;
		for(d=d.length;--d;)W[i++]=-l
	}
	D.wordN=n//;return culcFileSize(D)
}
async function extract(B,W,P,z,rate){
	var D=[],k=N_EXT,i=k-1,guardPos=z+i,c=B[0],C=new Uint32Array(65536),p;
	let fn=b=>wait0(c=>b(rate(i,z))),st=+new Date,m=rate&&rate.call?16383:-1;
	for(;i;)B[z+--i]=B[i];
	for(;i<z;C[c<<8|(c=B[++i])]++)P[i]=i;
	for(c=C[i=0];c<z;)c=C[++i]+=c;
	//radix sort
	for(;k>0;){
		for(k-=2,i=z;i;W[--C[B[p+k]<<8|B[p+k+1]]]=p)p=P[--i];
		for(k-=2;i<z;P[C[B[p+k]<<8|B[p+k+1]]++]=p)p=W[i++]
	}
	B[P[z]=guardPos]=B[P[z-1]]^255;//最後のぎりぎり
	C.fill(0,0,512);D.widthBorder=2;
	//単語を抽出. W:前回一致した所の開始位置を保持
	for(let pn=i=0;i<z;++i&m||Date.now()-st<100||await new Promise(fn,st=+new Date)){
		c=nextMatch(i,P,B);//次の部分列との一致数を計算
		if(c>pn)//前回より多く一致
			for(k=c;k>pn;)W[--k]=i;//登録用に今の位置を記録
		else if(c<pn)//前回より少なく一致
			for(k=c;k<pn;)addWord(W[k],i+1,++k,B,C,P,D);//順次登録していく。登録内容は first<=範囲<last length:長さ
		else c||pn||addWord(i,i+1,1,B,C,P,D);//1文字しか出現しないものは特別に登録
		pn=c
	}
	const ds=D.length,cmp=(a,b)=>b.point-a.point;
	qsort(D,0,ds-1,"point"),allocate2(D,W,P);
	for(k=2;k--;qsort(D,0,ds-1,"point"),allocate2(D,W,P))
		for(i=ds;i;c.point=culcPoint(c.hit,c.length,c.fe,c.le))c=D[--i];//point再計算
	for(qsort(D,0,ds-1,"hit");i<ds&&D[i].hit;)i++;
	D.kindN=i;return D
}
//write counter
function hits(O,H,k,n){
	for(var i=0,a=1,b=2,c=k,p=k,F=new Float64Array(33),S=new Float64Array(33),N=n,M=n;p;)F[log2b(H[--p]+1)]++;
	for(;i<33&&c>0;c-=f){
		var f=F[i++],l=M/(b-1)>>>0,h=c-(c*(b-2)<N);
		O.ev(l<c?c-l:0,f>>>0,h>>>0);
		N-=f*(b-2);M-=f*(a-1),a*=2,b*=2
	}
	for(i=33,a=(1<<30)*4,b=a*2,c=N=M=0;i--;b/=2)
		S[i]=c,c+=f=F[i],N+=f*(a-1),M+=f*(b-2),a/=2;
	for(;n;n-=k,O.ev(l>>>0,k>>>0,h>>>0)){
		for(i=0,a=1,b=2,k=H[p++],c=log2b(k+1);i<c;S[i++]--,a*=2,b*=2)
			if(f=F[i])O.enc(f>>>0,S[i]>>>0,f+S[i]>>>0);
		S[i]>0&&O.enc(0,F[i]>>>0,F[i]+S[i]>>>0);
		F[i]--;N-=--a;M-=b-=2;
		(l=n-M)<a&&(l=a);
		(h=n-N)>b&&(h=b)
	}
}
//math.log2
function log2b(a,b,c){
	c=(65535<a)<<4,c|=b=(255<(a>>>=c))<<3,c|=b=(15<(a>>=b))<<2;return(c|=b=(3<(a>>=b))<<1)|a>>b>>1
}
function isort(A,a,z,e){
	for(var i=a,b,c,d;a<z;A[c]=b)
		if((b=A[c=d=++a])[e]>A[i][e])for(;A[c]=A[--c],c>i;);
		else for(;b[e]>A[--d][e];)A[c]=A[c=d]
}
function qsort(A,a,z,e){
	if(z-a<9)return isort(A,a,z,e);
	var b=A[a][e],c,i=A[z][e],j=a,k=z,p=A[a+z>>1][e];
	b>i?i>p?p=i:b>p||(p=b):b>p?p=b:p>i&&(p=i);
	for(i=a;j<=k;)
		if((b=A[j])[e]<p)A[j]=A[k],A[k--]=b;
		else{if(b[e]>p)c=A[i],A[i++]=b,A[j]=c;j++}
	a<--i&&qsort(A,a,i,e);++k<z&&qsort(A,k,z,e)
}
function ssort(A,e){
	for(var a,b,c,z=A.length,h=z>>1;h;h=h^2?h*5/11|0:1)
		for(a=0;a<z;A[b]=c)
			for(c=A[b=a++];b>=h&&A[b-h][e]<c[e];)A[b]=A[b-=h]}
