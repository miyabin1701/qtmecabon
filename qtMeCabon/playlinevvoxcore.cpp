/**
 * @file playlinevvoxcore.cpp
 * @brief １行のテキストをvoicevox-coreでスピーチする
 * @author m.kawakami
 * @date 23/10/??
 */

#include "playlinevvoxcore.h"
#include "profilevalues.h"
#include "portaudioif.h"
#include "cmultilang.h"
#include <io.h>
#include <mmsystem.h>
#include <string.h>
//#include <codecvt>
#include <QThread>
#include "ttsthred.h"
#include <QMessageBox>
#include <QFile>
#include <string.h>
#include "talktext.h"

extern profileValues    *pprof;
HINSTANCE   PlayLineVvoxCore::hVvoxCoreDll = NULL;
int         PlayLineVvoxCore::ctVvoxCoreLoad = 0;

//#define VOICEVOXCOREDLL_PRELOAD 1
#ifdef VOICEVOXCOREDLL_PRELOAD
#pragma comment(lib, "voicevox_core.lib")
#else
LPvoicevox_make_default_initialize_options   PlayLineVvoxCore::fpvoicevox_make_default_initialize_options    = NULL;
LPvoicevox_initialize                        PlayLineVvoxCore::fpvoicevox_initialize    = NULL;
LPvoicevox_make_default_tts_options          PlayLineVvoxCore::fpvoicevox_make_default_tts_options    = NULL;
LPvoicevox_tts                               PlayLineVvoxCore::fpvoicevox_tts    = NULL;
LPvoicevox_wav_free                          PlayLineVvoxCore::fpvoicevox_wav_free    = NULL;
LPvoicevox_load_model                        PlayLineVvoxCore::fpvoicevox_load_model    = NULL;
LPvoicevox_is_model_loaded                   PlayLineVvoxCore::fpvoicevox_is_model_loaded    = NULL;
LPvoicevox_get_version                       PlayLineVvoxCore::fpvoicevox_get_version    = NULL;
LPvoicevox_finalize                          PlayLineVvoxCore::fpvoicevox_finalize    = NULL;

#define voicevox_make_default_initialize_options    fpvoicevox_make_default_initialize_options
#define voicevox_initialize         fpvoicevox_initialize
#define voicevox_make_default_tts_options           fpvoicevox_make_default_tts_options
#define voicevox_tts                fpvoicevox_tts
#define voicevox_wav_free           fpvoicevox_wav_free
#define voicevox_load_model         fpvoicevox_load_model
#define voicevox_is_model_loaded    fpvoicevox_is_model_loaded
#define voicevox_get_version        fpvoicevox_get_version
#define voicevox_finalize           fpvoicevox_finalize

#endif


/**
 * @fn
 * PlayLineVvoxCoreクラスのコンストラクタ
 * @brief PlayLineVvoxCoreクラスのコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail PlayLineVvoxCoreクラスのコンストラクタ
 */
PlayLineVvoxCore::PlayLineVvoxCore()
{
//    hVvoxCoreDll = NULL;
//    pprof->oper.iAppVolume = value;
//    initialize();
    dictdir = NULL;
}


/**
 * @fn
 * PlayLineVvoxCoreクラスのデストラクタ
 * @brief PlayLineVvoxCoreクラスのデストラクタ
 * @return なし
 * @sa
 * @detail PlayLineVvoxCoreクラスのデストラクタ
 */
PlayLineVvoxCore::~PlayLineVvoxCore()
{
    if ( dictdir != NULL )
    {   free(( void * )dictdir );
    }
    PlayLineVvoxCore::finalize();
    portaudioif::finalize();
//  voicevox_synthesizer_delete( synthesizer );
    unLoadVvoxCoreDll();
}


/**
 * @fn
 * PlayLineVvoxCoreクラスの後始末
 * @brief PlayLineVvoxCoreクラスの後始末
 * @return なし
 * @sa
 * @detail PlayLineVvoxCoreクラスの後始末
 */
