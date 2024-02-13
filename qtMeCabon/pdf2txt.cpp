/**
 * @file pdf2txt.cpp
 * @brief 小説家になろうサイトの縦書きpdfファイルをtxtファイルに変換する
 * @author m.kawakami
 * @date 21/05/??
 */

//#include <afxwin.h>
//#include "stdafx.h"
#include "PdfRdTxt.h"
#include <mlang.h>
#include <stdio.h>
#include <io.h>
#include <wchar.h>
#include <fcntl.h>
#include "cmultilang.h"


#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib, "Mlang.lib")
PdfReadText	PdfRead;


static int nCodePage = 0;


#if 0
/**
 * @fn
 * 指定コードページの文字列をユニコードに変換
 * @brief 指定コードページの文字列をユニコードに変換
 * @param (codepage) 元のコードページ
 * @param (*pszCPstr) 元のコードページの文字列
 * @param (*pszUni) ユニコードの文字列をストアするバッファ
 * @param (lenUni) ユニコードの文字列をストアするバッファの長さ(byte)
 * @return -1:エラー　0以上:ユニコードサイズ
 * @sa
 * @detail 指定コードページの文字列をユニコードに変換
 */
int CodePageToUni( UINT codepage, char *pszCPstr, int lenCPstr, char *pszUni, int lenUni )
{	IMultiLanguage2 *pIML2;		// IMultiLanguage オブジェクトを作成	暫定　毎度クリエイトするのは無駄 
	
	CoInitialize( NULL );
//	HRESULT hr = CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pIML2 ));
	HRESULT hr = CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage2, ( void ** )( &pIML2 ));
	if ( !SUCCEEDED( hr ))
	{	CoUninitialize();
		return ( -1 );
	}
	UINT pcSrcSize = lenCPstr;	//( lstrlen(( LPCTSTR )pszCPstr ) + 1 ) * sizeof( TCHAR );		// バイト数で格納するバッファへのポインタ
	UINT pcDestSize = lenUni;														// 文字列のバイト数
																					// コードページから Unicode 文字列に変換
	pIML2->ConvertStringToUnicode( 0, codepage, ( CHAR * )pszCPstr, &pcSrcSize, ( WCHAR * )pszUni, &pcDestSize );
	pIML2->Release();
	CoUninitialize();
	*((( WCHAR * )pszUni ) + pcDestSize ) = L'\0';
	return (( pcDestSize <= 0 )? 0: pcDestSize );
}
#endif


/**
 * @fn
 * コードページの初期化
 * @brief コードページの初期化
 * @return なし
 * @sa
 * @detail コードページの初期化
 */
void initializeCodePage()
{	nCodePage = 0;
}


/**
 * @fn
 * カレントコードページの取得
 * @brief カレントコードページの取得
 * @return なし
 * @sa
 * @detail カレントコードページの取得
 */
#define currentCodePage()	analizCodePage( NULL, 0 )


/**
 * @fn
 * コードページの解析
 * @brief コードページの解析
 * @param (*pBuf) ソース文字列
 * @param (cbBuf) ソース文字列長さ
 * @return コードページ
 * @sa
 * @detail コードページの解析
 */
