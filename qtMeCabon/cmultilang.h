/**
 * @file cmultilang.h
 * @brief .netの文字コード判別処理ifと文字コード変換関数のヘッダ
 * @author m.kawakami
 * @date 21/05/??
 */

#ifndef CMULTILANG_H
#define CMULTILANG_H

#include <mlang.h>

#define EOS '\0'
#define lengthof(x) (sizeof(x)/sizeof(x[0]))
#define MAX_LINE    (4096)


class CMultiLang
{
public:
    CMultiLang();
    ~CMultiLang();
    int analizCodePage( char *pBuf, int cbBuf );
    void UniToUTF8( char *pszUni, char *pszUtf8, int lenUtf8 );
    void UTF8ToUni( char *pszUtf8, char *pszUni, int lenUni );
    int CodePageToUni( int codepage, char *pszCPstr, int lenCPstr, char *pszUni, int lenUni );
    int CodePageToUni( int codepage, char *pszCPstr, char *pszUni, int lenUni );
    void ShiftJisToUni( char *pszSjis, char *pszUni, int lenUni );
    void ShiftJisToUTF8( char *pszSjis, char *pszUtf8, int lenUtf8 );
    void UTF8ToShiftJis( char *pszUtf8, char *pszSjis, int lenSjis );

};

#endif // CMULTILANG_H
