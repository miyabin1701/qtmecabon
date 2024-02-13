/**
 * @file cmultilang.cpp
 * @brief .netの文字コード判別処理ifと文字コード変換関数
 * @author m.kawakami
 * @date 21/05/??
 */

#include "cmultilang.h"
//#include <io.h>
//#include <wchar.h>
//#include <fcntl.h>


#pragma comment(lib, "comctl32.lib")


/**
 * @fn
 * CMultiLangクラスコンストラクタ
 * @brief CMultiLangクラスコンストラクタ
 * @return なし
 * @sa
 * @detail なんもしてない
 */
CMultiLang::CMultiLang()
{

}


/**
 * @fn
 * CMultiLangクラスデストラクタ
 * @brief CMultiLangクラスデストラクタ
 * @return なし
 * @sa
 * @detail なんもしてない
 */
CMultiLang::~CMultiLang()
{

}


/**
 * @fn
 * 文字列のコードページを判別する
 * @brief 文字列のコードページを判別する
 * @param (*pBuf) 判別するソース文字列
 * @param (cbBuf) 判別するソース文字列の長さ(byte)
 * @return コードページ
 * @sa
 * @detail .netの判別関数への中継を行う
 */
int CMultiLang::analizCodePage( char *pBuf, int cbBuf )
{   int nCodePage = -1;

    if ( pBuf != NULL )
    {	CoInitialize( NULL );	// 文字コード判定
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
                if ( nCodePage == 20127 )       	// コードページが 20127(US-ASCII) の場合 UTF-8 に変更
                {	nCodePage = 65001;
                }
                else if (( nCodePage != 1200 ) &&	// UTF-16LE
                         ( nCodePage != 1201 ) &&	// UTF-16BE
                         ( nCodePage != 65001 ))	// UTF-8
                {	nCodePage = 932;				// 日本語シフトJIS
            }	}
            pIMulLang->Release();
        }
        CoUninitialize();
    }
    return ( nCodePage );
}


/**
 * @fn
 * 指定コードページの文字列をユニコードに変換
 * @brief 指定コードページの文字列をユニコードに変換
 * @param (codepage) 元のコードページ
 * @param (*pszCPstr) 元のコードページの文字列
 * @param (lenCPstr) 元のコードページの文字列の長さ(byte)
 * @param (*pszUni) ユニコードの文字列をストアするバッファ
 * @param (lenUni) ユニコードの文字列をストアするバッファの長さ(byte)
 * @return -1:エラー　0以上:ユニコードサイズ
 * @sa
 * @detail .netの変換関数への中継を行う
 */
#if 1
int CMultiLang::CodePageToUni( int codepage, char *pszCPstr, int lenCPstr, char *pszUni, int lenUni )
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
 * ユニコード文字列をutf8に変換
 * @brief ユニコード文字列をutf8に変換
 * @param (*pszUni) ユニコード文字列
 * @param (*pszUtf8) utf8ストア先
 * @param (lenUtf8) utf8ストア先の長さ(byte)
 * @return なし
 * @sa
 * @detail utf16leからutf8へコード変換する
 */
void CMultiLang::UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 )
{	int iLenUnicode;

    iLenUnicode = ( int )wcslen(( const wchar_t* )pszUni );	// * sizeof( TCHAR );
    //		次に、UniocdeからUTF8に変換する	// サイズを計算する
    int iLenUtf8 = WideCharToMultiByte( CP_UTF8, 0, ( wchar_t * )pszUni, iLenUnicode, NULL, 0, NULL, NULL );
    if ( iLenUtf8 <= lenUtf8 )
    {	WideCharToMultiByte( CP_UTF8, 0, ( wchar_t * )pszUni, iLenUnicode, pszUtf8, lenUtf8, NULL, NULL );
        *( pszUtf8 + iLenUtf8 ) = EOS;
    }
}


/**
 * @fn
 * utf8文字列をユニコードに変換
 * @brief utf8文字列をユニコードに変換
 * @param (*pszUtf8) utf8文字列
 * @param (*pszUni) ユニコード文字列ストア先
 * @param (lenUni) ユニコード文字列ストア先の長さ(byte)
 * @return なし
 * @sa
 * @detail utf8からutf16leへコード変換する
 */
