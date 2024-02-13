/**
 * @file settingdlg.cpp
 * @brief 初期設定ダイヤログの処理
 * @author m.kawakami
 * @date 23/10/??
 */

#include "mainwindow.h"
#include "settingdlg.h"
#include "ui_settingdlg.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDial>
#include "profilevalues.h"
#include "../../AquesTalk.h"
#include <windows.h>
#include <mmsystem.h>
#include <sapi.h>
#include <sphelper.h>
#include "messagebuff.h"
#include "soundplaystate.h"

extern profileValues   *pprof;
QString qsRegistryEntry[] =
{   QString::fromWCharArray( SPCAT_VOICES ),
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech_OneCore\\Voices",
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech Server\\v11.0\\Voices",
//    NULL
};


/**
 * @fn
 * SettingDlgクラスコンストラクタ
 * @brief SettingDlgクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail SettingDlgの初期設定
 */
SettingDlg::SettingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDlg)
{
    ui->setupUi(this);
}


/**
 * @fn
 * SettingDlgクラスデストラクタ
 * @brief SettingDlgクラスデストラクタ
 * @return なし
 * @sa
 * @detail SettingDlgクラスデストラクタ
 */
SettingDlg::~SettingDlg()
{
    delete ui;
}


/**
 * @fn
 * レジストリ情報の取得
 * @brief レジストリ情報の取得
 * @param (hKey) 主キー
 * @param (*subKey) サブキー
 * @param (*value) 値格納ポインタ
 * @return なし
 * @sa
 * @detail レジストリ情報の取得
 */
static wchar_t *RegGetString( HKEY hKey, const wchar_t *subKey, const wchar_t *value )
{	DWORD dataSize;
    wchar_t *pszStr;
    LONG retCode;

    if ( ERROR_SUCCESS == ( retCode = ::RegGetValue( hKey, subKey, value, RRF_RT_REG_SZ, NULL, NULL, &dataSize )))
    {	if ( NULL != ( pszStr = ( wchar_t * )malloc( dataSize + sizeof( wchar_t ))))
        {	if ( ERROR_SUCCESS == ( retCode = ::RegGetValue( hKey, subKey, value, RRF_RT_REG_SZ, NULL, pszStr, &dataSize )))
            {	return ( pszStr );
    }	}	}
    return ( NULL );
}


/**
 * @fn
 * sapiボイスリストの取得
 * @brief sapiボイスリストの取得
 * @param (*p_VoiceSel) コンボボックスポインタ
 * @param (*pszEntry) エントリーポインタ
 * @return -1:エラー / >0:追加数
 * @sa
 * @detail sapiボイスリストの取得
 */
int SettingDlg::makeVoiceListAdd( QComboBox *p_VoiceSel, QString *pszEntry )
{	CComPtr<ISpObjectTokenCategory> cpSpCategory = NULL;
    CComPtr<IEnumSpObjectTokens>	cpSpEnumTokens;
    CComPtr<ISpObjectToken>			tok[64];
    HRESULT hr;
    ULONG i, n, cnt;
    LPWSTR ws;
    WCHAR *pName, *pGender, *pLanguage, *pAge, buf[1024];

    if ( S_OK != ( hr = SpGetCategoryFromId( reinterpret_cast<LPCWSTR>( pszEntry->utf16()), &cpSpCategory )))
    {	return( -1 );
    }
    if ( S_OK != ( hr = cpSpCategory->EnumTokens( NULL, NULL, &cpSpEnumTokens )))
    {	return( -1 );
    }
//    p_VoiceSel->clear();
    hr = cpSpEnumTokens->GetCount( &n );
    if ( n > lengthof( tok )) n = lengthof( tok );
    for ( cnt = i = 0; i < n; i++ )
    {	hr = cpSpEnumTokens->Item( i, &tok[i]);
        hr = tok[i]->GetId( &ws );
        swprintf( buf, lengthof( buf ) - 1, L"%s\\Attributes", ws );
        CoTaskMemFree( ws );
        pName = RegGetString( HKEY_LOCAL_MACHINE, &buf[19], L"Name" );
        if ( *( pprof->sapi.pwszVoicevox4sapiRunPath ) == "" )          // voicevox for sapi のボイス名は除外
        {   QString voiceName = QString::fromWCharArray( pName );
            if ( voiceName.startsWith( "VOICEVOX", Qt::CaseInsensitive ) == true )  // voiceNameは"VOICEVOX"で始まる?
            {   continue;
        }   }
        pGender = RegGetString( HKEY_LOCAL_MACHINE, &buf[19], L"Gender" );
        pLanguage = RegGetString( HKEY_LOCAL_MACHINE, &buf[19], L"Language" );
        pAge = RegGetString( HKEY_LOCAL_MACHINE, &buf[19], L"Age" );
        if ( p_VoiceSel )
        {	swprintf( buf, lengthof( buf ) - 1, L"%s[%s %s %s]", pName, pGender, pAge, pLanguage );
            if ( p_VoiceSel->findText( QString::fromWCharArray( buf )) <= -1 )
            {	p_VoiceSel->addItem( QString::fromWCharArray( buf ));
                QString validText = "Name=" + QString::fromWCharArray( buf ).left( QString::fromWCharArray( buf ).indexOf( "[" ));
                cnt++;
                qDebug() << "validText(" << validText << " )" << cnt;
                if ( *( pprof->sapi.pwszVoiceName ) == validText )
                {   p_VoiceSel->setCurrentIndex( cnt );
                    qDebug() << "validText(" << validText << " )" << cnt << "ここ";
                }
        }	}
        free( pAge );
        free( pLanguage );
        free( pGender );
        free( pName );
    }
    return ( cnt );
}


/**
 * @fn
 * sapiボイスリストの生成
 * @brief sapiボイスリストの生成
 * @param (*p_VoiceSel) コンボボックスポインタ
 * @return >0:コンボボックスの追加数
 * @sa
 * @detail sapiボイスリストの生成
 */
