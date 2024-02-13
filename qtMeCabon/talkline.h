/**
 * @file talkline.h
 * @brief エディタから取得したテキスト行のバファリングクラスのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef TALKLINE_H
#define TALKLINE_H

#include <QQueue>


struct LINEDATA
{   int     iLineNo;
    bool    eof;
    QString qsLineStr;
};


class TalkLine
{
public:
    TalkLine();

    static QQueue<struct LINEDATA>  QueTalkText;

};

#endif // TALKLINE_H