void CMultiLang::UTF8ToUni( char *pszUtf8, char *pszUni, int lenUni )
{   // int le = strlen( pszUtf8 );
    //	まずUniocdeに変換する		// サイズを計算する
    int iLenUnicode = MultiByteToWideChar( CP_UTF8, 0, pszUtf8, ( int )strlen( pszUtf8 ) + 1, NULL, NULL );
    if ( iLenUnicode <= lenUni )
    {	MultiByteToWideChar( CP_UTF8, 0, pszUtf8, ( int )strlen( pszUtf8 ) + 1, ( LPWSTR )pszUni, lenUni );
        //MultiByteToWideChar( 1201, 0, pszUtf8, ( int )strlen( pszUtf8 ) + 1, ( LPWSTR )pszUni, lenUni );
    }
}


/*  https://learn.microsoft.com/ja-jp/windows/win32/intl/code-page-identifiers
932 	shift_jis	ANSI/OEM 日本語;日本語 (Shift-JIS)
20127	us-ascii	US-ASCII (7 ビット)
1200	utf-16      Unicode UTF-16、リトル エンディアン バイト順 (ISO 10646 の BMP)。マネージド アプリケーションでのみ使用できる
1201	unicodeFFFE	Unicode UTF-16、ビッグ エンディアン バイト順。マネージド アプリケーションでのみ使用できる
65001	utf-8       Unicode (UTF-8)
*/


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
int CMultiLang::CodePageToUni( int codepage, char *pszCPstr, char *pszUni, int lenUni )
{
    //	まずUniocdeに変換する		// サイズを計算する
    int iLenUnicode = MultiByteToWideChar( codepage, 0, pszCPstr, ( int )strlen( pszCPstr ) + 1, NULL, NULL );
    if ( iLenUnicode <= 0 )
    {	return ( 0 );
    }
    if ( iLenUnicode <= lenUni )
    {	MultiByteToWideChar( codepage, 0, pszCPstr, ( int )strlen( pszCPstr ) + 1, ( LPWSTR )pszUni, lenUni );
        return ( iLenUnicode );
    }
    return ( -1 );
}


/**
 * @fn
 * shift-jis文字列をユニコードに変換
 * @brief shift-jis文字列をユニコードに変換
 * @param (*pszSjis) shift-jis文字列
 * @param (*pszUni) ユニコード文字列ストア先
 * @param (lenUni) ユニコード文字列ストア先の長さ(byte)
 * @return なし
 * @sa
 * @detail shift-jisからutf16leへコード変換する
 */
void CMultiLang::ShiftJisToUni( char *pszSjis, char *pszUni, int lenUni )
{//	wchar_t bufUnicode[MAX_LINE];

//	まずUniocdeに変換する	// サイズを計算する
    size_t iLenUnicode = MultiByteToWideChar( CP_ACP, 0, pszSjis, ( int )strlen( pszSjis ) + 1, NULL, NULL );
    if ( iLenUnicode <= lenUni )
    {	MultiByteToWideChar( CP_ACP, 0, pszSjis, ( int )strlen( pszSjis ) + 1, ( LPWSTR )pszUni, lenUni );
    }
}


/**
 * @fn
 * shift-jis文字列をutf8に変換
 * @brief shift-jis文字列をutf8に変換
 * @param (*pszSjis) shift-jis文字列
 * @param (*pszUtf8) utf8文字列ストア先
 * @param (lenUtf8) utf8文字列ストア先の長さ(byte)
 * @return なし
 * @sa
 * @detail shift-jisからutf8へコード変換する
 */
void CMultiLang::ShiftJisToUTF8( char *pszSjis, char *pszUtf8, int lenUtf8 )
{	wchar_t bufUnicode[MAX_LINE];

//	まずUniocdeに変換する	// サイズを計算する
//	int iLenUnicode = MultiByteToWideChar( CP_ACP, 0, szShiftJis, strlen( szShiftJis ) + 1, NULL, NULL );
    int iLenUnicode = MultiByteToWideChar( CP_ACP, 0, pszSjis, ( int )strlen( pszSjis ) + 1, NULL, NULL );
    if ( iLenUnicode <= ( sizeof( bufUnicode ) / sizeof( bufUnicode[0])))
    {	MultiByteToWideChar( CP_ACP, 0, pszSjis, ( int )strlen( pszSjis ) + 1, bufUnicode, sizeof( bufUnicode ));
//		次に、UniocdeからUTF8に変換する	// サイズを計算する
        int iLenUtf8 = WideCharToMultiByte( CP_UTF8, 0, bufUnicode, iLenUnicode, NULL, 0, NULL, NULL );
        if ( iLenUtf8 <= lenUtf8 )
        {	WideCharToMultiByte( CP_UTF8, 0, bufUnicode, iLenUnicode, pszUtf8, lenUtf8, NULL, NULL );
        }
    }
}


