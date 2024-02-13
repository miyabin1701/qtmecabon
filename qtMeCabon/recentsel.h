/**
 * @file recentsel.h
 * @brief 読み込みファイルの履歴選択処理のヘッダ
 * @author m.kawakami
 * @date 24/01/??
 */

#ifndef RECENTSEL_H
#define RECENTSEL_H

#include <QDialog>

namespace Ui {
class recentSel;
}

class recentSel : public QDialog
{
    Q_OBJECT

public:
    explicit recentSel(QWidget *parent = nullptr);
    ~recentSel();

private:
    Ui::recentSel *ui;

private  slots:
    void ItemDoubleClicked();
    void accept();
};

#endif // RECENTSEL_H