void PlayLineVvoxCore::finalize()
{   if ( hVvoxCoreDll != NULL )
//    if ( ctVvoxCoreLoad > 0 )
    {   //ctVvoxCoreLoad--;
        voicevox_finalize();
//        qDebug() << QString::asprintf("enter PlayLineVvoxCore::finalize(%08x)", stream );
//      mecabif::CloseMecabDll();
//        portaudioif::finalize();
//        qDebug() << QString::asprintf("exit PlayLineVvoxCore::finalize(%08x)", stream );
#if 0
        voicevox_synthesizer_delete( synthesizer );
#endif
    }
}


typedef void ( CALLBACK *LPvoicevox_synthesizer_delete )( void );
LPvoicevox_synthesizer_delete         fpvoicevox_synthesizer_delete    = NULL;


/**
 * @fn
 * voicevoxCore.dllのリンク
 * @brief voicevoxCore.dllのリンク
 * @return 0:エラー / 非0:dllハンドル
 * @sa
 * @detail 辞書ビルド時dllを切り離さないといけないので、この関数でdllをリンク
 */
HINSTANCE PlayLineVvoxCore::LoadVvoxCoreDll()
{   VvoxCoreLastError = 0;

#ifndef VOICEVOXCOREDLL_PRELOAD
//    VoicevoxInitializeOptions options = voicevox_make_default_initialize_options();
//    voicevox_initialize( options );
//    VoicevoxTtsOptions options = voicevox_make_default_tts_options();
//    auto result = voicevox_tts( utf8Buff, speaker_id, options, &plength, &pwav );
//    void voicevox_wav_free(uint8_t *wav);
//    VoicevoxResultCode voicevox_load_model(uint32_t speaker_id);
//    bool voicevox_is_model_loaded(uint32_t speaker_id);
//    const char *voicevox_get_version(void);
    if ( !QFile::exists( "voicevox_core.dll" ))
    {   return NULL;
    }
    if ( hVvoxCoreDll == NULL )
    {   if (( hVvoxCoreDll = LoadLibrary( L"voicevox_core.dll" )) == NULL )
        {	VvoxCoreLastError = -101;
            return ( NULL );
        }
        if (( fpvoicevox_make_default_initialize_options = ( LPvoicevox_make_default_initialize_options )
            GetProcAddress( hVvoxCoreDll, "voicevox_make_default_initialize_options" )) == NULL )
        {	unLoadPortAudioDll();
            VvoxCoreLastError = -102;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
       if (( fpvoicevox_initialize = ( LPvoicevox_initialize )
             GetProcAddress( hVvoxCoreDll, "voicevox_initialize" )) == NULL )
        {	unLoadPortAudioDll();
            VvoxCoreLastError = -102;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
         if (( fpvoicevox_make_default_tts_options = ( LPvoicevox_make_default_tts_options )
             GetProcAddress( hVvoxCoreDll, "voicevox_make_default_tts_options" )) == NULL )
        {	unLoadPortAudioDll();
            VvoxCoreLastError = -104;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpvoicevox_tts = ( LPvoicevox_tts )
             GetProcAddress( hVvoxCoreDll, "voicevox_tts" )) == NULL )
        {	unLoadPortAudioDll();
            VvoxCoreLastError = -105;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpvoicevox_wav_free = ( LPvoicevox_wav_free )
             GetProcAddress( hVvoxCoreDll, "voicevox_wav_free" )) == NULL )
        {	unLoadPortAudioDll();
            VvoxCoreLastError = -106;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpvoicevox_load_model = ( LPvoicevox_load_model )
             GetProcAddress( hVvoxCoreDll, "voicevox_load_model" )) == NULL )
        {	unLoadPortAudioDll();
            VvoxCoreLastError = -107;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpvoicevox_is_model_loaded = ( LPvoicevox_is_model_loaded )
             GetProcAddress( hVvoxCoreDll, "voicevox_is_model_loaded" )) == NULL )
        {	unLoadPortAudioDll();
            VvoxCoreLastError = -108;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpvoicevox_get_version = ( LPvoicevox_get_version )
             GetProcAddress( hVvoxCoreDll, "voicevox_get_version" )) == NULL )
        {	unLoadPortAudioDll();
            VvoxCoreLastError = -109;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpvoicevox_finalize = ( LPvoicevox_finalize )
             GetProcAddress( hVvoxCoreDll, "voicevox_finalize" )) == NULL )
        {	unLoadPortAudioDll();
            VvoxCoreLastError = -110;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
//        ctVvoxCoreLoad = 1;
    }
