/**
 * @file mecabif.h
 * @brief libmecab64.dllのインターフェースの処理のヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef MECABIF_H
#define MECABIF_H

#include <QString>
//#include "mecab.h"
#include "mecabdll.h"

class mecabif
{
public:
    mecabif();
    ~mecabif();
    HINSTANCE OpenMecabDll();
    long initial_dlls();
    mecab_model_t *mecab_model_new_build_option();
    static void CloseMecabDll();
    BOOL isOpenMecab();
    long getmecabDllLastError();
    void xchgJIS2004( QString *pline );
    int str2Corpas( QString str, QString &dst );

    static LPmecab_model_new			fpmecab_model_new;
    static LPgetLastError				fpgetLastError;
    static LPmecab_model_new_tagger		fpmecab_model_new_tagger;
    static LPmecab_dictionary_info		fpmecab_dictionary_info;
    static LPmecab_model_new_lattice	fpmecab_model_new_lattice;
    static LPmecab_destroy				fpmecab_destroy;
    static LPmecab_lattice_set_sentence	fpmecab_lattice_set_sentence;
    static LPmecab_parse_lattice		fpmecab_parse_lattice;
    static LPmecab_lattice_get_bos_node	fpmecab_lattice_get_bos_node;
    static LPmecab_lattice_destroy		fpmecab_lattice_destroy;
    static LPmecab_model_destroy		fpmecab_model_destroy;
    static LPmecab_lattice_tostr		fpmecab_lattice_tostr;
    static LPmecab_lattice_set_boundary_constraint fpmecab_lattice_set_boundary_constraint;
    static LPmecab_version              fpmecab_version;

    static HINSTANCE                    hMecabDLL;			// Handle to DLL
    static BOOL                         OkMecab;
    static long							mecabDllLastError;
    static mecab_model_t                *model;
    static mecab_model_t                *another_model;
    static mecab_t                      *mecab;
    static mecab_lattice_t              *lattice;
//    BOOL                        	OkMecab;
//    BOOL                        	fgInitialized;

};

#endif // MECABIF_H
