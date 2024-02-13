/**
 * @file about.h
 * @brief バージョン情報ダイヤログのヘッダ
 * @author m.kawakami
 * @date 21/05/??
 */

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

        public:
                 explicit About(QWidget *parent = nullptr);
    ~About();

private:
    Ui::About *ui;

private  slots:
    void onOk();
};

#endif // ABOUT_H
