/**
 * @file main.cpp
 * @brief qtmecabonのエントリー
 * @author m.kawakami
 * @date 23/10/??
 */

#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QAbstractEventDispatcher>
//#include <QTextCodec>
///* 以下の 2 行が無いとエラーになる http://blog.kmckk.com/archives/2573254.html */
//#include <QtPlugin>
//Q_IMPORT_PLUGIN(qjpcodecs)
/* リンク時には -L<path/to>\qt\4.3.2\win32\plugins\codecs -lqjpcodecs -lqtcore などが必要。 */
#include "profilevalues.h"
#include <QMessageBox>


//#include <QtGui>
#include "winpretranslatemessage.h"


//#define REDIRECTLOG
extern profileValues   *pprof;


/**
 * @fn
 * デバッグメッセージ等の出力先を変更する
 * @brief デバッグメッセージ等の出力先を変更する
 * @param (type) メッセージタイプ
 * @param (&context) コンテキスト
 * @param (&msg) メッセージ
 * @return なし
 * @sa
 * @detail デバッグログなどのメッセージのリダイレクトコールバック
 */
#ifdef REDIRECTLOG
void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    const auto &message = qFormatLogMessage(type, context, msg);
    QTextStream cerr(stderr);
    cerr << message << Qt::endl;
    QFile file( "dynamic_logging_setting.log" );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        cerr << "Cannot open log file:" << file.fileName();
        return;
    }
    QTextStream(&file) << message << Qt::endl;
}
#endif


/**
 * @fn
 * アプリケーションメイン
 * @brief アプリケーションメイン
 * @param (argc) 引数の数
 * @param (*argv[]) 引数文字列配列
 * @return エラーコード
 * @sa
 * @detail ウインドウズのプリトランスレートメッセージの中継などを行う　モニター休止メッセージ取得に必要
 */
int main(int argc, char *argv[])
{
    {   DWORD dwWaitProcessId = 0;
        bool bWaitTerminate = false;

// http://qt-log.open-memo.net/sub/system--get-command-arguments.html
        QStringList qsargv = QCoreApplication::arguments();
//        qsargv.removeAt(0);
//    qsargv.append( "-restart" );
        for ( int i = 0; i < qsargv.size(); i++ )
        {
            qDebug() << "argument = " << qsargv.at(i);
            if ( qsargv.at(i) == "-waitterminate" )
            {   pprof->fgRestart =
                    bWaitTerminate = true;
                dwWaitProcessId = qsargv.at(++i).toLong();
                qDebug() << "++argument = " << dwWaitProcessId;
            }
        }
//----------------
        if ( bWaitTerminate )
        {   HANDLE hWaitProcess = ::OpenProcess( SYNCHRONIZE, FALSE, dwWaitProcessId );
            if ( hWaitProcess != NULL )     // 10秒待っても前のプロセスが終了しない場合は、アプリケーションを終了します。
            {   if ( ::WaitForSingleObject( hWaitProcess, 10 * 1000 ) == WAIT_TIMEOUT )
                {
//                    QMessageBox::warning(this, tr("MessageBox"), tr("WAIT_TIMEOUT"));
                    return FALSE;
                }
            }
        }
    }
//
    QApplication a(argc, argv);
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());     // プログラム全体での文字コードの設定 QT6でQTextCodecは削除された
#ifdef REDIRECTLOG
    {   QFile file( "dynamic_logging_setting.log" );    // debug message to file

        file.remove();      // ファイル削除
    }
    qInstallMessageHandler(logHandler);
#endif
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Test02_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
//    winPreTranslateMessage  ptMsg;
//    a.installNativeEventFilter(&ptMsg); //this installs the instance ptMsg of the winPreTranslateMessage inside the widget w, on the application instance a.
//    a.installNativeEventFilter((QObject *)&ptMsg); //this installs the instance ptMsg of the winPreTranslateMessage inside the widget w, on the application instance a.
    winPreTranslateMessage filter( a, w );
    w.show();
//    w.setStyleSheet( "qApp {background-color:black}");
//    QAbstractEventDispatcher::instance()->installNativeEventFilter(
//        new winPreTranslateMessage);
    return a.exec();
}


