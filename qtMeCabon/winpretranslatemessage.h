/**
 * @file winpretranslatemessage.h
 * @brief プリトランスレートメッセージハンドリングクラス宣言
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef WINPRETRANSLATEMESSAGE_H
#define WINPRETRANSLATEMESSAGE_H

#include <QApplication>
#include <QMainWindow>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <powrprof.h>

class winPreTranslateMessage : public QObject, public QAbstractNativeEventFilter
{  Q_OBJECT

public:
//    winPreTranslateMessage();
//    winPreTranslateMessage(QObject * parent = 0) : QObject(parent) {}
    winPreTranslateMessage(QApplication &app, QMainWindow &parent );
    ~winPreTranslateMessage();
//    ~QAbstractNativeEventFilter() {};
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
//    const QMainWindow *preceiver;
    // Handle for power events
    HPOWERNOTIFY _hPowerNotify;

    // GUID for monitor power on event
    GUID _guidMonitorPowerOn;

    // Main window
    QMainWindow& _window;

signals:
    void sigConsoleDisplayState();
    void sigAboutbox();
};

#endif // WINPRETRANSLATEMESSAGE_H