int SettingDlg::makeVoiceList( QComboBox *p_VoiceSel )
{	int idx;

    p_VoiceSel->clear();
    p_VoiceSel->addItem( "未選択" );
    for ( idx = 0;  idx < lengthof( qsRegistryEntry ); idx++ )
    {	makeVoiceListAdd( p_VoiceSel, &( qsRegistryEntry[idx]));
    }
    return ( p_VoiceSel->count());
}


/**
 * @fn
 * sapiボイス名のセット
 * @brief sapiボイス名のセット
 * @param (*p_VoiceSel) コンボボックスポインタ
 * @return 無し
 * @sa
 * @detail sapiボイス名のセット　コンボボックスの選択行から必要な部分のみプロファイルデータにセット
 */
void SettingDlg::SetVoiceName()
{   QString selText, validText;

    selText = ui->sapiVoiceSelCmbbox->currentText();
    validText = "Name=" + selText.left( selText.indexOf( "[" ));
    if ( !pprof->sapi.pwszVoiceName->isEmpty())
    {	free( pprof->sapi.pwszVoiceName );
        pprof->sapi.pwszVoiceName = NULL;
    }
    pprof->sapi.pwszVoiceName = new QString( validText );
}	//"Name=%s Language = 411"


/**
 * @fn
 * 設定ダイヤログの表示前の値セット
 * @brief 設定ダイヤログの表示前の値セット
 * @return 無し
 * @sa
 * @detail 設定ダイヤログの表示前の値セット
 */
void SettingDlg::show()
{   char edBuf[16];

//    enum TTSNAME { SAPI54 = 0, AQTK10, VVoxCore };
    ui->ttsSelectorCombo->clear();
    ui->ttsSelectorCombo->addItem( "未選択" );
    ui->ttsSelectorCombo->addItem( "Sapi" );
    ui->ttsSelectorCombo->addItem( "Aqtk10" );
    ui->ttsSelectorCombo->addItem( "VoiceVox core" );
    ui->ttsSelectorCombo->setCurrentIndex( pprof->oper.ttsSelector + 1 );
    qDebug() << "ui->ttsSelectorCombo->setCurrentIndex( " << pprof->oper.ttsSelector << " )";

    ui->RAMDISKDIR->setText( *( pprof->other.pwszRamDiskPath ));
    ui->texyDisplayBlackBakMode->setChecked( pprof->other.selBakeColorBlack );
    ui->edtorExePathEdit->setText( *( pprof->other.pwszEdtorPath ));
    ui->edtorTagJumpCmdEdit->setText( *( pprof->other.pwszEdtorTagjumpCommand ));
    ui->infoWndlineLimitEdit->setText( QString::number( pprof->other.infoWndLineMaxLimmit ));

    ui->mecabPathEdit->setText( *( pprof->mecab.pwszMecabPath ));
    ui->systemDictionaryFolderEdit->setText( *( pprof->mecab.pwszSystemDictionaryFolder ));
    ui->userDictionaryPathEdit->setText( *( pprof->mecab.pwszUserDictionaryPath ));
    ui->userCsvDictionaryFolderEdit->setText( *( pprof->mecab.pwszUserDictionaryCsvPath ));
    ui->systemDictionaryBuildFolderEdit->setText( *( pprof->mecab.pwszSystemDictionaryBuildFolder ));

    ui->aqtkSpeedDial->setMaximum( 300 );
    ui->aqtkSpeedDial->setMinimum( 50 );
    ui->aqtkSpeedDial->setValue( pprof->aqtk.iSpeed );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iSpeed );
    ui->AqtkSpeedValue->setText( edBuf );
//    ui->aqtkSpeedDial->setNotchesVisible( 1 );  // 目盛表示
//    ui->aqtkSpeedDial->setWrapping( 1 );      // ぐるぐるがスムースになる

    ui->aqtkStepDial->setMaximum( 200 );
    ui->aqtkStepDial->setMinimum( 0 );
    ui->aqtkStepDial->setValue( pprof->aqtk.iStep );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iStep );
    ui->aqtkStepValue->setText( edBuf );

    ui->aqtkHighDial->setMaximum( 200 );
    ui->aqtkHighDial->setMinimum( 20 );
    ui->aqtkHighDial->setValue( pprof->aqtk.iHigh );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iHigh );
    ui->aqtkHighValue->setText( edBuf );

    ui->aqtkAccentDial->setMaximum( 200 );
    ui->aqtkAccentDial->setMinimum( 0 );
    ui->aqtkAccentDial->setValue( pprof->aqtk.iAccent );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iAccent );
    ui->aqtkAccentValue->setText( edBuf );

    ui->aqtkStep2Dial->setMaximum( 200 );
    ui->aqtkStep2Dial->setMinimum( 50 );
    ui->aqtkStep2Dial->setValue( pprof->aqtk.iStep2 );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iStep2 );
    ui->aqtkStep2Value->setText( edBuf );

    ui->aqtkVolumeDial->setMaximum( 300 );
    ui->aqtkVolumeDial->setMinimum( 0 );
    ui->aqtkVolumeDial->setValue( pprof->aqtk.iVolume );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iVolume );
    ui->aqtkVolumeValue->setText( edBuf );

    ui->aqtkVoiceSel->clear();
    ui->aqtkVoiceSel->addItem( "未選択" );
    ui->aqtkVoiceSel->addItem( "F1E" );
    ui->aqtkVoiceSel->addItem( "F2E" );
    ui->aqtkVoiceSel->addItem( "F3E" );
    ui->aqtkVoiceSel->addItem( "M1E" );
    ui->aqtkVoiceSel->addItem( "M2E" );
    ui->aqtkVoiceSel->addItem( "R1E" );
    ui->aqtkVoiceSel->addItem( "R2E" );
    ui->aqtkVoiceSel->setCurrentIndex( pprof->aqtk.iVoiceIdx + 1 );

    ui->sapiOutputModeSelCmbbox->clear();
    ui->sapiOutputModeSelCmbbox->addItem( "未選択" );
    ui->sapiOutputModeSelCmbbox->addItem( "カタカナダイレクト" );
    ui->sapiOutputModeSelCmbbox->addItem( "SSML" );
    ui->sapiOutputModeSelCmbbox->setCurrentIndex( pprof->sapi.iOutputMode + 1 );
    makeVoiceList( ui->sapiVoiceSelCmbbox );

    ui->sapiMuonZan->setText( QString::number( pprof->sapi.iMuonZan ));
    ui->sapiRateDial->setMaximum( 10 );
    ui->sapiRateDial->setMinimum( -10 );
    ui->sapiRateDial->setValue( pprof->sapi.iRate );
    sprintf_s( edBuf, "Rate:%3d", pprof->sapi.iRate );
    ui->sapiRateLabel->setText( edBuf );

    ui->sapiVolumeDial->setMaximum( 100 );
    ui->sapiVolumeDial->setMinimum( 0 );
    ui->sapiVolumeDial->setValue( pprof->sapi.iVolume );
    sprintf_s( edBuf, "Volume:%3d", pprof->sapi.iVolume );
    ui->sapiVolumeLabel->setText( edBuf );
    ui->sapiVoiceVox4sapiPathEdit->setText( *( pprof->sapi.pwszVoicevox4sapiRunPath ));

    ui->OpenJTalkDicPathEdit->setText( *( pprof->vvox.pwszOpenJTalkDicPath ));
    ui->OpenJTalkDicBuildDirEdit->setText( *( pprof->vvox.pwszOpenJTalkDicBuildDir ));
    VoicevoxmodelListRead( ui->voicevoxVoiceSelComboBox );
