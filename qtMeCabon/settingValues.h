/**
 * @file settingValues.h
 * @brief 設定データの宣言
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef SETTINGVALUES_H
#define SETTINGVALUES_H

#endif // SETTINGVALUES_H


enum TTSNAME { SAPI54 = 0, AQTK10, VVoxCore };


struct operationalSetting
{   bool        displayOutputOffThenPouse;
    bool        soundOutputSkip;
    bool        symbolSpeakingOff;
    int         idisplayMagnify;
    int         iCurrentPlayFileLine;
    wchar_t     *pwszCurrentPlayFile;
};


struct otherSetting
{   bool        selBakeColorBlack;
    wchar_t    *pwszEdtorPath;
    wchar_t    *pwszEdtorTagjumpCommand;
};


struct mecabSetting
{   wchar_t    *pwszMecabPath;
    wchar_t    *pwszSystemDictionaryFolder;
    wchar_t    *pwszUserDictionaryPath;
    wchar_t    *pwszUserDictionaryCsvPath;
    wchar_t    *pwszSystemDictionaryBuildFolder;
    wchar_t    chSetDictionary;
    wchar_t    chSetDonCsvDictionary;
};


struct sapiSetting
{   int     iRate;
    int     iVolume;
    wchar_t	*pwszVoiceName;	// Voice name
    int     iMuonZan;
    wchar_t *pwszVoicevox4sapiRunPath;
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
{

};


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
