/**
 * @file premecab.h
 * @brief mecabで分かち書きする。sapiの際はこれ１回のみ、voicevoxの際は前処理として
 * また、語彙が行末で分割されている場合分割されないようにする　のヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef PREMECAB_H
#define PREMECAB_H

#include <QObject>
#include <QQueue>
#include <QThread>
#include <QString>
//#include "mecabif.h"
#include "pretalk.h"
#include "messagebuff.h"
#include "talktext.h"
#include "talkline.h"


#define MAXTALKDATAQUE  2


/*
struct TalkData
{	int	lineNo;
    QString	kanaBuff;
    QString	textBuff;
};
*/


class premecab : public QObject, public mecabif
{
    Q_OBJECT



    QString     srcBuff;
    QString     numbers;
    BOOL        fgComma;
    TalkData    TalkDat;
//    QQueue<struct LINEDATA>  QueTalkText;
//    QQueue<struct TalkData>  QueTalkData;
    messageBuff msgBuff;

public:
    explicit premecab(QObject *parent = nullptr);
    void    clear();
    int     analyzmecab( struct LINEDATA qsTalkData );
    BOOL    extractHatuonfeature( QString &dst, QString src, int hirakata );
    BOOL    extractHatuonsurface( QString &dst, QString src );
    void    num2yomi( QString &pszYomi, QString szNumbers );
    void    toZensmall( QString &str );

signals:

};

#endif // PREMECAB_H