//    QMetaObject::Connection h = QObject::connect(ui->voicevoxVoiceSelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(voicevoxIndexChanged()));
//    qDebug() << "connect = " << h;
    ui->settingTab->setCurrentIndex( pprof->oper.ttsSelector );

    QDialog::show();
}


/**
 * @fn
 * テキストエディット背景色セット
 * @brief テキストエディット背景色セット
 * @param (arg1) ？
 * @return 無し
 * @sa
 * @detail テキストエディット背景色セット
 */
void SettingDlg::on_texyDisplayBlackBakMode_stateChanged(int arg1)
{
    pprof->other.selBakeColorBlack = ui->texyDisplayBlackBakMode->checkState();
}


/**
 * @fn
 * tts選択コンボボックス、選択変更時の処理
 * @brief tts選択コンボボックス、選択変更時の処理
 * @param (index) 選択インデックス
 * @return 無し
 * @sa
 * @detail tts選択コンボボックス、選択変更時の処理
 */
void SettingDlg::on_ttsSelectorCombo_currentIndexChanged(int index)     // TTS selector
{    int currentTab = ui->settingTab->currentIndex();
    soundPlayState  plyStat;

    if ( plyStat.getPlayState() != PLAYSTATE::STOP )    // stop状態でなければ
    {   ui->ttsSelectorCombo->setCurrentIndex( pprof->oper.ttsSelector + 1 );
        return;
    }
    qDebug() << "on_ttsSelectorCombo_currentIndexChanged( " << index << " )";
    if (( index <= 0 ) || ( ui->ttsSelectorCombo->count() == 0 ))
    {   return;
    }
    if ( index == 2 )       // アクエストークは選択しないように
    {   index = 3;
        ui->ttsSelectorCombo->setCurrentIndex( index );
    }
    pprof->oper.ttsSelector = ( enum TTSNAME )( index - 1 );
    if (( currentTab != 0 ) && ( currentTab != 4 ))
    {   ui->settingTab->setCurrentIndex( index /* + 1 */);
#if 0
        switch ( static_cast<TTSNAME>( index ))
        { case TTSNAME::SAPI54:           // TAB TTSNAME::SAPI54
            ui->settingTab->setCurrentIndex( index + 1 );
            break;
          case TTSNAME::AQTK10:           // TAB TTSNAME::AQTK10
            ui->settingTab->setCurrentIndex( index + 1 );
            break;
          case TTSNAME::VVoxCore:         // TAB TTSNAME::VVoxCore
            ui->settingTab->setCurrentIndex( index + 1 );
            break;
        }
#endif
    }
}


/**
 * @fn
 * OKボタンクリック時の処理
 * @brief OKボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail OKボタンクリック時の処理
 */
void SettingDlg::on_OkButton_clicked()                      // OK button
{
    if ( ui->voicevoxVoiceSelComboBox->count() == 0 )
    {   return;
    }
    QString selText = ui->voicevoxVoiceSelComboBox->currentText();
//    QString selText = ui->voicevoxVoiceSelComboBox->itemText( index );
    int idPos = selText.indexOf( ':' );
    if ( idPos >= 0 )
    {   QString idNum = selText.mid( idPos + 1, 2 );
        pprof->vvox.ispeaker_id = idNum.toInt();
//        qDebug() << pprof->vvox.ispeaker_id << " == ";
    }
    this->hide();
//    QMessageBox::warning(this, tr("MessageBox"), tr("on_OkButton_clicked"));

}


/**
 * @fn
 * タブ、選択変更時の処理
 * @brief タブ、選択変更時の処理
 * @param (index) 選択インデックス
 * @return 無し
 * @sa
 * @detail タブ、選択変更時の処理
 */
void SettingDlg::on_settingTab_currentChanged(int index)         // Tab changed
{
   switch( index )
    {
//    case 0:     // mecab
//        break;
    case 1:     // sapi
        if ( pprof->oper.ttsSelector != TTSNAME::SAPI54 )
        {   ui->settingTab->setCurrentIndex(( int )pprof->oper.ttsSelector + 1 );
        }
        break;
    case 2:     // aqtk10
        if ( pprof->oper.ttsSelector != TTSNAME::AQTK10 )
        {   ui->settingTab->setCurrentIndex(( int )pprof->oper.ttsSelector + 1 );
        }
        break;
    case 3:     // voicevox core
        if ( pprof->oper.ttsSelector != TTSNAME::VVoxCore )
        {   ui->settingTab->setCurrentIndex(( int )pprof->oper.ttsSelector + 1 );
        }
        break;
//    case 4:     // other
//        break;
    default:     //
        break;
    }
}


