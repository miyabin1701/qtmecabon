/**
 * @file exTextEdit.cpp
 * @brief メインウインドウのテキストエディタのサブクラス　フォントの拡大縮小、ポップアップメニューハンドラ
 * @author m.kawakami
 * @date 23/10/??
 */

#include "exTextEdit.h"
#include "profilevalues.h"
#include "cmultilang.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QWheelEvent>
#include <windows.h>
//#include <QTextCodec>
#include <QtCore/QDebug>
#include <QFileDialog>
#include <QApplication>
#include "recentsel.h"

//#include <icu.h>
//#include <UCharsetDetector>

extern profileValues   *pprof;
extern void Pdf2Txt( wchar_t *szFile, wchar_t *sztxtFile );

#if 0

//  http://mochiuwiki.e2.valueserver.jp/index.php?title=Qt%E3%81%AE%E5%9F%BA%E7%A4%8E_-_%E6%96%87%E5%AD%97%E3%82%B3%E3%83%BC%E3%83%89

enum charset = { UTF-8,  };

//    https://ykot.hateblo.jp/entry/20110112/1294821848
static charset jugementCharSet( QString text )
{
    UErrorCode error = U_ZERO_ERROR;
UCharsetDetector* detector = ucsdet_open(&error);
if (U_FAILURE(error))
    return NULL;

ucsdet_setText(detector, s.c_str(), s.size(), &error);
if (U_FAILURE(error))
    return NULL;
const UCharsetMatch* match = ucsdet_detect(detector, &error);
if (U_FAILURE(error))
    return NULL;
std::cout << "     neme:" << ucsdet_getName(match, &error)
          << " language:" << ucsdet_getLanguage(match, &error)
          << std::endl;


//https://cpp.hotexamples.com/jp/examples/-/-/ucsdet_close/cpp-ucsdet_close-function-examples.html
#ifdef TRUE //HAVE_ICU
UErrorCode status = U_ZERO_ERROR;
UCharsetDetector* detector = ucsdet_open(&status);
if (detector && !U_FAILURE(status))
{
    ucsdet_setText(detector, text.constData(), text.length(), &status);
    if (!U_FAILURE(status))
    {
        const UCharsetMatch* match = ucsdet_detect(detector, &status);
        if (match && !U_FAILURE(status))
            encoding = ucsdet_getName(match, &status);
    }
}

if (U_FAILURE(status)) {
    qWarning("detectEncoding() failed: %s", u_errorName(status));
}

ucsdet_close(detector);
#endif // HAVE_ICU

}
#endif


/**
 * @fn
 * exTextEditクラスコンストラクタ
 * @brief exTextEditクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail ファイルのドロップ受け入れ設定、センタースクロール設定
 */
//exTextEdit::exTextEdit(QWidget *parent) : QTextEdit(parent)
exTextEdit::exTextEdit(QWidget *parent) : QPlainTextEdit(parent)
{
    setAcceptDrops( true );
    setCenterOnScroll( true );
//    setStyleSheet( "QTextEdit{background-color:black; color:white; selection-background-color:green}");
//    setStyleSheet( "QPlainTextEdit{background-color:black; color:white; selection-background-color:green}");

}


/**
 * @fn
 * ポップっプメニュー表示前設定
 * @brief ポップっプメニュー表示前設定
 * @param (*event) イベント
 * @return なし
 * @sa
 * @detail 独自ポップアップメニュー、セパレータの追加
 */
void exTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    QAction* point;
    QAction *save = new QAction( tr( "上書き保存" ), this );
    connect( save, SIGNAL(triggered()), this, SLOT(fnsave()));
    QAction *saveas = new QAction( tr( "名前を付けて保存" ), this );
    connect( saveas, SIGNAL(triggered()), this, SLOT(fnsaveas()));
    QAction *delToThisLine = new QAction( tr( "カーソル行まで削除" ), this );
