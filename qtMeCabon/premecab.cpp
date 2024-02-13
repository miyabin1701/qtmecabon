/**
 * @file premecab.cpp
 * @brief mecabで分かち書きする。sapiの際はこれ１回のみ、voicevoxの際は前処理として
 * また、語彙が行末で分割されている場合分割されないようにする
 * @author m.kawakami
 * @date 23/10/??
 */

#include "premecab.h"
#include "cmultilang.h"
#include "mecab.h"
#include "mecabif.h"
#include "talktext.h"
#include "profilevalues.h"


extern profileValues   *pprof;
static QChar           szLstChar[4] = { EOS };

#define isUTF16HanASCII(c)	((' '<=c)&&(c<='}'))
#define isUTF16ZenASCII(c)	((u'　'<=c)&&(c<=u'｝'))
#define MAXCVTUNIT          (48)
//#define u8isalpha(c)        (((u8'A'<= c)&&(c<=u8'Z'))||((u8'a'<= c)&&(c<=u8'z')))
//#define u8zisalpha(p)        (((u8'Ａ'<= *p)&&(*p<=u8'Ｚ'))||((u8'ａ'<= *p)&&(*p<=u8'ｚ')))

/**
 * @fn
 * premecabクラスのコンストラクタ
 * @brief premecabクラスのコンストラクタ
 * @return なし
 * @sa
 * @detail premecabクラスのコンストラクタ
 */
premecab::premecab(QObject *parent)
//    : QObject{parent}
{   clear();
}


/**
 * @fn
 * クラス内バッファ類のクリア
 * @brief クラス内バッファ類のクリア
 * @return なし
 * @sa
 * @detail クラス内バッファ類のクリア
 */
void premecab::clear()
{   srcBuff.clear();
    numbers.clear();
    fgComma = FALSE;
    TalkDat.kanaBuff.clear();
    TalkDat.textBuff.clear();
}


/**
 * @fn
 * 記号などを仮名(アクエストーク発音記号付き)に置き換える
 * @brief 記号などを仮名(アクエストーク発音記号付き)に置き換える
 * @param (&dstStr) ディスティネーションバッファ
 * @param (srcStr) ソースバッファ
 * @param (hirakata) ヒラカナ/カタカナ
 * @return 置き換えたら:TRUE / FALSE
 * @sa
 * @detail 記号などを仮名(アクエストーク発音記号付き)に置き換える
 */
BOOL premecab::extractHatuonfeature( QString &dstStr, QString srcStr, int hirakata )
{   static QString ZENERRKIGOU	= u8"！ ＃ ＊  ， ． ／ ： ； ＜ ＝ ＞ ＾ ＿ ～―";
    static QString YOMIERRKIGOU[][2]	=
    {   {   u8"びっく\'り",     u8"ビック\'リ"    },
        {   u8"しゃ\'ーぷ",     u8"シャ\'ープ"    },
        {   u8"あ\'すた",      u8"ア\'スタ"    },
        {   u8"か\'んま",      u8"カ\'ンマ"    },
        {   u8"どっと",        u8"ドット"    },
        {   u8"すら\'っしゅ",    u8"スラ\'ッシュ"    },
        {   u8"こ\'ろん",      u8"コ\'ロン"    },
        {   u8"せみこ\'ろん",   u8"セミコ\'ロン"    },
        {   u8"しょ\'ーなり",    u8"ショ\'ーナリ"    },
        {   u8"いこ\'ーる",     u8"イコ\'ール"    },
        {   u8"だ\'いなり",    u8"ダ\'イナリ"    },
        {   u8"は\'っと",      u8"ハ\'ット"    },
        {   u8"あ\'んだー",    u8"ア\'ンダー"    },
        {   u8"から",         u8"カラ"    },
        {   u8"。",          u8"。"    }
      };
    QChar szLine[512], *buf, *pCanma[10];
    int IxCanma, val, idx;
    QChar swp;

    hirakata = (( 0 <= hirakata ) && ( hirakata <= lengthof(YOMIERRKIGOU[0])))? hirakata: 0;
    dstStr.clear();
    memset( &pCanma[0], 0, sizeof( pCanma ));
    auto src = srcStr.constBegin();
    for ( buf = &szLine[0], IxCanma = 0; src != srcStr.constEnd(); src++ )
    {	if (( *src == ',' ) && ( IxCanma < lengthof( pCanma )))
        {	pCanma[IxCanma++] = buf;
        }
        *buf++ = *src;
    }
    if ( pCanma[7] == NULL )
    {	return ( FALSE );
    }
    *buf = EOS;;
    swp = *( pCanma[7] + 4 );		/*4:Utf-8 3:ShiftJis*/
    *( pCanma[7] + 4 ) = EOS;
    if (( idx = ZENERRKIGOU.indexOf( *( pCanma[7] + 1 ))) >= 0 )
    {   if ( !pprof->oper.symbolSpeakingOff )
        {   val = idx / 4;  // ( strlen( u8"！" ) + 1 )
            dstStr += YOMIERRKIGOU[val][hirakata];		/*4:Utf-8 3:ShiftJis*/
//			*( pCanma[7] + 4 ) = swp;
    }	}
    else
    {	*( pCanma[7] + 4 ) = swp;	/*4:Utf-8 3:ShiftJis*/
        if ( pCanma[8] != NULL )    // 辞書に9個目のデータがあれば
        {    *( pCanma[8]) = EOS;
        }
        dstStr += pCanma[7] + 1;
        if (( idx = dstStr.length() - 1 ) >= 0 )
        {   if ( dstStr[idx] == '\n' )
            {   dstStr = dstStr.left( idx );
    }	}   }
    if (( idx = dstStr.length() - 3 ) >= 0 )
    {   szLstChar[0] = dstStr[idx++];
        szLstChar[1] = dstStr[idx++];
        szLstChar[2] = dstStr[idx];
    }
    else
    {   szLstChar[0] = szLstChar[1] = szLstChar[2] = EOS;
    }
    return ( TRUE );
}