/**
 * @fn
 * mecabパス参照ボタンクリック時の処理
 * @brief mecabパス参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail mecabパス参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::on_mecabPathRefButton_clicked()
{   QString selFilter = tr("実行ファイル(*.exe)");
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("MeCab.exeを選択"),
        *( pprof->mecab.pwszMecabPath ),
        tr("すべて(*.*);;実行ファイル(*.exe);;MeCabファイル(MeCab.exe)"),
        &selFilter,
        QFileDialog::DontUseCustomDirectoryIcons );
    if ( !fileName.isEmpty())
    {   ui->mecabPathEdit->setText( fileName );
        if ( !pprof->mecab.pwszMecabPath->isEmpty())
        {   delete pprof->mecab.pwszMecabPath;
        }
        pprof->mecab.pwszMecabPath = new QString( fileName );
        ui->mecabPathEdit->setText( *( pprof->mecab.pwszMecabPath ));
    }
}


/**
 * @fn
 * システム辞書ディレクトリパス参照ボタンクリック時の処理
 * @brief システム辞書ディレクトリパス参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail システム辞書ディレクトリパス参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::on_systemDictionaryFolderRefButton_clicked()
{   QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    fileDialog.setLabelText(QFileDialog::Accept,tr("Open"));
    fileDialog.setLabelText(QFileDialog::LookIn,tr("Open"));
    fileDialog.setWindowTitle("システム辞書のフォルダーを選択してください！");
    fileDialog.setDirectory( *( pprof->mecab.pwszSystemDictionaryFolder ));
    if(fileDialog.exec())
    {   QString filePaths = fileDialog.selectedFiles().at(0);
        ui->systemDictionaryFolderEdit->setText( filePaths );
        if ( !pprof->mecab.pwszSystemDictionaryFolder->isEmpty())
        {   delete pprof->mecab.pwszSystemDictionaryFolder;
        }
        qDebug() << QString("filePaths::%1").arg(filePaths);
        pprof->mecab.pwszSystemDictionaryFolder = new QString( filePaths );
        ui->systemDictionaryFolderEdit->setText( *( pprof->mecab.pwszSystemDictionaryFolder ));
        qDebug() << QString("*pprof->mecab.pwszSystemDictionaryFolder::%1").arg(*( pprof->mecab.pwszSystemDictionaryFolder));
    }
}


/**
 * @fn
 * ユーザー辞書パス参照ボタンクリック時の処理
 * @brief ユーザー辞書パス参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail ユーザー辞書パス参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::on_userDictionaryPathRefButton_clicked()
{   QString selFilter = tr("ユーザー辞書ファイル(*.dic)");
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("ユーザー辞書を選択"),
        *( pprof->mecab.pwszUserDictionaryPath ),
        tr("すべて(*.*);;ユーザー辞書ファイル(*.dic)"),
        &selFilter,
        QFileDialog::DontUseCustomDirectoryIcons );
    if ( !fileName.isEmpty())
    {   ui->userDictionaryPathEdit->setText( fileName );
        if ( !pprof->mecab.pwszUserDictionaryPath->isEmpty())
        {   delete pprof->mecab.pwszUserDictionaryPath;
        }
        pprof->mecab.pwszUserDictionaryPath = new QString( fileName );
        ui->userDictionaryPathEdit->setText( *( pprof->mecab.pwszUserDictionaryPath ));
    }
}


/**
 * @fn
 * ユーザー.csv辞書パス参照ボタンクリック時の処理
 * @brief ユーザー.csv辞書パス参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail ユーザー.csv辞書パス参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::on_userCsvDictionaryFolderRefButton_clicked()
{   QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    fileDialog.setLabelText(QFileDialog::Accept,tr("Open"));
    fileDialog.setLabelText(QFileDialog::LookIn,tr("Open"));
    fileDialog.setWindowTitle("ユーザーCSV辞書のフォルダーを選択してください！");
    fileDialog.setDirectory( *( pprof->mecab.pwszUserDictionaryCsvPath ));
    if(fileDialog.exec())
    {   QString filePaths = fileDialog.selectedFiles().at(0);
        ui->userCsvDictionaryFolderEdit->setText( filePaths );
        if ( !pprof->mecab.pwszUserDictionaryCsvPath->isEmpty())
        {   delete pprof->mecab.pwszUserDictionaryCsvPath;
        }
        pprof->mecab.pwszUserDictionaryCsvPath = new QString( filePaths );
        ui->userCsvDictionaryFolderEdit->setText( *( pprof->mecab.pwszUserDictionaryCsvPath ));
    }
}


/**
 * @fn
 * システム辞書ビルドフォルダー参照ボタンクリック時の処理
 * @brief システム辞書ビルドフォルダー参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail システム辞書ビルドフォルダー参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::on_systemDictionaryBuildFolderRefButton_clicked()
{   QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    fileDialog.setLabelText(QFileDialog::Accept,tr("Open"));
    fileDialog.setLabelText(QFileDialog::LookIn,tr("Open"));
    fileDialog.setWindowTitle("システム辞書の構築フォルダーを選択してください！");
    fileDialog.setDirectory( *( pprof->mecab.pwszSystemDictionaryBuildFolder ));
    if(fileDialog.exec())
    {   QString filePaths = fileDialog.selectedFiles().at(0);
        ui->systemDictionaryBuildFolderEdit->setText( filePaths );
        if ( !pprof->mecab.pwszSystemDictionaryBuildFolder->isEmpty())
        {   delete pprof->mecab.pwszSystemDictionaryBuildFolder;
        }
        pprof->mecab.pwszSystemDictionaryBuildFolder = new QString( filePaths );
        ui->systemDictionaryBuildFolderEdit->setText( *( pprof->mecab.pwszSystemDictionaryBuildFolder ));
    }
}


/**
 * @fn
 * mecabパスエディットボックス編集終了時の処理
 * @brief mecabパスエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail mecabパスエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::on_mecabPathEdit_editingFinished()
{   if ( !pprof->mecab.pwszMecabPath->isEmpty())
    {   delete pprof->mecab.pwszMecabPath;
    }
    pprof->mecab.pwszMecabPath = new QString( ui->mecabPathEdit->text());
}


/**
 * @fn
 * システム辞書フォルダーエディットボックス編集終了時の処理
 * @brief システム辞書フォルダーエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail システム辞書フォルダーエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::on_systemDictionaryFolderEdit_editingFinished()
{   if ( !pprof->mecab.pwszSystemDictionaryFolder->isEmpty())
    {   delete pprof->mecab.pwszSystemDictionaryFolder;
    }
    pprof->mecab.pwszSystemDictionaryFolder = new QString( ui->systemDictionaryFolderEdit->text());
}


/**
 * @fn
 * ユーザー辞書パスエディットボックス編集終了時の処理
 * @brief ユーザー辞書パスエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail ユーザー辞書パスエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::on_userDictionaryPathEdit_editingFinished()
{   if ( !pprof->mecab.pwszUserDictionaryPath->isEmpty())
    {   delete pprof->mecab.pwszUserDictionaryPath;
    }
    pprof->mecab.pwszUserDictionaryPath = new QString( ui->userDictionaryPathEdit->text());
}


/**
 * @fn
 * ユーザー.csv辞書パスエディットボックス編集終了時の処理
 * @brief ユーザー.csv辞書パスエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail ユーザー.csv辞書パスエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::on_userCsvDictionaryFolderEdit_editingFinished()
{   if ( !pprof->mecab.pwszUserDictionaryCsvPath->isEmpty())
    {   delete pprof->mecab.pwszUserDictionaryCsvPath;
    }
    pprof->mecab.pwszUserDictionaryCsvPath = new QString( ui->userCsvDictionaryFolderEdit->text());
}


/**
 * @fn
 * システム辞書ビルドフォルダーエディットボックス編集終了時の処理
 * @brief システム辞書ビルドフォルダーエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail システム辞書ビルドフォルダーエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::on_systemDictionaryBuildFolderEdit_editingFinished()
{   if ( !pprof->mecab.pwszSystemDictionaryBuildFolder->isEmpty())
    {   delete pprof->mecab.pwszSystemDictionaryBuildFolder;
    }
    pprof->mecab.pwszSystemDictionaryBuildFolder = new QString( ui->systemDictionaryBuildFolderEdit->text());
}


/**
 * @fn
 * エディター実行ファイル参照ボタンクリック時の処理
 * @brief エディター実行ファイル参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail エディター実行ファイル参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::on_edtorExePathRefButtonClicked_clicked()
{   QString selFilter = tr("実行ファイル(*.exe)");
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("エディター.exeを選択"),
        *( pprof->other.pwszEdtorPath ),
        tr("すべて(*.*);;実行ファイル(*.exe)"),
        &selFilter,
        QFileDialog::DontUseCustomDirectoryIcons );
    if ( !fileName.isEmpty())
    {   ui->mecabPathEdit->setText( fileName );
        if ( !pprof->other.pwszEdtorPath->isEmpty())
        {   delete pprof->other.pwszEdtorPath;
        }
        pprof->other.pwszEdtorPath = new QString( fileName );
        ui->edtorExePathEdit->setText( *( pprof->other.pwszEdtorPath ));
    }
}


/**
 * @fn
 * エディター実行ファイルエディットボックス編集終了時の処理
 * @brief エディター実行ファイルエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail エディター実行ファイルエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::on_edtorExePathEdit_editingFinished()
{   if ( !pprof->other.pwszEdtorPath->isEmpty())
    {   delete pprof->other.pwszEdtorPath;
    }
    pprof->other.pwszEdtorPath = new QString( ui->edtorExePathEdit->text());
}


/**
 * @fn
 * エディタータグジャンプコマンドエディットボックス編集終了時の処理
 * @brief エディタータグジャンプコマンドエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail エディタータグジャンプコマンドエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::on_edtorTagJumpCmdEdit_editingFinished()
{   if ( !pprof->other.pwszEdtorTagjumpCommand->isEmpty())
    {   delete pprof->other.pwszEdtorTagjumpCommand;
    }
    pprof->other.pwszEdtorTagjumpCommand = new QString( ui->edtorTagJumpCmdEdit->text());
}


/**
 * @fn
 * アクエストーク音声選択コンボボックス、選択変更時の処理
 * @brief アクエストーク音声選択コンボボックス、選択変更時の処理
 * @param (index) 選択インデックス
 * @return 無し
 * @sa
 * @detail アクエストーク音声選択コンボボックス、選択変更時の処理
 */
