/**
 * @file mainwindow.cpp
 * @brief qtmecabonのメインウインドウの処理
 * @author m.kawakami
 * @date 23/10/??
 */

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "profilevalues.h"
//#include "settingdialog.h"
#include "settingdlg.h"
#include "mecabif.h"
#include <QMessageBox>
#include "playlinesapi.h"
#include "talkline.h"
#include "soundplaystate.h"
#include "ttsthred.h"
#include "about.h"

#ifdef Q_OS_WIN
#include <QDebug>
#include <QFrame>
#include "Windows.h"
#include <QMenuBar>
//#include "settingdlg.h"
#include <QMainWindow>
#include <QQueue>
#endif //Q_WS_WIN
#include <fstream>          // for debug

QQueue<QString> msgQue;
profileValues   *pprof = NULL;


/*
bool isExistFile( QString path )
{   WIN32_FIND_DATAW FindFileData;
    HANDLE hFind;

    if (( hFind = FindFirstFileW( path.toStdWString().c_str(), &FindFileData )) == INVALID_HANDLE_VALUE )
    {   return ( false );
    }
    FindClose( hFind );
    return ( true );
}
*/


/**
 * @fn
 * メインウィンドウクラスコンストラクタ
 * @brief メインウィンドウクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail プロファイル情報の読み込み、変数の初期化、メッセージのコネクトなどを行う
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pprof = new profileValues;
    if ( pprof->other.pwszRamDiskPath != NULL )
    {   pprof->replaceEnv( "TMP", *( pprof->other.pwszRamDiskPath ));
    }
    if (( pprof->oper.iSizeMainwindowWidth > 100 ) && ( pprof->oper.iSizeMainwindowHight > 100 ))
    {   this->resize( QSize( pprof->oper.iSizeMainwindowWidth, pprof->oper.iSizeMainwindowHight )); // メインウインドウサイズ設定
    }
//----------------
#if 0
    // http://qt-log.open-memo.net/sub/system--get-command-arguments.html
    QStringList qsargv = QCoreApplication::arguments();
    qsargv.removeAt(0);
    qsargv.append( "-restart" );
    for ( QString arg: qsargv )
    {   if ( arg == "-restart" )
        {   pprof->fgRestart = true;
        }
        qDebug() << "argument = " << arg;
    }
#endif
//----------------
    iPlaylineno = 0;
    hWaveOut = 0;
    pSettingDlg = NULL;
    pCopasDlg = NULL;
    pOutputDlg = NULL;
    ppreTalk = NULL;
    pttsthred = NULL;
    if ( pprof->other.selBakeColorBlack )
    {   ui->talkTextEdit->setStyleSheet( "QPlainTextEdit{background-color:black; color:white; selection-color:yellow; selection-background-color:darkGreen}");
    }   // http://dorafop.my.coocan.jp/Qt/Qt105.html color define
    else
    {   ui->talkTextEdit->setStyleSheet( "QPlainTextEdit{background-color:lightGray; color:black; selection-color:cyan; selection-background-color:black}");
    }
#ifdef WIN32
//#ifdef Q_WS_WIN
    HMENU hMenu = GetSystemMenu(( HWND )winId(), FALSE );   //
    AppendMenu( hMenu, MF_SEPARATOR, 0, NULL );             // システムメニューにバージョン情報追加
    AppendMenu( hMenu, 0, IDM_ABOUTBOX, L"バージョン情報");
#endif
    connect( ui->talkTextEdit, SIGNAL(sigCorpas()), this, SLOT(fnCorpas()));
    connect( ui->talkTextEdit, SIGNAL(sigplayThisLine()), this, SLOT(fnplayThisLine()));
    connect( ui->talkTextEdit, SIGNAL(sigsettingDlg()), this, SLOT(fnsettingDlg()));
    connect( ui->talkTextEdit, SIGNAL(sigsmbolePass()), this, SLOT(fnsmbolePass()));
    connect( ui->talkTextEdit, SIGNAL(sigdisplayPwrOff()), this, SLOT(fndisplayPwrOff()));
    connect( ui->talkTextEdit, SIGNAL(sigmute()), this, SLOT(fnmute()));
    connect( ui->talkTextEdit, SIGNAL(sigContinue()), this, SLOT(fnContinue()));
//    connect(ui->talkTextEdit,SIGNAL(requestTitleUpdate()),this,SLOT(on_requestTitleUpdate(QString)));
    connect( ui->talkTextEdit, &exTextEdit::requestTitleUpdate, this, &MainWindow::on_requestTitleUpdate );

//    connect( &ptMsg, SIGNAL(sigConsoleDisplayState()), this, SLOT(fnConsoleDisplayState()));
    connect( this, SIGNAL(sigConsoleDisplayState()), this, SLOT(fnConsoleDisplayState()));
    connect( this, SIGNAL(sigAboutbox()), this, SLOT(fnAboutbox()));

    connect( ui->StopButton, &QPushButton::clicked, this, &MainWindow::on_StopButton_clicked );
    connect( ui->PlayButton, &QPushButton::clicked, this, &MainWindow::on_PlayButton_clicked );
    connect( ui->SarchButton, &QPushButton::clicked, this, &MainWindow::on_SarchButton_clicked );
    connect( ui->Volume, &QDial::valueChanged, this, &MainWindow::on_Volume_valueChanged );

//    connect( ppreTalk, SIGNAL(requestVolumeUpdate()), this, SLOT( on_requestVolumeUpdate()));
//    connect( ppreTalk, SIGNAL(requestVolumeUpdate()), this, SLOT( on_requestVolumeUpdate( int vol )));
//    hConnectrequestVolumeUpdate = connect(ppreTalk, &preTalk::requestVolumeUpdate,
//                                     this, &MainWindow::on_requestVolumeUpdate);

//    connect( ppreTalk, SIGNAL(requestNextLineString()), this, SLOT( fnrequestNextLineString()), Qt::DirectConnection );
//    connect( ppreTalk, SIGNAL(requestNextLineString()), this, SLOT( fnrequestNextLineString()), Qt::QueuedConnection );
//    connect( ppreTalk, SIGNAL(requestSelLine()), this, SLOT( fnrequestSelLine( int lineno )));
//    connect( ppreTalk, SIGNAL(requestStopButtonClick()), this, SLOT( on_StopButton_clicked()));
//    connect( ppreTalk->pTts, SIGNAL(ppreTalk->pTts::displayUnnounStr()), this, SLOT( fndisplayUnnounStr ));
//    connect( PlayLineSapi, SIGNAL(PlayLineSapi::displayUnnounStr()), this, SLOT( fndisplayUnnounStr ));
    if ( !pCopasDlg )
    {   pCopasDlg = new CorpasDialog;
/*
        int hight = pprof->oper.iSizeCorpaswindowHight;
        int wifth = pprof->oper.iSizeCorpaswindowWidth;
        if (( pprof->oper.iSizeCorpaswindowHight > 100 ) && ( pprof->oper.iSizeCorpaswindowWidth > 100 ))
        {   pCopasDlg->setGeometry( this->geometry().right() + 10, this->geometry().top(),
                                   pprof->oper.iSizeCorpaswindowHight, pprof->oper.iSizeCorpaswindowWidth );
        }   // コーパスウインドウ位置、サイズ指定
*/
        pCopasDlg->hide();
    }
    connect(pCopasDlg,&CorpasDialog::requestDisplyCorpas,this,&MainWindow::fnCorpas);
    connect(pCopasDlg,&CorpasDialog::sigreStart,this,&MainWindow::fnreStart);
    beforSel = -1;
    ui->Volume->setValue( pprof->oper.iAppVolume );
