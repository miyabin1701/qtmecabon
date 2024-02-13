/**
 * @file portaudioif.h
 * @brief portaudio.dllのインターフェースのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef PORTAUDIOIF_H
#define PORTAUDIOIF_H

#include <stddef.h>
#include "portaudio\\include\\portaudio.h"
#include "portaudiodll.h"
#include <windows.h>
#include <mmsystem.h>
#include <QRecursiveMutex>
#include <QObject>

/*ユーザ定義データ*/
typedef struct
{   volatile BOOL   callbackComplete;   // コールバックで全データを渡し済み
    volatile BOOL   fgcallbackExit;     // コールバック終了フラグ　立てばcomplete
    volatile BOOL   fgDataMoved;        // コールバック終了フラグ　立てばcomplete
//    volatile WORD   unitNoWrite;        // リングバッファへ書き込んだユニット番号
//    volatile WORD   unitNoRead;         // リングバッファから読みだしたユニット番号
    WORD    noChanel;                   // チャネル数
    WORD    dataformat;                 // データフォーマット
    DWORDLONG   index;                      // 読み出しindex
    DWORD   maxFrame;                   // 最大frame
    void    *lpWave;                    // wave file image
//    void    *lpWaveRingBuff;            // wave data ring buffer top
//    DWORD   wcofRingBuff;               // word count of ring buffer
//    volatile DWORD   inxReadRingBuff;   // ring buffer read index
//    volatile DWORD   inxWriteRingBuff;  // ring buffer write index
} padata;


#if 0
#define Pa_Initialize       fpPa_Initialize
#define Pa_Terminate        fpPa_Terminate
#define Pa_GetDefaultOutputDevice fpPa_GetDefaultOutputDevice
#define Pa_StartStream      fpPa_StartStream
#define Pa_StopStream       fpPa_StopStream
#define Pa_CloseStream      fpPa_CloseStream
#define Pa_OpenStream       fpPa_OpenStream
#define Pa_IsStreamStopped  fpPa_IsStreamStopped
#define Pa_Sleep            fpPa_Sleep
#define Pa_GetDeviceInfo    fpPa_GetDeviceInfo
#endif


#if 0
typedef struct PaStreamParameters
{
    PaDeviceIndex device;
    int channelCount;
    PaSampleFormat sampleFormat;
    PaTime suggestedLatency;
    void *hostApiSpecificStreamInfo;
} PaStreamParameters;


// 0x01:PCM
// 0x06:G.711 A-law
// 0x07:G.711 μ-law
typedef struct tWAVEFORMATEX {
    WORD  wFormatTag;
    WORD  nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD  nBlockAlign;
    WORD  wBitsPerSample;
    WORD  cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *NPWAVEFORMATEX, *LPWAVEFORMATEX;
    wfe.wFormatTag		= *( WORD * )( lpWave[ixPlayRec] + 20 );
    wfe.nChannels		= *( WORD * )( lpWave[ixPlayRec] + 22 );// nChannels									//モノラル
    wfe.wBitsPerSample	= *( WORD * )( lpWave[ixPlayRec] + 34 );//量子化ビット数
    wfe.nBlockAlign		= *( WORD * )( lpWave[ixPlayRec] + 32 );
    wfe.nSamplesPerSec	= sps;									//標本化周波数
    wfe.nAvgBytesPerSec	= wfe.nSamplesPerSec * wfe.nBlockAlign;
    if (( retstate = waveOutOpen( &hWaveOut, WAVE_MAPPER, &wfe, ( DWORD_PTR )text2wave::StaticWaveOutProc, ( DWORD_PTR )NULL, CALLBACK_FUNCTION )) != MMSYSERR_NOERROR )
    {	return ( retstate );
    }
    StateWaveOutOpen = WFSAPI;

    whdr[ixPlayRec].lpData = SkipMuonWave( sps, Byteps, ( LPSTR )lpWave[ixPlayRec] + ofs + 8, &cbWave );
    whdr[ixPlayRec].dwBufferLength = ( DWORD )cbWave;
#endif


class portaudioif //: public QObject
{    //Q_OBJECT

//signals:
//    void requestSelLine(int lineno);

public:
    portaudioif();
    ~portaudioif();
    int initialize();
    void finalize();
    HINSTANCE LoadPortAudioDll();
    void unLoadPortAudioDll();
    BOOL emptyWaveData();
    int setVolume( int volume );
    int getVolume();

    int open();
//    int open( WAVEFORMATEX wavform );
//    int reStart();

//    int open( double  SampleRate, PaStreamParameters &outParam );
    static int soundOutputCallback(                         // オーディオ処理コールバック関数
        const void                      *inputBuffer,       // 入力
        void                            *outputBuffer,      // 出力
        unsigned long                   framesPerBuffer,
        const PaStreamCallbackTimeInfo  *timeInfo,
        PaStreamCallbackFlags           statusFlags,
        void                            *userData );        // ユーザ定義データ

    WAVEFORMATEX                        waveformat;
    static PaStream                     *stream;
    static volatile float           f32paVolume;            // 音量
    static int                      paVolume;               // 音量

//    double      sampleRate;
    static volatile int             currentLine;
    static padata                   padt;
    static QRecursiveMutex          callBackmutex;

    static HINSTANCE                hPortAudioDLL;
    static int                      ctportAudioLoad;
    static BOOL                     fgportAudioStarted;
    static int                      portAudioLastError;

    static LPPa_Initialize          fpPa_Initialize;
    static LPPa_Terminate           fpPa_Terminate;
    static LPPa_GetDefaultOutputDevice fpPa_GetDefaultOutputDevice;
    static LPPa_StartStream         fpPa_StartStream;
    static LPPa_StopStream          fpPa_StopStream;
    static LPPa_CloseStream         fpPa_CloseStream;
    static LPPa_OpenStream          fpPa_OpenStream;
    static LPPa_IsStreamStopped     fpPa_IsStreamStopped;
    static LPPa_Sleep               fpPa_Sleep;
    static LPPa_GetDeviceInfo       fpPa_GetDeviceInfo;

//signals:
//    void requestSelLine(int);
};

#endif // PORTAUDIOIF_H
