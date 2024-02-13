/**
 * @file recentsel.cpp
 * @brief 読み込みファイルの履歴選択処理
 * @author m.kawakami
 * @date 24/01/??
 */

#include "recentsel.h"
#include "ui_recentsel.h"
#include "profilevalues.h"
//#include <QListWidgeItem>

extern profileValues   *pprof;


/**
 * @fn
 * recentSelクラスコンストラクタ
 * @brief recentSelクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail 履歴選択ダイヤログの初期設定
 */
recentSel::recentSel(QWidget *parent) :
      QDialog(parent),
      ui(new Ui::recentSel)
{
    ui->setupUi(this);
    for ( int i = 0; i < lengthof( pprof->oper.recent ); i++ )  // リセントリストの設定
    {   ui->recentListWidget->addItem( *pprof->oper.recent[i].pwszCurrentPlayFile );
//        mpSettings->setValue( szrecentidx[i], oper.recent[i].iCurrentPlayFileLine );  // 行番号を入れるなら、ただし、accept時に取得される文字列がファイルパスだけでなくなるのでそちらも変更を
    }
    ui->recentListWidget->setCurrentRow( 0 );
}


/**
 * @fn
 * recentSelクラスデストラクタ
 * @brief recentSelクラスデストラクタ
 * @return なし
 * @sa
 * @detail recentSelクラスデストラクタ
 */
recentSel::~recentSel()
{
    delete ui;
}


/**
 * @fn
 * 選択行ダブルクリック処理
 * @brief 選択行ダブルクリック処理
 * @return なし
 * @sa
 * @detail 選択行ダブルクリック処理
 */
void recentSel::ItemDoubleClicked()
{   accept();
}


/**
 * @fn
 * 選択行アクセプト処理
 * @brief 選択行アクセプト処理
 * @return なし
 * @sa
 * @detail 選択行アクセプト処理　選択行を履歴先頭に移動する
 */
void recentSel::accept()
{   QListWidgetItem *pItem = ui->recentListWidget->currentItem();
    QString str = pItem->text();

    pprof->updateRecentFile( str );     // 選択されたファイルをリセントリストトップに更新
    QDialog::accept();
}