#if 0
    {   // for debug
        std::ofstream ofs( "R:\\release\\work\\final\\char.bin", std::ios::binary|std::ios::out);
        if ( !ofs )
        {   DWORD err = ::GetLastError();
            qDebug() << QString("permission denied:[%1] R:\\release\\work\\final\\char.bin").arg(err);
            return;
        }
        else
        {   ofs.close();
    }   }
#endif
    // 引数に"-restart"が在ったら、以下のvoicevoxのプリロードをしないで、辞書ビルドする。char.binのロックをさせないため
    if ( pprof->fgRestart == false )
    {   if ( pprof->oper.ttsSelector == TTSNAME::VVoxCore )
        {   if ( checkfiles())
            {   ppreTalk = new class preTalk;
                hConnectrequestNextLineString = connect( ppreTalk, SIGNAL(requestNextLineString()), this, SLOT( fnrequestNextLineString()), Qt::QueuedConnection );
                hConnectrequestVolumeUpdate = connect(ppreTalk, &preTalk::requestVolumeUpdate,
                                                      this, &MainWindow::on_requestVolumeUpdate);
                hConnectrequestStopButtonClick = connect( ppreTalk, SIGNAL(requestStopButtonClick()), this, SLOT( on_StopButton_clicked()), Qt::QueuedConnection );
                ppreTalk->start( QThread::HighPriority );
    }   }   }
}


/**
 * @fn
 * メインウィンドウクラスデストラクタ
 * @brief メインウィンドウクラスデデストラクタ
 * @return なし
 * @sa
 * @detail プロファイル情報の書き込み、各種の後始末などを行う
 */
MainWindow::~MainWindow()
{
    if ( pOutputDlg )
    {   delete pOutputDlg;
    }
    if ( pCopasDlg )
    {   delete pCopasDlg;
    }
    if ( pSettingDlg )
    {   delete pSettingDlg;
    }
    if ( ppreTalk != NULL )
    {   soundPlayState  plyStat;

        if ( plyStat.getPlayState() != PLAYSTATE::STOP )
        {   plyStat.setPlayState( PLAYSTATE::PRESTOP );
            while ( plyStat.getPlayState() != PLAYSTATE::STOP )
            {   Sleep( 200 );
        }   }
        qDebug() << QString("~MainWindow() ppreTalk->quit()");
        ppreTalk->quit();
        while( !ppreTalk->isFinished())
        {   Sleep( 100 );        // まだスレッドが終わっていないのかエラーが出るため少し待ってみる
        }
        delete ppreTalk;
        ppreTalk = NULL;
    }
    if ( pttsthred != NULL )
    {   while ( pttsthred->fgRuning )
        {   Sleep( 200 );
        }
        pttsthred->quit();
        while( !pttsthred->isFinished())
        {   Sleep( 100 );        // まだスレッドが終わっていないのかエラーが出るため少し待ってみる
        }
        delete pttsthred;
        pttsthred = NULL;
    }
    delete pprof;
    delete ui;
}


/**
 * @fn
 * 実行に必要なファイルの存在確認を行い、無い場合はメッセージボックスでお知らせ
 * @brief 実行に必要なファイルの存在確認を行い、無い場合はメッセージボックスでお知らせ
 * @return false:必要なファイルが無い　/　true:大丈夫
 * @sa
 * @detail 必要なファイルが無いと、勝手にクラッシュしてくれるDllも在るのでトーク前に確認しておく
 */