//    connect( delToThisLine, SIGNAL(triggered()), parent(), SLOT(MainWindow::fndelToThisLine()));
    connect( delToThisLine, SIGNAL(triggered()), this, SLOT(fndelToThisLine()));
     QAction *delAllLine = new QAction( tr( "全削除" ), this );
    connect( delAllLine, SIGNAL(triggered()), this, SLOT(fndelAllLine()));
    QAction *Corpas = new QAction( tr( "Corpas" ), this );
    connect( Corpas, SIGNAL(triggered()), this, SLOT(fnCorpas()));
    QAction *playThisLine = new QAction( tr( "カーソル行から再生" ), this );
    connect( playThisLine, SIGNAL(triggered()), this, SLOT(fnplayThisLine()));
    QAction *settingDlg = new QAction( tr( "設定" ), this );
    connect( settingDlg, SIGNAL(triggered()), this, SLOT(fnsettingDlg()));
//    QAction *smbolePass = new QAction( tr( "記号読みパス" ), this );
//    connect( smbolePass, SIGNAL(triggered()), this, SLOT(fnsmbolePass()));
    // https://www.cloverfield.co.jp/2019/09/02/%E3%83%A9%E3%82%BA%E3%83%91%E3%82%A4%E3%83%87%E3%82%B9%E3%82%AF%E3%83%88%E3%83%83%E3%83%97%E3%81%A7pyqt%E5%85%A5%E9%96%804submenucheckmenucontextmenu/
    // チェッカブルメニューについて
//    smbolePass->setCheckable( TRUE );
//    smbolePass->setChecked( pprof->oper.symbolSpeakingOff );
//    QAction *displayPwrOff = new QAction( tr( "ディスプレイ電源連動Pause" ), this );
//    connect( displayPwrOff, SIGNAL(triggered()), this, SLOT(fndisplayPwrOff()));
//    displayPwrOff->setCheckable( TRUE );
//    displayPwrOff->setChecked( pprof->oper.displayOutputOffThenPouse );
    // viewStatAct.setChecked(True)
    QAction *mute = new QAction( tr( "発声スキップ" ), this );
    connect( mute, SIGNAL(triggered()), this, SLOT(fnmute()));
    mute->setCheckable( TRUE );
    mute->setChecked( pprof->oper.soundOutputSkip );
    QAction *loadLastFile = new QAction( tr( "前回の続きロード" ), this );
    connect( loadLastFile, SIGNAL(triggered()), this, SLOT(fnContinue()));

    point = menu->actions().at(8);
    menu->insertSeparator(point);
    point = menu->actions().at(9);
    menu->insertAction(point, delToThisLine);
    point = menu->actions().at(10);
    menu->insertAction(point, delAllLine);
    point = menu->actions().at(11);
    menu->insertSeparator(point);
    menu->addAction( save );              //    menu->addAction(tr("Corpas"));
    menu->addAction( saveas );              //    menu->addAction(tr("Corpas"));
    point = menu->actions().at(11);
    menu->addAction( Corpas );              //    menu->addAction(tr("Corpas"));
    menu->addAction( playThisLine );        //    menu->addAction(tr("カーソル行から再生"));
    point = menu->actions().at(13);
    menu->insertSeparator(point);
    menu->addAction( settingDlg );          //    menu->addAction(tr("設定"));
    point = menu->actions().at(16);
    menu->insertSeparator(point);
//    menu->addAction( smbolePass );          //    menu->addAction(tr("記号読みパス"));
//    menu->addAction( displayPwrOff );       //    menu->addAction(tr("ディスプレイ電源連動Pause"));
    menu->addAction( mute );                //    menu->addAction(tr("発声スキップ"));
    menu->addAction( loadLastFile );        //    menu->addAction(tr("前回の続きロード"));
    menu->exec( event->globalPos());
    delete menu;
}


/**
 * @fn
 * テキストファイルの読み込み
 * @brief テキストファイルの読み込み
 * @param (filepath) テキストファイルのパス
 * @return なし
 * @sa
 * @detail ファイルを文字コード変換しながらエディットに読み込む
 */
