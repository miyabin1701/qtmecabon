/**
 * @file playlinesapi.cpp
 * @brief sapiの１行トーク処理
 * @author m.kawakami
 * @date 23/10/??
 */

#include "profilevalues.h"
#include "playlinesapi.h"
#include "portaudioif.h"
#include "cmultilang.h"
#include <io.h>
#include <mmsystem.h>
#include <string.h>
#include "soundplaystate.h"
#include "waveque.h"
#include "ttsthred.h"
#include "talktext.h"

extern profileValues    *pprof;


#define DEFSIZE	163840		// default wave buffer size
#define WINMMEAPI   0

wchar_t	PlayLineSapi::szVoiceVoxPath[_MAX_PATH];
BOOL PlayLineSapi::fgVoiceVox = FALSE;
//extern struct PlayLineSapi::SAPI54SETTING	sapi54;
const wchar_t *PlayLineSapi::pszRegistryEntry[] =
{	SPCAT_VOICES,
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech_OneCore\\Voices",
    L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech Server\\v11.0\\Voices",
    NULL
};
PROCESS_INFORMATION	PlayLineSapi::VoiceVoxProcessInfomation = { 0 };


/**
 * @fn
 * PlayLineSapiクラスのコンストラクタ
 * @brief PlayLineSapiクラスのコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail PlayLineSapiクラスのコンストラクタ
 */
PlayLineSapi::PlayLineSapi()
{
#if 0
    mecabDllLastError = 0;
    hMecabDLL = 0;				// Handle to DLL
    model = NULL;
    another_model = NULL;
    mecab = NULL;
    lattice = NULL;
#endif
//    OkMecab = FALSE;
    errorISpVoice = 0;
    fgInitialized = FALSE;
    pVoice = NULL;
    m_szWFileName[0] = EOS;
//    for ( int idx = 0; idx < lengthof( waveBuffer ); idx++ )
//    {   waveBuffer[idx].iBuffNo = idx;
//        waveBuffer[idx].bcWave = 0;
//        waveBuffer[idx].lpWave = NULL;
//    }
}


/**
 * @fn
 * PlayLineSapiクラスのデストラクタ
 * @brief PlayLineSapiクラスのデストラクタ
 * @return なし
 * @sa
 * @detail PlayLineSapiクラスのデストラクタ
 */
PlayLineSapi::~PlayLineSapi()
{   qDebug() << QString::asprintf("enter ~PlayLineSapi::finalize(%08x)", stream );
    finalize();
    qDebug() << QString::asprintf("exit ~PlayLineSapi::finalize(%08x)", stream );
}


/**
 * @fn
 * SAPI再生の終了時処理
 * @brief SAPI再生の終了時処理
 * @return なし
 * @sa
 * @detail SAPI再生の終了時処理
 */
void PlayLineSapi::finalize()
{

//    qDebug() << QString::asprintf("enter PlayLineSapi::finalize(%08x)", stream );
    mecabif::CloseMecabDll();
    portaudioif::finalize();
//    qDebug() << QString::asprintf("exit PlayLineSapi::finalize(%08x)", stream );
    if ( pVoice )
    {   __nullterminated WCHAR *pszLastBookmark;
        SPVOICESTATUS Status;

        while ( 1 )
        {	HRESULT hr = pVoice->GetStatus( &Status, &pszLastBookmark );
            if ( Status.dwRunningState != SPRS_IS_SPEAKING )
            {	break;
            }
            Sleep( 100 );
        }
        pVoice->Release();
        pVoice = NULL;
    }
}


/**
 * @fn
 * SAPI再生の初期化処理
 * @brief SAPI再生の初期化処理
 * @return なし
 * @sa
 * @detail SAPI再生の初期化処理
 */
