/**
 * @file about.cpp
 * @brief バージョン情報ダイヤログ
 * @author m.kawakami
 * @date 21/05/??
 */

#include "about.h"
#include "ui_about.h"
#include "portaudiodll.h"
#include "playlinevvoxcore.h"
//#include "voicevox_core.h"
#include "mecabif.h"

/**
 * @fn
 * バージョン情報コンストラクタ
 * @brief 自バージョンと使用各dllなどのバージョン情報の設定
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail 各Labelにそれぞれのバージョン情報文字列をセットする
 */
About::About(QWidget *parent) :
      QDialog(parent),
      ui(new Ui::About)
{   QString verVvox;
    QString verMecab;

    ui->setupUi(this);
#if 0
    if ( hVvoxCoreDll != NULL )   // バージョンダイヤログを開いた際VoiceVox coreがロードされていないかもしれないので
    {   verVvox = QString(ui->verVoicevox->text()).arg(voicevox_get_version());
    }
    else
#endif
    {   verVvox = QString(ui->verVoicevox->text()).arg("0.14.5");
    }
    ui->verVoicevox->setText( verVvox );

    QString verPAudio = QString(ui->verPortaudio->text()).arg( Pa_GetVersionText());
    ui->verPortaudio->setText( verPAudio );

    if ( mecabif::hMecabDLL != NULL )
    {    verMecab = QString(ui->verMecab->text()).arg( mecabif::fpmecab_version());
    }
    else
    {    verMecab = QString(ui->verMecab->text()).arg( "0.996" );
    }
    ui->verMecab->setText( verMecab );
}


/**
 * @fn
 * バージョン情報デストラクタ
 * @brief クラスの後処理
 * @return なし
 * @sa
 * @detail クラスの後処理
 */
About::~About()
{
    delete ui;
}


/**
 * @fn
 * バージョン情報OKボタン押し下げ時の処理
 * @brief OKボタン押し下げ時の処理
 * @return なし
 * @sa
 * @detail OKボタン押し下げ時の処理にダイヤログを閉じる
 */
void About::onOk()
{   close();
}

