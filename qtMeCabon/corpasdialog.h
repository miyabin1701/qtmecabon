/**
 * @file corpasdialog.h
 * @brief サブ（インフォメーション）ウインドウの処理のヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */
#ifndef CORPASDIALOG_H
#define CORPASDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include "messagebuff.h"

namespace Ui {
class CorpasDialog;
}

class CorpasDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CorpasDialog(QWidget *parent = nullptr);
    ~CorpasDialog();
    void closeEvent( QCloseEvent *event );

signals:
    void requestDisplyCorpas(int nomsg=0);
    void sigreStart();

private slots:
    void fnreStart();
    void fnupdateMaxLine();

protected:
    void timerEvent(QTimerEvent * e);
//    void contextMenuEvent(QContextMenuEvent *event);
//    void fnsarchCsvDictionary();
//    void fnbuildSystemDictionary();

public:

private:
    int CorpasTimer;
    messageBuff msgBuff;
    Ui::CorpasDialog *ui;
};

#endif // CORPASDIALOG_H