/**
 * @fn
 * アクエストークでエラーになる記号などを仮名に置き換える
 * @brief アクエストークでエラーになる記号を仮名に置き換える
 * @param (&dst) ディスティネーションバッファ
 * @param (src) ソースバッファ
 * @return TRUE
 * @sa
 * @detail アクエストークでエラーになる記号を仮名に置き換える
 */
BOOL premecab::extractHatuonsurface( QString &dst, QString src )
{   static QString ZENERRKIGOU      = "！　＃　＊　，　．　／　：　；　＜　＝　＞　＾　＿　～　ー";
    static QString HALFERRKIGOU     = "!#*,./:;<=>^_~";
    static QString ZENKIGOU         = "！＃＄％＆＊＋，－．／：；＜＝＞？＠￥＾＿、。ー";
    static QString HALFKIGOU		= " !#$%&*+,-./:;<=>?@\\^_";
    static QString COAIUEO          = "ぁぃぅぇぉァィゥェォ";
    static QString YOMIERRKIGOU[]	=
        {	"びっく\'り",    "しゃ\'ーぷ",   "あ\'すた",
         "か\'んま",    "どっと",       "すら\'っしゅ",
         "こ\'ろん",    "せみこ\'ろん",  "しょ\'ーなり",
         "いこ\'ーる",   "だ\'いなり",   "は\'っと",
         "あ\'んだー",   "から",       "はいふん",       "。"
    };
    int val;
    int len, n;

    dst.clear();
    if ( src.isEmpty())					// 全角記号もくる
    {	return ( TRUE );
    }
    len = src.length();
    //    if ( src.indexOf( u'ー' ) >= 0 )
    //    {
    //        qDebug() << QString("extractHatuonsurface:[%1]").arg(src);
    //    }
    for ( n = 0; n < len; n++ )
    //    for ( auto srcidx = src.begin(); srcidx != src.end(); srcidx++ )    // イテレータでのループはダメのよう　ソース変更なし＆＋＋なしのはずだけど変なループしてそう
    //        "[\u0000]"
    //        "[\u0000]"
    //        "[\u0000]"
    //        "[\u0000]"
    //        "[\u0000]"
    //        "[\u0000]"
    {   //if ( src[n] == '\0' )
        //{    break;
        //}
        //        qDebug() << QString("[%1]").arg(src[n]);   // .arg(*srcidx);
        if ( isUTF16HanASCII( src[n]))   //*srcidx ))        // '℃'でエクセプション　上のqDebug入れたらエクセプションしないけど、メモリー壊してそう
        {   if (( val = HALFERRKIGOU.indexOf( src[n])) >= 0 )
//        {   if (( val = HALFERRKIGOU.indexOf( *srcidx )) >= 0 )
            {	if ( !pprof->oper.symbolSpeakingOff )
                {   dst += YOMIERRKIGOU[val];
            }   }
            else if (( val = HALFKIGOU.indexOf( src[n])) >= 0 )
//            else if (( val = HALFKIGOU.indexOf( *srcidx )) >= 0 )
            {   dst += ZENKIGOU[val];
            }
            else
            {	dst += src[n];
//            {	dst += *srcidx;
            }
            continue;
        }
        else
        {   if ( COAIUEO.indexOf( src[n]) >= 0 )
//      {   if ( COAIUEO.indexOf( *srcidx ) >= 0 )
            {   continue;
            }
            if (( val = ZENERRKIGOU.indexOf( src[n])) >= 0 )
//            if (( val = ZENERRKIGOU.indexOf( *srcidx )) >= 0 )
            {	if ( !pprof->oper.symbolSpeakingOff )
                {   dst += YOMIERRKIGOU[val / 2];
            }   }
            else if ( ZENKIGOU.indexOf( src[n]) >= 0 )
//            else if ( ZENKIGOU.indexOf( *srcidx ) >= 0 )
            {	dst += src[n];
//            {	dst += *srcidx;
            }
            else while ( n < len )
//          else while ( srcidx != src.end())
            {   if ( src[n] == '\t' )
//          {   if ( *srcidx == '\t' )
                {   break;
                }
                dst += src[n++];
//                dst += *srcidx++;
            }
            break;
    }   }
//    qDebug() << QString("exit extractHatuonsurface");
    return ( TRUE );
}