void SettingDlg::on_aqtkVoiceSel_currentIndexChanged(int index)
{    static const AQTK_VOICE *pBaseVoice[] =
        {	&gVoice_F1, &gVoice_F2, &gVoice_F3,
            &gVoice_M1, &gVoice_M2,
            &gVoice_R1, &gVoice_R2
        };
    char edBuf[16];

    if (( index <= 0 ) || ( ui->aqtkVoiceSel->count() == 0 ))
    {   return;
    }
    pprof->aqtk.iVoiceIdx = index - 1;
    ui->aqtkSpeedDial->setValue( pprof->aqtk.iSpeed = pBaseVoice[index]->spd );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iSpeed );
    ui->AqtkSpeedValue->setText( edBuf );

    ui->aqtkStepDial->setValue( pprof->aqtk.iStep = pBaseVoice[index]->lmd );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iStep );
    ui->aqtkStepValue->setText( edBuf );

    ui->aqtkHighDial->setValue( pprof->aqtk.iHigh = pBaseVoice[index]->pit );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iHigh );
    ui->aqtkHighValue->setText( edBuf );

    ui->aqtkAccentDial->setValue( pprof->aqtk.iAccent = pBaseVoice[index]->acc );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iAccent );
    ui->aqtkAccentValue->setText( edBuf );

    ui->aqtkStep2Dial->setValue( pprof->aqtk.iStep2 = pBaseVoice[index]->fsc );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iStep2 );
    ui->aqtkStep2Value->setText( edBuf );

    ui->aqtkVolumeDial->setValue( pprof->aqtk.iVolume = pBaseVoice[index]->vol );
    sprintf_s( edBuf, "%4d", pprof->aqtk.iStep2 );
    ui->aqtkVolumeValue->setText( edBuf );
}