void PlayLineSapi::initialize()
{
    if ( hMecabDLL == NULL )
    {   mecabif::initial_dlls();									// mecab & aquest & wave
    }
    if ( pVoice == NULL )
    {	CComPtr <ISpObjectToken>		cpAudioOutToken;
        CComPtr <ISpObjectToken>		cpToken;
        CComPtr <IEnumSpObjectTokens>	cpEnum;
        int i;

        if ( FAILED( ::CoInitialize( NULL )))
        {   errorISpVoice = ERROR_COINIT;
            return;
        }
        HRESULT hr = CoCreateInstance( CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, ( void ** )&pVoice );
        if ( !SUCCEEDED( hr ))		//
        {   errorISpVoice = ERROR_COCREAT;
            return;
        }
        fgVoiceVox = FALSE;
        wchar_t *pwszVoiceName = new wchar_t[pprof->sapi.pwszVoiceName->length() + 1];
        pprof->sapi.pwszVoiceName->toWCharArray( pwszVoiceName );
        for ( i = 0;  pszRegistryEntry[i] != NULL; i++ )
        {	hr = SpEnumTokens( pszRegistryEntry[i], pwszVoiceName, NULL, &cpEnum );	//Cortana voice
            if ( cpEnum != NULL )
            {	break;
        }	}
        delete [] pwszVoiceName;
        if ( cpEnum == NULL )
        {	// 指定のVoiceが見つからなかったら・・・
        }
        else if ( pprof->sapi.pwszVoiceName->contains( "VOICEVOX", Qt::CaseInsensitive ))
        {	fgVoiceVox = TRUE;
        }
        //			hr = SpEnumTokens( L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech_OneCore\\Voices", sapi54.pszVoiceName, NULL, &cpEnum );	//Cortana voice
        //			hr = SpEnumTokens( SPCAT_VOICES, L"language = 411", NULL, &cpEnum );	// 属性L "language = 411"の音声トークンを列挙します。
        //			hr = SpEnumTokens( L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech_OneCore\\Voices", L"Language = 411", NULL, &cpEnum );		//Cortana voice
        ///			hr = SpEnumTokens( L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech Server\\v11.0\\Voices", L"Language = 411", NULL, &cpEnum ); //Speech Platform v11.0 (Required runtime)
        //https://software.nzsw.net/windows-10-%E3%81%AB%E6%90%AD%E8%BC%89%E3%81%95%E3%82%8C%E3%82%8B%E9%9F%B3%E5%A3%B0%E5%90%88%E6%88%90%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6%E8%AA%BF%E3%81%B9%E3%81%A6%E3%81%BF%E3%81%9F/
                                        // SAPI default voice (OS language pack) 411:Japanese
        if ( SUCCEEDED( hr ))			// 最も一致するトークンを取得します。
        {	hr = cpEnum->Next( 1, &cpToken, NULL );
        }
        if ( SUCCEEDED( hr ))			// 音声を設定します。
        {	hr = pVoice->SetVoice( cpToken );
        }
        if ( SUCCEEDED( hr ))			// 出力をオーディオデバイスに設定します。
        {	hr = pVoice->SetOutput( cpAudioOutToken, TRUE );
        }
        hr = pVoice->SetVolume( 60 );	// 0 - 100
        if ( fgVoiceVox == TRUE )
        {
//        StartProcess( NULL, szVoiceVoxPath, NULL, &VoiceVoxProcessInfomation );
        }
    }
}


/**
 * @fn
 * SAPI生成のwaveファイルの末尾の長ーい無音期間削除
 * @brief SAPI生成のwaveファイルの末尾の長ーい無音期間削除
 * @param (sps) sample/sec
 * @param (Byteps) byte/sec
 * @param (pSrc) ソースポインタ
 * @param (*cbWave) ソースバイト長
 * @return なし
 * @sa
 * @detail SAPI生成のwaveファイルには末尾に長ーい無音期間が在りトークが途切れてしまうのでバイト数をその分減らす
 */