/**
 * @fn
 * 数字列の読み変換
 * @brief 数字列の読み変換
 * @param (&pszYomi) ディスティネーションバッファ
 * @param (szNumbers) ソース数字列バッファ
 * @return なし
 * @sa
 * @detail 数字列の読み変換
 */
void premecab::num2yomi( QString &pszYomi, QString szNumbers )
{	static QString szDigitYomi[] =
    {	/*uR*/"レー",	// 0 レー         uR = const utf16 のはずがエラーになる
        /*uR*/"イチ",	// 1 イチ
        /*uR*/"ニー",	// 2 ニー
        /*uR*/"サン",	// 3 サン
        /*uR*/"ヨン",	// 4 ヨン
        /*uR*/"ゴー",	// 5 ゴー
        /*uR*/"ロク",	// 6 ロク
        /*uR*/"ナナ",	// 7 ナナ
        /*uR*/"ハチ",	// 8 ハチ
        /*uR*/"キュー"	// 9 キュー
    };
    static QString szKetaYomi[] =
    {	"ジュー",      //		10 ジュー
        "ヒャク",  	//		百 ヒャク
        "セン",		// ^3	千 セン
        "マン",		// ^4	万マン
        "ジューマン",	//		十万ジューマン
        "ヒャクマン",	//		百万ヒャクマン
        "センマン",	//		千万センマン
        "オク",		// ^8	億オク
        "ジューオク",	//		十億ジューオク
        "ヒャクオク",	//		百億ヒャクオク
        "センオク",	//		千億センオク
        "チョー",      // ^12	兆チョー
        "ジュッチョー",	//		十兆ジュッチョー
        "ヒャクチョー",	//		百兆ヒャクチョー
        "センチョー",	//		千兆センチョー
        "ケイ",		// ^16	京ケイ
        "ジュッケイ",	//		十京ジュッケイ
        "ヒャッケイ",	//		百京ヒャッケイ
        "センケン",	//		千京センケン
        "ガイ",		// ^20	垓ガイ
        "ジュッガイ",	//		十垓ジュッガイ
        "ヒャクガイ",	//		百垓ヒャクガイ
        "センガイ",	//		千垓センガイ
                    // ^24  秭（し）秭（じょ）
                    // ^28　穰（じょう）
                    // ^32　溝（こう）
                    // ^36　澗（かん）
                    // ^40　正（せい）
                    // ^44　載（さい）
                    // ^48　極（ごく）
                    // ^52  恒河沙（ごうがしゃ）
                    // ^56  阿僧祇（あそうぎ）
                    // ^60  那由他（なゆた）
                    // ^64  不可思議（ふかしぎ）
                    // ^68  無量大数（むりょうたいすう）
    //	"",			//
    };
    int cbSeisuubu, cbSeisuubuCpy;

    cbSeisuubu = 0;
    for ( auto srcidx = szNumbers.begin(); srcidx != szNumbers.end(); cbSeisuubu++, srcidx++ )
    {   if ( *srcidx == '.' )
        {   break;
    }   }
    pszYomi.clear();
    cbSeisuubuCpy = cbSeisuubu;
    //    cbSeisuubu = 0;
    for ( auto srcidx = szNumbers.begin(); srcidx != szNumbers.end(); srcidx++ )
    {   if (( *srcidx ).isDigit())
        {	if ( cbSeisuubu > 1 )
            {	if (( *srcidx ) != '0' )
                {	if (( *srcidx ) != '1' )
                    {	pszYomi += szDigitYomi[( *srcidx ).digitValue()];
                    }   //					if ( cbSeisuubu >= 2 )
                    pszYomi += szKetaYomi[cbSeisuubu - 2];
                }
                cbSeisuubu--;
            }
            else
            {	if ((( *srcidx ) != '0' ) || ( cbSeisuubuCpy == 1 ))	// '0'で無いか、1桁の'0'なら発音
                {   pszYomi += szDigitYomi[( *srcidx ).digitValue()];
        }	}	}
        else if (( *srcidx ) == '.' )
        {	pszYomi += "テン";
        }
        else if (( *srcidx ) == '-' )
        {	pszYomi += "マイナス";
        }
    }
}


