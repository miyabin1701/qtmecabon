/**
 * @file buildsysdic.h
 * @brief システム辞書のビルド処理のヘッダ
 * @author m.kawakami
 * @date 21/05/??
 */

#ifndef BUILDSYSDIC_H
#define BUILDSYSDIC_H

#include <QThread>
#include <QObject>
#include <Windows.h>
#include <QPlainTextEdit>

class buildSysDic : public QThread
{

public:
    explicit buildSysDic(QObject *parent = nullptr);
    void run();
};

#endif // BUILDSYSDIC_H
