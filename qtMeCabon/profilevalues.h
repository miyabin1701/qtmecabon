/**
 * @file profilevalues.h
 * @brief 初期設定データのハンドリングのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef PROFILEVALUES_H
#define PROFILEVALUES_H

#include <QSettings>

enum TTSNAME { NONE = -1, SAPI54 = 0, AQTK10, VVoxCore };
#define lengthof(x)     (sizeof(x)/sizeof(x[0]))
#define EOS '\0'
#define MAX_LINE    (4096)
#define MAX_CORPUSBACKUP  (8)


struct RecentFile
{   int         iCurrentPlayFileLine;
    QString     *pwszCurrentPlayFile;
};


struct operationalSetting
{   enum TTSNAME ttsSelector;
    bool        displayOutputOffThenPouse;
    bool        soundOutputSkip;
    bool        symbolSpeakingOff;
    int         idisplayMagnify;
    int         iAppVolume;
    int         iSizeMainwindowHight;
    int         iSizeMainwindowWidth;
    int         iSizeCorpaswindowHight;
    int         iSizeCorpaswindowWidth;
    int         iSizeOutputwindowHight;
    int         iSizeOutputwindowWidth;
    RecentFile  recent[8];
};


struct otherSetting
{   bool       selBakeColorBlack;
    int         infoWndLineMaxLimmit;
    QString    *pwszRamDiskPath;
    QString    *pwszEdtorPath;
    QString    *pwszEdtorTagjumpCommand;
};


struct mecabSetting
{   QString    *pwszMecabPath;
    QString    *pwszSystemDictionaryFolder;
    QString    *pwszUserDictionaryPath;
    QString    *pwszUserDictionaryCsvPath;
    QString    *pwszSystemDictionaryBuildFolder;
    QString    *chSetDictionary;
    QString    *chSetDonCsvDictionary;
};


struct sapiSetting
{   int     iRate;
    int     iVolume;
    QString	*pwszVoiceName;	// Voice name
    int     iMuonZan;
    QString *pwszVoicevox4sapiRunPath;
    int     iOutputMode;
};


struct aqtk10Setting
{   int     iSpeed;
    int     iStep;
    int     iHigh;
    int     iAccent;
    int     iStep2;
    int     iVolume;
    int     iVoiceIdx;
};


struct voicevoxcoreSetting
{   int     ispeaker_id;
    QString *pwszOpenJTalkDicPath;
    QString *pwszOpenJTalkDicBuildDir;
};


/*
struct settingvalues
{
    enum TTSNAME                ttsSelector;
    struct operationalSetting   oper;
    struct otherSetting         other;
    struct mecabSetting         mecab;
    struct sapiSetting          sapi;
    struct aqtk10Setting        aqtk;
    struct voicevoxcoreSetting  vvox;
};
*/


class profileValues
{
public:
    profileValues();
    ~profileValues();
    void replaceEnv( QString envName, QString addStr );

public:
    QSettings *mpSettings;
    void updateRecentFile( QString FileName );
    QString getCorpusPath();
    void updateCorpusFile();

    bool                        fgRestart;
//    enum TTSNAME                ttsSelector;
    struct operationalSetting   oper;
    struct otherSetting         other;
    struct mecabSetting         mecab;
    struct sapiSetting          sapi;
    struct aqtk10Setting        aqtk;
    struct voicevoxcoreSetting  vvox;
};

#endif // PROFILEVALUES_H