void exTextEdit::LoadTxtFile( QString filepath )
{   QFile file( filepath );
    QFileInfo fInfo;
    fInfo.setFile( filepath );
    QString analyzeBuf;
    int codepage;
    CMultiLang cml;
    BOOL fgValid = FALSE;

    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ))
        return;
    QTextStream InStream( &file );
    analyzeBuf = InStream.read( MAX_LINE );
    InStream.seek( 0 );
    codepage = cml.analizCodePage(( char * )analyzeBuf.toStdString().c_str(), analyzeBuf.length());
    switch ( codepage )
    { case 932:    // 日本語シフトJIS setPlainText(const QString &text)
        InStream.setEncoding( QStringConverter::System );
        setPlainText(InStream.readAll());   // QStringはいつdeleteされる？
        emit requestTitleUpdate( fInfo.fileName());
        fgValid = TRUE;
        break;
      case 65001:    // UTF-8
        InStream.setEncoding( QStringConverter::Utf8 );
        setPlainText(InStream.readAll());   // QStringはいつdeleteされる？
        emit requestTitleUpdate( fInfo.fileName());
        fgValid = TRUE;
        break;
      case 1200:     // UTF-16LE
        InStream.setEncoding( QStringConverter::Utf16LE );
        setPlainText(InStream.readAll());
        emit requestTitleUpdate( fInfo.fileName());
        fgValid = TRUE;
        break;
      case 1201:     // UTF-16BE
        InStream.setEncoding( QStringConverter::Utf16BE );
        setPlainText(InStream.readAll());
        emit requestTitleUpdate( fInfo.fileName());
        fgValid = TRUE;
        break;
      default:     //
        emit requestTitleUpdate( QString( "" ));
        break;
    }
    if ( pprof->oper.idisplayMagnify > 0 )
    {   zoomIn( pprof->oper.idisplayMagnify );
    }
    else if ( pprof->oper.idisplayMagnify < 0 )
    {   zoomOut( -pprof->oper.idisplayMagnify );
    }
    if ( fgValid == TRUE )      // ドロップファイル読み込み有効 リセントリストの更新
    {   pprof->updateRecentFile( filepath );
#if 0
        QString dropFile = filepath;
        QString *swapSaveFileName;
        int i, swapSaveFileLineno;

        for ( i = 0; i < lengthof( pprof->oper.recent ); i++ )    // 同じファイルが在るかチェック
        {   if ( dropFile == *( pprof->oper.recent[i].pwszCurrentPlayFile ))    // 在ったら　リスト先頭に移動　行数更新・・・読み込み時だから不明
            {   if ( i > 0 )    // 先頭でなければ
                {   swapSaveFileName = pprof->oper.recent[i].pwszCurrentPlayFile;
                    swapSaveFileLineno = pprof->oper.recent[i].iCurrentPlayFileLine;
                    for ( ; i > 0; i-- )
                    {   pprof->oper.recent[i].pwszCurrentPlayFile =
                            pprof->oper.recent[i - 1].pwszCurrentPlayFile;
                        pprof->oper.recent[i].iCurrentPlayFileLine =
                            pprof->oper.recent[i - 1].iCurrentPlayFileLine;
                    }
                    pprof->oper.recent[0].pwszCurrentPlayFile = swapSaveFileName;
                    pprof->oper.recent[0].iCurrentPlayFileLine = swapSaveFileLineno;
                }
//                i = 0;
                return;
            }
        }
//        if ( i >= lengthof( pprof->oper.recent ))    // 無かったらリストシフトし先頭に入れる
        {   i = lengthof( pprof->oper.recent ) - 1;
            if ( pprof->oper.recent[i].pwszCurrentPlayFile != NULL )
            {   free( pprof->oper.recent[i].pwszCurrentPlayFile );
            }
            for ( ; i > 0; i-- )
            {   pprof->oper.recent[i].pwszCurrentPlayFile =
                    pprof->oper.recent[i - 1].pwszCurrentPlayFile;
                pprof->oper.recent[i].iCurrentPlayFileLine =
                    pprof->oper.recent[i - 1].iCurrentPlayFileLine;
            }
            pprof->oper.recent[0].pwszCurrentPlayFile = new ( QString )( dropFile );
            pprof->oper.recent[0].iCurrentPlayFileLine = 1;
        }
#endif
    }
}


/**
 * @fn
 * ドラッグエンターイベント処理
 * @brief ドラッグエンターイベント処理
 * @param (*event) イベントデータ
 * @return なし
 * @sa
 * @detail イベント処理を行う
 */
void exTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->urls().empty())
        event->acceptProposedAction();
}


