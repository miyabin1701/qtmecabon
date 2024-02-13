/**
 * @file waveque.cpp
 * @brief waveデータ型宣言＆キュー定義
 * @author m.kawakami
 * @date 23/10/??
 */

#include "waveque.h"

struct WAVEBUFFER           waveque::waveBuffer[MAXWAVEBUF] = {};    //
QQueue<struct WAVEBUFFER>   waveque::queWaveBuffer = {};    //


/**
 * @fn
 * wavequeクラスコンストラクタ
 * @brief wavequeクラスコンストラクタ
 * @return なし
 * @sa
 * @detail 何もしてない
 */
waveque::waveque()
{

}
