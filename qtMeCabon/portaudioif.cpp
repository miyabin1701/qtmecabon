/**
 * @file portaudioif.cpp
 * @brief portaudio.dllのインターフェース
 * @author m.kawakami
 * @date 23/10/??
 */

#include "portaudioif.h"
#include "playlinevvoxcore.h"
#include "profilevalues.h"
//#include "portaudio.h"
#include "text2wave.h"
#include <math.h>
#include "waveque.h"

//#pragma comment(lib, "portaudio_x86.lib")
#pragma comment(lib, "portaudio_x64.lib")

#define FLOATSOUND  TRUE

HINSTANCE               portaudioif::hPortAudioDLL      = NULL;
int                     portaudioif::ctportAudioLoad    = 0;
BOOL                    portaudioif::fgportAudioStarted = FALSE;
int                     portaudioif::portAudioLastError = 0;
volatile float          portaudioif::f32paVolume        = 0.5;            // 音量
int                     portaudioif::paVolume           = 50;             // 音量

LPPa_Initialize         portaudioif::fpPa_Initialize    = NULL;
LPPa_Terminate          portaudioif::fpPa_Terminate     = NULL;
LPPa_GetDefaultOutputDevice
                        portaudioif::fpPa_GetDefaultOutputDevice = NULL;
LPPa_StartStream        portaudioif::fpPa_StartStream   = NULL;
LPPa_StopStream         portaudioif::fpPa_StopStream    = NULL;
LPPa_CloseStream        portaudioif::fpPa_CloseStream   = NULL;
LPPa_OpenStream         portaudioif::fpPa_OpenStream    = NULL;
LPPa_IsStreamStopped    portaudioif::fpPa_IsStreamStopped    = NULL;
LPPa_Sleep              portaudioif::fpPa_Sleep         = NULL;
LPPa_GetDeviceInfo      portaudioif::fpPa_GetDeviceInfo = NULL;
QRecursiveMutex         portaudioif::callBackmutex;
padata                  portaudioif::padt = { paComplete, FALSE, TRUE, 1, 1, MAXLONGLONG, 1, NULL };
volatile int            portaudioif::currentLine = -1;

extern profileValues    *pprof;
PaStream    *portaudioif::stream = NULL;


/**
 * @fn
 * portaudioifクラスのコンストラクタ
 * @brief portaudioifクラスのコンストラクタ
 * @return なし
 * @sa
 * @detail portaudioifクラスのコンストラクタ
 */
portaudioif::portaudioif()
{
    waveformat.nSamplesPerSec = 0;
    padt.callbackComplete = paComplete;
    padt.fgcallbackExit = FALSE;
    padt.fgDataMoved = TRUE;
    setVolume( pprof->oper.iAppVolume );
    LoadPortAudioDll();
    Pa_Initialize();                //PortAudio初期化
}


/**
 * @fn
 * portaudioifクラスのデストラクタ
 * @brief portaudioifクラスのデストラクタ
 * @return なし
 * @sa
 * @detail portaudioifクラスのデストラクタ
 */
portaudioif::~portaudioif()
{
    if ( stream != NULL )
    {
        for ( padt.fgcallbackExit = TRUE; padt.callbackComplete != paComplete; )
        {   Sleep( 50 );
        }
        Sleep( 2000 );              // 再生終了まで待つ
//        qDebug() << QString::asprintf("Pa_StopStream(%08x)3", stream );
        Pa_StopStream(stream);
//        qDebug() << QString::asprintf("Pa_StopStream(%08x)4", stream );
        Pa_CloseStream(stream);
//        qDebug() << QString::asprintf("Pa_CloseStream(%08x)", stream );
        Pa_Terminate();
        hPortAudioDLL = NULL;
        stream = NULL;
    }
}


/**
 * @fn
 * portaudioの初期化
 * @brief portaudioの初期化
 * @return 0
 * @sa
 * @detail portaudioの初期化
 */