/**
 * @fn
 * ドロップイベント処理
 * @brief ドロップイベント処理
 * @param (*event) イベントデータ
 * @return なし
 * @sa
 * @detail ドロップイベント処理を行う（ドロップファイルの拡張子が.pdfならpdf変換後のテキストファイルを読み込む、.txtならそのまま読み込む）
 */
void exTextEdit::dropEvent(QDropEvent *event)
{   QFileInfo fInfo;
    fInfo.setFile( event->mimeData()->urls().first().toLocalFile());
    QString ext = fInfo.suffix();
//    ext = ext.toLower();

    if ( ext.toLower() == "pdf" )                     // pdf -> txt変換し　変換後のtxtファイルをロード
    {   wchar_t wszFile[_MAX_PATH * 2], *p, *srcpdf;
        QApplication::setOverrideCursor( Qt::WaitCursor );
//        QCursor waitcsr( Qt::WaitCursor );

        memset( wszFile, 0, sizeof( wszFile ));
        event->mimeData()->urls().first().toLocalFile().toWCharArray( wszFile );
        srcpdf = _wcsdup( wszFile );
        if (( p = wcsstr( wszFile, L".pdf" )) != NULL )
        {   wcscpy( p, L".txt" );
            Pdf2Txt( srcpdf, wszFile );                 // pdf loader -> txt
            QString qsTxtFile = QString::fromWCharArray( wszFile );
            LoadTxtFile( qsTxtFile );		// txt loader
            event->acceptProposedAction();
        }
        free( srcpdf );
        QApplication::restoreOverrideCursor();
    }
    else if ( ext == "txt" )
    {   LoadTxtFile( event->mimeData()->urls().first().toLocalFile());
        event->acceptProposedAction();
    }
}


/**
 * @fn
 * マウスホイールイベントハンドラ
 * @brief マウスホイールイベントハンドラ
 * @param (*event) イベントデータ
 * @return なし
 * @sa
 * @detail ctrl+ホイール方向でエディットボックスの文字サイズの拡大、縮小を行う
 */
void exTextEdit::wheelEvent(QWheelEvent *pEevent)
{
    // http://mochiuwiki.e2.valueserver.jp/index.php?title=Qt%E3%81%AE%E3%82%B3%E3%83%B3%E3%83%88%E3%83%AD%E3%83%BC%E3%83%AB_-_%E3%83%9E%E3%82%A6%E3%82%B9
    // マウスホイールの前方 : 正の値
    // マウスホイールの後方 : 負の値
    // QWheelEventクラスのdeltaメソッドの値 : マウスホイールを回した角度の8倍の値
    // 標準的なマウスでは1ステップ15度のため、8 * 15 = 120で除算する
    //    double dSteps = (double)pEevent->angleDelta() / 120.0;
    // VK_SHIFT 	0x10	Shift キー
    // VK_CONTROL	0x11	Ctrl キー
    // VK_MENU  	0x12	ALT キー

    double dSteps = ( int )pEevent->angleDelta().y();
    ushort  ctrlKeyState = 0;
#ifdef Q_OS_WIN
    ctrlKeyState = GetKeyState( VK_CONTROL ) & 0x80;
#endif
    if ( ctrlKeyState )
    {   if ( dSteps > 0 )
        {   zoomIn( 1 );
            pprof->oper.idisplayMagnify++;
        }
        else
        {   zoomOut( 1 );
            pprof->oper.idisplayMagnify--;
    }   }
    else
    {   QPlainTextEdit::wheelEvent( pEevent );
    }
}
//void QTextEdit:: setFontPointSize ( qreal s )
//    現在のフォーマットのポイント サイズをsに設定します。
//    sがゼロまたは負の場合、この関数の動作は定義されないことに注意してください。
//    fontPointSize ()

/*
//https://kenkyu-note.hatenablog.com/entry/2020/09/21/181100
bool exTextEdit::eventFilter(QObject* object, QEvent* event)
{
    if (object != this)
        return false;
    auto t = event->type();
    if (t == QEvent::Wheel)
    {
        QWheelEvent* Wheel = static_cast<QWheelEvent*>(event);
        wheelEvent(Wheel);
    }
    return false;
}
*/