bool MainWindow::checkfiles()
{   if ( !QFile::exists( *( pprof->mecab.pwszSystemDictionaryFolder )))
    {   QMessageBox::warning(this, tr("MessageBox"), tr("Mecabの辞書フォルダーが見つかりません！"));
        return ( false );
    }
    if ( !QFile::exists( *( pprof->vvox.pwszOpenJTalkDicPath )))
    {   QMessageBox::warning(this, tr("MessageBox"), tr("openJtalkの辞書フォルダーが見つかりません！"));
        return ( false );
    }
    if ( !QFile::exists( "model\\metas.json" ))
    {   QMessageBox::warning(this, tr("MessageBox"), tr("modelフォルダーまたはmetas.jsonが見つかりません！"));
        return ( false );
    }
    //
    // 50(余裕を観て48位？)MByteを超えるファイルはgithubの単ファイルのサイズ制限に引っかかるので圧縮(サイズ減ったので)して登録している
    // ∴解凍を忘れて実行した際の為にチェックをおこなう。辞書が大きくなってた。24/02/13
    //
//    qDebug() << QString("/final/sys.dic:%1").arg( *( pprof->mecab.pwszSystemDictionaryBuildFolder ) + "/final/sys.dic" );
    if ( !QFile::exists( *( pprof->mecab.pwszSystemDictionaryBuildFolder ) + "/final/sys.dic" ))
    {   QMessageBox::warning(this, tr("MessageBox"), tr("/final/sys.dic辞書が見つかりません！解凍しましたか？"));
        return ( false );
    }
    if ( !QFile::exists( *( pprof->mecab.pwszSystemDictionaryBuildFolder ) + "/final/model.def" ))
    {   QMessageBox::warning(this, tr("MessageBox"), tr("/final/model.defファイルが見つかりません！解凍しましたか？"));
        return ( false );
    }
    if ( !QFile::exists( *( pprof->mecab.pwszSystemDictionaryBuildFolder ) + "/seed/sys.dic" ))
    {   QMessageBox::warning(this, tr("MessageBox"), tr("/seed/model.defファイルが見つかりません！解凍しましたか？"));
        return ( false );
    }
    if ( !QFile::exists( *( pprof->mecab.pwszSystemDictionaryBuildFolder ) + "/seed/model" ))
    {   QMessageBox::warning(this, tr("MessageBox"), tr("/seed/modelファイルが見つかりません！解凍しましたか？"));
        return ( false );
    }
    if ( !QFile::exists( *( pprof->mecab.pwszSystemDictionaryBuildFolder ) + "/seed/mecab-ipadic-2.7.0-20070801U8.model" ))
    {   QMessageBox::warning(this, tr("MessageBox"), tr("/seed/mecab-ipadic-2.7.0-20070801U8.modelファイルが見つかりません！解凍しましたか？"));
        return ( false );
    }
    return ( true );
}


/**
 * @fn
 * メインウィンドウのクローズイベントのオーバーライド、生成したスレッドなどの終了処理を行う
 * @brief メインウィンドウのクローズイベントのオーバーライド、生成したスレッドなどの終了処理を行う
 * @param (*event) イベントデータ
 * @sa
 * @detail メインウィンドウのサイズをプロファイルに記録、STOPボタン押し下げ、各ダイヤログとスレッドを終了する。
 */
// Qtでは、closeEventをオーバーライドする事で実現可能です。
void MainWindow::closeEvent( QCloseEvent *event )
{   pprof->oper.iSizeMainwindowWidth = this->geometry().width();
    pprof->oper.iSizeMainwindowHight = this->geometry().height();
    on_StopButton_clicked();
    if(pOutputDlg)
    {   delete pOutputDlg;
        pOutputDlg = NULL;
    }
    if( pCopasDlg )
    {   delete pCopasDlg;
        pCopasDlg = NULL;
    }
    if( pSettingDlg )
    {   delete pSettingDlg;
        pSettingDlg = NULL;
    }
/*
    int x0 = this->geometry().x();  //x;
    int x1 = this->geometry().y();  //x;
    int x2 = this->geometry().top();  //x;
    int x3 = this->geometry().left();  //x;
    int x4 = this->geometry().bottom();  //x;
    int x5 = this->geometry().right();  //x;
*/
    if ( ppreTalk != NULL )
    {   soundPlayState  plyStat;

//        qDebug() << QString("playState:%1").arg(plyStat.playState);
        if (( plyStat.getPlayState() == PLAYSTATE::PLAY ) ||
            ( plyStat.getPlayState() == PLAYSTATE::PAUSE ))
        {   plyStat.setPlayState( PLAYSTATE::PRESTOP );
            while ( plyStat.getPlayState() != PLAYSTATE::STOP )
            {   Sleep( 200 );
        }   }
        qDebug() << QString("closeEvent() ppreTalk->quit()");
        ppreTalk->quit();
        while( ppreTalk->isRunning())
        {   Sleep( 200 );        // まだスレッドが終わっていないのかエラーが出るため少し待ってみる
        }
        delete ppreTalk;
        ppreTalk = NULL;
    }
    if ( pttsthred != NULL )
    {   while ( pttsthred->fgRuning )
        {   Sleep( 200 );
        }
        pttsthred->quit();
        while( pttsthred->isRunning())
        {   Sleep( 200 );        // まだスレッドが終わっていないのかエラーが出るため少し待ってみる
        }
        delete pttsthred;
        pttsthred = NULL;
    }
}


/**
 * @fn
 * STOPボタン押し下げ時の処理
 * @brief STOPボタン押し下げ時の処理
 * @sa
 * @detail ボタンプッシュイベントはqtの不具合なのか１プッシュで２度来るので、先ず、２秒以内に来た場合はそのままリターン
 * talkスレッドなどに停止をステートで伝達し、スレッドが停止するまで待つ
 * STOPボタンを押していないのに、実行開始直後に何度か呼ばれているようです。どうデバッグしろと？コールスタックに犯人らしきもの無し
 */