int portaudioif::initialize()
{
    Pa_Initialize();            //PortAudio初期化
    LoadPortAudioDll();
    padt.fgDataMoved = TRUE;
    return ( 0 );
}


/**
 * @fn
 * portaudioifクラスの後始末
 * @brief portaudioifクラスの後始末
 * @return なし
 * @sa
 * @detail portaudioifクラスの後始末
 */
void portaudioif::finalize()
{    waveque wavque;
    soundPlayState      plyStat;

    if ( hPortAudioDLL != 0 )
    {   while ( wavque.queWaveBuffer.empty())      // キューが空でないなら
        {   Pa_Sleep( 50 );
            if ( plyStat.getPlayState() != PLAY )
            {   break;
        }   }
        for ( padt.fgcallbackExit = TRUE; padt.callbackComplete != paComplete; )
        {   Sleep( 50 );            // callbackが終了しても、再生はその後なので
        }
        if ( stream != NULL )
        {
//        Pa_IsStreamStopped( stream );
            Pa_Sleep( 2000 );           // 再生終了まで待つ
//        qDebug() << QString::asprintf("Pa_StopStream(%08x)1", stream );
            Pa_StopStream(stream);      // 再生終了まで待つらしい？　切れてるけど
//        qDebug() << QString::asprintf("Pa_StopStream(%08x)2", stream );
            Pa_CloseStream(stream);
//        qDebug() << QString::asprintf("Pa_CloseStream(%08x)", stream );
            stream = NULL;
        }
        Pa_Terminate();
        hPortAudioDLL = NULL;
    }
//    qDebug() << QString::asprintf("exit finalize(%08x)", stream );
 }


 /**
  * @fn
  * portaudioのコールバック
  * @brief portaudioのコールバック
  * @param (*inputBuffer) 入力バッファ
  * @param (*outputBuffer) 出力バッファ
  * @param (framesPerBuffer) バッファのフレーム数
  * @param (*timeInfo) タイムインフォ
  * @param (statusFlags) コールバックステータス
  * @param (*userData) ユーザ定義データ
  * @return コンプリートステータス
  * @sa
  * @detail portaudioのコールバック
  */
