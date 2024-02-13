/**
 * @file mecabif.cpp
 * @brief libmecab64.dllのインターフェースの処理
 * @author m.kawakami
 * @date 23/10/??
 */

#include "mecabif.h"
#include "mecab.h"
#include "cmultilang.h"
#include "profilevalues.h"
#include <QStringDecoder>
#include "messagebuff.h"

extern profileValues   *pprof;

LPmecab_model_new               mecabif::fpmecab_model_new = NULL;
LPgetLastError                  mecabif::fpgetLastError = NULL;
LPmecab_model_new_tagger		mecabif::fpmecab_model_new_tagger = NULL;
LPmecab_dictionary_info         mecabif::fpmecab_dictionary_info = NULL;
LPmecab_model_new_lattice   	mecabif::fpmecab_model_new_lattice = NULL;
LPmecab_destroy             	mecabif::fpmecab_destroy = NULL;
LPmecab_lattice_set_sentence	mecabif::fpmecab_lattice_set_sentence = NULL;
LPmecab_parse_lattice           mecabif::fpmecab_parse_lattice = NULL;
LPmecab_lattice_get_bos_node	mecabif::fpmecab_lattice_get_bos_node = NULL;
LPmecab_lattice_destroy         mecabif::fpmecab_lattice_destroy = NULL;
LPmecab_model_destroy           mecabif::fpmecab_model_destroy = NULL;
LPmecab_lattice_tostr           mecabif::fpmecab_lattice_tostr = NULL;
LPmecab_lattice_set_boundary_constraint mecabif::fpmecab_lattice_set_boundary_constraint = NULL;
LPmecab_version                 mecabif::fpmecab_version = NULL;

HINSTANCE                       mecabif::hMecabDLL = NULL;			// Handle to DLL
BOOL                            mecabif::OkMecab = FALSE;
long							mecabif::mecabDllLastError = 0;
mecab_model_t                   *mecabif::model = NULL;
mecab_model_t                   *mecabif::another_model = NULL;
mecab_t                         *mecabif::mecab = NULL;
mecab_lattice_t                 *mecabif::lattice = NULL;


/**
 * @fn
 * mecabifクラスのコンストラクタ
 * @brief mecabifクラスのコンストラクタ
 * @return なし
 * @sa
 * @detail なんもしてない
 */
mecabif::mecabif()
{

}


/**
 * @fn
 * mecabifクラスのデストラクタ
 * @brief mecabifクラスのデストラクタ
 * @return なし
 * @sa
 * @detail なんもしてない　ここでdllを閉じると不具合が起こる
 */
mecabif::~mecabif()
{   //CloseMecabDll();
}


/**
 * @fn
 * 辞書ビルドオプション文字列の生成
 * @brief 辞書ビルドオプション文字列の生成
 * @return なし
 * @sa
 * @detail 辞書ビルドオプション（辞書フォルダー設定など）文字列の生成
 */
mecab_model_t *mecabif::mecab_model_new_build_option()
{	char *ppArgv[16] = { NULL };
//	static char *ppArgv[] = { "", "-d", "D:\\Mytool\\mecab-0.996\\work\\final\\", 0, 0 };
    char szSysDic[_MAX_PATH] = { NULL };
    char szUsrDic[_MAX_PATH] = { NULL };
    int iIdxArg = 0;
    char *p;
//  QString qsBuildOption;

//    qDebug() << QString("*pprof->mecab.pwszSystemDictionaryFolder::%1").arg(*( pprof->mecab.pwszSystemDictionaryFolder));
    qDebug() << QString("*pprof->mecab.pwszSystemDictionaryFolder::%1").arg(*pprof->mecab.pwszSystemDictionaryFolder);
    qDebug() << QString("*pprof->mecab.pwszSystemDictionaryFolder->toLocal8Bit()::%1").arg(pprof->mecab.pwszSystemDictionaryFolder->toUtf8());
    ppArgv[iIdxArg++] = ( char * )"";
    strcpy_s( szSysDic, sizeof( szSysDic ), pprof->mecab.pwszSystemDictionaryFolder->toUtf8().toStdString().c_str());
    strcpy_s( szUsrDic, sizeof( szUsrDic ), szSysDic );
    if (( p = strstr( szUsrDic, "work\\" )) != NULL )
    {	strcpy_s( p, sizeof( szUsrDic ) - ( p - szUsrDic ), "etc\\mecabrc" );
        ppArgv[iIdxArg++] = ( char * )"-r";
        ppArgv[iIdxArg++] = szUsrDic;
//		ppArgv[iIdxArg++] = "D:\\Tools\\MeCabon\\mecab-0.996\\etc\\mecabrc";
    }
    if ( !( *pprof->mecab.pwszSystemDictionaryFolder ).isEmpty())
    {	ppArgv[iIdxArg++] = ( char * )"-d";
//		UniToUTF8(( char * )szSysDictionary, szSysDic, sizeof( szSysDic ) - 1 );
        ppArgv[iIdxArg++] = szSysDic;
    }
    if ( !pprof->mecab.pwszUserDictionaryPath->isEmpty())
    {	szUsrDic[0] = '\0';
        ppArgv[iIdxArg++] = ( char * )"-u";
        strcpy_s( szUsrDic, sizeof( szUsrDic ), pprof->mecab.pwszUserDictionaryPath->toUtf8().toStdString().c_str());
        ppArgv[iIdxArg++] = szUsrDic;
    }
    qDebug() << QString("mecab_model_new_build_option()");
    return ( fpmecab_model_new( iIdxArg, ppArgv ));
}


