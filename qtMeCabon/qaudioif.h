/**
 * @file qaudioif.cpp
 * @brief qaudioでの再生処理　未実装のヘッダ
 * @author
 * @date
 */

#if 0
#ifndef QAUDIOIF_H
#define QAUDIOIF_H

#include <windows.h>
#include <mmsystem.h>
#include <QObject>
#include <QtMultimedia\QAudioOutput>
#include <QtMultimedia\QAudioFormat>
#include <QtMultimedia\QAudioSink>
#include <memory>

class qaudioif: public QObject
{
    Q_OBJECT

public:
    qaudioif();
    ~qaudioif();
    int initialize();
    void finalize();

    int open( WAVEFORMATEX wavform );
    int playwave( char *wave, int cbwave );
    int stopwave();

    WAVEFORMATEX        waveformat;
//    static PaStream     *stream;

private:
    std::shared_ptr<QAudioOutput> audio_output;
    QIODevice *audio_output_device;
    QAudioSink *audio_sink;
    int count;
    int idTimer;

protected:
    void timerEvent(QTimerEvent *event);

};

#endif // QAUDIOIF_H
#endif // 0
