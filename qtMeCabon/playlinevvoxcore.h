/**
 * @file playlinevvoxcore.h
 * @brief １行のテキストをvoicevox-coreでスピーチするのヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef PLAYLINEVVOXCORE_H
#define PLAYLINEVVOXCORE_H

#include "text2wave.h"
#include "voicevox_core.h"
#include <QObject>


//    VoicevoxInitializeOptions options = voicevox_make_default_initialize_options();
typedef VoicevoxInitializeOptions ( CALLBACK *LPvoicevox_make_default_initialize_options )( void );
extern LPvoicevox_make_default_initialize_options fpvoicevox_make_default_initialize_options;

//    voicevox_initialize( options );
typedef void ( CALLBACK *LPvoicevox_initialize )( VoicevoxInitializeOptions );
extern LPvoicevox_initialize fpvoicevox_initialize;

//    VoicevoxTtsOptions options = voicevox_make_default_tts_options();
typedef VoicevoxTtsOptions ( CALLBACK *LPvoicevox_make_default_tts_options )( void );
extern LPvoicevox_make_default_tts_options fpvoicevox_make_default_tts_options;

//    auto result = voicevox_tts( utf8Buff, speaker_id, options, &plength, &pwav );
typedef VoicevoxResultCode ( CALLBACK *LPvoicevox_tts )( const char *text,
                                                     uint32_t speaker_id,
                                                     struct VoicevoxTtsOptions options,
                                                     uintptr_t *output_wav_length,
                                                     uint8_t **output_wav );
extern LPvoicevox_tts fpvoicevox_tts;

//    void voicevox_wav_free(uint8_t *wav);
typedef void ( CALLBACK *LPvoicevox_wav_free )( uint8_t *wav );
extern LPvoicevox_wav_free fpvoicevox_wav_free;

//    VoicevoxResultCode voicevox_load_model(uint32_t speaker_id);
typedef VoicevoxResultCode ( CALLBACK *LPvoicevox_load_model )( uint32_t speaker_id );
extern LPvoicevox_load_model fpvoicevox_load_model;

//    bool voicevox_is_model_loaded(uint32_t speaker_id);
typedef bool ( CALLBACK *LPvoicevox_is_model_loaded )( uint32_t speaker_id );
extern LPvoicevox_is_model_loaded fpvoicevox_is_model_loaded;

//    const char *voicevox_get_version(void);
typedef char *( CALLBACK *LPvoicevox_get_version )( void );
extern LPvoicevox_get_version fpvoicevox_get_version;

//PlayLineVvoxCore::voicevox_finalize();
typedef void ( CALLBACK *LPvoicevox_finalize )( void );
extern LPvoicevox_finalize fpvoicevox_finalize;

class PlayLineVvoxCore : public text2wave
{
   // VoicevoxSynthesizer* synthesizer;
    int                 VvoxCoreLastError;
    static int          ctVvoxCoreLoad;
    static HINSTANCE    hVvoxCoreDll;
    const char          *dictdir;

public:
    PlayLineVvoxCore();
    ~PlayLineVvoxCore();
    void finalize();
    void initialize();
    HINSTANCE LoadVvoxCoreDll();
    static void unLoadVvoxCoreDll();
    int currentTalkLine();

//    int PlayLine( QString *pLine, struct WAVEBUFFER *ps );
    int towav( struct TalkData qsTalkData, QThread *pThred );		// TTS毎に処理違うので
    void freeWav( uint8_t* wav );
    int setVolume( int value );
    int getVolume();

//    void cvtText2Wave();

private:
    static LPvoicevox_make_default_initialize_options   fpvoicevox_make_default_initialize_options;
    static LPvoicevox_initialize                        fpvoicevox_initialize;
    static LPvoicevox_make_default_tts_options          fpvoicevox_make_default_tts_options;
    static LPvoicevox_tts                               fpvoicevox_tts;
    static LPvoicevox_wav_free                          fpvoicevox_wav_free;
    static LPvoicevox_load_model                        fpvoicevox_load_model;
    static LPvoicevox_is_model_loaded                   fpvoicevox_is_model_loaded;
    static LPvoicevox_get_version                       fpvoicevox_get_version;
    static LPvoicevox_finalize                          fpvoicevox_finalize;
};

#endif // PLAYLINEVVOXCORE_H