int portaudioif::soundOutputCallback(                   // オーディオ処理コールバック関数
    const void                      *inputBuffer,       // 入力
    void                            *outputBuffer,      // 出力
    unsigned long                   framesPerBuffer,
    const PaStreamCallbackTimeInfo  *timeInfo,
    PaStreamCallbackFlags           statusFlags,
    void                            *userData )         // ユーザ定義データ
 {  short *__ptr64 psrc;
#ifdef FLOATSOUND
    FLOAT *pdst = ( FLOAT * )outputBuffer;
#else
    short *pdst = ( WORD * )outputBuffer;
#endif
    unsigned long i, cbFrame;    //, cfsrc, rInx, wInx;
    struct WAVEBUFFER wb;
    padata *padt = ( padata * )userData;
    FLOAT   mag = f32paVolume;
    waveque wavque;
    soundPlayState      plyStat;

#if 1
    i = 0;  //
    padt->callbackComplete = paContinue;
    if ( !wavque.queWaveBuffer.empty())              // キューが空でないなら
    {   wb = wavque.queWaveBuffer.head();
        padt->fgDataMoved = FALSE;
        if ((( plyStat.getPlayState() == PLAY ) ||
             ( plyStat.getPlayState() == WAITENDOFTALK )) &&
            ( padt->index >= padt->maxFrame ))        // 新規データ
        {   PaSampleFormat fmt = paInt16;
            padt->noChanel = wb.wfe.nChannels;
            switch ( wb.wfe.wBitsPerSample )
            { case 0x0800:  fmt = paUInt8;  cbFrame = sizeof( BYTE );   break;
              case 0x1000:
              default:      fmt = paInt16;  cbFrame = sizeof( WORD );    break;
              case 0x1800:  fmt = paInt24;  cbFrame = sizeof( BYTE ) * 3;   break;
              case 0x2000:  fmt = paInt32;  cbFrame = sizeof( DWORD );   break;
            }
            if ( wb.wfe.nChannels == 0 )
            {
                qDebug() << QString("noChanel:=%1, Length:=%2, BitsPer:=%3, lpData:=%4").arg(wb.wfe.nChannels).arg(wb.whdr.dwBufferLength).arg(wb.wfe.wBitsPerSample).arg(wb.whdr.lpData);
                padt->noChanel = 1;
            }
            qDebug() << QString("currentLine:=%1").arg(currentLine);
            currentLine = wb.iLineNo;
            padt->dataformat = fmt; // pafloat32;
            padt->index = 0;
            padt->maxFrame = ( wb.whdr.dwBufferLength ) / cbFrame / padt->noChanel;
            padt->lpWave = wb.whdr.lpData;
//            padt->callbackComplete = paContinue;
            //ofs = *(( DWORD * )(( BYTE * )padt->lpWave + 16 )) + 28;
            psrc = ( short * )( wb.whdr.lpData );
        }
        else if ( padt->index < padt->maxFrame )        // 前の続き？
        {   //ofs = *(( DWORD * )(( BYTE * )padt->lpWave + 16 )) + 28;
            psrc = ( short * )( wb.whdr.lpData ) + padt->index;
        }
        if (( plyStat.getPlayState() == PLAY ) ||
            ( plyStat.getPlayState() == WAITENDOFTALK ))
        {   for ( ; i < framesPerBuffer; i++ )
            {   if ( padt->index >= padt->maxFrame )
                {   break;
                }
#ifdef FLOATSOUND
//              *pdst++ = (( float )( *psrc++ ) / 32768.0 ) * mag;
                *pdst++ = ( float )( *psrc++ ) / mag;
#else
                *pdst++ = ( *psrc++ );
#endif
                padt->index++;
//              qDebug() << QString("rInx:=%1/wInx:=%2").arg(padt->inxReadRingBuff).arg(padt->inxWriteRingBuff);
                if ( padt->noChanel == 2 )
                {
#ifdef FLOATSOUND
                    *pdst++ = ( float )( *psrc++ ) / mag;
#else
                    *pdst++ = ( *psrc++ );
#endif
//                  padt->index++;
            }   }
            if ( padt->index >= padt->maxFrame )
//          {   wb = text2wave::queWaveBuffer.dequeue();
            {   wb = wavque.queWaveBuffer.dequeue();
                if (( wb.allocType == mallocType::OPENJTALK ) && ( wb.lpWave != NULL ))
                {   wb.pVvox->freeWav(( uint8_t * )wb.lpWave );
                }
//              wb = text2wave::queWaveBuffer.head();
//              emit requestSelLine(wb.iLineNo);
            }
        }
    }
    if ( i < framesPerBuffer )
    {   if (( i <= 0 ) && ( plyStat.getPlayState() != PAUSE ))
        {   if (( wavque.queWaveBuffer.empty()) &&
                ( plyStat.getPlayState() == WAITENDOFTALK ))
            {   plyStat.setPlayState( PRESTOP );
            }
            if (( padt->fgcallbackExit ) || ( plyStat.getPlayState() == PRESTOP ))
            {    padt->callbackComplete = paComplete;
                qDebug() << QString("padt->callbackComplete:=%1").arg(padt->callbackComplete);
            }
        }
        for ( ; i < framesPerBuffer; i++ )
        {
#ifdef FLOATSOUND
            *pdst++ = 0.0;
#else
            *pdst++ = 0;
#endif
            if ( padt->noChanel == 2 )
            {
#ifdef FLOATSOUND
                *pdst++ = 0.0;
#else
                *pdst++ = 0;
#endif
        }   }
        if ( plyStat.getPlayState() == PRESTOP )
        {   while ( !wavque.queWaveBuffer.empty())          // prestopなら　キューを空にする
            {   wb = wavque.queWaveBuffer.dequeue();
        }   }
        if (( wavque.queWaveBuffer.empty()) ||              // キューが空なら
            ( plyStat.getPlayState() == PRESTOP ))
        {   padt->fgDataMoved = TRUE;
            qDebug() << QString("padt->callbackComplete:=%1").arg(padt->callbackComplete);
            return ( padt->callbackComplete );
    }   }
    return ( padt->callbackComplete = paContinue );
#endif
}