/**
 * @fn
 * アクエストークスピードダイヤル、変更時の処理
 * @brief アクエストークスピードダイヤル、変更時の処理
 * @param (value) 選択値
 * @return 無し
 * @sa
 * @detail アクエストークスピードダイヤル、変更時の処理
 */
void SettingDlg::on_aqtkSpeedDial_valueChanged(int value)
{   char edBuf[16];

    pprof->aqtk.iSpeed = value;
    sprintf_s( edBuf, "%4d", value );
    ui->AqtkSpeedValue->setText( edBuf );
//    QMessageBox::warning(this, tr("MessageBox"), tr("on_aqtkSpeedDial_valueChanged"));
}


/**
 * @fn
 * アクエストークステップダイヤル、変更時の処理
 * @brief アクエストークステップダイヤル、変更時の処理
 * @param (value) 選択値
 * @return 無し
 * @sa
 * @detail アクエストークステップダイヤル、変更時の処理
 */
void SettingDlg::on_aqtkStepDial_valueChanged(int value)
{   char edBuf[16];

    pprof->aqtk.iStep = value;
    sprintf_s( edBuf, "%4d", value );
    ui->aqtkStepValue->setText( edBuf );
}


/**
 * @fn
 * アクエストークハイダイヤル、変更時の処理
 * @brief アクエストークハイダイヤル、変更時の処理
 * @param (value) 選択値
 * @return 無し
 * @sa
 * @detail アクエストークハイダイヤル、変更時の処理
 */
void SettingDlg::on_aqtkHighDial_valueChanged(int value)
{   char edBuf[16];

    pprof->aqtk.iHigh = value;
    sprintf_s( edBuf, "%4d", value );
    ui->aqtkHighValue->setText( edBuf );
}


/**
 * @fn
 * アクエストークアクセントダイヤル、変更時の処理
 * @brief アクエストークアクセントダイヤル、変更時の処理
 * @param (value) 選択値
 * @return 無し
 * @sa
 * @detail アクエストークアクセントダイヤル、変更時の処理
 */
void SettingDlg::on_aqtkAccentDial_valueChanged(int value)
{   char edBuf[16];

    pprof->aqtk.iAccent = value;
    sprintf_s( edBuf, "%4d", value );
    ui->aqtkAccentValue->setText( edBuf );
}


/**
 * @fn
 * アクエストークステップ２ダイヤル、変更時の処理
 * @brief アクエストークステップ２ダイヤル、変更時の処理
 * @param (value) 選択値
 * @return 無し
 * @sa
 * @detail アクエストークステップ２ダイヤル、変更時の処理
 */
void SettingDlg::on_aqtkStep2Dial_valueChanged(int value)
{   char edBuf[16];

    pprof->aqtk.iStep2 = value;
    sprintf_s( edBuf, "%4d", value );
    ui->aqtkStep2Value->setText( edBuf );
}


/**
 * @fn
 * アクエストークボリュームダイヤル、変更時の処理
 * @brief アクエストークボリュームダイヤル、変更時の処理
 * @param (value) 選択値
 * @return 無し
 * @sa
 * @detail アクエストークボリュームダイヤル、変更時の処理
 */
void SettingDlg::on_aqtkVolumeDial_valueChanged(int value)
{   char edBuf[16];

    pprof->aqtk.iVolume = value;
    sprintf_s( edBuf, "%4d", value );
    ui->aqtkVolumeValue->setText( edBuf );
}


//-----------------------------------------------------------------< sapi


/**
 * @fn
 * sapiレートダイヤル、変更時の処理
 * @brief sapiレートダイヤル、変更時の処理
 * @param (value) 選択値
 * @return 無し
 * @sa
 * @detail sapiレートダイヤル、変更時の処理
 */
void SettingDlg::on_sapiRateDial_valueChanged(int value)
{   char edBuf[16];

    pprof->sapi.iRate = value;
    sprintf_s( edBuf, "Rate:%3d", value );
    ui->sapiRateLabel->setText( edBuf );
}


/**
 * @fn
 * sapiボリュームダイヤル、変更時の処理
 * @brief sapiボリュームダイヤル、変更時の処理
 * @param (value) 選択値
 * @return 無し
 * @sa
 * @detail sapiボリュームダイヤル、変更時の処理
 */
void SettingDlg::on_sapiVolumeDial_valueChanged(int value)
{   char edBuf[16];

    pprof->sapi.iVolume = value;
    sprintf_s( edBuf, "Volume:%3d", value );
    ui->sapiVolumeLabel->setText( edBuf );
}


/**
 * @fn
 * sapi音声選択コンボボックス、選択変更時の処理
 * @brief sapi音声選択コンボボックス、選択変更時の処理
 * @param (index) 選択インデックス
 * @return 無し
 * @sa
 * @detail sapi音声選択コンボボックス、選択変更時の処理
 */
void SettingDlg::on_sapiVoiceSelCmbbox_currentIndexChanged(int index)
{
    if (( index <= 0 ) || ( ui->sapiVoiceSelCmbbox->count() == 0 ))
    {   return;
    }
   SetVoiceName();
}


