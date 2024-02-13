/**
 * @file messagebuff.cpp
 * @brief インフォメーションウィンドへ出力するメッセージのバッファクラス
 * @author m.kawakami
 * @date 23/10/??
 */

#include "messagebuff.h"


QMutex messageBuff::mutexBuff;
/*
QChar messageBuff::buffer[BufferSize];
int messageBuff::ixEnterBuff = 0;
int messageBuff::ixDeentBuff = 0;
QSemaphore messageBuff::cbbufferfree( sizeof( messageBuff::buffer ));
QSemaphore messageBuff::cbbufferused( 0 );
*/
QQueue<QString>   messageBuff::queMsgBuffer;   //


/**
 * @fn
 * messageBuffクラスのコンストラクタ
 * @brief messageBuffクラスのコンストラクタ
 * @return なし
 * @sa
 * @detail なんもしてない
 */
messageBuff::messageBuff()
{

}


/**
 * @fn
 * インフォメーションウインドウへ表示するためメッセージキューへ登録する
 * @brief インフォメーションウインドウへ表示するためメッセージキューへ登録する
 * @param (msg) キューに登録するメッセージ
 * @return なし
 * @sa
 * @detail メッセージキューへ登録する（ダブりチェック付、辞書未登録語彙を何度も出さないため）
 * 複数のスレッドからメッセージを入れるので一応再入禁止
 */
void messageBuff::enterMessageWcheck( QString msg )
{
    mutexBuff.lock();
    queMsgBuffer.enqueue( "\\@" + msg );    // 先頭に追加してるのはダブりチェックのエスケープシーケンス
    mutexBuff.unlock();
}


/**
 * @fn
 * インフォメーションウインドウへ表示するためメッセージキューへ登録する
 * @brief インフォメーションウインドウへ表示するためメッセージキューへ登録する
 * @param (msg) キューに登録するメッセージ
 * @return なし
 * @sa
 * @detail メッセージキューへ登録する（ダブりチェック無し）
 * 複数のスレッドからメッセージを入れるので一応再入禁止
 */
void messageBuff::enterMessage( QString msg )
{
    mutexBuff.lock();
    queMsgBuffer.enqueue( msg );
    mutexBuff.unlock();
}


#if 0
/**
 * @fn
 * メッセージキューから１メッセージ取り出す
 * @brief メッセージキューから１メッセージ取り出す
 * @param (msg) キューに登録するメッセージ
 * @return なし
 * @sa
 * @detail 使ってない
 */
QString messageBuff::deentMessage( QString &msg )
{
    msg.clear();
    for ( int i = cbbufferused.available(); i > 0; i-- )
    {   cbbufferused.acquire();
        msg += buffer[ixDeentBuff++];
        ixDeentBuff %= sizeof( messageBuff::buffer );
        cbbufferfree.release();
    }
    return ( msg );
}


/**
 * @fn
 * メッセージキューにメッセージが在るか確認
 * @brief メッセージキューにメッセージが在るか確認
 * @param (msg) キューに登録するメッセージ
 * @return なし
 * @sa
 * @detail 使ってない
 */
int messageBuff::availableMessage()
{
#if 0
   return ( cbbufferused.available());
#endif
    return ( 0 );
}
#endif

