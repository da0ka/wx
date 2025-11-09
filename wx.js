/* WX 2025.11.9
dictionary based compressor*/
async function WXe(A,done,rate=a=>a){
	function enc(c,f,t){
		R=R/t>>>0;L+=c*R;
		for(R*=f;R<16777216;L=L<<8>>>0,R*=256)
			if((c=0xffffffff<L)||255>L>>>24)
				for(O[o++]=255&c--+M,M=L>>>24;N;N--)O[o++]=255&c;
			else++N
	}
	function pb(b,c){
		for(L+=(R>>>=1)*b;R<16777216;L=L<<8>>>0,R*=256)
			if((c=0xffffffff<L)||255>L>>>24)
				for(O[o++]=255&c--+M,M=L>>>24;N;N--)O[o++]=255&c;
			else++N
	}
	function eb(i,b,P,s=5){
		var p=P[i],c=(R>>>12)*p;
		b?R=c:(R-=c,L+=c);
		for(P[i]-=(p-(b<<12)>>s)+b;R<16777216;L=L<<8>>>0,R*=256)
			if((c=0xffffffff<L)||255>L>>>24)
				for(O[o++]=255&c--+M,M=L>>>24;N;N--)O[o++]=255&c;
			else++N
	}
	function ev(n,v,m,o){
		for(;m-n>65535;v>o?n=o+1:m=o)o=n+(m-n>>1),pb(v>o);
		n^m&&enc(v-n,1,m-n+1)
	}
	var z=A.length,B=new Uint8Array(z+N_EXT),P=new Int32Array(z+1),W=new Int32Array(z>N_EXT?z:N_EXT),
		o=-1,O=[],L=0,R=o>>>0,M,N=0;
	B.set(A);O.eb=eb;O.ev=ev;O.enc=enc;
	let D=z<N_EXT||await extract(B,W,P,z,rate),i=D.length,ds=i,wn=0|D.wordN,kn=0|D.kindN,s=0,cs=0,E=kn>256?kn+1:257,F=new Uint32Array(E),C=new Uint32Array(E+1),I=new Uint32Array(ds+1);
	//write sizes
	for(let c=64,n=kn;n-=O[++o]=n&c-1;c=256)n/=c,O[0]+=64;
	for(let c=64,n=wn,p=o+1;n-=M=O[++o]=n&c-1;c=256)n/=c,O[p]+=64;
	if(z<N_EXT){for(let c of A)O[++o]=c;done&&done(O,z,++o);return O}
	for(;i;)I[D[--i].number]=i;
	//scan words and write its length
	for(E=0;N_EXT-1>>++E;);E--;
	for(let G=Uint16Array.from(Array(60<<3),a=>2048),a=0,c=0,d;i<kn;){
		let l=D[i].length,b=l;
		//length is gamma coded
		eb(29,l>1,G,4);s+=l;
		if(l>1){
			for(--b;b>>=1;)eb(a++<<3|c,0,G,5);
			for(a<E&&eb(a<<3|c,1,G,5),d=c,c=a;a;)eb(a+29<<3|d,l-1>>--a&1,G,6);
		}
		for(b=P[D[i++].first];l;)++F[B[--l+b]]
	}
	hits(O,F,256,s);
	for(i=0;i<s;)C[cs+1]=i+=F[cs++];
	//write words
	for(E=F.slice(i=0);s;)
		for(let l=D[i].length,p=P[D[i++].first],a=0,b;a<l;){
			enc(C[b=B[a+++p]],F[b],s);
			if(!--E[b])for(b=s=0;b<cs;)C[b+1]=s+=F[b]=E[b++]
		}
	for(i=0;i<kn;)C[i+1]=s+=P[kn-i]=F[i]=E[i]=D[i++].hit;
	//frequency converted to reversed sequence of differences(much compressible)
	for(P[i=0]=F[kn-1];i<kn;)P[++i]=P[i+1]-P[i];
	//write above
	for(let G=Uint16Array.from(Array(60),a=>2048),a=i=0;i<kn;){
		let l=P[i++],b=l;
		eb(29,l>0,G,4);
		if(l){
			for(;b>>=1;)eb(a++,0,G,5);
			for(eb(a,1,G,5);a;)eb(a+29,l>>--a&1,G,6)
		}
	}
	//write sequence
	let fn=b=>wait0(c=>b(rate(i,z))),st=+new Date;
	for(i=0;s;i&32767||Date.now()-st<100||await new Promise(fn,st=+new Date))if((D=W[i++])>0){
		enc(C[D=I[D]],F[D],s);
		if(!--E[D])for(D=s=0;D<kn;)C[D+1]=s+=F[D]=E[D++]
	}
	for(;s++<5;L=L<<8>>>0)
		if((c=0xffffffff<L)||255>L>>>24)
			for(O[o++]=255&c--+M,M=L>>>24;N;N--)O[o++]=255&c;
		else++N;
	done&&done(O,z,o);return O
}
async function WXd(A,done,rate=a=>a){
	function cum(t){
		for(;R<16777216;R*=256)L=(L<<8|A[a++])>>>0;
		R=R/t>>>0;return L/R>>>0
	}
	function gb(b){
		for(;R<16777216;R*=256)L=(L<<8|A[a++])>>>0;
		R>>>=1,b=L-R>>>31,L-=R&--b;
		return-b
	}
	function db(i,P,s=5){
		for(;R<16777216;R*=256)L=(L<<8|A[a++])>>>0;
		let p=P[i],c=(R>>>12)*p,b=1;
		L<c?R=c:(R-=c,L-=c,b=0);
		P[i]-=(p-(b<<12)>>s)+b;
		return b
	}
	function dv(n,m,o){
		for(;m-n>65535;gb()?n=o+1:m=o)o=n+(m-n>>1);
		if(n^m)n+=o=cum(m-n+1),L-=R*o;
		return n
	}
	function gets(H,k,n){
		for(var i=0,a=1,b=2,c=k,p=0,F=new Float64Array(33),S=new Float64Array(33),N=n,M=n;i<33&&c>0;c-=f){
			var l=M/(b-1)>>>0,h=c-(c*(b-2)<N),f=F[i++]=dv(l<c?c-l:0,h>>>0);
			N-=f*(b-2);M-=f*(a-1),a*=2,b*=2
		}
		for(i=33,a=(1<<30)*4,b=a*2,c=N=M=0;i--;b/=2)
			S[i]=c,c+=f=F[i],N+=f*(a-1),M+=f*(b-2),a/=2;
		for(;n;n-=H[p++]=dv(l>>>0,h>>>0)){
			for(i=0,a=1,b=2;S[i]>0;S[i++]--,a*=2,b*=2)if(f=F[i]){
				if(cum(f+S[i]>>>0)<f){R*=f;break}
				L-=R*f;R*=S[i]
			}
			F[i]--;N-=--a;M-=b-=2;
			(l=n-M)<a&&(l=a);
			(h=n-N)>b&&(h=b)
		}
	}
	var a=0,i=A[a++],s=64,z=A.length,kn=i&63,wn,L,R=-1>>>0,o=0,O=[];
	for(i>>=6;i--;s*=256)kn+=A[a++]*s;//read dict size
	i=A[a++],s=64,wn=i&63;
	for(i>>=6;i--;s*=256)wn+=A[a++]*s;//read CFG size
	if(!kn&&!wn){for(s=A.length;a<s;)O[o++]=A[a++];done&&done(O,z,o);return O}
	for(;++i<4;)L=(L<<8|A[a++])>>>0;
	//read word lengths
	let D=[],E=0;
	for(;N_EXT-1>>++E;);E--;
	for(let G=Uint16Array.from(Array(60<<3),a=>2048),a=i=s=0,c=0,d,l;i<kn;s+=D[i++]=l+1)
		if(l=db(29,G,4)){
			for(;a<E&&!db(a<<3|c,G,5);)a++;
			for(d=c,c=a,l=1<<a;a;)l|=db(a+29<<3|d,G,6)<<--a
		}
	//build dict
	let cs=i=0,F=new Uint32Array(c=kn>256?kn+1:257),C=new Uint32Array(c+1);
	for(gets(F,256,s);i<s;)C[cs+1]=i+=F[cs++];
	for(E=F.slice(i=0);s;){
		for(let l=D[i],p=0,W=D[i++]=new Uint8Array(l);p<l;){
			for(;R<16777216;R*=256)L=(L<<8|A[a++])>>>0;
			R=R/s>>>0;
			let b=0,c=cs-1,d,e=L/R>>>0;
			for(;b<c;C[d+1]>e?c=d:b=++d)d=b+c>>1;
			L-=R*C[W[p++]=b],R*=F[b];
			if(!--E[b])for(b=s=0;b<cs;)C[b+1]=s+=F[b]=E[b++]
		}
	}
	//extract frequency
	for(let G=Uint16Array.from(Array(60),a=>2048),a=0,p=kn,l;p;F[--p]=l)
		if(l=db(29,G,4)){
			for(;!db(a,G,5);)a++;
			for(l=1<<a;a;)l|=db(a+29,G,6)<<--a
		}
	for(i=kn,s=0;i;)F[i]=s+=F[--i];
	for(i=s=0;i<kn;)C[i+1]=s+=F[i]=E[i++]=F[i];
	let fn=b=>wait0(c=>b(rate(a,z))),st=+new Date
	//extract sequence
	for(;s;a&16383||Date.now()-st<100||await new Promise(fn,st=+new Date)){
		for(;R<16777216;R*=256)L=(L<<8|A[a++])>>>0;
		R=R/s>>>0;
		let b=0,c=kn-1,d,e=L/R>>>0;
		for(;b<c;C[d+1]>e?c=d:b=++d)d=b+c>>1;
		L-=R*C[b],R*=F[b];
		if(!--E[b])for(i=s=0;i<kn;)C[i+1]=s+=F[i]=E[i++];
		O.push.apply(O,D[b])
	}done&&done(O,z,O.length);return O
}