void MainWindow::on_StopButton_clicked()
{   static qint64 beforproced = 0;
    static short ctEntered = 2;
    static BOOL fgreEntOn_StopButton_clicked = FALSE;
    qint64 currentms, lockms;

    if ( ctEntered > 0 )                        // 起動後に２度、クリックしてないのにこのハンドラが呼ばれる対策
    {   ctEntered--;                            // 確実に２度来るならこの処置で良いが、不確実ならクリックが
        return;                                 // 効かないことになる…まぁ勝手に止まるより、何度かクリックする方がましか？
    }
    currentms = QDateTime::currentMSecsSinceEpoch();
    lockms = currentms - beforproced - 2000;    // クリックイベントが1クリックで2度来るので
    if ( lockms < 0 )                           // 短時間(2秒)で来たイベントはスルー
    {   return;
    }
    if ( fgreEntOn_StopButton_clicked != FALSE )
    {   return;
    }
    fgreEntOn_StopButton_clicked = TRUE;
    if ( ppreTalk != NULL )
    {   soundPlayState  plyStat;

        if ( plyStat.getPlayState() != PLAYSTATE::STOP )
        {   if ( plyStat.getPlayState() != PLAYSTATE::PRESTOP )
            {   plyStat.setPlayState( PLAYSTATE::PRESTOP );
                qDebug() << QString("ppreTalk->playState = PLAYSTATE::PRESTOP ::%1:%2").arg(plyStat.playState).arg(PLAYSTATE::PRESTOP);
            }
            while ( plyStat.getPlayState() != PLAYSTATE::STOP )
            {   Sleep( 200 );
                qDebug() << QString("while ( ppreTalk->playState != PLAYSTATE::STOP )::%1:%2").arg(plyStat.playState).arg(PLAYSTATE::STOP);
        }   }                   // pretalk thred でSTOPにしてすぐこの関数を呼ぶので
        while( ppreTalk->isRunning())
        {   Sleep( 100 );       // まだスレッドが終わっていないのかエラーが出るため少し待ってみる
        }
        QObject::disconnect( hConnectrequestVolumeUpdate );
        QObject::disconnect( hConnectrequestNextLineString );
//        QObject::disconnect( hConnectrequestSelLine );
        QObject::disconnect( hConnectrequestStopButtonClick );
        delete ppreTalk;
        ppreTalk = NULL;
        iPlaylineno = 0;
    }
    if ( pttsthred != NULL )
    {   while( pttsthred->isRunning())
        {   Sleep( 100 );        // まだスレッドが終わっていないのかエラーが出るため少し待ってみる
        }
        QObject::disconnect( hConnectrequestSelLine );
        delete pttsthred;
        pttsthred = NULL;
    }
    beforSel = -1;
    ui->PlayButton->setChecked( FALSE );      // 初期状態に戻った
    fgreEntOn_StopButton_clicked = FALSE;
    beforproced = QDateTime::currentMSecsSinceEpoch();
}


/**
 * @fn
 * PLAY/POUSEボタン押し下げ時の処理
 * @brief PLAY/POUSEボタン押し下げ時の処理
 * @sa
 * @detail ボタンプッシュイベントはqtの不具合なのか１プッシュで２度来るので、先ず、２秒以内に来た場合はそのままリターン
 * playとpouseボタンを兼用しているので２度来るのは勘弁してほしい
 * talkスレッドなどに再生/再開をステートで伝達し、各スレッドを生成、イベントをコネクトする
 */
void MainWindow::on_PlayButton_clicked()
{   static qint64 beforproced = 0;
    qint64 currentms, lockms;
    soundPlayState  plyStat;

    if ( ui->talkTextEdit->document()->characterCount() <= 0 )
    {   return;
    }
    currentms = QDateTime::currentMSecsSinceEpoch();
    lockms = currentms - beforproced - 4000;    // クリックイベントが1クリックで2度来るので
//    beforproced = currentms;
    if ( lockms < 0 )                           // 短時間(4秒)で来たイベントはスルー
    {   return;
    }
    qDebug() << QString("on_PlayButton_clicked::enter time:%1:%2:%3").arg(lockms).arg(currentms).arg(beforproced);
    if ( ui->PlayButton->isChecked())
    {   if ( pprof->oper.ttsSelector == TTSNAME::VVoxCore )
        {   if ( !checkfiles())
            {   return;
        }   }
#if 1
        if ( pttsthred == NULL )
        {   pttsthred = new class ttsthred;
            hConnectrequestSelLine = connect(pttsthred, &ttsthred::requestSelLine,
                                         this, &MainWindow::fnrequestSelLine);
        }
        if ( ppreTalk == NULL )
        {   //iPlaylineno = 0;
            ppreTalk = new class preTalk;
//            hConnectrequestNextLineString = connect( ppreTalk, SIGNAL(requestNextLineString()), this, SLOT( fnrequestNextLineString()), Qt::BlockingQueuedConnection );
            hConnectrequestNextLineString = connect( ppreTalk, SIGNAL(requestNextLineString()), this, SLOT( fnrequestNextLineString()), Qt::QueuedConnection );
//            hConnectrequestSelLine = connect( ppreTalk, SIGNAL( preTalk::requestSelLine( const int )), this, SLOT( void MainWindow::fnrequestSelLine( const int )), Qt::QueuedConnection );
//            hConnectrequestSelLine = connect( ppreTalk, SIGNAL( preTalk::requestSelLine(int)), this, SLOT( void MainWindow::fnrequestSelLine(int)), Qt::QueuedConnection );
//            hConnectrequestSelLine = connect( ppreTalk, SIGNAL(requestSelLine(int)), this, SLOT(void fnrequestSelLine(int)), Qt::DirectConnection );
//            hConnectrequestSelLine = connect(ppreTalk, &preTalk::requestSelLine,
//                                 this, &MainWindow::fnrequestSelLine);
            hConnectrequestVolumeUpdate = connect(ppreTalk, &preTalk::requestVolumeUpdate,
                                      this, &MainWindow::on_requestVolumeUpdate);

//            hConnectrequestSelLine = connect( ppreTalk, SIGNAL(requestSelLine()), this, SLOT(void fnrequestSelLine()), Qt::QueuedConnection );
            hConnectrequestStopButtonClick = connect( ppreTalk, SIGNAL(requestStopButtonClick()), this, SLOT( on_StopButton_clicked()), Qt::QueuedConnection );
//            plyStat.setPlayState( PLAYSTATE::PLAY );
            ppreTalk->start( QThread::HighPriority );
//            pttsthred->start( QThread::HighPriority );
        }
        qDebug() << QString("on_PlayButton_clicked::state:%1").arg(plyStat.playState);
        if ( plyStat.getPlayState() == PLAYSTATE::STOP )    // stop->play時はカーソル行から再生開始
        {   // iPlaylineno = 0;
            QTextCursor csr = ui->talkTextEdit->textCursor();
            int selline = csr.blockNumber();
//            if ( selline != beforSel )  // PAUSE時の選択行が変わっていたら
            {   iPlaylineno = selline;
                int lastLine = ui->talkTextEdit->document()->lastBlock().blockNumber();
                if ( iPlaylineno >= lastLine  )
                {   iPlaylineno = 0;
                }
                // 要各キューのフラッシュ
                fnrequestSelLine( iPlaylineno, TRUE );
        }   }
        if ( !ppreTalk->isRunning())
        {   ppreTalk->start( QThread::HighPriority );
        }
        if ( !pttsthred->isRunning())
        {   pttsthred->start( QThread::HighPriority );
        }
        plyStat.setPlayState( PLAYSTATE::PLAY );
//        qDebug() << QString("on_PlayButton_clicked::%1").arg(plyStat.playState);
//        qDebug() << QString("on_PlayButton_clicked::%1:%2:%3:%4:%5").arg(plyStat.playState)
//                        .arg(PLAYSTATE::STOP).arg(PLAYSTATE::PRESTOP).arg(PLAYSTATE::PLAY).arg(PLAYSTATE::PAUSE);
        qDebug() << QString("on_PlayButton_clicked::PLAY");
#endif
        ui->PlayButton->setChecked( TRUE );       // 押されたときにやること
    }
    else
    {
//        QTextCursor csr = ui->talkTextEdit->textCursor();
//        beforSel = csr.blockNumber();
//        beforSel = -1;
        plyStat.setPlayState( PLAYSTATE::PAUSE );
        qDebug() << QString("on_PlayButton_clicked::PAUSE");
        ui->PlayButton->setChecked( FALSE );      // 初期状態に戻った
    }
    qDebug() << QString("on_PlayButton_clicked::exit");
    beforproced = QDateTime::currentMSecsSinceEpoch();
}


