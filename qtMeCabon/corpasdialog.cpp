/**
 * @file corpasdialog.cpp
 * @brief サブ（インフォメーション）ウインドウの処理
 * @author m.kawakami
 * @date 23/10/??
 */

#include "mainwindow.h"
#include "corpasdialog.h"
#include "ui_corpasdialog.h"
#include "profilevalues.h"
#include "messagebuff.h"
#include <QMenu>
#include <QMessageBox>


extern profileValues   *pprof;


/**
 * @fn
 * CorpasDialogクラスコンストラクタ
 * @brief CorpasDialogクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail コンストラクタ、エディットのバックカラー＆最大行数設定、タイマーの設定
 */
CorpasDialog::CorpasDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CorpasDialog)
{
    ui->setupUi(this);
    CorpasTimer = startTimer( 500 );    //500ミリ秒間隔のタイマーをスタート
    ui->CorpasEdit->setMaximumBlockCount( pprof->other.infoWndLineMaxLimmit );    // 最大行数設定
    if ( pprof->other.selBakeColorBlack )
    {   ui->CorpasEdit->setStyleSheet( "QPlainTextEdit{background-color:black; color:white; selection-color:yellow; selection-background-color:darkGreen}");
    }   // http://dorafop.my.coocan.jp/Qt/Qt105.html color define
    else
    {   ui->CorpasEdit->setStyleSheet( "QPlainTextEdit{background-color:lightGray; color:black; selection-color:cyan; selection-background-color:black}");
    }
    connect( ui->CorpasEdit, SIGNAL(sigreStart()), this, SLOT( fnreStart()));
}


/**
 * @fn
 * CorpasDialogクラスデストラクタ
 * @brief CorpasDialogクラスデストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail タイマーの等の後始末
 */
CorpasDialog::~CorpasDialog()
{   killTimer( CorpasTimer );
    delete ui;
}


/**
 * @fn
 * closeEventハンドラ
 * @brief closeEventハンドラ
 * @param (*event) イベント引数
 * @return なし
 * @sa
 * @detail 表示サイズの取得、次回クリエイト時に再現するためプロファイル値にセット
 */
// Qtでは、closeEventをオーバーライドする事で実現可能です。
void CorpasDialog::closeEvent( QCloseEvent *event )
{   if ( pprof != NULL )
    {   pprof->oper.iSizeCorpaswindowWidth = this->geometry().width();
        pprof->oper.iSizeCorpaswindowHight = this->geometry().height();
}   }


/**
 * @fn
 * timerEventハンドラ
 * @brief timerEventハンドラ
 * @param (*e) イベント引数
 * @return なし
 * @sa
 * @detail タイマー時にメッセージキューを観て、在れば表示する。CRだけでカーソルを行頭に戻すと同様の動作するようにした
 */
void CorpasDialog::timerEvent( QTimerEvent *e )
{   int     retry;

    if( e->timerId() == CorpasTimer )
    {   for ( retry = 5; ( !msgBuff.queMsgBuffer.isEmpty()) && ( retry > 0 ); retry-- )
        {   QString msg, msg2;

            emit requestDisplyCorpas( 1 );
            msg = msgBuff.queMsgBuffer.dequeue();
            if ( msg.indexOf( "\\@" ) == 0 )        // 文字列先頭が\\@ならダブりチェック
            {   msg = msg.mid( 2 );                 // \\@カット
                msg2 = msg;                         // "\n"ではなく、"u'\u2029'"
                msg2.replace( u'\n', u'\u2029' );
                QTextDocument *doc = ui->CorpasEdit->document();
                QTextCursor newcsr = doc->find( msg2 ); // 既出文字列検索
                if ( !newcsr.isNull())
                {   continue;
            }   }
            if ( !msg.isEmpty())                    // 行頭が'\r'ならカーソルを最終行の先頭に
            {   if ( msg[0] == '\r' )               // 行頭が'\r'ならカーソルを最終行の先頭に
                {   QTextCursor csr = ui->CorpasEdit->textCursor(); // オーバーライトが効かなかったら最終行全体を選択

                    csr.movePosition( QTextCursor::PreviousBlock, QTextCursor::KeepAnchor ); // 選択されるんじゃ？
//                  csr.movePosition( QTextCursor::StartOfLine, QTextCursor::KeepAnchor ); // 選択されるんじゃ？
                    csr.removeSelectedText();
                    msg.remove( 0, 1 );
            }   }
            if ( !msg.isEmpty())                    // 行頭が'\r'ならカーソルを最終行の先頭に
            {   if ( msg[0] == '\n' )               // 行頭が'\r'ならカーソルを最終行の先頭に
                {   msg.remove( 0, 1 );
            }   }
//            if (( inx = msg.indexOf( '\n' )) >= 0 )
//            {   msg.remove( inx, 1 );
//            }
//            while (( inx = msg.indexOf( '\r' )) >= 0 )
//            {   msg.remove( inx, 1 );
//            }
//            if ( msg.indexOf( "emitting " ) >= 0 )
//            {
//                ui->CorpasEdit->setOverwriteMode( true );       // 上書きセット
//            }
//            ui->CorpasEdit->setOverwriteMode( true );           // 上書きセット
            ui->CorpasEdit->appendPlainText( msg );
//          qDebug() << QString("timerEvent:=%1").arg(msg);
        }
    }
}
/**
 * @fn
 * 再起動中継
 * @brief 再起動中継
 * @return なし
 * @sa
 * @detail 再起動中継
 */
void CorpasDialog::fnreStart()
{
    emit sigreStart();
}


void CorpasDialog::fnupdateMaxLine()
{
    ui->CorpasEdit->setMaximumBlockCount( pprof->other.infoWndLineMaxLimmit );    // 最大行数設定
}
