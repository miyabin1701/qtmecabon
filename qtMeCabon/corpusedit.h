/**
 * @file corpusedit.h
 * @brief サブ（インフォメーション）ウインドウのテキストエディタのサブクラスのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */
#ifndef CORPUSEDIT_H
#define CORPUSEDIT_H

#include <Windows.h>
#include <QPlainTextEdit>
#include "buildsysdic.h"
#include <QObject>

class corpusEdit : public QPlainTextEdit
{    Q_OBJECT

public:
    corpusEdit(QWidget *parent = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void wheelEvent(QWheelEvent *pEevent) override ;
    bool formatCheckLine( QString lineText );
    bool formatCheck( QString selected_text );

private  slots:
    void fnsarchCsvDictionary();
    void fntagJumpEditor();
    void fnbuildSystemDictionary();
    void fnappendCorpus();
    void updateMenuItem();
    void fnsaveas();

signals:
    void sigbuildSystemDictionary();
    void sigreStart();

protected:
    QAction *sarchCsvDictionary;
    QAction *buildSystemDictionary;
    void grep( char *pszStr, wchar_t *oszfname );
    DWORD execProcess( wchar_t *processPath, wchar_t *processOption, wchar_t *execFolder, QPlainTextEdit *pQPlainTextEdit, DWORD *pdwExitCode );

private:
    class buildSysDic   *pbuildthred;

};

#endif // CORPUSEDIT_H