/**
 * @fn
 * 辞書ビルド時にlibMecab64.Dllをリンクする
 * @brief 辞書ビルド時にlibMecab64.Dllをリンクする
 * @return なし
 * @sa
 * @detail 辞書ビルド時にlibMecab64.Dllを切り離し辞書ファイルのロックを外し、更新可能にするため、この関数でlibMecab64.dllをリンク
 */
HINSTANCE mecabif::OpenMecabDll()
{	if ( hMecabDLL == NULL )
    {   //	wchar_t	szlibMeCabdll[_MAX_PATH];

//		wcscpy_s( szlibMeCabdll, lengthof( szlibMeCabdll ), szMeCabExe );	// インストールされているDLLを使おうとしたが
//		exchgFileName( szlibMeCabdll, szlibMeCabdll, L"libmecab.dll" );		// 64bitアプリから32bitDLL呼び出しとなりエラー
//		if (( hMecabDLL = LoadLibrary( szlibMeCabdll )) == NULL )
//        if (( hMecabDLL = LoadLibrary( L"libmecab.dll" )) == NULL )
        if (( hMecabDLL = LoadLibrary( L"libmecab64.dll" )) == NULL )
        {	mecabDllLastError = -101;
//			mecabDllLastError = GetLastError();		// error code 193 32bitlib ng
            return ( NULL );
        }
        if (( fpmecab_model_new = ( LPmecab_model_new )GetProcAddress( hMecabDLL, "mecab_model_new" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -102;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
#if 0									// 常数なのか？取得できない
        if (( fpgetLastError = ( LPgetLastError )GetProcAddress( hMecabDLL, "MeCab::getLastError" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -103;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
#endif
        if (( fpmecab_model_new_tagger = ( LPmecab_model_new_tagger )GetProcAddress( hMecabDLL, "mecab_model_new_tagger" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -104;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_dictionary_info = ( LPmecab_dictionary_info )GetProcAddress( hMecabDLL, "mecab_dictionary_info" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -105;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_model_new_lattice = ( LPmecab_model_new_lattice )GetProcAddress( hMecabDLL, "mecab_model_new_lattice" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -106;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_destroy = ( LPmecab_destroy )GetProcAddress( hMecabDLL, "mecab_destroy" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -107;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_lattice_set_sentence = ( LPmecab_lattice_set_sentence )GetProcAddress( hMecabDLL, "mecab_lattice_set_sentence" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -108;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_parse_lattice = ( LPmecab_parse_lattice )GetProcAddress( hMecabDLL, "mecab_parse_lattice" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -109;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_lattice_get_bos_node = ( LPmecab_lattice_get_bos_node )GetProcAddress( hMecabDLL, "mecab_lattice_get_bos_node" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -110;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_lattice_destroy = ( LPmecab_lattice_destroy )GetProcAddress( hMecabDLL, "mecab_lattice_destroy" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -111;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_model_destroy = ( LPmecab_model_destroy )GetProcAddress( hMecabDLL, "mecab_model_destroy" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -112;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_lattice_tostr = ( LPmecab_lattice_tostr )GetProcAddress( hMecabDLL, "mecab_lattice_tostr" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -113;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_lattice_set_boundary_constraint = ( LPmecab_lattice_set_boundary_constraint )GetProcAddress( hMecabDLL, "mecab_lattice_set_boundary_constraint" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -114;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpmecab_version = ( LPmecab_version )GetProcAddress( hMecabDLL, "mecab_version" )) == NULL )
        {	CloseMecabDll();
            mecabDllLastError = -115;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
    }	}
//    pSarchCsv->PostMessageW( UM_STATE_CHANGE, PlayState, 0 );
    return ( hMecabDLL );
}


/**
 * @fn
 * 辞書ビルド時にlibMecab64.Dllを切り離す
 * @brief 辞書ビルド時にlibMecab64.Dllを切り離す
 * @return なし
 * @sa
 * @detail 辞書ビルド時にlibMecab64.Dllを切り離す
 */
void mecabif::CloseMecabDll()
{   DWORD lasterr = 0;

    if ( hMecabDLL != NULL )
    {   fpmecab_lattice_destroy( lattice );
        fpmecab_destroy( mecab );
        fpmecab_model_destroy( model );
        if ( FreeLibrary( hMecabDLL ) == 0 )
        {   lasterr = GetLastError();
            qDebug() << QString("mecabif::FreeLibrary::error code:%1").arg( lasterr );
        }
        hMecabDLL = NULL;
    }
//    if ( pSarchCsv != 0 )
//    {	if ( pSarchCsv->m_hWnd != 0 )
//        {	pSarchCsv->PostMessageW( UM_STATE_CHANGE, PlayState, 0 );
//    }	}
}


/**
 * @fn
 * libMecab64.Dllを開いているかどうかを返す
 * @brief libMecab64.Dllを開いているかどうかを返す
 * @return true:開いている / false:開いていない
 * @sa
 * @detail libMecab64.Dllを開いているかどうかを返す
 */
BOOL mecabif::isOpenMecab()
{	return ( hMecabDLL != NULL );
}


/**
 * @fn
 * libMecab64.Dllで起こった最終エラーコードを返す
 * @brief libMecab64.Dllで起こった最終エラーコードを返す
 * @return 最終エラーコード
 * @sa
 * @detail libMecab64.Dllで起こった最終エラーコードを返す
 */
long mecabif::getmecabDllLastError()
{	return ( mecabDllLastError );
}


/**
 * @fn
 * libMecab64.Dllの初期化
 * @brief libMecab64.Dllの初期化
 * @return 非ゼロ:最終エラーコード　0:エラーなし
 * @sa
 * @detail libMecab64.Dllの初期化
 */
long mecabif::initial_dlls()
{   messageBuff msgBuff;

//    qDebug() << QString("initial_dlls() 001");
    OkMecab = FALSE;
    if ( OpenMecabDll() == NULL )
    {   msgBuff.enterMessage( "libmecab64.dllが開けません。" );            //
        return ( getmecabDllLastError());
    }
//    qDebug() << QString("initial_dlls() 002");
    if (( model = mecab_model_new_build_option()) == NULL )
    {   CloseMecabDll();
        msgBuff.enterMessage( "辞書設定が違うかもしれません。" );            //
        return ( -1 );
    }
//    qDebug() << QString("initial_dlls() 003");
    if (( mecab = fpmecab_model_new_tagger( model )) == NULL )
    {   CloseMecabDll();
        msgBuff.enterMessage( "mecab_model_new_taggerエラー" );            //
        return ( -1 );
    }
//    qDebug() << QString("initial_dlls() 004");
    const mecab_dictionary_info_t *pDict = fpmecab_dictionary_info( mecab );
    /*
struct mecab_dictionary_info_t {
  const char                     *filename;		// On Windows, filename is stored in UTF-8 encoding
  const char                     *charset;		// character set of the dictionary. e.g., "SHIFT-JIS", "UTF-8"
  unsigned int                    size;			// How many words are registered in this dictionary.
  int                             type;			// this value should be MECAB_USR_DIC, MECAB_SYS_DIC, or MECAB_UNK_DIC.
  unsigned int                    lsize;		// left attributes size
  unsigned int                    rsize;		// right attributes size
  unsigned short                  version;		// version of this dictionary
  struct mecab_dictionary_info_t *next;			// pointer to the next dictionary info.
};
*/
    if (( lattice = fpmecab_model_new_lattice( model )) == NULL )
    {   CloseMecabDll();
        fpmecab_destroy( mecab );
        msgBuff.enterMessage( "mecab_model_new_latticeエラー" );            //
        return ( -1 );
    }
//    qDebug() << QString("initial_dlls() 005");
    OkMecab = TRUE;
    return ( 0 );
}


/**
 * @fn
 * JIS2004追加漢字の置換を行う
 * @brief JIS2004追加漢字の置換を行う
 * @return 非ゼロ:最終エラーコード　0:エラーなし
 * @param (*pline) ソース文字列
 * @sa
 * @detail JIS2004追加漢字の置換を行う　下記１０文字
 * 叱叱　屏屏　并并　倶俱　剥剝　呑吞　嘘噓　妍姸　痩瘦　繋繫
 * JIS 4f54　7e7b　7e7c
 */
void mecabif::xchgJIS2004( QString *pline )
{	static QString unsafe[][2] =
    {   {	"俱",	"倶"	},        {	"剝",	"剥"	},
        {	"吞",	"呑"	},        {	"噓",	"嘘"	},
        {	"姸",	"妍"	},        {	"瘦",	"痩"	},
        {	"繫",	"繋"	}
    };	//辞書未定義？
    int no;

    for ( no = 0; no < lengthof( unsafe ); no++ )
    {   pline->replace( unsafe[no][0], unsafe[no][1]);
    }
}


/**
 * @fn
 * 引数文字列を分かち書き（コーパス）化する
 * @brief 引数文字列を分かち書き（コーパス）化する
 * @return 非ゼロ:最終エラーコード　0:エラーなし
 * @param (str) ソース文字列
 * @param (&dst) 分かち書き化文字列
 * @sa
 * @detail 引数文字列を分かち書き（コーパス）化する
 */
int mecabif::str2Corpas( QString str, QString &dst )
{   CMultiLang cml;
    char *pszOrg;
    char bufUTF8[MAX_LINE]; //, szOutput[MAX_LINE * 2];
    static mecab_model_t                *model;
    static mecab_t                      *mecab;
    static mecab_lattice_t              *lattice;
//    static char *szDaitai = "????,????,*,*,*,*,*";
//    static mecab_model_t *model;
//    static mecab_t *mecab;
//    static mecab_lattice_t *lattice;
// 	const mecab_node_t *node;

    if ( OpenMecabDll() == NULL )
    {	return ( -1 );
    }
    if (( model = mecab_model_new_build_option()) == NULL )
    {	//CString errorMessage = ( LPCTSTR )MeCab::getLastError();
        return ( -2 );
    }
    if (( mecab = fpmecab_model_new_tagger( model )) == NULL )
    {	return ( -3 );
    }
    if (( lattice = fpmecab_model_new_lattice( model)) == NULL )
    {	fpmecab_destroy( mecab );
        return ( -4 );
    }
    if ( str.length() * 3 > lengthof( bufUTF8 ))
    {	fpmecab_destroy( mecab );
        return ( -5 );
    }
    xchgJIS2004( &str );					// JIS2004追加漢字を以前の漢字に置き換え
    memset( bufUTF8, 0, sizeof( bufUTF8 ));
//    wcscpy(( wchar_t * )szOutput, ( wchar_t * )str.toUtf8().toStdString().c_str());
//    wcscat(( wchar_t * )szOutput, L"\n" );
    wcscpy(( wchar_t * )bufUTF8, ( wchar_t * )str.toUtf8().toStdString().c_str());
//    UniToUTF8(( char * )str.toStdU16String().c_str(), &( bufUTF8[0]), sizeof( bufUTF8 ));
    if (( pszOrg = _strdup( bufUTF8 )) == NULL )
    {	return ( -6 );
    }
    fpmecab_lattice_set_sentence( lattice, pszOrg );
    fpmecab_parse_lattice( mecab, lattice );
    char *pszlattice = _strdup( fpmecab_lattice_tostr( lattice ));
//    sprintf_s( szOutput, sizeof( szOutput ), "%s\n%s", bufUTF8, fpmecab_lattice_tostr( lattice ));
    auto fromUtf8 = QStringDecoder(QStringDecoder::Utf8);
    dst = str + "\n" + fromUtf8( pszlattice );      //
//    dst.fromUtf8( szOutput, strlen( szOutput ));  // 変換しない
//    dst.fromUtf8( szOutput );                     // 変換しない
//    cml.UTF8ToUni( szOutput, bufUTF8, sizeof( bufUTF8 ));
//    dst.fromWCharArray(( const wchar_t * )bufUTF8, wcslen(( wchar_t * )bufUTF8 ));
//    msgBuff.enterMessage( str );    // bufUTF8
    if ( pszlattice != NULL )	free( pszlattice );
    if ( pszOrg != NULL )	free( pszOrg );
    fpmecab_destroy( mecab );
    fpmecab_lattice_destroy( lattice );
    fpmecab_model_destroy( model );
    //	CloseMecabDll();                            // クローズしてはいけない
    return ( 0 );
}

