/**
 * @file text2wave.cpp
 * @brief 文字列の補正を行う
 * @author m.kawakami
 * @date 23/10/??
 */

#include "text2wave.h"
#include "profilevalues.h"

extern profileValues   *pprof;
text2wave           *pTts = NULL;

QChar               szLstChar[4] = { EOS };
//text2wave           *text2wave::pTts = NULL;
CComPtr <ISpStream> text2wave::cpWavStream;
CRITICAL_SECTION    text2wave::cstPlayRec;
int                 text2wave::ctRcvDone;
int                 text2wave::ctCnfDone;
volatile int        text2wave::ctPlayRec;				// waveOutWrite touroku count
int                 text2wave::ixPlayRec;				// waveOutWrite touroku index
//WAVEHDR            	text2wave::whdr[1];             // wave header
//LPBYTE              text2wave::lpWave[1];				// wave buf
//long long int       text2wave::bcWave[1];				// alloc size
//struct WAVEBUFFER   text2wave::waveBuffer[2];           //
//QQueue<struct WAVEBUFFER>   text2wave::queWaveBuffer;   //


#define isUTF16HanASCII(c)	((' '<=c)&&(c<='}'))
#define isUTF16ZenASCII(c)	((u'　'<=c)&&(c<=u'｝'))


/**
 * @fn
 * text2waveクラスコンストラクタ
 * @brief text2waveクラスコンストラクタ
 * @return なし
 * @sa
 * @detail クラス変数、バッファの初期化
 */
text2wave::text2wave()
{
    ctRcvDone = 0;
    ctCnfDone = 0;
    ixPlayRec = 0;              // waveOutWrite touroku index
    ctPlayRec = 0;              // waveOutWrite touroku count
//    lpWave[0] = NULL;			// wave buf
//    bcWave[0] = 0;				// alloc size
    InitializeCriticalSection( &cstPlayRec );
    memset( &waveBuffer, 0, sizeof( waveBuffer ));
    for ( int i = 0; i < lengthof( waveBuffer ); i++ )
    {   //waveBuffer[i].iLineNo = 0;
        waveBuffer[i].iBuffNo = i;
        waveBuffer[i].allocType = mallocType::SAPI;
    }
}


/**
 * @fn
 * text2waveクラスデストラクタ
 * @brief text2waveクラスデストラクタ
 * @return なし
 * @sa
 * @detail クリティカルセクションの破棄、バッファの解放
 */
text2wave::~text2wave()
{
    DeleteCriticalSection( &cstPlayRec );
    for ( int i = 0; i < lengthof( waveBuffer ); i++ )
    {   if ( waveBuffer[i].lpWave != NULL )
        {   free( waveBuffer[i].lpWave );
            waveBuffer[i].lpWave = NULL;
            waveBuffer[i].bcWave = 0;
        }
    }
}


/**
 * @fn
 * アプリケーションボリュームセット
 * @brief アプリケーションボリュームセット
 * @return なし
 * @sa
 * @detail アプリケーションボリュームセット
 */
void text2wave::appVolumeSet( unsigned short volume )
{
    if ( hWaveOut != 0 )
    {   unsigned short value = ( volume << 16 ) | ( volume & 0xffff );

        if ( waveOutSetVolume( hWaveOut, value ) != MMSYSERR_NOERROR )
        {	return;
    }	}
}


/**
 * @fn
 * wave終了処理
 * @brief wave終了処理
 * @return なし
 * @sa
 * @detail wave終了処理
 */
int text2wave::waveFainalize()
{	if ( hWaveOut != 0 )
    {	waveOutClose( hWaveOut );
        hWaveOut = 0;
    }
    StateWaveOutOpen = WFNONOPEN;
    return ( 0 );
}


/**
 * @fn
 * wave終了時のコールバック処理
 * @brief wave終了時のコールバック処理
 * @param (hwo) wave出力デバイスハンドル
 * @param (uMsg) メッセージ
 * @param (dwInstance) インスタンス
 * @param (dwParam1) パラメータ１
 * @param (dwParam2) パラメータ２
 * @return なし
 * @sa
 * @detail wave終了時のコールバック処理
 */
//WAVE出力デバイスコールバック関数（スタティック関数）
void __stdcall text2wave::StaticWaveOutProc( HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance,
                                 DWORD dwParam1, DWORD dwParam2 )
{
    if ( uMsg == MM_WOM_DONE )
    {	// _sleep( 3000 );
        ctRcvDone++;
        EnterCriticalSection( &cstPlayRec );
        if ( ctPlayRec > 0 )
        {	ctPlayRec--;
        }
        LeaveCriticalSection( &cstPlayRec );
    }
}


/**
 * @fn
 * wave出力リスタート処理
 * @brief wave出力リスタート処理
 * @return なし
 * @sa
 * @detail wave出力リスタート処理
 */
void text2wave::reStart()
{
//    qDebug() << QString("text2wave::reStart-001:=%1").arg(queWaveBuffer.count());
    if ( queWaveBuffer.count() > 0 )            // que non empty?
//    {   if ( paif.Pa_IsStreamStopped( &paif.stream ) != 1 )
//        {   //paif.reStart();
//            struct WAVEBUFFER wb = queWaveBuffer.head();
//            paif.open( wb.wfe );
            paif.open();
//        }
//    }
}