/**
 * @fn
 * portaudio.dllのリンク
 * @brief portaudio.dllのリンク
 * @return dllインスタンス
 * @sa
 * @detail この関数でportaudio.dllをリンク
 */
HINSTANCE portaudioif::LoadPortAudioDll()
{   portAudioLastError = 0;

    return ( NULL );
    if ( hPortAudioDLL == NULL )
    {   if (( hPortAudioDLL = LoadLibrary( L"portaudio_x64.dll" )) == NULL )
        {	portAudioLastError = -101;
            return ( NULL );
        }
        if (( fpPa_Initialize = ( LPPa_Initialize )GetProcAddress( hPortAudioDLL, "Pa_Initialize" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -102;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpPa_Terminate = ( LPPa_Terminate )GetProcAddress( hPortAudioDLL, "Pa_Terminate" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -104;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpPa_GetDefaultOutputDevice = ( LPPa_GetDefaultOutputDevice )GetProcAddress( hPortAudioDLL, "Pa_GetDefaultOutputDevice" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -105;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpPa_StartStream = ( LPPa_StartStream )GetProcAddress( hPortAudioDLL, "Pa_StartStream" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -106;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpPa_StopStream = ( LPPa_StopStream )GetProcAddress( hPortAudioDLL, "Pa_StopStream" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -107;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpPa_CloseStream = ( LPPa_CloseStream )GetProcAddress( hPortAudioDLL, "Pa_CloseStream" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -108;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpPa_OpenStream = ( LPPa_OpenStream )GetProcAddress( hPortAudioDLL, "Pa_OpenStream" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -109;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpPa_IsStreamStopped = ( LPPa_IsStreamStopped )GetProcAddress( hPortAudioDLL, "Pa_IsStreamStopped" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -110;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpPa_Sleep = ( LPPa_Sleep )GetProcAddress( hPortAudioDLL, "Pa_Sleep" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -111;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        if (( fpPa_GetDeviceInfo = ( LPPa_GetDeviceInfo )GetProcAddress( hPortAudioDLL, "Pa_GetDeviceInfo" )) == NULL )
        {	unLoadPortAudioDll();
            portAudioLastError = -112;
            return ( NULL );			// ERROR_DELAY_LOAD_FAILED
        }
        ctportAudioLoad = 1;
    }
    else
    {   ctportAudioLoad++;
    }
    return ( hPortAudioDLL );
}


/**
 * @fn
 * portaudio.dllのアンリンク
 * @brief portaudio.dllのアンリンク
 * @return dllインスタンス
 * @sa
 * @detail この関数でportaudio.dllをアンリンク
 */
void portaudioif::unLoadPortAudioDll()
{	if ( hPortAudioDLL != NULL )
    {   if ( ctportAudioLoad > 0 )
        {   ctportAudioLoad--;
        }
        else
        {   FreeLibrary( hPortAudioDLL );
            hPortAudioDLL = NULL;
        }
    }
}


/**
 * @fn
 * portaudioのオープン
 * @brief portaudioのオープン
 * @return dllインスタンス
 * @sa
 * @detail portaudioのオープン
 */
#if 1
int portaudioif::open()
{   PaStreamParameters outParam = { 0 };
    PaSampleFormat fmt = paInt16;
    struct WAVEBUFFER wb;
    int paState, retry; //, i, ofs, retry;
    waveque wavque;

    if ( wavque.queWaveBuffer.empty())                  // キューが空なら
    {   return 0;
    }
    int cntbfr = wavque.queWaveBuffer.count();
    if ( cntbfr <= 0 )                                  // キューが空なら
    {   return 0;
    }
    wb = wavque.queWaveBuffer.head();
    if (( waveformat.nSamplesPerSec != wb.wfe.nSamplesPerSec ) ||  // format change?
        ( waveformat.wBitsPerSample != wb.wfe.wBitsPerSample ) ||
        ( padt.callbackComplete == paComplete ))
    {   while ( padt.fgDataMoved != TRUE )                  // 発声中なら
        {   Pa_Sleep( 10 );
        }
        if ( fgportAudioStarted )
        {   paState = Pa_StopStream( stream );              // 止める
        }
        waveformat = wb.wfe;
        switch ( wb.wfe.wBitsPerSample )
        { case 0x0800:  fmt = paUInt8;  break;
          case 0x1000:
          default:      fmt = paInt16;  break;
          case 0x1800:  fmt = paInt24;  break;
          case 0x2000:  fmt = paInt32;  break;
        }
        try
        {
            outParam.channelCount = wb.wfe.nChannels;
#ifdef FLOATSOUND
            outParam.sampleFormat = paFloat32;
#else
            outParam.sampleFormat = fmt;
#endif
            outParam.device = Pa_GetDefaultOutputDevice(); // デフォルトのオーディオデバイス
            outParam.hostApiSpecificStreamInfo = NULL;
            outParam.suggestedLatency = Pa_GetDeviceInfo( outParam.device )->defaultLowOutputLatency;
        } catch ( const std::runtime_error& e )
        {   qDebug() << QString("portaudioif::open-004(%1:%2)").arg( e.what()).arg( outParam.device );
        }
        padt.noChanel = wb.wfe.nChannels;
        padt.dataformat = fmt;
        padt.index = 0;
        padt.maxFrame = ( wb.whdr.dwBufferLength ) / sizeof( WORD );
        padt.lpWave = wb.whdr.lpData;
        padt.callbackComplete = paContinue;
        for ( retry = 3; retry > 0; retry-- )
        {   paState = Pa_OpenStream(                            // PortAudioオープン
                &stream,
                NULL,
                &outParam,
                wb.wfe.nSamplesPerSec,                          // サンプリング周波数
                8192,   //16384, //4096,        // paFramesPerBufferUnspecified,   // バッファサイズ
                paClipOff,
                portaudioif::soundOutputCallback,
                &padt );
            if ( paState != paNoError )
            {   Pa_Terminate();
                continue;
            }
            paState = Pa_StartStream( stream );                 // PortAudioスタート
            if ( paState == paNoError )
            {   break;
            }
            Pa_Terminate();
        }
        if ( retry <= 0 )
        {   qDebug() << QString("Pa_StartStream:=%1").arg(paState);
    }   }
    return 0;
}
#endif


/**
 * @fn
 * portaudioのオープン
 * @brief portaudioのオープン
 * @return dllインスタンス
 * @sa
 * @detail portaudioのオープン
 */
BOOL portaudioif::emptyWaveData()
{   return ( padt.fgDataMoved );
}


/**
 * @fn
 * ボリューム値のセット
 * @brief ボリューム値のセット
 * @param (volume) ボリューム値
 * @return それまでのボリューム値
 * @sa
 * @detail ボリューム値のセット
 */
int portaudioif::setVolume( int volume )
{   int volume0 = paVolume;

    paVolume = volume;
//volume in dB 0db = unity gain, no attenuation, full amplitude signal
//           -20db = 10x attenuation, significantly more quiet
//    f32paVolume = pow(10, ( -( 100 - volume ) / 60.f));
    f32paVolume = 32768.0 / pow(10, ( -( 100 - volume ) / 60.f ));
    return ( volume0 );
}


/**
 * @fn
 * ボリューム値の取得
 * @brief ボリューム値の取得
 * @return ボリューム値
 * @sa
 * @detail ボリューム値の取得
 */
int portaudioif::getVolume()
{   setVolume( pprof->oper.iAppVolume );
    return ( paVolume );
}