/**
 * @fn
 * sapi無音残時間エディットボックス編集終了時の処理
 * @brief sapi無音残時間エディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail sapi無音残時間エディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::on_sapiMuonZan_editingFinished()
{   pprof->sapi.iMuonZan = ui->sapiMuonZan->text().toInt();
}


/**
 * @fn
 * sapi　VoiceVox4sapiPathエディットボックス編集終了時の処理
 * @brief sapi　VoiceVox4sapiPathエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail sapi　VoiceVox4sapiPathエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::on_sapiVoiceVox4sapiPathEdit_editingFinished()
{   QString fileName = ui->sapiVoiceVox4sapiPathEdit->text();
    if ( !pprof->sapi.pwszVoicevox4sapiRunPath->isEmpty())
    {   delete pprof->sapi.pwszVoicevox4sapiRunPath;
    }
    pprof->sapi.pwszVoicevox4sapiRunPath = new QString( fileName );
}


/**
 * @fn
 * voicevox run.exeファイル参照ボタンクリック時の処理
 * @brief voicevox run.exeファイル参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail voicevox run.exeファイル参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::on_sapiVoicevox4sapiRefButton_clicked()
{   QString selFilter = tr("実行ファイル(*.exe)");
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("run.exeを選択"),
        *( pprof->sapi.pwszVoicevox4sapiRunPath ),
        tr("すべて(*.*);;実行ファイル(*.exe);;VoiceVoxファイル(run.exe)"),
        &selFilter,
        QFileDialog::DontUseCustomDirectoryIcons );
    if ( !fileName.isEmpty())
    {   ui->sapiVoiceVox4sapiPathEdit->setText( fileName );
        if ( !pprof->sapi.pwszVoicevox4sapiRunPath->isEmpty())
        {   delete pprof->sapi.pwszVoicevox4sapiRunPath;
        }
        pprof->sapi.pwszVoicevox4sapiRunPath = new QString( fileName );
    }
}


/**
 * @fn
 * sapi出力モード選択コンボボックス、選択変更時の処理
 * @brief sapi出力モード選択コンボボックス、選択変更時の処理
 * @param (index) 選択インデックス
 * @return 無し
 * @sa
 * @detail sapi出力モード選択コンボボックス、選択変更時の処理
 */
void SettingDlg::on_sapiOutputModeSelCmbbox_currentIndexChanged(int index)
{   if (( index <= 0 ) || ( ui->sapiOutputModeSelCmbbox->count() == 0 ))
    {   return;
    }
    pprof->sapi.iOutputMode = index - 1;
}


/**
 * @fn
 * ramディスク参照ボタンクリック時の処理
 * @brief ramディスク参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail ramディスク参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::RamDiskDirRefClicked()
{   QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    fileDialog.setLabelText(QFileDialog::Accept,tr("Open"));
    fileDialog.setLabelText(QFileDialog::LookIn,tr("Open"));
    fileDialog.setWindowTitle("ラムディスクのフォルダーを選択してください！");
    fileDialog.setDirectory( *( pprof->other.pwszRamDiskPath ));
    if (fileDialog.exec())
    {   QString filePaths = fileDialog.selectedFiles().at(0);
        ui->RAMDISKDIR->setText( filePaths );
        if ( !pprof->other.pwszRamDiskPath->isEmpty())
        {   delete pprof->other.pwszRamDiskPath;
        }
        pprof->other.pwszRamDiskPath = new QString( filePaths );
        ui->RAMDISKDIR->setText( *( pprof->other.pwszRamDiskPath ));
        if ( pprof->other.pwszRamDiskPath != NULL )
        {   pprof->replaceEnv( "TMP", *( pprof->other.pwszRamDiskPath ));
        }
    }
}


/**
 * @fn
 * ramディスクエディットボックス編集終了時の処理
 * @brief ramディスクエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail ramディスクエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::RamDiskDirEditingFinished()
{   if ( !pprof->other.pwszRamDiskPath->isEmpty())
    {   delete pprof->other.pwszRamDiskPath;
    }
    pprof->other.pwszRamDiskPath = new QString( ui->RAMDISKDIR->text());
    if ( pprof->other.pwszRamDiskPath != NULL )
    {   pprof->replaceEnv( "TMP", *( pprof->other.pwszRamDiskPath ));
    }
}


/**
 * @fn
 * openJtakl辞書フォルダーエディットボックス編集終了時の処理
 * @brief openJtakl辞書フォルダーエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail openJtakl辞書フォルダーエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::OpenJTalkDicPathEditFinished()
{   QString dicName = ui->OpenJTalkDicPathEdit->text();
    if ( !pprof->vvox.pwszOpenJTalkDicPath->isEmpty())
    {   delete pprof->vvox.pwszOpenJTalkDicPath;
    }
    pprof->vvox.pwszOpenJTalkDicPath = new QString( dicName );
}


/**
 * @fn
 * openJtakl辞書フォルダー参照ボタンクリック時の処理
 * @brief openJtakl辞書フォルダー参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail openJtakl辞書フォルダー参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::OpenJTalkDicPathRefClicked()
{   QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    fileDialog.setLabelText(QFileDialog::Accept,tr("Open"));
    fileDialog.setLabelText(QFileDialog::LookIn,tr("Open"));
    fileDialog.setWindowTitle("OpenJTalkの辞書フォルダーを選択してください！");
    fileDialog.setDirectory( *( pprof->vvox.pwszOpenJTalkDicPath ));
    if (fileDialog.exec())
    {   QString filePaths = fileDialog.selectedFiles().at(0);
        ui->OpenJTalkDicPathEdit->setText( filePaths );
        if ( !pprof->vvox.pwszOpenJTalkDicPath->isEmpty())
        {   delete pprof->vvox.pwszOpenJTalkDicPath;
        }
        pprof->vvox.pwszOpenJTalkDicPath = new QString( filePaths );
        ui->OpenJTalkDicPathEdit->setText( *( pprof->vvox.pwszOpenJTalkDicPath ));
    }
}


/**
 * @fn
 * openJtakl辞書ビルドフォルダーエディットボックス編集終了時の処理
 * @brief openJtakl辞書ビルドフォルダーエディットボックス編集終了時の処理
 * @return 無し
 * @sa
 * @detail openJtakl辞書ビルドフォルダーエディットボックス編集終了時の処理　プロファイルデータを更新する
 */
void SettingDlg::OpenJTalkDicBuildDirEditFinished()
{   QString dicPath = ui->OpenJTalkDicBuildDirEdit->text();
    if ( !pprof->vvox.pwszOpenJTalkDicBuildDir->isEmpty())
    {   delete pprof->vvox.pwszOpenJTalkDicBuildDir;
    }
    pprof->vvox.pwszOpenJTalkDicBuildDir = new QString( dicPath );
}


/**
 * @fn
 * openJtakl辞書ビルドフォルダー参照ボタンクリック時の処理
 * @brief openJtakl辞書ビルドフォルダー参照ボタンクリック時の処理
 * @return 無し
 * @sa
 * @detail openJtakl辞書ビルドフォルダー参照ボタンクリック時の処理　ファイルダイヤログを表示する
 */