/**
 * @fn
 * エディットボックス先頭からカーソル行まで削除する
 * @brief エディットボックス先頭からカーソル行まで削除する
 * @return なし
 * @sa
 * @detail 聞き終わった部分を削除する場合に使用、ファイルを変更削除していい場合のみ
 */
#if 1
void exTextEdit::fndelToThisLine()
{   QTextBlock block = document()->findBlockByNumber(0);

//    QMessageBox::warning(this, tr("MessageBox"), tr("これは警告メッセージのサンプルです"));
    QTextCursor csr = textCursor();
    int nextBlockNo = csr.blockNumber() + 1;
    QTextBlock nextBlock = document()->findBlockByNumber( nextBlockNo );
    csr.setPosition( nextBlock.position(), QTextCursor::MoveAnchor );   //選択位置の次行先頭にカーソルを持ってくる
    csr.setPosition( block.position(), QTextCursor::KeepAnchor );
    setTextCursor(csr);
    cut();
}
#endif


/**
 * @fn
 * エディットボックス内データを全て削除する
 * @brief エディットボックス内データを全て削除する
 * @return なし
 * @sa
 * @detail エディットボックス内データを全て削除する
 */
void exTextEdit::fndelAllLine()
{
    if ( QMessageBox::question(this, tr("MessageBox"), tr("全削除しますか？"),
            QMessageBox::Cancel | QMessageBox::Ok ) == QMessageBox::Cancel )
    {   return;
    }
    selectAll();
    cut();
}


/**
 * @fn
 * エディットボックス内で選択されている文字列をmecabで分かち書きする
 * @brief エディットボックス内で選択されている文字列をmecabで分かち書きする
 * @return なし
 * @sa
 * @detail エディットボックス内で選択されている文字列をmecabで分かち書きする
 */
//https://qiita.com/hoshianaaa/items/51b201f80b003c7b8565
void exTextEdit::fnCorpas()
{
//    QMessageBox::warning(this, tr("MessageBox"), tr("fnCorpas"));
    emit sigCorpas();
}


/**
 * @fn
 * エディットボックス内で選択されている行から再生する
 * @brief エディットボックス内で選択されている行から再生する
 * @return なし
 * @sa
 * @detail エディットボックス内で選択されている行から再生する
 */
void exTextEdit::fnplayThisLine()
{
//    QMessageBox::warning(this, tr("MessageBox"), tr("fnplayThisLine"));
    emit sigplayThisLine();
}


/**
 * @fn
 * 設定ダイヤログを表示する
 * @brief 設定ダイヤログを表示する
 * @return なし
 * @sa
 * @detail 設定ダイヤログを表示する
 */
void exTextEdit::fnsettingDlg()
{
//    QMessageBox::warning(this, tr("MessageBox"), tr("fnsettingDlg"));
    emit sigsettingDlg();
}


/**
 * @fn
 * このプログラム内で置き換えている記号の読みをスキップする
 * @brief このプログラム内で置き換えている記号の読みをスキップする
 * @return なし
 * @sa
 * @detail このプログラム内で置き換えている記号の読みをスキップする
 */
void exTextEdit::fnsmbolePass()
{
//    QMessageBox::warning(this, tr("MessageBox"), tr("fnsmbolePass"));
    emit sigsmbolePass();
}


/**
 * @fn
 * モニター休止連動ポーズ設定
 * @brief モニター休止連動ポーズ設定
 * @return なし
 * @sa
 * @detail モニター休止連動ポーズ設定
 */
void exTextEdit::fndisplayPwrOff()
{
//    QMessageBox::warning(this, tr("MessageBox"), tr("fndisplayPwrOff"));
    emit sigdisplayPwrOff();
}


/**
 * @fn
 * ミュート設定
 * @brief ミュート設定
 * @return なし
 * @sa
 * @detail ミュート設定　発音せずに辞書未登録語彙をインフォメーションウインドウに抽出する際に使用
 */
void exTextEdit::fnmute()
{
//    QMessageBox::warning(this, tr("MessageBox"), tr("fnmute"));
    emit sigmute();
}


/**
 * @fn
 * リセントリストの表示、選択したファイルの読み込みを行う
 * @brief リセントリストの表示、選択したファイルの読み込みを行う
 * @return なし
 * @sa
 * @detail 履歴を表示し、選択した履歴のファイルを読み込む
 */
