/**
 * @file extdial.h
 * @brief qdialのサブクラス　最小から最大、また逆に一気にいかないように対策のヘッダ
 * 完全には防げないが少しだけましに
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef EXTDIAL_H
#define EXTDIAL_H

#include <QDial>
#include <QObject>

class extDial : public QDial
{
public:
    extDial();
    explicit extDial(QWidget *parent = nullptr);
    void mouseMoveEvent(QMouseEvent * e);
//    void sliderMoved( int position );
//    void sliderChange(QAbstractSlider::SliderChange change);
//    void setSliderPosition(int);
};

#endif // EXTDIAL_H
