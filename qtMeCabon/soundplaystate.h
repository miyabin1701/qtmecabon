/**
 * @file soundplaystate.h
 * @brief 再生ステータスの宣言
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef SOUNDPLAYSTATE_H
#define SOUNDPLAYSTATE_H

#include <QMainWindow>
#include <windows.h>


//#define USEbitFeled

#ifdef USEbitFeled
enum PLAYSTATE {
    STOP                    = 0b000,
    PRESTOP                 = 0b001,
    WAITENDOFTALK           = 0b011,
    PLAY                    = 0b111,
    PAUSE                   = 0b110,
//    PAUSE_QUECLEARREQ       = 0b10000100,
//    PAUSE_QUECLEARED_SOUND  = 0x10000101,
//    PAUSE_QUECLEARED_TEXT   = 0x10000110
};


struct _PLAYSTATE
{   enum PLAYSTATE  STATE:3;
    WORD            FILLER:2;
    WORD            CUECLEARREQ:1;
    WORD            CUECLEAREDSOUND:1;
    WORD            CUECLEAREDTEXT:1;
};


//union __PLAYSTATE
//{   struct _PLAYSTATE   b;
//    WORD                a;
//};

//#define PAUSE_QUECLEARREQ               ( 0b00000100 )
//#define BITMASK_QUECLEARED_TEXT         ( 0b00000010 )
//#define BITMASK_QUECLEARED_SOUND        ( 0b00000001 )

#else
enum PLAYSTATE {
    STOP                    = 0b0000000000000000,
    PRESTOP                 = 0b0001000000000000,
    WAITENDOFTALK           = 0b0010000000000000,
    PLAY                    = 0b0100000000000000,
    PAUSE                   = 0b1000000000000000,
    //    PAUSE_QUECLEARREQ       = 0b10000100,
    //    PAUSE_QUECLEARED_SOUND  = 0x10000101,
    //    PAUSE_QUECLEARED_TEXT   = 0x10000110
};
#define STATEMASK                  ( 0b1111000000000000 )
#define PAUSE_QUECLEARREQ          ( 0b0000000000000111 )
#define BITMASK_QUECLEARED_TEXT    ( 0b0000000000000010 )
#define BITMASK_QUECLEARED_SOUND   ( 0b0000000000000001 )
#endif



class soundPlayState
{
public:
    soundPlayState();
#ifdef USEbitFeled
    enum PLAYSTATE getPlayState()               { qDebug() << QString("getPlayState:=%1").arg(playState.STATE); return ( playState.STATE ); }
//    enum PLAYSTATE getPlayState()               { return ( playState.STATE ); }
    void setPlayState( enum PLAYSTATE state )   { playState.STATE = state; qDebug() << QString("setPlayState(%1):=%2").arg(state).arg(playState.STATE); }
//"setPlayState(7):=-1"     // state が正しく変わらない！
//    void setPlayState( enum PLAYSTATE state )   { playState.STATE = state; }
    void setPlayStateQueClearReq()              { playState.CUECLEARREQ = 1; }
    void setPlayStateQueClearedSound()          { playState.CUECLEAREDSOUND = 1; }
    void setPlayStateQueClearedText()           { playState.CUECLEAREDTEXT = 1; }

    static volatile struct _PLAYSTATE  playState;
#else
    enum PLAYSTATE getPlayState()               { return (( enum PLAYSTATE )( playState & STATEMASK )); }
    void setPlayState( enum PLAYSTATE state )   { playState = state; }
    void setPlayStateQueClearReq()              { playState |= PAUSE_QUECLEARREQ; }
    int  getPlayStateQueClearReq()              { return (( playState & PAUSE_QUECLEARREQ ) != 0 ); }
    void setPlayStateQueClearedSound()          { playState &= !BITMASK_QUECLEARED_TEXT; }
    void setPlayStateQueClearedText()           { playState &= !BITMASK_QUECLEARED_SOUND; }
    int  getPlayStateQueClearede()              { return (( playState & ( BITMASK_QUECLEARED_TEXT | BITMASK_QUECLEARED_SOUND )) == 0 ); }

    static volatile WORD  playState;

#endif
};

#endif // SOUNDPLAYSTATE_H