void exTextEdit::fnContinue()
{   recentSel recent;

    if ( recent.exec())
    {   //QString filePaths = recent.selectedFiles().at(0);
//        QListWidgetItem *pItem = recent.ui->recentListWidget->currentItem();
//        QString str = pItem->text();
//        ui->recentListWidget;
//        QMessageBox::warning(this, tr("MessageBox"), tr("fnContinue"));
        emit sigContinue();
    }
}


/**
 * @fn
 * 読み上げ該当行を選択する
 * @brief 読み上げ該当行を選択する
 * @param (lineno) 行番号
 * @return なし
 * @sa
 * @detail 現在読み上げている行を選択し、表示エリアから離れていたらスクロールするが、
 * イベント＋キューを2重に通過するため、ずれが発声する場合が在る。
 */
void exTextEdit::selectLine( int lineno )
{
    qDebug() << QString("selectLine(%1)").arg(lineno);
    QTextBlock block = document()->findBlockByNumber( lineno );
    if ( !block.isValid())    //ブロックが無効なら何もしない
    {   return;
    }
    selLine = lineno;
    QTextCursor cursor( block );
//    start = block.position();
    cursor.setPosition( block.position(), QTextCursor::MoveAnchor );    //選択位置の初めにカーソルを持ってくる
    if ( block.position() != document()->lastBlock().position()) {      //選択位置の終わりまでカーソルを移動させる。
        cursor.setPosition( block.position() + block.length() - 1, QTextCursor::KeepAnchor );
//        stop = block.position() + block.length() - 1;
//        len = stop - start;
        qDebug() << QString("line:=%1, text=%2").arg(lineno).arg(block.text());
    }
    else
        cursor.setPosition( block.position() + block.length() - 1, QTextCursor::KeepAnchor );
    setTextCursor( cursor );
//    show();
}
/*
 * 下記デバッグログのように毎ライン呼ばれているが、反転表示は1行おきになる
"line:=0, start=0, stop=20, text=CMakeLists.txtで試したこと"
"line:=1, start=21, stop=47, text=set(BUILD_SHARED_LIBS YES)"
"line:=2, start=48, stop=80, text=find_package(portaudio REQUIRED)"
"line:=3, start=81, stop=107, text=add_library(portaudio_x64)"
"line:=4, start=108, stop=158, text=add_library(portaudio_x64 MODULE EXCLUDE_FROM_ALL)"
"line:=5, start=159, stop=220, text=target_link_libraries(portaudio_x64 PUBLIC portaudio_x64.lib)"
"line:=6, start=221, stop=243, text=いずれもリンクエラー解消に至りませんでした。"
"line:=7, start=244, stop=250, text=次にソースに"
"line:=8, start=251, stop=292, text=#pragma comment(lib, \"portaudio_x64.lib\")"
"line:=9, start=293, stop=332, text=を追記したところリンクエラーは消えましたが、実行時main()に到達するまでに"
"line:=10, start=333, stop=382, text=「アプリケーションを正しく起動できませんでした。(0xc000007b)」で終了するしかない状態に"
"line:=11, start=383, stop=429, text=portaudio.dllを呼び出している部分を全てコメントアウトし、上記の#pragmaも"
"line:=12, start=430, stop=449, text=コメントアウトすると起動するようです。"
 */

