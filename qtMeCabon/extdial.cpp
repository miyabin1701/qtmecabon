/**
 * @file extdial.cpp
 * @brief qdialのサブクラス　最小から最大、また逆に一気にいかないように対策
 * 完全には防げないが少しだけましに
 * @author m.kawakami
 * @date 23/10/??
 */

#include "extdial.h"
#include <QMessageBox>
#include <QMouseEvent>


/**
 * @fn
 * extDialクラスコンストラクタ
 * @brief extDialクラスコンストラクタ
 * @return なし
 * @sa
 * @detail なんもしてない
 */
extDial::extDial()
{

}


/**
 * @fn
 * extDialクラスコンストラクタ
 * @brief extDialクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail なんもしてない
 */
extDial::extDial( QWidget *parent )
{

}


#if 0
void extDial::sliderMoved( int position )
{
    if (( minimum() <= position ) && ( position <= maximum()))
    {   QDial::sliderMoved( position );
    }
}

void extDial::sliderChange(QAbstractSlider::SliderChange change)
{   if ( change == QAbstractSlider::SliderChange::SliderValueChange )
    {
//        QMessageBox::warning(this, tr("MessageBox"), tr("sliderChange"));
//    maximum : int
//            minimum : int
//value : int
    }
}

void extDial::setSliderPosition( int pos )
{   int orgPos = sliderPosition();

    QDial::setSliderPosition( pos );
    int newPos = sliderPosition();
    if (( orgPos >= maximum()) && ( newPos <= minimum()))
    {   QDial::setSliderPosition( maximum());
    }
    if (( orgPos <= minimum()) && ( newPos >= maximum()))
    {   QDial::setSliderPosition( minimum());
    }
}
#endif


/**
 * @fn
 * マウスムーブイベントハンドラ
 * @brief マウスムーブイベントハンドラ
 * @param (*e) マウスムーブイベントデータ
 * @return なし
 * @sa
 * @detail ダイヤルで最大から最小、また最小から最大に回ってしまうので、音量設定に不向きなので
 * 完全に防げないが、ある程度は制限できるようにした
 */
void extDial::mouseMoveEvent(QMouseEvent * e)
{
//    Q_D(QDial);
    if (!(e->buttons() & Qt::LeftButton))
    {   e->ignore();
        return;
    }
    int orgPos = sliderPosition();
    QDial::mouseMoveEvent( e );
    int newPos = sliderPosition();
//    int quarter = ( maximum() - minimum()) / 4;
    int third = ( maximum() - minimum()) / 3;
    if (( orgPos >= maximum() - third ) && ( newPos <= minimum() + third ))
    {   QDial::setSliderPosition( maximum());
    }
    else if (( orgPos <= minimum() + third ) && ( newPos >= maximum() - third ))
    {   QDial::setSliderPosition( minimum());
    }
//    e->accept();
//    d->doNotEmit = true;
//    setSliderPosition(d->valueFromPoint(e->pos()));
//    d->doNotEmit = false;
#if 0
/*----------------------------------------------------------------------
    function valueFromPoint( x, y ) {
        var yy = root.height / 2 - y;
        var xx = x - root.width / 2;
        var angle = ( xx || yy ) ? Math.atan2( yy, xx ) : 0;
        if ( angle < -Math.PI / 2 )
            angle += 2 * Math.PI;
        var dist = 0;
        var minv = range.minimumValue;
        var maxv = range.maximumValue;
        if ( minimumValue < 0 ) {
            dist = -range.minimumValue;
            minv = 0;
            maxv = range.maximumValue + dist;
        }
        var v = ( minv + ( maxv - minv ) * ( Math.PI * 4 / 3 - angle ) / ( Math.PI * 10 / 6 ) );
        if ( dist > 0 )
            v -= dist;
        return Math.max( range.minimumValue, Math.min( range.maximumValue, v ) );
    }
/---------------------------------------------------------------------- */
#endif
}