/**
 * @fn
 * アルファベットの全角小文字化
 * @brief アルファベットの全角小文字化
 * @param (&str) ソース＆ディスティネーションバッファ
 * @return なし
 * @sa
 * @detail アルファベットの全角小文字化(辞書には全角小文字で登録しているので)
 * ついでに数字を半角化(数字列の読み変換のため)
 */
void premecab::toZensmall( QString &str )
{
    for ( auto ix = str.begin(); ix != str.end(); ix++ )
    {   if (( 'A' <= *ix ) && ( *ix <= 'Z' ))           // 半角大文字
        {   *ix = QChar(( *ix ).unicode() + ( u'ａ' -  u'A' ));
//          A	0x41	0x4100  Z	0x5A	0x5A00
//          a	0x61	0x6100  z	0x7A	0x7A00
        }
        else if (( 'a' <= *ix ) && ( *ix <= 'z' ))      // 半角小文字
        {   *ix = QChar(( *ix ).unicode() + ( u'ａ' -  u'a' ));
//          A	0x41	0x4100  Z	0x5A	0x5A00
//          a	0x61	0x6100  z	0x7A	0x7A00
        }
        else if (( u'Ａ' <= *ix ) && ( *ix <= u'Ｚ' ))   // 全角大文字
        {   *ix = QChar(( *ix ).unicode() + ( u'ａ' -  u'Ａ' ));
//          区 点 JIS  SJIS EUC  UTF-8  UTF-16 字
//          03 33 2341 8260 A3C1 EFBCA1 FF21   Ａ
//          03 58 235A 8279 A3DA EFBCBA FF3A   Ｚ
        }
#if 0
        else if (( u'ａ' <= *ix ) && ( *ix <= u'ｚ' ))
        {   *ix = QChar(( *ix ).unicode() + ( u'a' -  u'ａ' ));
//          区 点 JIS  SJIS EUC  UTF-8  UTF-16 字
//          03 65 2361 8281 A3E1 EFBD81 FF41   ａ
//          03 90 237A 829A A3FA EFBD9A FF5A   ｚ
        }
#endif
        else if (( u'０' <= *ix ) && ( *ix <= u'９' ))
        {   *ix = QChar(( *ix ).unicode() - u'０' + u'0' );
        }
    }
}


/**
 * @fn
 * utf8文字列でポインタ位置の文字が英字か判定する
 * @brief utf8文字列でポインタ位置の文字が英字か判定する
 * @param (＊ｐ) utf8ポインタ
 * @return true:英字 / false:非英字
 * @sa
 * @detail utf8文字列でポインタ位置の文字が英字か判定する
 * utf8なので3バイトで1文字を表すので
 */
bool u8isalpha( BYTE *p )
{   bool utf8alpha = false;
    BYTE ch;

    if ( *p == 0xef )  // 全角(UTF8)英文字チェック
    {   ch = *( p + 1 );
        if ( ch == 0xbc )
        {   ch = *( p + 2 );
            utf8alpha = (( 0xa1 <= ch ) || ( ch <= 0xba ))? true: false;
        }
        else if ( ch == 0xbd )
        {   ch = *( p + 2 );
            utf8alpha = (( 0x81 <= ch ) || ( ch <= 0x9a ))? true: false;
    }   }
    return ( utf8alpha );
}