int analizCodePage( char *pBuf, int cbBuf )
{   CMultiLang cml;

	if (( nCodePage == 0 ) &&		// ( seekPos == 0 )	// 初回の読み込み？ 
				( pBuf != NULL ))
    {
        nCodePage = cml.analizCodePage( pBuf, cbBuf );
#if 0
        CoInitialize( NULL );	// 文字コード判定
		IMultiLanguage2 *pIMulLang;
		if ( CoCreateInstance( CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER,
			IID_IMultiLanguage2, ( void** )&pIMulLang ) == S_OK )
		{	int					detectEncCount = 1, cbBufcpy = cbBuf;
			DetectEncodingInfo	detectEnc;

			// DetectInputCodepage関数の場合
			//   byte *data    - 調べたいデータ
			//   int   dataLen - 調べたいデータのバイト数を渡し、
			//                   処理されたバイト数を受け取る
			if ( pIMulLang->DetectInputCodepage(
				MLDETECTCP_NONE, 0, pBuf, &cbBufcpy,
					&detectEnc, &detectEncCount ) == S_OK )
			{	nCodePage = detectEnc.nCodePage;
				// コードページが UNICODE 以外の場合 ANSI に変更		
				if ( nCodePage == 20127 )	// コードページが 20127(US-ASCII) の場合 UTF-8 に変更
				{	nCodePage = 65001;
				}
				else if (( nCodePage != 1200 ) &&	// UTF-16LE
						( nCodePage != 1201 ) &&	// UTF-16BE
						( nCodePage != 65001 ))		// UTF-8
				{	nCodePage = 932;				// 日本語シフトJIS
			}	}
			pIMulLang->Release();
		}
		CoUninitialize();
#endif
    }
	return ( nCodePage );
}


#if 0
wchar_t *exchgExt( wchar_t *szFile, wchar_t *szNewExt )
{	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR * 2];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];
	wchar_t path[_MAX_PATH * 2];

	_wsplitpath( szFile, drive, dir, fname, ext );
	_wmakepath( path, drive, dir, fname, szNewExt );
	return ( _wcsdup( path ));
}
#endif


/**
 * @fn
 * コードページの解析
 * @brief コードページの解析
 * @param (*szFile) ソース.pdfファイル
 * @param (*szOutFile) 変換後書き込むテキストファイル名
 * @return なし
 * @sa
 * @detail 小説家になろうサイトの縦書き.pdfファイルからテキストを抽出し出力ファイルへ書き出す
 */
void Pdf2Txt( wchar_t *szFile, wchar_t *szOutFile )
{	DWORD page, pageLen;
    CMultiLang cml;
	FILE *fp;
	unsigned char ReadBuf[MAXPAGELENGTH];

	initializeCodePage();	//	nCodePage = 0;
    if (( fp = _wfopen( szOutFile, L"wt" )) == NULL )
	{	return;
	}
	PdfRead.PdfOpen( szFile );
#if 0						// file read debug
	PdfRead.repgetForwardToken(( char * )( &ReadBuf[0]));
	PdfRead.repgetPrevToken(( char * )( &ReadBuf[0]));
#else
	for ( page = 0; page < PdfRead.PageCnt; page++ )
	{	pageLen = PdfRead.PdfReadPage( page, ( char * )ReadBuf, sizeof( ReadBuf ));
		if ( page == 0 )
		{	analizCodePage(( char * )ReadBuf, pageLen );
        }
        if (( ReadBuf[0] | ReadBuf[1]) != 0 )	// 23/09/03 ユニコード(utf16)なので2バイト単位
        {   char Utf8Buf[MAXPAGELENGTH];

            cml.UniToUTF8(( char * )ReadBuf, ( char * )Utf8Buf,  ( int )sizeof( Utf8Buf ));
            fprintf( fp,  "%s\n", Utf8Buf );
	}	}
#endif
	PdfRead.PdfClose();
	fclose( fp );
}


#if 0						// file read debug


/**
 * @fn
 * デバッグ用メイン
 * @brief デバッグ用メイン
 * @param (argc) 引数の数
 * @param (*argv[]) 引数文字列の配列
 * @return 0
 * @sa
 * @detail デバッグ用メイン
 */
int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleCP( 65001 );
	SetConsoleOutputCP( 65001 );
//	SetConsoleCP( UTF-16LE );
//	SetConsoleOutputCP( CP_WINUNICODE );
//	SetConsoleOutputCP( 1200 );
//	_setmode( _fileno(stdout), _O_U16TEXT );
//	Pdf2Txt( L"D:\\転生したらスライムだった件\\N2791GD 聖女の魔力は万能です　～ 番外編 ～.pdf" );
    Pdf2Txt( L"D:\\転生したらスライムだった件\\N7078HE 僕を聖女と呼ばないで！.pdf" );
	return 0;
}
#endif

