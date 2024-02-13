/**
 * @file messagebuff.h
 * @brief インフォメーションウィンドへ出力するメッセージのバッファクラスのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef MESSAGEBUFF_H
#define MESSAGEBUFF_H

#include <QtCore>


const int BufferSize = 8192;

//struct infoMessage
//{   int     fgDelMultWord;
//   QString message;
//};


class messageBuff
{
public:
    messageBuff();
    void enterMessageWcheck( QString msg );
    void enterMessage( QString msg );
//    QString deentMessage( QString &msg );
//    int availableMessage();
    static QQueue<QString>   queMsgBuffer;   //

private:
    static QMutex mutexBuff;
/*
    static QChar buffer[BufferSize];
    static int ixEnterBuff;
    static int ixDeentBuff;
    static QSemaphore cbbufferfree;
    static QSemaphore cbbufferused;
*/
};

#endif // MESSAGEBUFF_H
