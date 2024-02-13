/**
 * @file ttsthred.h
 * @brief premecabを通したテキストキューでttsを呼び出すのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef TTSTHRED_H
#define TTSTHRED_H

#include <QThread>
#include "pretalk.h"

class ttsthred : public QObject, /*public QThread, */public preTalk
{    Q_OBJECT

public:
    static BOOL fgRuning;
public:
    explicit ttsthred(QObject *parent = nullptr);
    void run();
    void setProcLine( int currentLine );

signals:
    void requestSelLine(int lineno, BOOL forceUpDate=FALSE);
};

#endif // TTSTHRED_H
