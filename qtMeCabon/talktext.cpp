/**
 * @file talktext.cpp
 * @brief トークデータのクラス（sapiは読み変換後、voicevoxは適切なイントネーション生成のため元の漢字混ざり文字列で）
 * @author m.kawakami
 * @date 23/10/??
 */

#include "talktext.h"


QQueue<struct TalkData>  TalkText::QueTalkData;


/**
 * @fn
 * TalkTextクラスコンストラクタ
 * @brief TalkTextクラスコンストラクタ
 * @return なし
 * @sa
 * @detail なんもしてない
 */
TalkText::TalkText()
{

}
