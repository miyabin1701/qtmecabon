/**
 * @file playlinesapi.h
 * @brief sapiの１行トーク処理
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef PLAYLINESAPI_H
#define PLAYLINESAPI_H

#include "text2wave.h"
#include "mecabdll.h"
#include <QMainWindow>
#include <QObject>
#include <sapi.h>
#include <sphelper.h>
#include "profilevalues.h"
#include "messagebuff.h"


#define ERROR_COINIT    -1001
#define ERROR_COCREAT   -1002


class PlayLineSapi : public text2wave
{
    Q_OBJECT

public:
    PlayLineSapi();
    ~PlayLineSapi();
//    int cvt2wave( QString *pLine, struct WAVEBUFFER *ps );
    int PlayLine( QString *pLine, struct WAVEBUFFER *ps );
    int towav( struct TalkData qsTalkData, QThread *pThred );		// TTS毎に処理違うので
//    void cvtText2Wave();
    void finalize();
    void initialize();
    int setVolume( int value );
    int getVolume();
    int currentTalkLine();

signals:
//    void displayUnnounStr( QString szSurface );
    void requestSelLine(int lineno);

private:
    LPSTR SkipMuonWave( int sps, int Byteps, LPSTR pSrc, DWORD *cbWave );
    messageBuff msgBuff;

//    BOOL                        	OkMecab;
    BOOL                        	fgInitialized;
    ISpVoice                        *pVoice;
    wchar_t                         m_szWFileName[L_tmpnam_s];
//    char                            bufUTF8[MAX_LINE];
    QString                         bufUTF16;
    int                             errorISpVoice;

public:
    static wchar_t  szVoiceVoxPath[_MAX_PATH];
    static BOOL     fgVoiceVox;
    static struct   SAPI54SETTING	sapi54;
    static const wchar_t  *pszRegistryEntry[];
    static PROCESS_INFORMATION	VoiceVoxProcessInfomation;

};

#endif // PLAYLINESAPI_H