LPSTR PlayLineSapi::SkipMuonWave( int sps, int Byteps, LPSTR pSrc, DWORD *cbWave )
{	DWORD cw, cu;
    DWORD AftMuon;  //, BefMuon;
    WORD *pw;
    DWORD *pd;
    BYTE *pb;
    int iMuonZan;

    if ( pprof->sapi.iMuonZan >= 1000 )						// 1000 >= non cut
    {	return ( pSrc );
    }
    iMuonZan = pprof->sapi.iMuonZan;
    iMuonZan = ( iMuonZan < 1 )? 1: iMuonZan;               // 0 div escape
//	BefMuon = sps / 5;                                      // 50mSec 要調整
    AftMuon = sps / ( 1000 / iMuonZan );            		// 20 = 50mSec 要調整
    if ( Byteps == 2 )										// 16bit
    {	pw = (( WORD * )( pSrc + *cbWave )) - 1;			// 末尾ポインタ
        for ( cu = cw = *cbWave / sizeof( WORD ); *pw == 0 && cw > 0; pw-- )
        {	cw--;
        }
        if (( cu - cw ) > AftMuon )							// 末尾無音が閾値を超えていたら
        {	cw += AftMuon;
            *cbWave = cw * sizeof( WORD );					// 末尾無音を*cbwaveを減じる
        }
/*                                                          // 先頭無音のカット　ポインタの移動＋データ数減
        for ( pw = ( WORD * )pSrc, cu = cw = *cbWave / sizeof( WORD ); *pw == 0 && cw > 0; pw++ )
        {	cw--;
        }
        if (( cu - cw ) > BefMuon )							// 先頭無音が閾値を超えていたら
        {	pSrc = ( LPSTR )( pw - BefMuon );				// 先頭無音をポインタを移動する
            *cbWave -= ( cw - BefMuon ) * sizeof( WORD );	// 先頭無音を*cbwaveを減じる
        }
*/
    }
    else if ( Byteps == 4 )									// 32bit
    {	pd = ( DWORD * )( pSrc + *cbWave ) - 1;				// 末尾ポインタ
        for ( cu = cw = *cbWave / sizeof( DWORD ); *pd == 0 && cw > 0; pd-- )
        {	cw--;
        }
        if (( cu - cw ) > AftMuon )							// 末尾無音が閾値を超えていたら
        {	cw += AftMuon;
            *cbWave = cw * sizeof( DWORD );					// 末尾無音を*cbwaveを減じる
        }
/*
        for ( pd = ( DWORD * )pSrc, cu = cw = *cbWave / sizeof( DWORD ); *pd == 0 && cw > 0; pd++ )
        {	cw--;
        }
        if (( cu - cw ) > BefMuon )							// 先頭無音が閾値を超えていたら
        {	pSrc = ( LPSTR )( pd - BefMuon );				// 先頭無音をポインタを移動する
            *cbWave -= ( cw - BefMuon ) * sizeof( DWORD );	// 先頭無音を*cbwaveを減じる
        }
*/
    }
    else if ( Byteps == 1 )									// 8bit
    {	pb = ( BYTE * )( pSrc + *cbWave ) - 1;				// 末尾ポインタ
        for ( cu = cw = *cbWave / sizeof( BYTE ); *pb == 0 && cw > 0; pb-- )
        {	cw--;
        }
        if (( cu - cw ) > AftMuon )							// 末尾無音が閾値を超えていたら
        {	cw += AftMuon;
            *cbWave = cw * sizeof( BYTE );					// 末尾無音を*cbwaveを減じる
        }
/*
        for ( pb = ( BYTE * )pSrc, cu = cw = *cbWave / sizeof( BYTE ); *pb == 0 && cw > 0; pb++ )
        {	cw--;
        }
        if (( cu - cw ) > BefMuon )							// 先頭無音が閾値を超えていたら
        {	pSrc = ( LPSTR )( pb - BefMuon );				// 先頭無音をポインタを移動する
            *cbWave -= ( cw - BefMuon ) * sizeof( BYTE );	// 先頭無音を*cbwaveを減じる
        }
*/
    }
    return ( pSrc );
}


/**
 * @fn
 * ボリューム値のセット
 * @brief ボリューム値のセット
 * @param (value) ボリューム値
 * @return セットしたボリューム値
 * @sa
 * @detail ボリューム値のセット
 */
int PlayLineSapi::setVolume( int value )
{
#if 0
    if ( hWaveOut != 0 )    // アプリケーションの音量設定
    {	unsigned int val = ( value << 16 ) | ( value & 0xffff );
        if ( waveOutSetVolume( hWaveOut, val ) != MMSYSERR_NOERROR )
        {	return ( value );
    }	}
    return ( value );
#endif
    return( portaudioif::setVolume( value ));
}


/**
 * @fn
 * ボリューム値のゲット
 * @brief ボリューム値のゲット
 * @return ボリューム値
 * @sa
 * @detail ボリューム値のゲット
 */
int PlayLineSapi::getVolume()
{
#if 0
    DWORD val;

    if ( hWaveOut != 0 )
    {	if ( waveOutGetVolume( hWaveOut, &val ) == MMSYSERR_NOERROR )
        {	return ( val & 0xffff );
    }	}
   return ( 0 );
#endif
    return( portaudioif::getVolume());
}