/**
 * @fn
 * サーチボタン押し下げ時の処理
 * @brief サーチボタン押し下げ時の処理
 * @sa
 * @detail ボタンプッシュイベントはqtの不具合なのか１プッシュで２度来るので、先ず、２秒以内に来た場合はそのままリターン
 * サーチも２つ目の検索になるので２度来るのは勘弁してほしい
 * 検索ボックスに入力された文字列をトークボックスで検索する
 */
void MainWindow::on_SarchButton_clicked()
{   static qint64 beforproced = 0;
    qint64 currentms, lockms;

    currentms = QDateTime::currentMSecsSinceEpoch();
    lockms = currentms - beforproced - 2000;    // クリックイベントが1クリックで2度来るので
    if ( lockms < 0 )                           // 短時間(2秒)で来たイベントはスルー
    {   return;
    }
    QString str = ui->SarchStr->text();
    QTextCursor csr = ui->talkTextEdit->textCursor();
    QTextDocument *doc = ui->talkTextEdit->document();
    QTextCursor newcsr;
#ifdef Q_OS_WIN
    ushort  shiftKeyState = 0;
//    QTextDocument::FindFlag option = 0;   // オプション無しのシンボルが定義されていないので0をセットしようとしたがエラー

    shiftKeyState = GetKeyState( VK_SHIFT ) & 0x80;
    if ( shiftKeyState )                    // 仕方ないのでオプション有無で呼び分け
    {   newcsr = doc->find( str, csr, QTextDocument::FindBackward );
    }
    else
    {   newcsr = doc->find( str, csr );
    }
#endif
    ui->talkTextEdit->setTextCursor( newcsr );
    iPlaylineno = newcsr.blockNumber();
/*
int QTextCursor::blockNumber() const
     * QTextDocument *QPlainTextEdit::document() const
    QTextCursor	find(const QString &subString, const QTextCursor &cursor, QTextDocument::FindFlags options = FindFlags()) const
    QTextCursor	find(const QString &subString, int position = 0, QTextDocument::FindFlags options = FindFlags()) const
フラグ QTextDocument:: FindFlags
この列挙型は、使用可能なオプションを説明します。QTextDocumentの検索関数。オプションは、次のリストから OR で結合できます。
 絶え間ない	価値	説明
 QTextDocument::FindBackward	0x00001	前方ではなく後方に検索します。
 QTextDocument::FindCaseSensitively	0x00002	デフォルトでは、find は大文字と小文字を区別せずに動作します。このオプションを指定すると、動作が大文字と小文字を区別する検索操作に変更されます。
 QTextDocument::FindWholeWords	0x00004	完全な単語のみを検索して一致させます。
*/
    beforproced = QDateTime::currentMSecsSinceEpoch();
}


/**
 * @fn
 * ボリュームダイヤル回転時の時の処理
 * @brief ボリュームダイヤル回転時の時の処理
 * @param (value) ボリューム値
 * @sa
 * @detail ボリューム値の変更時、ポートオーディオに中継する
 */
void MainWindow::on_Volume_valueChanged(int value)
{
#if 0
    if ( hWaveOut != 0 )    // アプリケーションの音量設定
    {	unsigned int val = ( value << 16 ) | ( value & 0xffff );
        if ( waveOutSetVolume( hWaveOut, val ) != MMSYSERR_NOERROR )
        {	return;
    }	}
#endif
    if ( ppreTalk != NULL )
    {   ppreTalk->setVolume( value );
    }
    pprof->oper.iAppVolume = value;
}


#if 0
void MainWindow::fndelToThisLine()
{
    QMessageBox::warning(this, tr("MessageBox"), tr("これは警告メッセージのサンプルです2"));
    return;
}
#endif


/**
 * @fn
 * 前回読み上げていたファイルの読み込み
 * @brief 前回読み上げていたファイルの読み込み
 * @sa
 * @detail 前回読み上げていたファイルの読み込み、エディットボックスクラスに移動
 */
void MainWindow::fnContinue()
{
    ui->talkTextEdit->LoadTxtFile( *( pprof->oper.recent[0].pwszCurrentPlayFile ));
    iPlaylineno = pprof->oper.recent[0].iCurrentPlayFileLine;
    fnrequestSelLine( iPlaylineno, TRUE );
}


