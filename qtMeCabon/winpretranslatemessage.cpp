/**
 * @file winpretranslatemessage.cpp
 * @brief プリトランスレートメッセージハンドリング処理　モニター電源メッセージハンドリング
 * @author m.kawakami
 * @date 23/10/??
 */

#include "mainwindow.h"
#include "winpretranslatemessage.h"
#include <QByteArray>

#ifdef Q_OS_WIN
#include <windows.h>
#endif //Q_WS_WIN


/**
 * @fn
 * winPreTranslateMessageクラスコンストラクタ
 * @brief winPreTranslateMessageクラスコンストラクタ
 * @param (&app) アプリケーション
 * @param (&parent) 親ウィンドウ
 * @return なし
 * @sa
 * @detail イベントフィルターのセット
 */
winPreTranslateMessage::winPreTranslateMessage( QApplication &app, QMainWindow &parent ) : _window(parent)
{
    app.installNativeEventFilter(this);

/*
    _guidMonitorPowerOn = {0x02731015, 0x4510, 0x4526, {0x99, 0xe6, 0xe5, 0xa1, 0x7e, 0xbd, 0x1a, 0xea}};
//    GUID_MONITOR_POWER_ON     02731015-4510-4526-99E6-E5A17EBD1AEA
//    GUID_CONSOLE_DISPLAY_STATE    6FE69556-704A-47A0-8F24-C28D936FDA47
    Data メンバーは、次のいずれかの値を持つ DWORD です。
        0x0 - ディスプレイがオフです。
        0x1 - ディスプレイがオンです。
        0x2 - ディスプレイは淡色表示になっています。
*/
    _guidMonitorPowerOn = {0x6FE69556, 0x704A, 0x47A0, {0x8F, 0x24, 0xC2, 0x8D, 0x93, 0x6F, 0xDA, 0x47}};
    _hPowerNotify = RegisterPowerSettingNotification(HWND(_window.winId()), &_guidMonitorPowerOn, DEVICE_NOTIFY_WINDOW_HANDLE);
}


/**
 * @fn
 * winPreTranslateMessageクラスデストラクタ
 * @brief winPreTranslateMessageクラスデストラクタ
 * @return なし
 * @sa
 * @detail イベントフィルターのリセット
 */
winPreTranslateMessage::~winPreTranslateMessage()
{
    UnregisterPowerSettingNotification(_hPowerNotify);
}


#ifdef Q_OS_WIN
/**
 * @fn
 * イベントフィルター
 * @brief イベントフィルター
 * @return なし
 * @sa
 * @detail イベントフィルター　モニター休止メッセージを掴みメッセージをエミットする　Windows専用
 */
bool winPreTranslateMessage::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
//    Q_DECL_OVERRIDE
{
    if ( eventType == "windows_generic_MSG" )   //  windows_dispatcher_MSG
    {   MSG *msg = static_cast<MSG *>(message);

        if ( msg->message == WM_POWERBROADCAST )
        {   if ( msg->wParam == PBT_POWERSETTINGCHANGE )
            {	POWERBROADCAST_SETTING *ppbcs = ( POWERBROADCAST_SETTING * )( msg->lParam );

                if ( ppbcs->PowerSetting == GUID_CONSOLE_DISPLAY_STATE )
                {   qDebug() << "eventType " << msg->message << "WM_POWERBROADCAST PBT_POWERSETTINGCHANGE GUID_CONSOLE_DISPLAY_STATE" << ppbcs->Data[0];
                    if ( ppbcs->Data[0] == 0 )				// ディスプレイがOFF
                    {   emit (( MainWindow & )_window ).sigConsoleDisplayState();   // ここでMainWindowにシグナル
        }	}	}   }
        if ( msg->message == WM_SYSCOMMAND )
        {    if (( msg->wParam & 0xFFF0) == IDM_ABOUTBOX )
            {   emit (( MainWindow & )_window ).sigAboutbox();   // ここでMainWindowにシグナル
    }   }   }
    return false;
}
#endif //Q_WS_WIN


