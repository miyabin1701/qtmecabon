/**
 * @file qaudioif.cpp
 * @brief qaudioでの再生処理　未実装
 * @author
 * @date
 */

#if 0
#include "qaudioif.h"



qaudioif::qaudioif()
{
    count = 0;
    idTimer = 0;

}


qaudioif::~qaudioif()
{

}




int qaudioif::initialize()
{
    Pa_Initialize();            //PortAudio初期化
    return ( 0 );
}


void qaudioif::finalize()
{
    if ( stream != NULL )
    {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        Pa_Terminate();
        stream = NULL;
    }
}


int qaudioif::open( WAVEFORMATEX wavform )
{   enum QAudioFormat::SampleFormat fmt = QAudioFormat::Unknown;

    waveformat = wavform;

    switch ( wavform.wBitsPerSample )
    { case 0x0800:
        fmt = QAudioFormat::UInt8;
        break;
    case 0x1000:
        fmt = QAudioFormat::Int16;
        break;
    case 0x2000:
        fmt = QAudioFormat::Int32;
        break;
    default:
        fmt = QAudioFormat::Unknown;
        break;
    }
    QAudioFormat format;
    format.setChannelCount( wavform.nChannels );
    format.channelOffset( QAudioFormat::FrontCenter );
    format.setSampleFormat( fmt );
    format.setSampleRate( wavform.nSamplesPerSec );
//    format.setByteOrder(QAudioFormat::LittleEndian);
//    format.setCodec( "audio/pcm" );
//    format.setSampleSize( wavform.wBitsPerSample );
//    format.setSampleType(QAudioFormat::SignedInt);
//    QAudioOutput *audiooutput = new QAudioOutput;
//    QAudioOutput *audiooutput = new QAudioOutput(format, this);
//    audio_output = std::shared_ptr<QAudioOutput>
//            ( new QAudioOutput( format ));
//    audio_output_device = audio_output->start();
    audio_sink = new QAudioSink(format, this);
//    connect(audio, QAudioSink::stateChanged, this, &AudioInputExample::handleStateChanged);
    audio_sink->start();
    startTimer(100);
    return 0;
}


void qaudioif::timerEvent(QTimerEvent *event)
{
    int len = audio_output->bytesFree();
    int n = len / 2;
    if (n > 0) {
        std::vector<int16_t> buf(n);
        for (int i = 0; i < n; i++) {
            buf[i] = (count & 8) ? 10000 : -10000;
            count++;
            if (count >= 8000) {
                count = 0;
            }
        }
        audio_output_device->write((char const *)&buf[0], buf.size() * 2);
    }
}


int qaudioif::playwave( char *wave, int cbwave )
{

}


int qaudioif::stopwave()
{

}

#endif