/**
 * @fn
 * エディットボックス内で選択されている文字列をmecabで分かち書きする
 * @brief エディットボックス内で選択されている文字列をmecabで分かち書きする
 * @param (nomsg) 0:選択文字列を分かち書きする　!0:インフォメーションウィンドウを表示するだけ
 * @sa
 * @detail 先ず、インフォメーションウィンドウを表示状態にし
 * エディットボックス内で選択されている文字列をmecabで分かち書きする
 */
void MainWindow::fnCorpas(int nomsg)
{   mecabif mecab;
    soundPlayState  plyStat;
    int err;

/*
    if ( !pCopasDlg )
    {   pCopasDlg = new CorpasDialog;
        int hight = pprof->oper.iSizeCorpaswindowHight;
        int wifth = pprof->oper.iSizeCorpaswindowWidth;
        if (( pprof->oper.iSizeCorpaswindowHight > 100 ) && ( pprof->oper.iSizeCorpaswindowWidth > 100 ))
        {   pCopasDlg->setGeometry( this->geometry().right() + 10, this->geometry().top(),
                                   pprof->oper.iSizeCorpaswindowHight, pprof->oper.iSizeCorpaswindowWidth );
        }   // コーパスウインドウ位置、サイズ指定
    }
*/
    if ( pCopasDlg->isHidden())
    {
        int hight = pprof->oper.iSizeCorpaswindowHight;
        int wifth = pprof->oper.iSizeCorpaswindowWidth;
        if (( pprof->oper.iSizeCorpaswindowHight > 100 ) && ( pprof->oper.iSizeCorpaswindowWidth > 100 ))
        {   pCopasDlg->setGeometry( this->geometry().right() + 10, this->geometry().top(),
                                   pprof->oper.iSizeCorpaswindowHight, pprof->oper.iSizeCorpaswindowWidth );
        }   // コーパスウインドウ位置、サイズ指定
        pCopasDlg->show();
    }
    else
    {   //pCopasDlg->activateWindow();
        pCopasDlg->show();
    }
    if ( nomsg == 0 )
    {   while ( 1 )
        {   if ( ppreTalk != NULL )   // PLAY中でないか？ PLAY中に以下実行するとmecabの解析中文字列が置き換えられ例外に繋がる
            {   if ( plyStat.getPlayState() == PLAYSTATE::PLAY )   // PLAY中でないか？ PLAY中に以下実行するとmecabの解析中文字列が置き換えられ例外に繋がる
                {   //break;
            }   }
            QTextCursor csr = ui->talkTextEdit->textCursor();
            QString str = csr.selectedText();
            QString dst;

            str = str.trimmed();
            if ( !str.isEmpty())                        // 選択文字列があるか？
            {   if (( err = mecab.str2Corpas( str, dst )) < 0 )
                {   if ( err == -5 )
                    {   QMessageBox::warning( this, tr("MessageBox"), tr("選択文字列が長すぎます！"));
                    }
                    else
                    {   QMessageBox::warning( this, tr("MessageBox"), tr("mecab error！"));
                }   }
                msgBuff.enterMessage( dst );            //
            }
            return;
        }
        QMessageBox::warning( this, tr("MessageBox"), tr("朗読中はcorpas生成できません！"));
    }
}


/**
 * @fn
 * エディットボックス内で選択されている行から再生する
 * @brief エディットボックス内で選択されている行から再生する
 * @sa
 * @detail 各スレッドを生成起動して、エディットボックス内で選択されている行から再生する
 *
 */
void MainWindow::fnplayThisLine()
{    soundPlayState  plyStat;

    //    QMessageBox::warning(this, tr("MessageBox"), tr("fnplayThisLine"));
    if ( pttsthred == NULL )
    {   pttsthred = new class ttsthred;
        hConnectrequestSelLine = connect(pttsthred, &ttsthred::requestSelLine,
                                         this, &MainWindow::fnrequestSelLine);
//        hConnectrequestSelLine = connect(pttsthred, &PlayLineSapi::requestSelLine,
//                                         this, &MainWindow::fnrequestSelLine);
        pttsthred->start( QThread::HighPriority );
    }
    if ( ppreTalk == NULL )
    {   ppreTalk = new class preTalk;
        qDebug() << QString("fnplayThisLine::ppreTalk == NULL");
        plyStat.setPlayState( PLAYSTATE::STOP );
        //            hConnectrequestNextLineString = connect( ppreTalk, SIGNAL(requestNextLineString()), this, SLOT( fnrequestNextLineString()), Qt::BlockingQueuedConnection );
        hConnectrequestNextLineString = connect( ppreTalk, SIGNAL(requestNextLineString()), this, SLOT( fnrequestNextLineString()), Qt::QueuedConnection );
        //            hConnectrequestSelLine = connect( ppreTalk, SIGNAL( preTalk::requestSelLine( const int )), this, SLOT( void MainWindow::fnrequestSelLine( const int )), Qt::QueuedConnection );
        //            hConnectrequestSelLine = connect( ppreTalk, SIGNAL( preTalk::requestSelLine(int)), this, SLOT( void MainWindow::fnrequestSelLine(int)), Qt::QueuedConnection );
        //            hConnectrequestSelLine = connect( ppreTalk, SIGNAL(requestSelLine(int)), this, SLOT(void fnrequestSelLine(int)), Qt::DirectConnection );
//        hConnectrequestSelLine = connect(ppreTalk, &preTalk::requestSelLine,
//                                         this, &MainWindow::fnrequestSelLine);
        hConnectrequestVolumeUpdate = connect(ppreTalk, &preTalk::requestVolumeUpdate,
                                              this, &MainWindow::on_requestVolumeUpdate);

        //            hConnectrequestSelLine = connect( ppreTalk, SIGNAL(requestSelLine()), this, SLOT(void fnrequestSelLine()), Qt::QueuedConnection );
        hConnectrequestStopButtonClick = connect( ppreTalk, SIGNAL(requestStopButtonClick()), this, SLOT( on_StopButton_clicked()), Qt::QueuedConnection );
        ppreTalk->start( QThread::HighPriority );
    }
    //        qDebug() << QString("on_PlayButton_clicked::%1:%2:%3:%4:%5").arg(ppreTalk->playState)
    //                        .arg(PLAYSTATE::STOP).arg(PLAYSTATE::PRESTOP).arg(PLAYSTATE::PLAY).arg(PLAYSTATE::PAUSE);
    {   QTextCursor csr = ui->talkTextEdit->textCursor();
        iPlaylineno = csr.blockNumber();
    }
    ui->PlayButton->setChecked( TRUE );       // 押されたときにやること
    plyStat.setPlayState( PLAYSTATE::PLAY );
    return;
}