/**
 * @fn
 * mecabで解析する
 * @brief mecabで解析する
 * @param (qsTalkData) トークデータ構造体
 * @return 0:正常終了 / 非0:エラー
 * @sa
 * @detail mecabで解析する（かな(sapi用)バッファと漢字かな交じり(open jtalk)バッファ生成）
 * また、行末で語彙が分断されている可能性を補正する
 */
int premecab::analyzmecab( struct LINEDATA qsTalkData )
{   CMultiLang cml;
    soundPlayState  plyStat;
    const mecab_node_t *node;
    TalkText talkTxt;
    QString bufHiraganaJis;
    QString HatuonBuff, bufShiftJis;
    static char szSurface[MAX_LINE];
    char    xmlBuff[MAX_LINE + 1024];	// for debug
    char    utf8Buff[MAX_LINE + 1024];	// for debug
    int     len;

    if (( hMecabDLL == NULL ) || ( mecabif::OkMecab == FALSE ) || ( lattice == NULL ))
    {	return ( 1 );
    }
    if ( qsTalkData.qsLineStr.isEmpty())		//
    {   if ( plyStat.getPlayState() == WAITENDOFTALK )
        {   if ( TalkDat.kanaBuff.length() > 0 )
            {   while ( talkTxt.QueTalkData.count() >= ( MAXTALKDATAQUE - 1 ))
                {   if (( plyStat.getPlayState() != PLAY ) && ( plyStat.getPlayState() != PAUSE ) && ( plyStat.getPlayState() != WAITENDOFTALK ))
                    {   return ( -1 );
                    }
                    QThread::msleep( 500 );
                }
                talkTxt.QueTalkData.enqueue( TalkDat );
                TalkDat.kanaBuff.clear();
                TalkDat.textBuff.clear();
                return ( 0 );           	//
    }   }   }
    if ( qsTalkData.qsLineStr == "\n" )		// utf8でもおなじ？
    {	return ( 0 );           	// 空行　改行だけの行は
    }
    TalkDat.lineNo = qsTalkData.iLineNo;
    xchgJIS2004( &qsTalkData.qsLineStr );		// JIS2004追加漢字を以前の漢字に置き換え
    toZensmall( qsTalkData.qsLineStr );         // アルファベット全角小文字化
    srcBuff += qsTalkData.qsLineStr;
//    bufUTF16 = srcBuff;
//    qDebug() << "0[" << srcBuff << "]";
    strcpy_s( utf8Buff, sizeof( utf8Buff ) - 1, srcBuff.toUtf8().data());
    fpmecab_lattice_set_sentence( lattice, utf8Buff );
    fpmecab_parse_lattice( mecab, lattice );
    node = fpmecab_lattice_get_bos_node( lattice );
//    qDebug() << QString("fpmecab_lattice_get_bos_node() exit");
    bufHiraganaJis.clear();
    for ( node = node->next; node != NULL; node = node->next )
    {	if ( plyStat.getPlayState() == PLAYSTATE::STOP )
        {	break;
        }
        if ( node->posid == 0 )
        {
#if 0									// 次行来た時でいいんじゃ？
            if ( numbers.isEmpty())		// 数字列バッファは空？
            {	continue;
            }
            QString szYomi;

            num2yomi( szYomi, numbers );
            HatuonBuff = szYomi;
            numbers.clear();
#endif
            continue;
        }
        else
        {	if (( node->next )->posid == 0 )        // 行末
            {	strncpy_s( szSurface, sizeof( szSurface ), ( char * )node->surface, node->length );
                szSurface[node->length] = EOS;
                if ( u8isalpha(( BYTE * )szSurface ) == false )
                {   srcBuff = szSurface;            // 日本語は行末で単語が分断されている事（原稿用紙とかは特に）が在るので
                    continue;                       // 行末ワードを次行の先頭に
                }
                srcBuff = u8"　";                   // 対して英語は別の単語をくっつける事になるので、次行行頭にくっつけるバッファは空白1個
            }
            size_t featurelen = strlen( node->feature );
            strncpy_s( szSurface, sizeof( szSurface ), ( char * )node->feature, featurelen );
            szSurface[featurelen] = EOS;
            cml.UTF8ToUni(( char * )szSurface, xmlBuff, sizeof( xmlBuff ));
//			qDebug() << QString("xmlBuff:=%1").arg(( wchar_t * )xmlBuff);
            bufShiftJis = QString::fromWCharArray(( wchar_t * )&( xmlBuff[0]));
//			qDebug() << QString("bufShiftJis:=%1").arg(bufShiftJis);
            if ( extractHatuonfeature( HatuonBuff, bufShiftJis, IDKATAKANA ) == 0 )
            {	strncpy_s( szSurface, sizeof( szSurface ), ( char * )node->surface, node->length );
                szSurface[node->length] = EOS;
                cml.UTF8ToUni(( char * )szSurface, xmlBuff, sizeof( xmlBuff ));
                bufShiftJis = QString::fromWCharArray(( wchar_t * )&( xmlBuff[0]));
                extractHatuonsurface( HatuonBuff, bufShiftJis );
//                bufShiftJis += "\n";
//                msgBuff.enterMessage( bufShiftJis );
                msgBuff.enterMessageWcheck( bufShiftJis );
//                qDebug() << "1[" << bufShiftJis << "]";
            }
            else    // isalpha() 0xe3 でエクセプション'「'アルファベットじゃないよね？
            {
#if 0               // 英単語辞書登録を全角小文字にしたので
                if ( u8isalpha( *(( char * )node->surface ))) // 事前にアルファベットは全角小文字にしているので
                {   strcpy_s( szSurface, sizeof( szSurface ), HatuonBuff.toUtf8().toStdString().c_str());
                }       // vvoxに半角小文字英単語を送っても文字ごとに読むので、仕方なくカタカナ変換後文字列を送る！辞書のイントネーションが再現されない
                else    // なお、全角英単語の場合は未確認！
#endif
                {   strncpy_s( szSurface, sizeof( szSurface ), ( char * )node->surface, node->length );
                    szSurface[node->length] = EOS;
//  				qDebug() << "2[" << HatuonBuff << "]";
            }   }
            QString word = szSurface;
            TalkDat.textBuff += word;
            if ( u8isalpha(( BYTE * )szSurface ) != false )
            {   TalkDat.textBuff += u8"　";              // アルファベットの時だけ全角空白をアペンド
            }
            if ( HatuonBuff.isEmpty())
            {	continue;
            }
            if (( !numbers.isEmpty()) && ( bufShiftJis[0] == ',' ) &&	// 数字列のカンマ区切り
                ( isdigit( *((( char * )node->surface ) + 1 ))))
            {	continue;
            }
            else if ( HatuonBuff[0].isDigit())
            {	if ( numbers.isEmpty())
                {	fgComma = FALSE;
                }
                numbers += HatuonBuff;
                if ( numbers.length() < MAXCVTUNIT )
                {   continue;   // 数字ばかりのソースで無限continueしないよう
            }   }
            else if (( HatuonBuff[0] == '.' ) && ( fgComma == FALSE ))	// 小数点対応
            {	fgComma = TRUE;
                numbers += HatuonBuff;
                continue;
            }
            else if ( HatuonBuff[0] == ':' )							// ':' 対応
            {	numbers += HatuonBuff;
                continue;
            }
            else if ( !numbers.isEmpty())								// 半角数字でない文字出現時に数字列バッファが空でなければ
            {	QString szYomi;

                num2yomi( szYomi, numbers );
                szYomi += HatuonBuff;
                HatuonBuff = szYomi;
                numbers.clear();
        }	}
//        toZensmall( HatuonBuff );              // アルファベット半角小文字化
        TalkDat.kanaBuff += HatuonBuff;
//		qDebug() << QString("pLine:=%1").arg(TalkDat.kanaBuff);
        if (( len = TalkDat.kanaBuff.length()) <= 0 )
        {	continue;
        }
        if ( len < MAXCVTUNIT )
        {   if ( node->next != 0 )
            {	if (( node->next )->posid != 0 )
                {	continue;
        }   }   }
//        qDebug() << QString("bufShiftJis:=%1").arg( TalkDat.kanaBuff );
        qDebug() << QString("bufShiftJis:=%1").arg( TalkDat.textBuff );
        while ( talkTxt.QueTalkData.count() >= ( MAXTALKDATAQUE - 1 ))
        {   if (( plyStat.getPlayState() != PLAY ) && ( plyStat.getPlayState() != PAUSE ) && ( plyStat.getPlayState() != WAITENDOFTALK ))
            {   return ( -1 );
            }
            QThread::msleep( 500 );
        }
        if (( plyStat.getPlayState() != STOP ) && ( plyStat.getPlayState() != PRESTOP ))
        {   talkTxt.QueTalkData.enqueue( TalkDat );
        }
        TalkDat.kanaBuff.clear();
        TalkDat.textBuff.clear();
    }
    return ( 0 );
}

