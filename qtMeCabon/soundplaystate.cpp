/**
 * @file soundplaystate.cpp
 * @brief 再生ステータスクラス
 * @author m.kawakami
 * @date 23/10/??
 */

#include "soundplaystate.h"


#ifdef USEbitFeled
volatile struct _PLAYSTATE  soundPlayState::playState = { STOP, 0, 0, 0, 0 };
/*
volatile struct _PLAYSTATE  soundPlayState::playState.STATE = STOP;
volatile struct _PLAYSTATE  soundPlayState::playState.FILLER = 0;
volatile struct _PLAYSTATE  soundPlayState::playState.CUECLEARREQ = 0;
volatile struct _PLAYSTATE  soundPlayState::playState.CUECLEAREDSOUND = 0;
volatile struct _PLAYSTATE  soundPlayState::playState.CUECLEAREDTEXT = 0;
*/
#else
volatile WORD  soundPlayState::playState = 0;
#endif


/**
 * @fn
 * soundPlayStateクラスコンストラクタ
 * @brief soundPlayStateクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail なんもしてない
 */
soundPlayState::soundPlayState()
{

}