/*      Shift-JIS誤変換可能性文字達
http://x0213.org/wiki/wiki.cgi?page=%C7%C8%A5%C0%A5%C3%A5%B7%A5%E5%CC%E4%C2%EA
日本語通用名称	区点      SJIS	文字名				正しいUnicode符号位置      誤り
波ダッシュ		1-33	8160	WAVE DASH				U+301C				U+FF5E
双柱			1-34	8161	DOUBLE VERTICAL LINE	U+2016				U+2225
負符号		1-61	817C	MINUS SIGN				U+2212				U+FF0D
セント記号		1-81	8191	CENT SIGN				U+00A2				U+FFE0
ポンド記号		1-82	8192	POUND SIGN				U+00A3				U+FFE1
否定記号		2-44	81CA	NOT SIGN				U+00AC				U+FFE2
ダッシュ(全角)	1-29	815C	EM DASH					U+2014				U+2015
*/


/**
 * @fn
 * utf8文字列をshift-jisに変換
 * @brief utf8文字列をshift-jisに変換
 * @param (*pszUtf8) utf8文字列
 * @param (*pszSjis) shift-jis文字列ストア先
 * @param (lenSjis) shift-jis文字列ストア先の長さ(byte)
 * @return なし
 * @sa
 * @detail utf8からshift-jisへコード変換する（誤変換可能性文字置き換え含む）
 */
void CMultiLang::UTF8ToShiftJis( char *pszUtf8, char *pszSjis, int lenSjis )
{	static WORD tblOfSjisNg[][2] =
    {	{	0x2212, 0xFF0D },
        {	0x301C, 0xFF5E },
        {	0x2016, 0x2225 },
        {	0x00A2, 0xFFE0 },
        {	0x00A3, 0xFFE1 },
        {	0x00AC, 0xFFE2 },
        {	0x2014, 0x2015 }
    };
    wchar_t bufUnicode[MAX_LINE], *p;
    int iSj, n;

//	まずUniocdeに変換する		// サイズを計算する
    int iLenUnicode = MultiByteToWideChar( CP_UTF8, 0, pszUtf8, ( int )strlen( pszUtf8 ) + 1, NULL, NULL );
    if ( iLenUnicode <= sizeof( bufUnicode ) / sizeof( bufUnicode[0]))
    {	MultiByteToWideChar( CP_UTF8, 0, pszUtf8, ( int )strlen( pszUtf8 ) + 1, bufUnicode, sizeof( bufUnicode ));
        for ( n = 0; n < lengthof( tblOfSjisNg ); n++ )	// シフトJIS誤変換文字の置き換え
        {	while (( p = wcschr( bufUnicode, tblOfSjisNg[n][0])) != NULL )
            {	*p = tblOfSjisNg[n][1];
        }	}
//		次に、UniocdeからShiftJisに変換する		// サイズを計算する
        int iLenUtf8 = WideCharToMultiByte( CP_ACP, 0, bufUnicode, iLenUnicode, NULL, 0, NULL, NULL );
        if ( iLenUtf8 <= lenSjis )
        {	iSj = WideCharToMultiByte( CP_ACP, 0, bufUnicode, iLenUnicode, pszSjis, lenSjis, NULL, NULL );
            pszSjis[iSj] = EOS;
//			if (( *pszSjis == '?' ) && ( *( pszSjis + 1 ) == EOS ))
            if ( *pszSjis == '?' )
            {	if ( bufUnicode[0] == 0x301c )		//
//				if ( bufUnicode[0] == L'～' )
                {	strcpy_s( pszSjis + 1, lenSjis, pszSjis );
                    strncpy_s( pszSjis, lenSjis, ( char * )L"～", 2 );
                }
            }
        }
    }
}


