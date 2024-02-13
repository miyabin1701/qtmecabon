/**
 * @file pretalk.cpp
 * @brief テキストからwavデータに変換するスレッドメイン premecab等を呼び出す
 * @author m.kawakami
 * @date 23/10/??
 */

//#include "mainwindow.h"
#include "text2wave.h"
#include "pretalk.h"
#include "playlinesapi.h"
#include "playlinevvoxcore.h"
#include "profilevalues.h"
//#include <QQueue>
#include "premecab.h"
#include <fstream>          // for debug

//text2wave               *pTts = NULL;
//volatile enum PLAYSTATE playState = STOP;
//QQueue<struct LINEDATA> QueTalkLine;
//struct WAVEBUFFER       wavbuf = { SAPI, NULL, 0, 0, 0, 0, { 0 }, { 0 }, 0, NULL };
enum TTSNAME            currentTts = NONE;
volatile int            ctPlayRec = 0;		// waveOutWrite touroku count

extern profileValues    *pprof;


/**
 * @fn
 * preTalkクラスのコンストラクタ
 * @brief preTalkクラスのコンストラクタ
 * @param (*parent) 親ウィジェット
 * @param (playState) 再生ステート
 * @return なし
 * @sa
 * @detail preTalkクラスのコンストラクタ
 */
preTalk::preTalk( QObject* parent, enum PLAYSTATE playState )
{   soundPlayState      plyStat;

    qDebug() << QString("enter preTalk()=%1").arg(plyStat.playState);
//    playState = STOP;
//    pTts = NULL;
//    memset(( void * )wavbuf, 0, sizeof( wavbuf ));
//    wavbuf.lpWave = NULL;
//    wavbuf.bcWave = 0;
//    setStackSize( 1024 * 512 );   // set thred stack size-for voicevox hangup debug
    if ( pprof->oper.ttsSelector == TTSNAME::VVoxCore )
    {   if ( pTts == NULL )     // 試行 スレッド起動時Voicevoxが選択されていたら、とりあえず初期化（起動が遅いので）
        {   currentTts = pprof->oper.ttsSelector;
            pTts = new PlayLineVvoxCore;
            pTts->initialize();
#if 0
            {   // for debug
                std::ofstream ofs( "R:\\release\\work\\final\\char.bin", std::ios::binary|std::ios::out);
                if ( !ofs )
                {   DWORD err = ::GetLastError();
                    qDebug() << QString("permission denied:[%1] R:\\release\\work\\final\\char.bin").arg(err);
                    return;
                }
                else
                {   ofs.close();
            }   }
#endif
    }   }
}


/**
 * @fn
 * preTalkクラスのデストラクタ
 * @brief preTalkクラスのデストラクタ
 * @return なし
 * @sa
 * @detail preTalkクラスのデストラクタ
 */
preTalk::~preTalk()
{
    if ( pTts != NULL )
    {   soundPlayState      plyStat;

//        qDebug() << QString("enter ~preTalk()=%1").arg(playState);
//        pTts->finalize();
        qDebug() << QString("exit ~preTalk()=%1").arg(plyStat.playState);
    }
}


/**
 * @fn
 * ボリューム値のセット
 * @brief ボリューム値のセット
 * @param (value) ボリューム値
 * @return 以前のボリューム値 / 0:ttsが開かれていなかったら
 * @sa
 * @detail ボリューム値のセット
 */
int preTalk::setVolume( int value )
{   if ( pTts != NULL )
    {   return( pTts->setVolume( value ));
    }
    return ( 0 );
}


/**
 * @fn
 * ボリューム値の取得
 * @brief ボリューム値の取得
 * @return 現在ののボリューム値 / 0:ttsが開かれていなかったら
 * @sa
 * @detail ボリューム値の取得
 */
int preTalk::getVolume()
{   if ( pTts != NULL )
    {   return( pTts->getVolume());
    }
    return ( 0 );
}


/**
 * @fn
 * プリトークスレッド
 * @brief プリトークスレッド
 * @return なし
 * @sa
 * @detail プリトークスレッド　エディットクラスにトーク行データを要求し、premecab解析し、選択ttsへwave変換する
 */