/**
 * @fn
 * 設定ダイヤを開く
 * @brief 設定ダイヤを開く
 * @sa
 * @detail 設定ダイヤを開く
 *
 */
void MainWindow::fnsettingDlg()
{
    if( !pSettingDlg )
    {   pSettingDlg = new SettingDlg;
//        pSettingDlg = new SettingDialog;
//        pSettingDlg = new TestDialog;
//        connect(secondDialog, SIGNAL(okButtonClicked()), this, SLOT(setTextLabel()));
        connect( pSettingDlg, SIGNAL(sigupdateMaxLine()), pCopasDlg, SLOT( fnupdateMaxLine()));
    }
    if( pSettingDlg->isHidden())
    {   pSettingDlg->show();
    }else
    {   pSettingDlg->activateWindow();
    }
}


/**
 * @fn
 * プリmecabスレッドで置き換えている分の記号読みを置換なくする
 * @brief プリmecabスレッドで置き換えている分の記号読みを置換なくする
 * @sa
 * @detail プリmecabスレッドで置き換えている分の記号読みを置換なくする
 *
 */
void MainWindow::fnsmbolePass()
{
    pprof->oper.symbolSpeakingOff = !( pprof->oper.symbolSpeakingOff );
}


/**
 * @fn
 * モニター休止連動ポーズ設定
 * @brief モニター休止連動ポーズ設定
 * @return なし
 * @sa
 * @detail モニター休止連動ポーズ設定
 */
void MainWindow::fndisplayPwrOff()
{
    pprof->oper.displayOutputOffThenPouse = !( pprof->oper.displayOutputOffThenPouse );
}


/**
 * @fn
 * ミュート設定
 * @brief ミュート設定
 * @return なし
 * @sa
 * @detail ミュート設定　発音せずに辞書未登録語彙をインフォメーションウインドウに抽出する際に使用
 */
void MainWindow::fnmute()
{
    pprof->oper.soundOutputSkip = !( pprof->oper.soundOutputSkip );
}


/**
 * @fn
 * モニター休止連動ポーズ設定状況通知
 * @brief モニター休止連動ポーズ設定状況通知
 * @return なし
 * @sa
 * @detail モニター休止連動ポーズ設定状況通知　メニューにチェックを入れるため
 */
void MainWindow::fnConsoleDisplayState()
{    soundPlayState  plyStat;

//    pprof->oper.displayOutputOffThenPouse = !( pprof->oper.displayOutputOffThenPouse );
    if ( pprof->oper.displayOutputOffThenPouse )
    {   if ( plyStat.getPlayState() == PLAYSTATE::PLAY )
        {   plyStat.setPlayState( PLAYSTATE::PAUSE );
            ui->PlayButton->setChecked( FALSE );      // 初期状態に戻った
    }   }
//    QMessageBox::warning(this, tr("MessageBox"), tr("fnConsoleDisplayState"));
}


/**
 * @fn
 * 次の行の文字列ををキューに積む
 * @brief 次の行の文字列ををキューに積む
 * @return なし
 * @sa
 * @detail 次の行の文字列ををキューに積む　行取得の際選択しないといけないので、
 * 元々選択されていた読み上げ行が非反転になってしまうため、再度選択しなおす
 */
void MainWindow::fnrequestNextLineString()
{   struct LINEDATA NextLine;
    soundPlayState  plyStat;

    if ( plyStat.getPlayState() == PLAYSTATE::STOP )
    {   while ( !ppreTalk->QueTalkText.empty())          // stopなら　キューを空にする
        {   ppreTalk->QueTalkText.dequeue();
        }
        return;
    }
    if ( ui->talkTextEdit->getLine( NextLine.iLineNo = iPlaylineno, NextLine.qsLineStr ))
    {   iPlaylineno++;            // 選択行が変わってしまうので
        NextLine.eof = FALSE;
    }
    else
    {   NextLine.eof = TRUE;
    }
//    qDebug() << QString("fnrequestNextLineString::%1:%2:%3").arg(iPlaylineno).arg(NextLine.eof).arg(NextLine.qsLineStr);
                //    NextLine.eof = !( ui->talkTextEdit->getLine( NextLine.iLineNo = iPlaylineno++, NextLine.qsLineStr ));
    if ( ppreTalk != NULL )
    {   ppreTalk->QueTalkText.enqueue( NextLine );
        qDebug() << QString("ppreTalk->QueTalkText.enqueue(%1[%2]:%3").arg(NextLine.iLineNo).arg(NextLine.qsLineStr).arg(ppreTalk->QueTalkText.count());
        fnrequestSelLine( iSelectlineno, TRUE );  // 元の選択行を選択しなおす
    }
//    qDebug() << QString("fnrequestNextLineString:ret");
}


/**
 * @fn
 * 指定行を選択状態（反転表示）にする。
 * @brief 指定行を選択状態（反転表示）にする。
 * @return なし
 * @sa
 * @detail 読み上げ行を選択（反転表示）しにする、上記の次行文字列取得時に選択行が変わってしまうため
 * 変わってしまうため、ここで選択した行番号を記録しておく
 */