//    else
//    {   ctVvoxCoreLoad++;
//    }
#endif
    return ( hVvoxCoreDll );
}


/**
 * @fn
 * voicevoxCore.dllのアンロード
 * @brief voicevoxCore.dllのアンロード
 * @return なし
 * @sa
 * @detail 辞書ビルド時dllを切り離さないといけないので、この関数でdllをアンロード
 */
void PlayLineVvoxCore::unLoadVvoxCoreDll()
{   DWORD lasterr = 0;

//#ifndef VOICEVOXCOREDLL_PRELOAD
    if ( hVvoxCoreDll != NULL )
    {
//        finalize();
        voicevox_finalize();
//        if ( ctVvoxCoreLoad > 0 )
//        {   ctVvoxCoreLoad--;
//        }
//        qDebug() << QString("unLoadVvoxCoreDll::ctVvoxCoreLoad:%1").arg( ctVvoxCoreLoad );
//        if ( ctVvoxCoreLoad <= 0 )
        {   if ( FreeLibrary( hVvoxCoreDll ) == 0 )
            {   lasterr = GetLastError();
                qDebug() << QString("PlayLineVvoxCore::FreeLibrary::error code:%1").arg( lasterr );
            }
            hVvoxCoreDll = NULL;
    }    }
//#endif
}


/**
 * @fn
 * このttwの初期化
 * @brief このttwの初期化
 * @return なし
 * @sa
 * @detail このttwの初期化
 */
void PlayLineVvoxCore::initialize()
{
    LoadVvoxCoreDll();
    if ( hMecabDLL == NULL )
    {   mecabif::initial_dlls();									// mecab & aquest & wave
    }
#if 1
    VoicevoxInitializeOptions options = voicevox_make_default_initialize_options();
    dictdir = _strdup( pprof->vvox.pwszOpenJTalkDicPath->toUtf8().toStdString().c_str());
    options.open_jtalk_dict_dir = dictdir;
    options.load_all_models = false;
    voicevox_initialize( options );
#else
    VoicevoxInitializeOptions options = voicevox_make_default_initialize_options();
//    std::string dict = GetOpenJTalkDict();
    std::string dict = pprof->vvox.pwszOpenJTalkDicPath->toUtf8().toStdString();
    options.load_all_models = true;

    OpenJtalkRc* open_jtalk;
    auto result = voicevox_open_jtalk_rc_new( dict.c_str(), &open_jtalk );
    if ( result != VoicevoxResultCode::VOICEVOX_RESULT_OK )
    {   OutErrorMessage(result);
        return 0;
    }
    result = voicevox_synthesizer_new_with_initialize( open_jtalk, options, &synthesizer );
    if ( result != VoicevoxResultCode::VOICEVOX_RESULT_OK )
    {   synthesizer = NULL;
        OutErrorMessage(result);
        return 0;
    }
    voicevox_open_jtalk_rc_delete( open_jtalk );
#endif
}


/**
 * @fn
 * voicevox-coreでアロケートしたwaveバッファエリアの解放
 * @brief voicevox-coreでアロケートしたwaveバッファエリアの解放
 * @return なし
 * @sa
 * @detail voicevox-coreでアロケートしたwaveバッファエリアの解放
 */
void PlayLineVvoxCore::freeWav( uint8_t *wav )
{
#if 1
    voicevox_wav_free(wav);
#endif
}


/**
 * @fn
 * ボリューム値のセット
 * @brief ボリューム値のセット
 * @param (value) ボリューム値
 * @return ボリューム値
 * @sa
 * @detail ボリューム値のセット
 */
int PlayLineVvoxCore::setVolume( int value )
{   return( portaudioif::setVolume( value ));
}