void preTalk::run()
{   TalkLine TalkLineText;
    struct LINEDATA    qsTalkData;
    soundPlayState      plyStat;
    premecab        prmecab;

//  if ( initial_dlls() != 0 )								// mecab & aquest & wave
//  {	AfxMessageBox( L"DLL オープンエラー！設定をご確認ください" );
//      return ( 0 );
//}
//    qDebug() << QString("preTalk::stack=%1").arg(stackSize());
    while( !QueTalkText.empty())
    {   QueTalkText.dequeue();
    }
    emit requestNextLineString();
    emit requestNextLineString();
    if ( pprof->oper.ttsSelector != currentTts )
    {   if ( pTts != NULL )
        {   pTts->finalize();
        }
        switch ( currentTts = pprof->oper.ttsSelector )
        { case TTSNAME::SAPI54:
          default:
            pTts = new PlayLineSapi;
            break;
          case TTSNAME::VVoxCore:
            pTts = new PlayLineVvoxCore;
            break;
        }
        pTts->initialize();
        emit requestVolumeUpdate(pTts->getVolume());
    }
    for ( ; plyStat.getPlayState() != STOP; )
    {	if ( plyStat.getPlayState() == PAUSE )
        {	while ( plyStat.getPlayState() == PAUSE )
            {   QThread::msleep( 500 );
            }
            if ( QueTalkText.empty())
            {   emit requestNextLineString();
                emit requestNextLineString();
                while ( QueTalkText.empty())
                {   if ( plyStat.getPlayState() == PRESTOP )
                    {   break;
                    }
                    QThread::msleep( 200 );
            }   }
//            qDebug() << QString("run.msleep-000:=%1").arg(plyStat.playState);
//            continue;
        }
//        qDebug() << QString("run-001:=%1").arg(plyStat.getPlayState());
        if (( plyStat.getPlayState() == PLAY ) || ( plyStat.getPlayState() == WAITENDOFTALK ))
        {   if ( pprof->oper.ttsSelector != currentTts )
            {
//              qDebug() << QString("run-002:=%1").arg(plyStat.playState);
                if ( pTts != NULL )
                {   pTts->finalize();
                }
                switch ( currentTts = pprof->oper.ttsSelector )
                { case TTSNAME::SAPI54:
                  default:
                    pTts = new PlayLineSapi;
                    break;
                  case TTSNAME::VVoxCore:
                    pTts = new PlayLineVvoxCore;
                    break;
                }
                pTts->initialize();
                emit requestVolumeUpdate(pTts->getVolume());
            }
            if ( TalkLineText.QueTalkText.empty())
            {   QThread::msleep( 500 );
                continue;
            }
            qsTalkData = TalkLineText.QueTalkText.dequeue();
//            qDebug() << QString("dequeue(%1:%2)").arg(qsTalkData.iLineNo).arg(qsTalkData.qsLineStr);
            if ( qsTalkData.eof )
            {   plyStat.setPlayState( WAITENDOFTALK );
//                qDebug() << QString("qsTalkData.eof-001:=%1").arg(qsTalkData.eof);
            }
//            else
            {   //wavbuf.iLineNo = qsTalkData.iLineNo;
                //emit requestSelLine(wavbuf.iLineNo);
//                qsTalkData.qsLineStr = qsTalkData.qsLineStr.trimmed();
                // ここでmecabで文字列正規化　行末で語彙が分割されないよう＆長すぎないよう(かなで４８文字)
                prmecab.analyzmecab( qsTalkData );
//                analyzmecab( qsTtsData, qsTalkData );
//                pTts->PlayLine( &( qsTalkData.qsLineStr ), &wavbuf );
                emit requestNextLineString();
            }
            pTts->reStart();
        }
        else if (( plyStat.getPlayState() == PRESTOP ) || ( plyStat.getPlayState() == WAITENDOFTALK ))
        {   qsTalkData.iLineNo = 0;
            qsTalkData.eof = TRUE;
            qsTalkData.qsLineStr = QString( "" );
            prmecab.analyzmecab( qsTalkData );
            QThread::msleep( 500 );
//            qDebug() << QString("run-004:=%1").arg(plyStat.playState);
            break;
        }
    }
    plyStat.setPlayState( STOP );
    emit requestStopButtonClick();
//    qDebug() << QString("run-999:=%1").arg(plyStat.playState);
    while ( !TalkLineText.QueTalkText.empty())
    {   qsTalkData = TalkLineText.QueTalkText.dequeue();
    }
}