/**
 * @fn
 * 現在のトーク行番号取得
 * @brief 現在のトーク行番号取得
 * @return 現在のトーク行番号
 * @sa
 * @detail 現在のトーク行番号取得
 */
int PlayLineSapi::currentTalkLine()
{   return ( currentLine );
}


/**
 * @fn
 * Sapiトーク文字列データからwaveデータを生成する
 * @brief Sapiトーク文字列データからwaveデータを生成する
 * @param (qsTalkData) トーク文字列構造体
 * @param (pThred) スレッドポインタ　未使用
 * @return 0:非エラー / !0:エラー
 * @sa
 * @detail トーク文字列データからwaveデータを生成する（sapiはメモリー渡し出来ないので、ファイル経由のためラムディスク等を使うのが最適）
 */
int PlayLineSapi::towav( struct TalkData qsTalkData, QThread *pThred )		// TTS毎に処理違うので
{   waveque wavque;
    soundPlayState  plyStat;
    QString numbers, bufHiraganaJis;
    QString HatuonBuff, bufShiftJis;
//    static char szSurface[MAX_LINE];
    char    xmlBuff[MAX_LINE + 1024];	// for debug
//    static char szUTF8TEST[MAX_LINE] = u8"感動詞,*";
//    static char szUNI16leTEST[MAX_LINE];
//    QString szSurface;
//    char    utf8Buff[MAX_LINE + 1024];	// for debug
    long long int filesize;
    int     idx;

    if ( currentLine <= 0 )
    {   currentLine = qsTalkData.lineNo;
    }
//    qDebug() << QString("pLine:=[%1:%2] emit selline %3").arg(qsTalkData.lineNo).arg(qsTalkData.kanaBuff).arg(currentLine);
//    if ( pThred != NULL )
//    {   (( ttsthred * )pThred )->setProcLine( currentLine );
//    }
//    emit requestSelLine(currentLine);
//        speakText( bufHiraganaJis );    // カタカナテキスト発声
//        numbers = bufHiraganaJis.toLocal8Bit();
//    strcpy( xmlBuff, bufHiraganaJis.toLocal8Bit().toStdString().c_str());
//    std::string szSiftJis = bufHiraganaJis.toLocal8Bit().toStdString();         // shiftjis化
//    std::string szSiftJis = qsTalkData.kanaBuff.toLocal8Bit().constData();         // shiftjis化toStdU16String
    std::u16string szSiftJis = qsTalkData.kanaBuff.toStdU16String();         // shiftjis化toStdU16String
//        qDebug() << QString("befor while");
    while ( pVoice && !pprof->oper.soundOutputSkip )
//        while ( !pprof->oper.soundOutputSkip )
    {	HRESULT hr;	// = pVoice->WaitUntilDone( INFINITE );
        CSpStreamFormat OriginalFmt;
        CComPtr <ISpStream> cpWavStream;
        CComPtr <ISpStreamFormat> cpOldStream;

        try
        {
//			wchar_t *m_szWFileName = L"test.wav";
//			wchar_t *m_szWFileName = ( wchar_t * )NULL;		// grobal for free std::getenv("HOME")	GetTempPath	_wtempnam
            if ( m_szWFileName[0] == EOS )
            {   //_wcscpy( m_szWFileName, L"MeCabonWav-" );             // ファイル名のプリフィックス付かない...
                //                    _wtmpnam_s( m_szWFileName, lengthof( m_szWFileName ));
                wchar_t *lptmpname = _wtempnam( NULL, L"aspiWav-" );
                wcscpy_s( m_szWFileName, lengthof( m_szWFileName ), lptmpname );
                free( lptmpname );
            }
//                qDebug() << QString("_wtmpnam_s:%1").arg(m_szWFileName);
// wavファイル結合すること、出力ストリームを作成	https://www.codetd.com/ja/article/6564768
            if (( hr = pVoice->GetOutputStream( &cpOldStream )) != S_OK )
            {	break;
            }
            if (( hr = OriginalFmt.AssignFormat( cpOldStream )) != S_OK )
            {	break;
            }
            if (( hr = SPBindToFile( m_szWFileName, SPFM_CREATE_ALWAYS, &cpWavStream,
                                   &OriginalFmt.FormatId(), OriginalFmt.WaveFormatExPtr())) != S_OK )
            {	break;		// sphelper.hを提供する機能使用してWAVファイルを作成
            }
//                qDebug() << QString("SPBindToFile");
            pVoice->SetOutput( cpWavStream, TRUE );
            pVoice->SetRate( pprof->sapi.iRate );		// 話速 -10 - +10
            pVoice->SetVolume( pprof->sapi.iVolume );	// 音量 0 - 100
//                qDebug() << QString::asprintf("%08x->SetVolume after", pVoice );
            if ( pprof->sapi.iOutputMode == 1 )			// SSML?
            {//	char xmlBuff[MAX_LINE + 1024];

//https://docs.microsoft.com/ja-jp/azure/cognitive-services/speech-service/speech-synthesis-markup?tabs=csharp
//
//	現在、 <lang xml:lang> 要素は、prosody および break 要素と互換性がありません。
//	この要素では、ピッチ、輪郭、速さ、期間、ボリュームなど、一時停止や韻律を調整することはできません。
//
//	<prosody pitch="value" contour="value" range="value" rate="value" duration="value" volume="value"></prosody>
//                  wcscpy(( wchar_t * )xmlBuff, L"<speak version=\"1.0\" xml:lang=\"ja-JP\"> <prosody pitch=\"high\"> </prosody><phoneme alphabet=\"sapi\" ph=\"" );
//NG                wcscpy(( wchar_t * )xmlBuff, L"<speak version=\"1.0\" xml:lang=\"ja-JP\"> <rate speed=\"9\"> <phoneme alphabet=\"sapi\" ph=\"" );
#if 1
                wcscpy(( wchar_t * )xmlBuff, L"<speak version=\"1.0\" xml:lang=\"ja-JP\"> <phoneme alphabet=\"sapi\" ph=\"" );
//                  wcscat(( wchar_t * )xmlBuff, ( wchar_t * )L"・アドルフィーネノソーダン" );
                wcscat(( wchar_t * )xmlBuff, ( wchar_t * )szSiftJis.c_str());
                wcscat(( wchar_t * )xmlBuff, L"\"> </phoneme> </speak>" );
                hr = pVoice->Speak(( wchar_t* )xmlBuff, SPF_ASYNC, NULL );
//                  hr = pVoice->Speak(( wchar_t* )xmlBuff, SPF_ASYNC | SPF_IS_XML, NULL );
#endif
//NG			hr = pVoice->Speak( L"<pron sym=\"アイウエオ\">あいうえお</pron>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<pron sym=\"ムヘンカン\">無変換</pron>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<p><pron sym=\"ムヘンカン\"/><p>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<speak version= \"1.0\" xml:lang= \"ja-JP\" ><pron sym=\"ムヘンカン\"/>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<speak version=\"1.0\" xml:lang=\"ja-JP\"><pron sym=\"ムヘンカン\"/>", SPF_ASYNC, NULL );
//https://kunsen.net/2020/10/18/post-3343/
//OK			hr = pVoice->Speak( L"<speak version=\"1.0\" xml:lang=\"ja-JP\">関数の呼び出し元に渡す値を「<phoneme alphabet=\"sapi\" ph=\"モドリチ\">戻り値</phoneme>」と呼びます。</speak>", SPF_ASYNC, NULL );
//OK			hr = pVoice->Speak( L"<speak version=\"1.0\" xml:lang=\"ja-JP\"><phoneme alphabet=\"sapi\" ph=\"ムヘンカン\"></phoneme></speak>", SPF_ASYNC, NULL );
//NG			hr = pVoice->Speak( L"<speak version=\"1.0\" xml:lang=\"ja-JP\"><phoneme alphabet=\"sapi\" ph=\"１００００\"></phoneme></speak>", SPF_ASYNC, NULL );
//				Sleep(100);
//<pron sym="h eh 1 l ow & w er 1 l d "/>
//<pron sym="h eh 1 l ow & w er 1 l d"> hello world </pron>
//<pron sym="ムヘンカン"/>無変換<pron>
            }
            else
            {	//hr = pVoice->Speak(( wchar_t * )szSiftJis.c_str(), SPF_ASYNC, NULL );
                wcscpy(( wchar_t * )xmlBuff, ( wchar_t * )szSiftJis.c_str());
//                wcscpy(( wchar_t * )xmlBuff, ( wchar_t * )qsTalkData.kanaBuff.toStdU16String().c_str());
//                wcscpy(( wchar_t * )xmlBuff, ( wchar_t * )L"・アドルフィーネノソーダン" );
//                qDebug() << QString::asprintf("%08x->Speak befor", pVoice );
                hr = pVoice->Speak(( wchar_t * )xmlBuff, SPF_ASYNC, NULL );
            }
//            qDebug() << QString::asprintf("%08x->Speak after", pVoice );
//            if ( !pVoice->WaitUntilDone( INFINITE ))        // パイプ出力にすると返ってこない
            if ( !pVoice->WaitUntilDone( 5000 ))			// 5秒で戻らなければエラー パイプ出力にすると返ってこない
            {   ;
            }
//                qDebug() << QString("WaitUntilDone");
            cpWavStream.Release();                          // 元のストリームに再配置//出力
//                qDebug() << QString("Release");
            pVoice->SetOutput( cpOldStream, FALSE );
//                qDebug() << QString("SetOutput");
            if ( hr != S_OK )		// SPERR_UNSUPPORTED_PHONEME	0x8004507A
            {   bufUTF16 += "\n";
//                    qDebug() << QString("hr != S_OK");
//                    pCorpasDlg->OnDsplayOutputDlg();
//                    pSarchCsv->CatUnnounStr( bufUTF16 );
//                    emit displayUnnounStr( bufUTF16 );
                msgBuff.enterMessage( bufUTF16 );
//                    qDebug() << QString("enterMessage:%1").arg(bufUTF16);
                if ( pprof->sapi.iOutputMode == 1 )				// SSML?
                {	// pSarchCsv->CatUnnounStr(( wchar_t * )xmlBuff );	// for debug
//                        emit displayUnnounStr( xmlBuff );
//                        qDebug() << QString("enterMessage:%1").arg(xmlBuff);
                    msgBuff.enterMessage( xmlBuff );
                }
                break;
            }
//                qDebug() << QString("SetOutput");
            FILE *fp;
//              if (( fp = _wfopen( m_szWFileName, L"rb" )) == NULL )
            if (( fp = _wfopen( m_szWFileName, L"rbD" )) == NULL )
            {	break;
            }
            filesize = _filelengthi64( _fileno( fp ));			//	ファイルサイズ取得
#if WINMMEAPI
            while ( 1 )
            {	EnterCriticalSection( &cstPlayRec );
                i = ctPlayRec;
                LeaveCriticalSection( &cstPlayRec );
                if ( i < lengthof( waveBuffer ))
                {	break;
                }
                Sleep( 50 );
            }
#else
            while ( emptyWaveData() == FALSE )
            {    QThread::msleep( 50 );
            }
#endif
//                qDebug() << QString("filesize:=%1:%2:%3").arg(filesize).arg(queWaveBuffer.count()).arg(lengthof( waveBuffer ));
            while ( queWaveBuffer.count() >= lengthof( waveBuffer ))    // que full?
            {
//                    qDebug() << QString("filesize:=%1:%2").arg(queWaveBuffer.count()).arg(lengthof( waveBuffer ));
                reStart();
//                if ( pThred != NULL )
//                {   (( ttsthred * )pThred )->setProcLine( currentLine );
//                }
//                emit requestSelLine(currentLine);
//                qDebug() << QString("emit selline %1").arg(currentLine);
                if ( plyStat.getPlayState() == STOP )
                {    return ( 0 );
                }
                QThread::msleep( 250 );
            }
            idx = ( queWaveBuffer.empty())? 0: ( queWaveBuffer.head().iBuffNo + 1 ) % lengthof( waveBuffer );
//                if ( queWaveBuffer.empty())
//                {   idx = 0;
//                }
//                else
//                {   idx = ( queWaveBuffer.head().iBuffNo + 1 ) % lengthof( waveBuffer );
//                }
            if ( waveBuffer[idx].bcWave < filesize )
            {	waveBuffer[idx].bcWave = filesize * 12 / 10;            // ２割増し
//                    qDebug() << QString("waveBuffer[%1].bcWave:=%2").arg(idx).arg(waveBuffer[idx].bcWave);
                if ( waveBuffer[idx].bcWave < DEFSIZE )                 // 最低サイズ未満なら最低サイズに
                {	waveBuffer[idx].bcWave = DEFSIZE;
                }
                if ( waveBuffer[idx].lpWave != NULL )                   // ヒープ確保済みなら解放
                {	// waveOutUnprepareHeader( hWaveOut, &whdr[ixPlayRec], sizeof( WAVEHDR ));
//                        qDebug() << QString::asprintf("befor free waveBuffer[%d]%08x(%d)", idx, waveBuffer[idx].lpWave, waveBuffer[idx].bcWave );
                    free( waveBuffer[idx].lpWave );
                    waveBuffer[idx].lpWave = NULL;
                }
                if (( waveBuffer[idx].lpWave = ( LPBYTE )calloc( waveBuffer[idx].bcWave, sizeof( char ))) == NULL )
                {   fclose( fp );
                    break;
                }
//                    qDebug() << QString::asprintf("after calloc waveBuffer[%d]%08x(%d)", idx, waveBuffer[idx].lpWave, waveBuffer[idx].bcWave );
                waveBuffer[idx].whdr.lpData = ( LPSTR )waveBuffer[idx].lpWave;
//      			waveBuffer[idx].whdr.dwBufferLength = size;
                waveBuffer[idx].whdr.dwBufferLength = waveBuffer[idx].bcWave;
                waveBuffer[idx].whdr.dwFlags = 0;
/* flags for dwFlags field of WAVEHDR */
//#define WHDR_DONE       0x00000001  /* done bit */
//#define WHDR_PREPARED   0x00000002  /* set if this header has been prepared */
//#define WHDR_BEGINLOOP  0x00000004  /* loop start block */
//#define WHDR_ENDLOOP    0x00000008  /* loop end block */
//#define WHDR_INQUEUE    0x00000010  /* reserved for driver */
                waveBuffer[idx].whdr.dwLoops = 0;
//                  waveOutPrepareHeader( hWaveOut, &waveBuffer[idx].whdr, sizeof( waveBuffer[idx].whdr ));
            }
            LPBYTE pwave = waveBuffer[idx].lpWave;
//                qDebug() << QString::asprintf("waveBuffer[%08x]", pwave );
            fread( waveBuffer[idx].lpWave, sizeof( char ), filesize, fp );
            fclose( fp );
            if ( strncmp(( const char * )waveBuffer[idx].lpWave, "RIFF", 4 ) != 0 )
            {	break;
            }
            if ( strncmp(( const char * )waveBuffer[idx].lpWave + 8, "WAVEfmt ", 8 ) != 0 )
            {	break;
            }
            int sps = *( DWORD * )( waveBuffer[idx].lpWave + 24 );		// nSamplePerSec
            int Byteps = *( WORD * )( waveBuffer[idx].lpWave + 32 );	// nBlockAlign
            int ofs = *( WORD * )( waveBuffer[idx].lpWave + 16 ) + 20;
            if ( strncmp(( const char * )waveBuffer[idx].lpWave + ofs, "data", 4 ) != 0 )
            {	break;
            }
            if ( pprof->oper.ttsSelector != TTSNAME::SAPI54 )
            {	waveFainalize();
            }
            if ( StateWaveOutOpen != WFSAPI )	// WaveOutOpen
            {
// 0x01:PCM
// 0x06:G.711 A-law
// 0x07:G.711 μ-law
#if 0
                MMRESULT retstate;

                    typedef struct wavehdr_tag {
                        LPSTR       lpData;                 /* pointer to locked data buffer */
                        DWORD       dwBufferLength;         /* length of data buffer */
                        DWORD       dwBytesRecorded;        /* used for input only */
                        DWORD_PTR   dwUser;                 /* for client's use */
                        DWORD       dwFlags;                /* assorted flags (see defines) */
                        DWORD       dwLoops;                /* loop control counter */
                        struct wavehdr_tag FAR *lpNext;     /* reserved for driver */
                        DWORD_PTR   reserved;               /* reserved for driver */
                    } WAVEHDR, *PWAVEHDR, NEAR *NPWAVEHDR, FAR *LPWAVEHDR;
#endif
                waveBuffer[idx].wfe.wFormatTag		= *( WORD * )( waveBuffer[idx].lpWave + 20 );
                waveBuffer[idx].wfe.nChannels		= *( WORD * )( waveBuffer[idx].lpWave + 22 );// nChannels									//モノラル
                waveBuffer[idx].wfe.wBitsPerSample	= *( WORD * )( waveBuffer[idx].lpWave + 34 );//量子化ビット数
                waveBuffer[idx].wfe.nBlockAlign 	= *( WORD * )( waveBuffer[idx].lpWave + 32 );
                waveBuffer[idx].wfe.nSamplesPerSec	= sps;									//標本化周波数
                waveBuffer[idx].wfe.nAvgBytesPerSec	= wfe.nSamplesPerSec * wfe.nBlockAlign;
//                    qDebug() << QString("nSamplesPerSec:=%1").arg(waveBuffer[idx].wfe.nSamplesPerSec);
#if WINMMEAPI
                if (( retstate = waveOutOpen( &hWaveOut, WAVE_MAPPER, &( waveBuffer[idx].wfe ), ( DWORD_PTR )text2wave::StaticWaveOutProc, ( DWORD_PTR )NULL, CALLBACK_FUNCTION )) != MMSYSERR_NOERROR )
                {	return ( retstate );
                }
#endif
                StateWaveOutOpen = WFSAPI;
            }
            waveBuffer[idx].iLineNo = qsTalkData.lineNo;
            waveBuffer[idx].allocType = mallocType::SAPI;
            waveBuffer[idx].pVvox = NULL;
            waveBuffer[idx].wfe.nChannels = *( WORD * )( waveBuffer[idx].lpWave + 22 );// nChannels   //モノラル
            if ( waveBuffer[idx].wfe.nChannels <= 0 )
            {   qDebug() << QString("nChannels:=%1").arg(waveBuffer[idx].wfe.nChannels);
                waveBuffer[idx].wfe.nChannels = 1;
            }
            char *lpwave = ( char * )( waveBuffer[idx].lpWave + ofs + 8 );
            DWORD cbWave = *( DWORD * )( waveBuffer[idx].lpWave + ofs + 4 );
//                qDebug() << QString("cbWave:=%1:%2").arg(cbWave).arg(lpwave);
#if WINMMEAPI
            waveOutUnprepareHeader( hWaveOut, &waveBuffer[idx].whdr, sizeof( WAVEHDR ));
#endif
//              memset( lpWave[ixPlayRec], 0, bcWave[ixPlayRec]);
            waveBuffer[idx].whdr.lpData = SkipMuonWave( sps, Byteps, ( LPSTR )waveBuffer[idx].lpWave + ofs + 8, &cbWave );
//                waveBuffer[idx].whdr.lpData = ( LPSTR )waveBuffer[idx].lpWave + ofs + 8;
            waveBuffer[idx].whdr.dwBufferLength = ( DWORD )cbWave;
//                qDebug() << QString("cbWave-SkipMuonWave:=%1").arg(cbWave);
#if WINMMEAPI
            waveOutPrepareHeader( hWaveOut, &waveBuffer[idx].whdr, sizeof( WAVEHDR ));
            waveOutWrite( hWaveOut, &waveBuffer[idx].whdr, sizeof( WAVEHDR ));
            ixPlayRec = ( ixPlayRec + 1 ) % lengthof( waveBuffer );
            EnterCriticalSection( &cstPlayRec );
            ctPlayRec++;
            LeaveCriticalSection( &cstPlayRec );
#else
//                while ( 1 )
//                {
//            if ( pThred != NULL )
//            {   (( ttsthred * )pThred )->setProcLine( currentLine );
//            }
//            emit requestSelLine(currentLine);
            qDebug() << QString("emit selline %1").arg(currentLine);
            wavque.queWaveBuffer.enqueue( waveBuffer[idx]);
//            queWaveBuffer.enqueue( waveBuffer[idx]);
//                    while ( queWaveBuffer.count() >= lengthof( waveBuffer ))    // que full?
//                    {
                        reStart();
//                        Sleep( 50 );
//                    }
//                }
#endif
            break;
        }	catch( ... )
        {	continue;
        }
    }
    numbers.clear();
    bufHiraganaJis.clear();
#if 0
    if ( pVoice )
    {	SPVOICESTATUS Status;
        __nullterminated WCHAR *pszLastBookmark;

        while ( 1 )
        {	HRESULT hr = pVoice->GetStatus( &Status, &pszLastBookmark );
            if (( Status.ulLastStreamQueued - Status.ulCurrentStream ) < 10 )
            {	break;
            }
            Sleep( 100 );
    }	}
#endif
    return ( 0 );
}