/*
 * textCursor()
 * setTextCursor(const QTextCursor &cursor)
 *
 *
//  void QTextCursor::setPosition(int pos, QTextCursor::MoveMode m = MoveAnchor)
//QTextCursor::MoveAnchor	0	アンカーをカーソル自体と同じ位置に移動します。
//QTextCursor::KeepAnchor	1	アンカーをその場に保持します。
//bool QTextCursor::movePosition(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode = MoveAnchor, int n = 1)
QTextCursor::NoMove	0	カーソルをその位置に置いたままにします
    QTextCursor::Start	1	文書の先頭に移動します。
        QTextCursor::StartOfLine	3	現在の行の先頭に移動します。
    QTextCursor::StartOfBlock	4	現在のブロックの先頭に移動します。
    QTextCursor::StartOfWord	5	現在の単語の先頭に移動します。
        QTextCursor::PreviousBlock	6	前のブロックの先頭に移動します。
    QTextCursor::PreviousCharacter	7	前の文字に移動します。
    QTextCursor::PreviousWord	8	前の単語の先頭に移動します。
    QTextCursor::Up	2	1 行上に移動します。
        QTextCursor::Left	9	1 文字左に移動します。
    QTextCursor::WordLeft	10	1 単語左に移動します。
    QTextCursor::End	11	文書の最後に移動します。
        QTextCursor::EndOfLine	13	現在の行の末尾に移動します。
    QTextCursor::EndOfWord	14	現在の単語の末尾に移動します。
        QTextCursor::EndOfBlock	15	現在のブロックの最後に移動します。
    QTextCursor::NextBlock	16	次のブロックの先頭に移動します。
    QTextCursor::NextCharacter	17	次の文字に移動します。
    QTextCursor::NextWord	18	次の単語に移動します。
    QTextCursor::Down	12	1 行下に移動します。
        QTextCursor::Right	19	1 文字右に移動します。
    QTextCursor::WordRight	20	1 単語右に移動します。
    QTextCursor::NextCell	21	現在のテーブル内の次のテーブル セルの先頭に移動します。現在のセルが行の最後のセルである場合、カーソルは次の行の最初のセルに移動します。
    QTextCursor::PreviousCell	22	現在のテーブル内の前のテーブル セルの先頭に移動します。現在のセルが行の最初のセルの場合、カーソルは前の行の最後のセルに移動します。
    QTextCursor::NextRow	23	現在のテーブルの次の行の最初の新しいセルに移動します。
        QTextCursor::PreviousRow	24	現在のテーブルの前の行の最後のセルに移動します。
*/


/**
 * @fn
 * エディットボックスから１行取得する
 * @brief エディットボックスから１行取得する
 * @param (lineno) 行番号
 * @param (&QstrLine) 行データストア先
 * @return false:エラー/true:OK
 * @sa
 * @detail エディットボックスから１行取得する
 */
bool exTextEdit::getLine( int lineno, QString &QstrLine )
{
    QTextBlock block = document()->findBlockByNumber( lineno );
    if ( !block.isValid())    //ブロックが無効なら何もしない
    {   return FALSE;
    }
    QstrLine = block.text();
//    qDebug() << QString("lineno:=%1, text=%4").arg(lineno).arg(QstrLine);
    selectLine( selLine );
    return ( TRUE );
}


/**
 * @fn
 * エディットボックス内のデータをセーブする
 * @brief エディットボックス内のデータをセーブする
 * @return なし
 * @sa
 * @detail エディットボックス内のデータを上書き保存する　辞書を改善せずに元のデータを変更で読みの改善の可能性
 */
void exTextEdit::fnsave()
{
    if( pprof->oper.recent[0].pwszCurrentPlayFile->isEmpty())
    {   fnsaveas();
        return;
    }
    if ( QMessageBox::question( this, *pprof->oper.recent[0].pwszCurrentPlayFile,
            tr("上書きしてよろしいでしょうか？"),
                    QMessageBox::Cancel | QMessageBox::Ok ) == QMessageBox::Cancel )
    {   return;
    }
    QFile saveFile( *pprof->oper.recent[0].pwszCurrentPlayFile );
    saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
    saveFile.write( toPlainText().toUtf8().toStdString().c_str());
}


/**
 * @fn
 * エディットボックス内のデータをセーブする
 * @brief エディットボックス内のデータをセーブする
 * @return なし
 * @sa
 * @detail エディットボックス内のデータを名前を付けて保存する　辞書を改善せずに元のデータを変更で読みの改善の可能性
 */
void exTextEdit::fnsaveas()
{   QString SelFilter = "";
    QString FileName = QFileDialog::getSaveFileName(this, tr("名前を付けて保存"), ".",
            tr("txt document(*.txt)"),
                    &SelFilter, QFileDialog::DontUseCustomDirectoryIcons);
    if( FileName.isEmpty())
    {  return;
    }
    QFile saveFile( FileName );
    saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
    saveFile.write( toPlainText().toUtf8().toStdString().c_str());
    pprof->updateRecentFile( FileName );
    emit requestTitleUpdate( FileName );
}