//void MainWindow::fnrequestSelLine( const int lineno )
void MainWindow::fnrequestSelLine( int lineno, BOOL forceUpDate )
{
    if (( iSelectlineno != lineno ) ||  // 選択行更新時か
        ( forceUpDate ))                // 強制更新か
    {
        qDebug() << QString("fnrequestSelLine(%1)").arg(lineno);
        int lastLine = ui->talkTextEdit->document()->lastBlock().blockNumber();
        if ( lastLine > lineno )
        {   pprof->oper.recent[0].iCurrentPlayFileLine = iSelectlineno = lineno;
            ui->talkTextEdit->selectLine( lineno );
    }   }
}


/**
 * @fn
 * 辞書未登録語彙をインフォメーションウィンドウに表示するために送る
 * @brief 辞書未登録語彙をインフォメーションウィンドウに表示するために送る
 * @return なし
 * @sa
 * @detail インフォメーションウィンドウを表示状態にし
 * 辞書未登録語彙をインフォメーションウィンドウに表示するために送る
 */
void MainWindow::fndisplayUnnounStr( QString szSurface )
{
//    QMessageBox::warning(this, tr("MessageBox"), tr("fndisplayUnnounStr"));
    if ( !pCopasDlg )
    {   pCopasDlg = new CorpasDialog;
        int hight = pprof->oper.iSizeCorpaswindowHight;
        int wifth = pprof->oper.iSizeCorpaswindowWidth;
        if (( pprof->oper.iSizeCorpaswindowHight > 100 ) && ( pprof->oper.iSizeCorpaswindowWidth > 100 ))
        {   pCopasDlg->setGeometry( this->geometry().right() + 10, this->geometry().top(),
                                   pprof->oper.iSizeCorpaswindowHight, pprof->oper.iSizeCorpaswindowWidth );
        }   // コーパスウインドウ位置、サイズ指定
    }
    if ( pCopasDlg->isHidden())
    {   pCopasDlg->show();
    }
    else
    {   pCopasDlg->activateWindow();
    }
    msgBuff.enterMessage( szSurface );    //
}


/**
 * @fn
 * トークテキストエディットボックスに文字が書き込まれているかチェックする
 * @brief トークテキストエディットボックスに文字が書き込まれているかチェックする
 * @return なし
 * @sa
 * @detail トークテキストエディットボックスに文字が書き込まれているかチェックする
 * エディットボックスが空の時にPLAYボタンを無効にするため
 */
void MainWindow::on_talkTextEdit_textChanged()
{
    ui->PlayButton->setEnabled(( ui->talkTextEdit->document()->characterCount() <= 0 )? false: true );
}


/**
 * @fn
 * ボリュームダイヤルに値をセットする　起動時などにダイヤルへの更新
 * @brief ボリュームダイヤルに値をセットする　起動時などにダイヤルへの更新
 * @return なし
 * @sa
 * @detail ボリュームダイヤルに値をセットする　起動時などにダイヤルへの更新
 */
void MainWindow::on_requestVolumeUpdate(int vol)
//void MainWindow::on_requestVolumeUpdate()
{
    ui->Volume->setValue( vol );
//    return;
}


/**
 * @fn
 * ボリュームダイヤルプッシュ時の処理
 * @brief ボリュームダイヤルプッシュ時の処理
 * @return なし
 * @sa
 * @detail ボリュームダイヤルプッシュ時の処理　ダイヤルプッシュでミュート出来るかと
 * 思ったが、値が変わるだけだった・・・
 */
void MainWindow::onVolumePressed()
{

}


/**
 * @fn
 * ウインドウタイトル更新
 * @brief ウインドウタイトル更新
 * @return なし
 * @param (title) 読み込んだファイル名
 * @sa
 * @detail ウインドウタイトル更新　読み込んだファイル名をアプリ名の後に追加し表示する
 */
void MainWindow::on_requestTitleUpdate( QString title )
{   QString orgTitle, updTitle;

    orgTitle = windowTitle();
    int n = orgTitle.indexOf(" ");
    if ( n > 0 )
    {   updTitle = orgTitle.mid(0, n);
    }
    else
    {   updTitle = orgTitle;
    }
    updTitle += " " + title;
    setWindowTitle( updTitle );
    return;
}


/**
 * @fn
 * バージョンダイヤログの起動
 * @brief バージョンダイヤログの起動
 * @return なし
 * @sa
 * @detail バージョンダイヤログの起動
 */
void MainWindow::fnAboutbox()
{   About about;

    about.exec();
//    QMessageBox::warning(this, tr("MessageBox"), tr("fnAboutbox"));
}


/**
 * @fn
 * 自身のリスタート処理
 * @brief 自身のリスタート処理
 * @return なし
 * @sa
 * @detail 自身のリスタート処理　dllのオープンしているファイルが、dllアンロードしてもロック解除されないためリスタートで対応する
 */
void MainWindow::fnreStart()
{   TCHAR strPath[MAX_PATH];
    TCHAR strCurDir[MAX_PATH];
    TCHAR strArg[MAX_PATH];

//    ::GetModuleFileName( AfxGetInstanceHandle(), strPath, sizeof( strPath ) / sizeof( TCHAR ));
    ::GetModuleFileName( NULL, strPath, sizeof( strPath ) / sizeof( TCHAR ));
    ::GetCurrentDirectory( sizeof( strCurDir ) / sizeof( TCHAR ), strCurDir );
    // 元プロセスのコマンドライン引数を継承する必要があるか検討して下さい。
    DWORD dwProcessId = ::GetCurrentProcessId();
    swprintf( strArg, MAX_PATH, L"-waitterminate %u", dwProcessId );
    qDebug() << QString("fnreStart：strPath(%1)").arg(strPath);
    qDebug() << QString("fnreStart：strCurDir(%1)").arg(strCurDir);
    qDebug() << QString("fnreStart：strArg(%1)").arg(strArg);
    ::ShellExecute( NULL, NULL, strPath, strArg, strCurDir, SW_SHOWNORMAL );
    QApplication::exit( 0 );     // アプリケーションを終了する。
}


