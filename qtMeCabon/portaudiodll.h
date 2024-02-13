/**
 * @file portaudiodll.h
 * @brief portaudio.dllの関数エントリーポイント宣言ヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */
#ifndef PORTAUDIODLL_H
#define PORTAUDIODLL_H


/* C interface  */
#ifdef __cplusplus
#include <cstdio>
#else
#include <stdio.h>
#endif

#include "portaudio\include\portaudio.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _WIN32
#include <windows.h>
#  ifdef DLL_EXPORT
#    define MECAB_DLL_EXTERN  __declspec(dllexport)
#    define MECAB_DLL_CLASS_EXTERN  __declspec(dllexport)
#  else
#    define MECAB_DLL_EXTERN  __declspec(dllimport)
#  endif
#endif


typedef PaError ( CALLBACK *LPPa_Initialize )( void );
extern LPPa_Initialize fpPa_Initialize;

typedef PaError ( CALLBACK *LPPa_Terminate )( void );
extern LPPa_Terminate fpPa_Terminate;

typedef PaDeviceIndex ( CALLBACK *LPPa_GetDefaultOutputDevice )( void );
extern LPPa_GetDefaultOutputDevice fpPa_GetDefaultOutputDevice;

typedef PaError ( CALLBACK *LPPa_StartStream )( PaStream * );
extern LPPa_StartStream fpPa_StartStream;

typedef PaError ( CALLBACK *LPPa_StopStream )( PaStream * );
extern LPPa_StopStream fpPa_StopStream;

typedef PaError ( CALLBACK *LPPa_CloseStream )( PaStream * );
extern LPPa_CloseStream fpPa_CloseStream;

typedef PaError ( CALLBACK *LPPa_IsStreamStopped )( PaStream * );
extern LPPa_IsStreamStopped fpPa_IsStreamStopped;

typedef PaError ( CALLBACK *LPPa_OpenStream )(
    PaStream**,
    const PaStreamParameters *,
    const PaStreamParameters *,
    double,
    unsigned long,
    PaStreamFlags,
    PaStreamCallback *,
    void * );
extern LPPa_OpenStream fpPa_OpenStream;

typedef void ( CALLBACK *LPPa_Sleep )( long );
extern LPPa_Sleep fpPa_Sleep;

typedef PaDeviceInfo *( CALLBACK *LPPa_GetDeviceInfo )( PaDeviceIndex );
extern LPPa_GetDeviceInfo fpPa_GetDeviceInfo;


#ifdef __cplusplus
}
#endif

#endif  // PORTAUDIODLL_H


