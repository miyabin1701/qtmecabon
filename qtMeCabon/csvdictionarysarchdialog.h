/**
 * @file csvdictionarysarchdialog.h
 * @brief .csv辞書（インフォメーション）ダイヤログの処理のヘッダ
 * @author m.kawakami
 * @date 21/05/??
 */

#ifndef CSVDICTIONARYSARCHDIALOG_H
#define CSVDICTIONARYSARCHDIALOG_H

#include <QDialog>

namespace Ui {
class CsvDictionarySarchDialog;
}

class CsvDictionarySarchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CsvDictionarySarchDialog(QWidget *parent = nullptr);
    ~CsvDictionarySarchDialog();

private:
    Ui::CsvDictionarySarchDialog *ui;
};

#endif // CSVDICTIONARYSARCHDIALOG_H
