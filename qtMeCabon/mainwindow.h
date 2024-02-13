/**
 * @file mainwindow.h
 * @brief qtmecabonのメインウインドウの処理のヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <mmsystem.h>
#include <mmeapi.h>
//#include "settingdialog.h"
#include "settingdlg.h"
//#include "testdialog.h"
#include "corpasdialog.h"
#include "csvdictionarysarchdialog.h"
//#include "winpretranslatemessage.h"
#include "pretalk.h"
#include "playlinesapi.h"

#pragma comment (lib, "winmm.lib")

#define IDM_ABOUTBOX                    0x0010
//enum PLAYSTATE  { PLAYSTATE_STOP = 0, PLAYSTATE_PRESTOP, PLAYSTATE_POUSE, PLAYSTATE_PLAY };


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent( QCloseEvent *event );
    //  void mousePressEvent(QMouseEvent *pEevent);
    //  void mosueReleaseEvent(QMouseEvent *pEevent);
    //  void mouseMoveEvent(QMouseEvent *pEevent);
    bool checkfiles();

#ifdef Q_OS_WIN
//    virtual bool nativeEventFilter( const QByteArray &eventType, void *message, long *result );
//    virtual bool winEventFilter(MSG *message, long *result);
//private:
//  QColor savedInactiveButtonText_;
#endif //Q_WS_WIN

signals:
    void sigConsoleDisplayState();
    void sigAboutbox();
//    void requestNextLineString();
//    void requestSelLine( int lineno );
    void requestVolumeUpdate(int vol);
    void requestTitleUpdate( QString title );

private slots:
    void on_StopButton_clicked();
    void on_PlayButton_clicked();
    void on_SarchButton_clicked();
    void on_Volume_valueChanged(int value);
    void onVolumePressed();
    void on_requestVolumeUpdate(int vol);
//    void on_requestVolumeUpdate();
//    void fndelToThisLine();
    void fnContinue();
    void fnCorpas(int nomsg = 0);
    void fnplayThisLine();
    void fnsettingDlg();
    void fnsmbolePass();
    void fndisplayPwrOff();
    void fnmute();
    void fnConsoleDisplayState();
    void fnrequestNextLineString();
    void fnrequestSelLine(int lineno, BOOL forceUpDate=FALSE);
//    void fnrequestSelLine( int lineno );
//    void fnrequestSelLine( const int lineno );
    void fndisplayUnnounStr( QString szSurface );
    void on_requestTitleUpdate( QString title );
//    void requestDisplyCorpas();
    void fnAboutbox();
    void fnreStart();

    void on_talkTextEdit_textChanged();

private:
    Ui::MainWindow *ui;
    class preTalk *ppreTalk;
    class ttsthred *pttsthred;
    messageBuff msgBuff;
//    SettingDialog   *pSettingDlg;
    SettingDlg   *pSettingDlg;
//    TestDialog   *pSettingDlg;
    CorpasDialog    *pCopasDlg;
    CsvDictionarySarchDialog    *pOutputDlg;
//    CsvDictionarySarchDialog  *pOutputDlg;
    QMetaObject::Connection hConnectrequestNextLineString;
    QMetaObject::Connection hConnectrequestSelLine;
    QMetaObject::Connection hConnectrequestStopButtonClick;
    QMetaObject::Connection hConnectrequestVolumeUpdate;

    HWAVEOUT				hWaveOut = 0;
    int     iPlaylineno;
    int     iSelectlineno;
    int     beforSel;

public:
//    winPreTranslateMessage  ptMsg;

};
#endif // MAINWINDOW_H
