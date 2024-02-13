/**
 * @file talktext.h
 * @brief トークデータの宣言（sapiは読み変換後、voicevoxは適切なイントネーション生成のため元の漢字混ざり文字列で）
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef TALKTEXT_H
#define TALKTEXT_H

#include <QQueue>


struct TalkData
{	int	lineNo;
    QString	kanaBuff;
    QString	textBuff;
};

//#define MAXTALKDATAQUE  (4)

class TalkText
{
public:
    TalkText();

    static QQueue<struct TalkData>  QueTalkData;

};

#endif // TALKTEXT_H
