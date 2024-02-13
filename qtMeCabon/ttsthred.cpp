/**
 * @file ttsthred.cpp
 * @brief premecabを通したテキストキューでttsを呼び出す
 * @author m.kawakami
 * @date 23/10/??
 */

#include "ttsthred.h"
#include "text2wave.h"
//#include "pretalk.h"
#include "premecab.h"
#include "talktext.h"
#include "soundplaystate.h"
//#include "profilevalues.h"

//extern profileValues    *pprof;
//enum TTSNAME currentTts = NONE;
BOOL ttsthred::fgRuning = FALSE;


/**
 * @fn
 * ttsthredクラスコンストラクタ
 * @brief ttsthredクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail 何もしてない
 */
ttsthred::ttsthred(QObject *parent)
//    : QThread{parent}
{
}


/**
 * @fn
 * 処理中行セット
 * @brief 処理中行セット
 * @param (currentLine) 処理中行番号
 * @return なし
 * @sa
 * @detail 処理中行セット
 */
void ttsthred::setProcLine( int currentLine )
{   static int selectedLine = -1;
//    if ( selectedLine != currentLine )
    {   emit requestSelLine(selectedLine = currentLine);
}   }


/**
 * @fn
 * ttsスレッドメイン処理
 * @brief ttsスレッドメイン処理
 * @return なし
 * @sa
 * @detail ttsスレッドメイン処理
 */
void ttsthred::run()
{   TalkText        TalkTexts;
    struct TalkData qsTalkData;
    class preTalk   pretk;
    soundPlayState  plyStat;

    fgRuning = TRUE;
    while (( plyStat.getPlayState() != STOP ) && ( plyStat.getPlayState() != PRESTOP ))
    {   if ( TalkTexts.QueTalkData.empty())
        {   QThread::msleep( 200 );
            if (( pTts != NULL ) && ( pTts->currentTalkLine() >= 0 ))
            {   qDebug() << QString("ttsthred::run()-setProcLine(%1)").arg(pTts->currentTalkLine());
                setProcLine( pTts->currentTalkLine());
            }
            continue;
        }
        if ( pTts != NULL )
        {   qsTalkData = TalkTexts.QueTalkData.dequeue();
            setProcLine( pTts->currentTalkLine());
            pTts->towav( qsTalkData, this );		// TTS毎に処理違うので
    }   }
    while ( !TalkTexts.QueTalkData.empty())
    {   qsTalkData = TalkTexts.QueTalkData.dequeue();
    }
    fgRuning = FALSE;
//    qDebug() << QString("ttsthred::run()- exit");
}