void SettingDlg::OpenJTalkDicBuildDirRefClicked()
{   QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    fileDialog.setLabelText(QFileDialog::Accept,tr("Open"));
    fileDialog.setLabelText(QFileDialog::LookIn,tr("Open"));
    fileDialog.setWindowTitle("OpenJTalk辞書ビルドフォルダーを選択してください！");
    fileDialog.setDirectory( *( pprof->vvox.pwszOpenJTalkDicBuildDir ));
    if (fileDialog.exec())
    {   QString filePaths = fileDialog.selectedFiles().at(0);
        ui->OpenJTalkDicBuildDirEdit->setText( filePaths );
        if ( !pprof->vvox.pwszOpenJTalkDicBuildDir->isEmpty())
        {   delete pprof->vvox.pwszOpenJTalkDicBuildDir;
        }
        pprof->vvox.pwszOpenJTalkDicBuildDir = new QString( filePaths );
        ui->OpenJTalkDicBuildDirEdit->setText( *( pprof->vvox.pwszOpenJTalkDicBuildDir ));
    }
}


#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


/**
 * @fn
 * voicevox-core音声選択リストの生成の処理
 * @brief voicevox-core音声選択リストの生成の処理
 * @param (*p_VoiceSel) コンボボックスポインタ
 * @return 無し
 * @sa
 * @detail voicevox-core音声選択リストの生成の処理　metas.jsonファイルから取得する
 */
void SettingDlg::VoicevoxmodelListRead( QComboBox *p_VoiceSel )
{   QFile file("model\\metas.json");

    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ))
    {   messageBuff msgBuff;    // for debug

        msgBuff.enterMessage( "file[metas.json] open error!\n" );    // for debug
        qDebug() << "file[metas.json] open error!";
        return;
    }
    QTextStream in( &file );                    ///JSONファイルから読み込み
    QJsonDocument jsonDoc = QJsonDocument::fromJson( in.readAll().toUtf8());
    if ( !jsonDoc.isArray())
    {   messageBuff msgBuff;    // for debug

        msgBuff.enterMessage( "file[metas.json] error!\n" );    // for debug
        qDebug() << "file[metas.json] error!";
        return;
    }
    p_VoiceSel->clear();
    QString dummy = u8"未選択 --- 操作時以外の IndexChanged messege 回避用";
    p_VoiceSel->addItem( dummy );
//    qDebug() << "isArray = " << jsonDoc.isArray() << jsonDoc.isObject() << jsonDoc.isNull();
    QJsonArray array = jsonDoc.array();
    int idx2, idx = 1;
    foreach( QJsonValue value, array )
    {   QJsonArray array2 = value["styles"].toArray();
        foreach( QJsonValue value2, array2 )
        {   QString item = value["name"].toString() + "-" + value2["name"].toString() + " ID:" + QString::number( value2["id"].toInt());
            if ( p_VoiceSel->findText( item ) <= -1 )
            {	p_VoiceSel->addItem( item );
                if ( pprof->vvox.ispeaker_id == value2["id"].toInt())
                {   idx2 = idx;
                    p_VoiceSel->setCurrentIndex( idx );
//                    qDebug() << pprof->vvox.ispeaker_id << " == " << value2["id"].toInt() << ":" << idx;
                }
                idx++;
            }
    }   }
//    p_VoiceSel->setCurrentIndex( idx2 );
    file.close();
}


/**
 * @fn
 * voicevox-core音声選択コンボボックス、選択変更時の処理
 * @brief voicevox-core音声選択コンボボックス、選択変更時の処理
 * @param (index) 選択インデックス
 * @return 無し
 * @sa
 * @detail voicevox-core音声選択コンボボックス、選択変更時の処理
 */
void SettingDlg::voicevoxIndexChanged(int index)
{   // 操作で変更時以外(表示時など)にも来るので設定を変更できない
    qDebug() << "voicevoxIndexChanged( " << index << " )";
    if ( index <= 0 )
    {   return;
    }
    if ( ui->voicevoxVoiceSelComboBox->count() == 0 )
    {   return;
    }
    QString selText = ui->voicevoxVoiceSelComboBox->itemText( index );
    int idPos = selText.indexOf( ':' );
    if ( idPos >= 0 )
    {   QString idNum = selText.mid( idPos + 1, 2 );
        pprof->vvox.ispeaker_id = idNum.toInt();
//    qDebug() << pprof->vvox.ispeaker_id << " == ";
    }
}


/**
 * @fn
 * アプリ内記号、読み置換処理のスキップ設定の処理
 * @brief アプリ内記号、読み置換処理のスキップ設定の処理
 * @return 無し
 * @sa
 * @detail アプリ内記号、読み置換処理のスキップ設定の処理
 */
void SettingDlg::onClickedsymbolSkip()
{   pprof->oper.symbolSpeakingOff = ui->symbleTalkSkip->checkState();
}


/**
 * @fn
 * モニター休止連動ポーズの設定の処理
 * @brief モニター休止連動ポーズの設定の処理
 * @return 無し
 * @sa
 * @detail モニター休止連動ポーズの設定の処理
 */
void SettingDlg::onClickedMonitorSleepPause()
{   pprof->oper.displayOutputOffThenPouse = ui->monitorSleepPouse->checkState();
}


/**
 * @fn
 * インフォメーションウインドウの行数制限値の最小値補正処理
 * @brief インフォメーションウインドウの行数制限値の最小値補正処理
 * @return 無し
 * @sa
 * @detail インフォメーションウインドウの行数制限値の最小値補正処理　５００未満なら５００にする
 */
void SettingDlg::infoWndMaxLineLimmitTextChangeFinished()
{   int linemax = ui->infoWndlineLimitEdit->text().toInt();

//    linemax = ( linemax < 500 )? 500: linemax;      // 最小行数補正 0:無制限があるので補正しない
    pprof->other.infoWndLineMaxLimmit = linemax;
    emit sigupdateMaxLine();                            // ここでMainWindowにシグナル
}

