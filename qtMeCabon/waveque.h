/**
 * @file waveque.h
 * @brief waveデータ型宣言＆キュー定義
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef WAVEQUE_H
#define WAVEQUE_H

#include <QQueue>
#include <windows.h>
#include <mmsystem.h>
#include <mmeapi.h>


enum mallocType {
    SAPI        = 0x00,
    OPENJTALK,
};


struct WAVEBUFFER
{   enum mallocType         allocType;          // SAPI5.4 / open JTalk
    class PlayLineVvoxCore  *pVvox;             //
    int                     iLineNo;            // for edit window sel line
//    int                     ixs;                //
//    int                     ixe;                //
    int                     iBuffNo;            //
    WAVEFORMATEX            wfe;                // wave format
    WAVEHDR                 whdr;               // wave header
    long long int           bcWave;				// alloc size
    LPBYTE                  lpWave;				// wave buf
};

#define MAXWAVEBUF  (4)
#define MAXWAVEQUE  (4)

class waveque
{
public:
    waveque();

    static struct WAVEBUFFER           waveBuffer[MAXWAVEBUF];  //
    static QQueue<struct WAVEBUFFER>   queWaveBuffer;   //

};


#endif // WAVEQUE_H
