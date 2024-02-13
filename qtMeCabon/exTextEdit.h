/**
 * @file exTextEdit.h
 * @brief メインウインドウのテキストエディタのサブクラスのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef EXTEXTEDIT_H
#define EXTEXTEDIT_H

//#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBlock>


//class exTextEdit : public QTextEdit
class exTextEdit : public QPlainTextEdit
{
    Q_OBJECT

    int selLine;

public:
    exTextEdit(QWidget *parent = 0);
    void contextMenuEvent(QContextMenuEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
//    virtual bool eventFilter(QObject* obj, QEvent* ev) override;
    void selectLine( int lineno );
    bool getLine( int lineno, QString &QstrLine );
    void LoadTxtFile( QString filepath );

protected:
//  void mousePressEvent(QMouseEvent *pEevent);
//  void mosueReleaseEvent(QMouseEvent *pEevent);
//  void mouseMoveEvent(QMouseEvent *pEevent);
    void wheelEvent(QWheelEvent *pEevent) override ;

private  slots:
    void fndelToThisLine();
    void fndelAllLine();
    void fnCorpas();
    void fnplayThisLine();
    void fnsettingDlg();
    void fnsmbolePass();
    void fndisplayPwrOff();
    void fnmute();
    void fnContinue();
    void fnsave();
    void fnsaveas();

signals:
    void sigCorpas();
    void sigplayThisLine();
    void sigsettingDlg();
    void sigsmbolePass();
    void sigdisplayPwrOff();
    void sigmute();
    void sigContinue();
    void requestTitleUpdate( QString title );

};

#endif // EXTEXTEDIT_H