/**
 * @fn
 * ボリューム値の取得
 * @brief ボリューム値の取得
 * @return ボリューム値
 * @sa
 * @detail ボリューム値の取得
 */
int PlayLineVvoxCore::getVolume()
{   return( portaudioif::getVolume());
}


/**
 * @fn
 * 現在のトーク行の取得
 * @brief 現在のトーク行の取得
 * @return 現在のトーク行番号
 * @sa
 * @detail 現在のトーク行の取得
 */
int PlayLineVvoxCore::currentTalkLine()
{   return ( currentLine );
}


/**
 * @fn
 * トークデータテキストからwaveデータへの変換
 * @brief トークデータテキストからwaveデータへの変換
 * @param (qsTalkData) トークデータ構造体
 * @param (pThred) スレッドポインタ
 * @return 0:非エラー / !0:エラー
 * @sa
 * @detail トークデータテキストからwaveデータへの変換
 */
int PlayLineVvoxCore::towav( struct TalkData qsTalkData, QThread *pThred )		// TTS毎に処理違うので
{   uint8_t *pwav;
    uintptr_t plength;
    int     idx;
    int     speaker_id = pprof->vvox.ispeaker_id;
    char    utf8Buff[MAX_LINE + 1024];	// for debug
    soundPlayState  plyStat;

    if ( currentLine <= 0 )
    {   currentLine = qsTalkData.lineNo;
    }
    qDebug() << QString("pLine:=[%1:%2]").arg(qsTalkData.lineNo).arg(qsTalkData.textBuff);
//    if ( pThred != NULL )
//    {   (( ttsthred * )pThred )->setProcLine( currentLine );
//    }
    while ( !pprof->oper.soundOutputSkip )
    {
        strcpy( utf8Buff, qsTalkData.textBuff.toUtf8().data());
        try
        {   if ( !voicevox_is_model_loaded( speaker_id ))
            {   auto result = voicevox_load_model( speaker_id );
                if ( result != VoicevoxResultCode::VOICEVOX_RESULT_OK )
                {   //OutErrorMessage( result );
                    return ( 0 );
            }   }
//            qDebug() << QString("PlayLineVvoxCore::towav() 002");
            VoicevoxTtsOptions options = voicevox_make_default_tts_options();
            auto result = voicevox_tts( utf8Buff, speaker_id, options, &plength, &pwav );
//            qDebug() << QString("PlayLineVvoxCore::towav() 003");
//            auto result = voicevox_synthesizer_tts( &synthesizer, utf8Line.toStdString().c_str(), speaker_id, options, &plength, &pwav );
            if ( result != VoicevoxResultCode::VOICEVOX_RESULT_OK )
            {   //OutErrorMessage( result );
//                qDebug() << QString("PlayLineVvoxCore::towav() 004");
                return ( 0 );
            }
///-----------------------------------------------------------------------------
            while ( queWaveBuffer.count() >= lengthof( waveBuffer ))    // que full?
            {   reStart();
//                if ( pThred != NULL )
//                {   (( ttsthred * )pThred )->setProcLine( currentLine );
//                }
                QThread::msleep( 500 );
                if ( plyStat.getPlayState() == STOP )
                {    return ( 0 );
                }
            }
//            qDebug() << QString("PlayLineVvoxCore::towav() 005");
            idx = ( queWaveBuffer.empty())? 0: ( queWaveBuffer.head().iBuffNo + 1 ) % lengthof( waveBuffer );
#if 0
            if ( queWaveBuffer.empty())
            {   idx = 0;
            }
            else
            {   idx = ( queWaveBuffer.head().iBuffNo + 1 ) % lengthof( waveBuffer );
            }
#endif
            waveBuffer[idx].whdr.lpData = ( char * )pwav;
            waveBuffer[idx].lpWave = ( LPBYTE )pwav;
            waveBuffer[idx].whdr.dwBufferLength = waveBuffer[idx].bcWave = plength;
            waveBuffer[idx].whdr.dwLoops = 0;
            waveBuffer[idx].whdr.dwFlags = 0;
/* flags for dwFlags field of WAVEHDR */
//#define WHDR_DONE       0x00000001  /* done bit */
//#define WHDR_PREPARED   0x00000002  /* set if this header has been prepared */
//#define WHDR_BEGINLOOP  0x00000004  /* loop start block */
//#define WHDR_ENDLOOP    0x00000008  /* loop end block */
//#define WHDR_INQUEUE    0x00000010  /* reserved for driver */
//          waveOutPrepareHeader( hWaveOut, &waveBuffer[idx].whdr, sizeof( waveBuffer[idx].whdr ));
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
            if ( pprof->oper.ttsSelector != TTSNAME::VVoxCore )
            {	waveFainalize();
            }
//            if ( StateWaveOutOpen != WFVOICEVOX )	// WaveOutOpen
            {	// MMRESULT retstate;

                waveBuffer[idx].wfe.wFormatTag		= *( WORD * )( waveBuffer[idx].lpWave + 20 );
                waveBuffer[idx].wfe.nChannels		= *( WORD * )( waveBuffer[idx].lpWave + 22 );// nChannels//モノラル
//                if ( waveBuffer[idx].wfe.nChannels == 0 )
//                {
//                    qDebug() << QString("nChannels:=%1").arg(waveBuffer[idx].wfe.nChannels);
//                }
                waveBuffer[idx].wfe.wBitsPerSample	= *( WORD * )( waveBuffer[idx].lpWave + 34 );//量子化ビット数
                waveBuffer[idx].wfe.nBlockAlign 	= *( WORD * )( waveBuffer[idx].lpWave + 32 );
                waveBuffer[idx].wfe.nSamplesPerSec	= sps;									//標本化周波数
                waveBuffer[idx].wfe.nAvgBytesPerSec	= sps * waveBuffer[idx].wfe.nBlockAlign;
//                waveBuffer[idx].wfe.nAvgBytesPerSec	= wfe.nSamplesPerSec * wfe.nBlockAlign;
//              qDebug() << QString("nSamplesPerSec:=%1").arg(waveBuffer[idx].wfe.nSamplesPerSec);
                StateWaveOutOpen = WFVOICEVOX;
            }
//            qDebug() << QString("PlayLineVvoxCore::towav() 006");
            waveBuffer[idx].iLineNo = qsTalkData.lineNo;
            waveBuffer[idx].allocType = mallocType::OPENJTALK;
            waveBuffer[idx].pVvox = this;
//           char *lpwave = ( char * )( waveBuffer[idx].lpWave + ofs + 4 );
            DWORD cbWave = *( DWORD * )( waveBuffer[idx].lpWave + ofs + 4 );
//          qDebug() << QString("cbWave:=%1:%2").arg(cbWave).arg(lpwave);
//          memset( lpWave[ixPlayRec], 0, bcWave[ixPlayRec]);
//            waveBuffer[idx].whdr.lpData = SkipMuonWave( sps, Byteps, ( LPSTR )waveBuffer[idx].lpWave + ofs + 8, &cbWave );
            waveBuffer[idx].whdr.lpData = ( LPSTR )waveBuffer[idx].lpWave + ofs + 8;
            waveBuffer[idx].whdr.dwBufferLength = ( DWORD )cbWave;
//          qDebug() << QString("cbWave-SkipMuonWave:=%1").arg(cbWave);
//            qDebug() << QString("enqueue:nChannels:=%1,Length:=%2").arg(waveBuffer[idx].wfe.nChannels).arg(waveBuffer[idx].whdr.dwBufferLength);
            if ( waveBuffer[idx].wfe.nChannels == 0 )
            {   waveBuffer[idx].wfe.nChannels = 1;
            }
//            qDebug() << QString("PlayLineVvoxCore::towav() 007");
            if (( plyStat.getPlayState() != STOP ) && ( plyStat.getPlayState() != PRESTOP ))
            {   queWaveBuffer.enqueue( waveBuffer[idx]);
            }
//            if ( pThred != NULL )
//            {   (( ttsthred * )pThred )->setProcLine( currentLine );
//            }
            break;
        }	catch( ... )
        {	continue;
        }
    }
    return ( 0 );
}


