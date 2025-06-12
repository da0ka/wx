#ifndef __SystemTime_cxx__
#define __SystemTime_cxx__

// Copyright(c) Satoshi Yamazaki 2001. All rights reserved.
// 詳細については、yz2-Copyright.txt をご覧下さい。

#include <stdio.h>        // sprintf()

//-----------------
//  WIN32
#ifdef WIN32

	// WIN32 には gettimeofday() が無いので timeGetTime() を使う
	#include <windows.h>
	#include <mmsystem.h>               // timeGetTime()
	#pragma comment( lib, "winmm.lib" ) // timeGetTime()

#else  // WIN32

//-----------------
//  LINUX
#ifdef LINUX

	// LINUX には gettimeofday() が無いので ftime() を使う

	#include <sys/timeb.h>    // ftime(), struct timeb

#else  // LINUX

//-----------------
//  BSDOS & MS-DOS(DJGPP)

	#include <time.h>         // gettimeofday(), struct timeval

#endif // LINUX
#endif // WIN32

namespace CXL{
//-------------------------------------------------------------------
// 起動時間 1/100 sec. を取得し、各種形式で返す。

class  SystemTime{
	private: char  _ca_time[12] ; // 時間 文字列 'xxx.xx': 1/100 sec.

	//-----------------------
	//  constructor
	//  機能 : 現在のprogram起動時間を 1/1000 単位で設定
public:
	SystemTime()         // in : なし
	{
		Reset();
	}
	//-----------------------
	//  関数値 : int : 数値 1/100 sec.
public:
	operator int        // out: int : 数値 1/100 sec.
	(                   // ---: ----------
	)                   // in : なし
	{
		return  int_Get() ;
	}
	//-----------------------
	//  関数値 : char * : 文字列 'xxx.xx': 1/100 sec.
public:
	operator char*      // out: char * : 文字列 'xxx.xx': 1/100 sec.
	(                   // ---: ----------
	)                   // in : なし
	{
		return  str_Get() ;
	}

	//-----------------
	#ifdef WIN32
	// WIN32 は timeGetTime() を使う
	private: unsigned long   _start_time ;  // 起動時間 ftime()

	//-----------------------
	//  機能   : callされた時間のprogram起動時間を 1/1000 単位で設定
public:
	void                // out: なし
	Reset(              // ---: ----------
	)                   // in : なし
	{
		_start_time = timeGetTime() ;
	}
	//-----------------------
	//  関数値 : char * : 文字列 'xxx.xx': 1/100 sec.
public:
	char *              // out: char * : 文字列 'xxx.xx': 1/100 sec.
	str_Get(            // ---: ----------
	)                   // in : なし
	{
		unsigned long  ul = timeGetTime() - _start_time ; // 秒数を計算

		sprintf( _ca_time, "%2.2f", ((float)ul)/1000.0 ) ;
		return  _ca_time ;
	}


	//-----------------------
	//  関数値 : int : 数値 1/100 sec.
public:
	int                 // out: int : 数値 1/100 sec.
	int_Get(            // ---: ----------
	) const             // in : なし
	{
		unsigned long  ul = timeGetTime() - _start_time ; // 秒数を計算

		return  (int)(ul/10) ;  //  1/100秒を返す
	}
	#else  // WIN32

	//-----------------
	//  LINUX
	#ifdef LINUX

	// LINUX には gettimeofday() が無いので ftime() を使う
	private: struct timeb    _start_time ;  // 起動時間 ftime()

	//-----------------------
	//  機能   : callされた時間のprogram起動時間を 1/1000 単位で設定
public:
	void                // out: なし
	Reset(              // ---: ----------
	)                   // in : なし
	{
	   // LINUX には gettimeofday() が無いので ftime() を使う
	   ftime( & _start_time ) ;
	}
	//-----------------------
	//  関数値 : char * : 文字列 'xxx.xx': 1/100 sec.
public:
	char *              // out: char * : 文字列 'xxx.xx': 1/100 sec.
	str_Get(            // ---: ----------
	)                   // in : なし
	{
		struct timeb  now_time ; // 現在の時間
		ftime( & now_time ) ;

		unsigned long  ul = now_time.time - _start_time.time ; // 秒数を計算
		ul *= 1000 ;
		ul += now_time.millitm - _start_time.millitm ;

		sprintf( _ca_time, "%2.2f", ((float)ul)/1000.0 ) ;
		return  _ca_time ;
	}


