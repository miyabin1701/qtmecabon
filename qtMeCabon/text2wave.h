/**
 * @file text2wave.h
 * @brief 文字列の補正を行うのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef TEXT2WAVE_H
#define TEXT2WAVE_H

#include <QMainWindow>
#include <QObject>
#include <QQueue>
#include <windows.h>
#include <mmsystem.h>
#include <mmeapi.h>
#include <io.h>
#include <sapi.h>
#include <sphelper.h>
#include "mecabif.h"
#include "portaudioif.h"
#include "soundplaystate.h"
#include "waveque.h"
#include <QThread>
//#include "ttsthred.h"
//#ifndef PLAYLINEVVOXCORE_H
//#include "playlinevvoxcore.h"
//#endif

enum WAVEOUTOPENTATE {
    WFNONOPEN	= 0x00,
    WFAQTK10	= 0x01,
    WFSAPI		= 0x02,
    WFVOICEVOX	= 0x03
};


/*
enum PLAYSTATE {
    STOP	= 0x00,
    PRESTOP	= 0x01,
    PLAY	= 0x02,
    PAUSE	= 0x03,
};
*/


enum KANATYPE {
    IDHIRAKANA = 0,
    IDKATAKANA
};

/*
struct PLAYINGSENTENCE			// CEdit select char pos
{	int		iLineNo;
    int		ixs;
    int		ixe;
};
*/

/*
enum mallocType {
    SAPI        = 0x00,
    OPENJTALK,
};


struct WAVEBUFFER
{   enum mallocType     allocType;          // SAPI5.4 / open JTalk
    class PlayLineVvoxCore    *pVvox;       //
    int                 iLineNo;            // for edit window sel line
    int                 ixs;                //
    int                 ixe;                //
    int                 iBuffNo;            //
    WAVEFORMATEX        wfe;                // wave format
    WAVEHDR            	whdr;               // wave header
    long long int       bcWave;				// alloc size
    LPBYTE              lpWave;				// wave buf
};
*/

class text2wave: public QObject, public mecabif, public portaudioif, public waveque
{
    Q_OBJECT

public:
    text2wave();
    virtual ~text2wave() = 0;
//    virtual void cvtText2Wave()=0;
//    virtual int cvt2wave( QString *, struct WAVEBUFFER * ) = 0;
//    virtual int PlayLine( QString *pLine, struct WAVEBUFFER *ps ) = 0;
    virtual int towav( struct TalkData qsTalkData, QThread *pThred ) = 0;		// TTS毎に処理違うので
    virtual void finalize() = 0;
    virtual void initialize() = 0;
    virtual int setVolume( int value ) = 0;
    virtual int getVolume() = 0;
    virtual int currentTalkLine() = 0;

    static void __stdcall StaticWaveOutProc( HWAVEOUT hwo, UINT uMsg,
                            DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2 );
    int waveFainalize();
//    void xchgJIS2004( QString *pline );
//    BOOL extractHatuonfeature( QString &dst, QString src, int hirakata );
//    BOOL extractHatuonsurface( QString &dst, QString src );
//    void num2yomi( QString &pszYomi, QString szNumbers );
    void conv2Katakana( QString &pKatakana, QString pSorceStr );
    void reStart();

    void appVolumeSet( unsigned short volume );

    enum WAVEOUTOPENTATE	StateWaveOutOpen = WAVEOUTOPENTATE::WFNONOPEN;	// WaveOutOpen wfe設定の種別
                                                            // WFNONOPEN:	no open
                                                            // WFAQTK10:	Aqtk10
                                                            // WFSAPI:		Sapi
                                                            // WaveOutOpenの設定（Bitrate等）がAqtk10用かSapi用か
    portaudioif                 paif;
    WAVEFORMATEX                wfe;
    HWAVEOUT                    hWaveOut = 0;
//    enum PLAYSTATE              iPlayState;
//    static text2wave            *pTts;
    static CComPtr <ISpStream>  cpWavStream;
    static CRITICAL_SECTION     cstPlayRec;
    static int                  ctRcvDone;
    static int                  ctCnfDone;
    static volatile int         ctPlayRec;				// waveOutWrite touroku count
    static int                  ixPlayRec;				// waveOutWrite touroku index
//    static WAVEHDR            	whdr[1];				// wave header
//    static LPBYTE               lpWave[1];				// wave buf
//    static long long int        bcWave[1];				// alloc size
//    struct WAVEBUFFER           wBuffer[2];               // wave buffer
//    static struct WAVEBUFFER    waveBuffer[2];          //
//    static QQueue<struct WAVEBUFFER>   queWaveBuffer;   //

};

extern text2wave           *pTts;

#endif // TEXT2WAVE_H
