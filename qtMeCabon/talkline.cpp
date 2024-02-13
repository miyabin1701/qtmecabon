/**
 * @file talkline.cpp
 * @brief エディタから取得したテキスト行のバファリングクラス
 * @author m.kawakami
 * @date 23/10/??
 */

#include "talkline.h"


QQueue<struct LINEDATA>  TalkLine::QueTalkText;


/**
 * @fn
 * TalkLineクラスコンストラクタ
 * @brief TalkLineクラスコンストラクタ
 * @return なし
 * @sa
 * @detail なんもしてない
 */
TalkLine::TalkLine()
{

}