	//-----------------------
	//  関数値 : int : 数値 1/100 sec.
public:
	int                 // out: int : 数値 1/100 sec.
	int_Get(            // ---: ----------
	) const             // in : なし
	{
		struct timeb  now_time ; // 現在の時間
		ftime( & now_time ) ;

		unsigned long  ul = now_time.time - _start_time.time ; // 秒数を計算
		ul *= 1000 ;
		ul += now_time.millitm - _start_time.millitm ;

		return  (int)(ul/10) ;  //  1/100秒を返す
	}
	#else  // LINUX

	//-----------------
	//  BSDOS & MS-DOS(DJGPP)

	private: struct timeval  _start_time ;  // 起動時間 gettimeofday()

	//-----------------------
	//  関数値 : void : 無し
public:
	void                // out: なし
	Reset(              // ---: ----------
	)                   // in : なし
	{
		gettimeofday( & _start_time , 0 ) ;
	}
	//-----------------------
	//  関数値 : char * : 文字列 'xxx.xx': 1/100 sec.
public:
	char *              // out: char * : 文字列 'xxx.xx': 1/100 sec.
	str_Get(            // ---: ----------
	)                   // in : なし
	{
		struct timeval now_time;
		gettimeofday( & now_time , 0 ) ;

		unsigned long  ul = now_time.tv_sec - _start_time.tv_sec ; // 秒数を計算
		ul *= 1000 ;
		ul += (now_time.tv_usec - _start_time.tv_usec)/1000 ;

		sprintf( _ca_time, "%2.2f", ((float)ul)/1000.0 ) ;
		return  _ca_time ;
	}
	//-----------------------
	//  関数値 : int : 数値 1/100 sec.
public:
	int             // out: int : 数値 1/100 sec.
	int_Get(        // ---: ----------
	) const         // in : なし
	{
		struct timeval now_time;
		gettimeofday( & now_time , 0 ) ;

		unsigned long  ul = now_time.tv_sec - _start_time.tv_sec ; // 秒数を計算
		ul *= 1000 ;
		ul += (now_time.tv_usec - _start_time.tv_usec)/1000 ;

		return  (int)(ul/10) ;  //  1/100秒を返す
	}
	#endif // LINUX
	#endif // WIN32
};
}; // end namespace CXL

//===================================================================
#ifdef __UnitTest__SystemTime_cxx__

#define TEST(A) printf( "\n---- TEST NO.%d ----\n[%s]\n",unit_test_counter++,(A))
static  unit_test_counter = 1 ;

int main(){
	using namespace CXL ;

	//-----------------------
	// TEST SystemTime
	{
		SystemTime  time_now ;

		printf( "Hit [enter] Key!" ) ; getchar() ;

		//-----------------------
		//  時間を取得 ( 文字列 xxx.xx )
		TEST( "str_Get() で 時間の文字列 xxx.xx が取得できるか?" ) ;

		printf( "time_now: %s (sec.)\n", time_now.str_Get() ) ;
		//-----------------------
		//  時間を取得 ( 文字列 xxx.xx )
		TEST( "operator char* で 時間の文字列 xxx.xx が取得できるか?" ) ;
		printf( "time_now: %s (sec.)\n", (char*)time_now ) ;

		//-----------------------
		//  時間(秒数)を取得 ( 数値 )
		TEST( "int_Get() で 時間(秒数)の数値が取得できるか?" ) ;
		{
			int  t = time_now.int_Get() ;
			printf( "time_now: %d (1/100sec.), %d.%02d (sec.)\n", t, t/100, t%100 ) ;
		}
		//-----------------------
		//  時間(秒数)を取得 ( 数値 )
		TEST( "operator int で 時間(秒数)の数値が取得できるか?" ) ;
		{
			int  t = time_now ;
			printf( "time_now: %d (1/100sec.), %d.%02d (sec.)\n", t, t/100, t%100 ) ;
		}
		//-----------------------
		//  今の時間に設定
		TEST( "Reset() で 時間の再設定できるか?" ) ;
		printf( "Hit [enter] Key!" ) ; getchar() ;
		time_now.Reset() ;

		//-----------------------
		//  時間を取得 ( 文字列 hh:mm:ss )
		TEST( "Reset() 後、operator char* で 時間の文字列 hh:mm:ss が取得できるか?" ) ;
		printf( "time_now: %s\n", (char*)time_now ) ;

		//-----------------------
		//  時間を取得 ( 数値 )
		TEST( "Reset() 後、operator int で 時間(秒数)の数値が取得できるか?" ) ;
		{
			int  t = time_now ;
			printf( "time_now: %d (1/100sec.), %02d.%02d (sec.)\n", t, t/100, t%100 ) ;
		}
	}
	printf( "Hit [enter] Key!" ) ; getchar() ;

	return 0 ;
}
#endif // __UnitTest__SystemTime_cxx__

#endif // __SystemTime_cxx__