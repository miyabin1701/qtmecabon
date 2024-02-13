/**
 * @file pretalk.h
 * @brief テキストからwavデータに変換するスレッドメインのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef PRETALK_H
#define PRETALK_H

#include "qmainwindow.h"
#include "text2wave.h"
#include "talkline.h"
#include <QQueue>
#include <QThread>
#include <QString>

//QQueue<QString> queue;

/*
struct LINEDATA
{   int     iLineNo;
    bool    eof;
    QString qsLineStr;
};
*/

class preTalk: public QThread, public TalkLine
{    Q_OBJECT

public:
    preTalk(QObject* parent = nullptr, enum PLAYSTATE playState = STOP );
    ~preTalk();
    int setVolume( int value );
    int getVolume();
    void run();

//    static text2wave                *pTts;
//    static enum TTSNAME             currentTts;
    static volatile enum PLAYSTATE  playState;
    static QQueue<struct LINEDATA>  QueTalkLine;
    static volatile int             ctPlayRec;		// waveOutWrite touroku count
    static struct WAVEBUFFER        wavbuf;

signals:
//    void reqNextLineString();
    void requestNextLineString();
//    void requestSelLine( const int lineno );
    void requestSelLine(int lineno);
    void requestStopButtonClick();
    void requestVolumeUpdate(int vol);
//    void requestVolumeUpdate();

private slots:
//    void signal0Slot();

};

#endif // PRETALK_H
