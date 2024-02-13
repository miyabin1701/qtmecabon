/**
 * @file csvdictionarysarchdialog.cpp
 * @brief .csv辞書（インフォメーション）ダイヤログの処理
 * @author m.kawakami
 * @date 21/05/??
 */

#include "csvdictionarysarchdialog.h"
#include "ui_csvdictionarysarchdialog.h"


/**
 * @fn
 * CsvDictionarySarchDialogクラスコンストラクタ
 * @brief CsvDictionarySarchDialogクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail なんもしてない
 */
CsvDictionarySarchDialog::CsvDictionarySarchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CsvDictionarySarchDialog)
{
    ui->setupUi(this);
}


/**
 * @fn
 * CsvDictionarySarchDialogクラスデストラクタ
 * @brief CsvDictionarySarchDialogクラスデストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail なんもしてない
 */
CsvDictionarySarchDialog::~CsvDictionarySarchDialog()
{
    delete ui;
}