/**
 * @fn
 * カタカナ変換処理
 * @brief カタカナ変換処理
 * @param (&pKatakana) カタカナ変換後ストア先
 * @param (pSorceStr) ソース文字列
 * @return なし
 * @sa
 * @detail カタカナ変換処理
 */
void text2wave::conv2Katakana( QString &pKatakana, QString pSorceStr )
{   static const QString pszOnseikigouZen( "゜ ・ ー ヽ ヾ 。 、" );
    static const QString pszOnseikigouHan( "!+.?_|" );		// '除外
    QChar srcch;

    pKatakana.clear();
    for ( auto srcidx = pSorceStr.begin(); srcidx != pSorceStr.end(); srcidx++ )
    {   srcch = ( *srcidx );
        if ( isUTF16HanASCII( srcch ))    //  utf16le 0x0020-0x007d   半角ascii     全角は0xff00-0xff5d
        {   if ( !srcch.isLetterOrNumber() && ( srcch != '-' ) && ( srcch != '.' ))
            {   if ( pszOnseikigouHan.indexOf( srcch ) < 0 )
                {   continue;
            }   }
            pKatakana += srcch;
        }
        else // if ( _ismbblead( *p ))
        {   if (( u'ゖ' >= srcch ) && ( srcch >= u'ぁ' ))	//    'ぁ' ~ 'ゖ'
            {   srcch.setCell( srcch.unicode() + ( u'ァ' - u'ぁ' ));
            }
            else if ( u'゜' == srcch )										//    '゜'
            {	;
            }
            if ( isUTF16ZenASCII( srcch ))    //  utf16le 0x0020-0x007d   半角ascii     全角は0xff00-0xff5d
            {   srcch.setCell(( srcch.unicode() & 0xff ) + u' ' );  // 全角ASCIIは半角ASCIIに直す
            }
            else if ( pszOnseikigouZen.indexOf( srcch ) < 0 )
            {	continue;
            }
//            0xff66-0xff9f     半角カタカナ
/*  全角カタカナ
U+30Ax  ァ ア ィ イ ゥ ウ ェ エ ォ オ カ ガ キ ギ ク
U+30Bx	グ ケ ゲ コ ゴ サ ザ シ ジ ス ズ セ ゼ ソ ゾ タ
U+30Cx	ダ チ ヂ ッ ツ ヅ テ デ ト ド ナ ニ ヌ ネ ノ ハ
U+30Dx	バ パ ヒ ビ ピ フ ブ プ ヘ ベ ペ ホ ボ ポ マ ミ
U+30Ex	ム メ モ ャ ヤ ュ ユ ョ ヨ ラ リ ル レ ロ ヮ ワ
U+30Fx	ヰ ヱ ヲ ン ヴ ヵ ヶ ヷ ヸ ヹ ヺ

    半角カタカナ  濁点、半濁点など2文字を１全角文字に置き換えないといけない
U+FF6x	｠ ｡ ｢ ｣ ､ ･ ｦ ｧ ｨ ｩ ｪ ｫ ｬ ｭ ｮ ｯ
U+FF7x	ｰ ｱ ｲ ｳ ｴ ｵ ｶ ｷ ｸ ｹ ｺ ｻ ｼ ｽ ｾ ｿ
U+FF8x	ﾀ ﾁ ﾂ ﾃ ﾄ ﾅ ﾆ ﾇ ﾈ ﾉ ﾊ ﾋ ﾌ ﾍ ﾎ ﾏ
U+FF9x	ﾐ ﾑ ﾒ ﾓ ﾔ ﾕ ﾖ ﾗ ﾘ ﾙ ﾚ ﾛ ﾜ ﾝ ﾞ ﾟ
*/
                if (( u'ヾ' >= srcch ) && ( srcch >= u'ァ' ))		// カタカナ？
            {	if ( u'ヂ' == srcch )            // 発音無いらしいので
                {	srcch = u'ジ';
                }
                else if ( u'ヅ' == srcch )       // 発音無いらしいので
                {	srcch = u'ズ';
                }
                else if ( u'・' == srcch )		// Microsoft Speech API 5.4 Japanese Phonemes
                {	continue;               	// Middle dot と記載されているが通すとエラーになる
            }	}
            pKatakana += srcch;
        }
    }
//	Japanese Phonemes Microsoft Speech API 5.4
//		UNI		SJIS
//	!	0021	21		Sentence end (exclamation)
//	'	0027	27		Accent position				1重引用符と発音する…
//	+	002B	2b		Accent boundary
//	.	002E	2e		Sentence end (standard)
//	?	003F	3f		Sentence end (interrogative)
//	_	005F	5f		One mora pause
//	|	007C	7c		Phrase boundary
//	゜	309C	814b	Semi-voiced sound
//	・	30FB	8145	Middle dot
//	ー	30FC	815b	Prolonged sound
//	ヽ	30FD	8152	Iteration
//	ヾ	30FE	8153	Voiced iteration

//	　	3000	8140	space	pouse
//	、	3001	8141
//	。	3002	8142
}



