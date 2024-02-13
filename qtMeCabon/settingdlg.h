/**
 * @file settingdlg.h
 * @brief 初期設定ダイヤログの処理のヘッダ
 * @author m.kawakami
 * @date 23/10/??
 */

#ifndef SETTINGDLG_H
#define SETTINGDLG_H

#include <QDialog>
#include <QComboBox>
//#include "..\..\..\qtMeCabon\extdial.h"

namespace Ui {
class SettingDlg;
}

class SettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDlg(QWidget *parent = nullptr);
    ~SettingDlg();
    void show();
    void VoicevoxmodelListRead( QComboBox *p_VoiceSel );

private:
    int makeVoiceListAdd( QComboBox *p_VoiceSel, QString *pszEntry );
    int makeVoiceList( QComboBox *p_VoiceSel );

signals:
    void sigupdateMaxLine();

private slots:
    void on_texyDisplayBlackBakMode_stateChanged(int arg1);
    void on_mecabPathRefButton_clicked();
    void on_systemDictionaryFolderRefButton_clicked();
    void on_userDictionaryPathRefButton_clicked();
    void on_userCsvDictionaryFolderRefButton_clicked();
    void on_systemDictionaryBuildFolderRefButton_clicked();
    void on_ttsSelectorCombo_currentIndexChanged(int index);
    void on_OkButton_clicked();
    void on_mecabPathEdit_editingFinished();
    void on_systemDictionaryFolderEdit_editingFinished();
    void on_userDictionaryPathEdit_editingFinished();
    void on_userCsvDictionaryFolderEdit_editingFinished();
    void on_systemDictionaryBuildFolderEdit_editingFinished();
    void on_settingTab_currentChanged(int index);
    void on_edtorExePathRefButtonClicked_clicked();
    void on_edtorExePathEdit_editingFinished();
    void on_edtorTagJumpCmdEdit_editingFinished();
    void on_aqtkVoiceSel_currentIndexChanged(int index);
    void on_aqtkSpeedDial_valueChanged(int value);
    void on_aqtkStepDial_valueChanged(int value);
    void on_aqtkHighDial_valueChanged(int value);
    void on_aqtkAccentDial_valueChanged(int value);
    void on_aqtkStep2Dial_valueChanged(int value);
    void on_aqtkVolumeDial_valueChanged(int value);
    void on_sapiRateDial_valueChanged(int value);
    void on_sapiVolumeDial_valueChanged(int value);
    void on_sapiVoiceSelCmbbox_currentIndexChanged(int index);
    void on_sapiMuonZan_editingFinished();
    void on_sapiVoiceVox4sapiPathEdit_editingFinished();
    void on_sapiVoicevox4sapiRefButton_clicked();
    void on_sapiOutputModeSelCmbbox_currentIndexChanged(int index);
    void RamDiskDirRefClicked();
    void RamDiskDirEditingFinished();
    void OpenJTalkDicPathEditFinished();
    void OpenJTalkDicPathRefClicked();
    void OpenJTalkDicBuildDirEditFinished();
    void OpenJTalkDicBuildDirRefClicked();
    void voicevoxIndexChanged(int index);
    void onClickedsymbolSkip();
    void onClickedMonitorSleepPause();
    void infoWndMaxLineLimmitTextChangeFinished();

    //QObject::connect(OpenJTalkDicBuildDirRefButton, SIGNAL(clicked()), SettingDlg, SLOT(OpenJTalkDicBuildDirRefClicked()));
    //QObject::connect(OpenJTalkDicPathRefButton, SIGNAL(clicked()), SettingDlg, SLOT(OpenJTalkDicPathRefClicked()));
    //QObject::connect(OpenJTalkDicPathEdit, SIGNAL(editingFinished()), SettingDlg, SLOT(OpenJTalkDicPathEditFinished()));
    //QObject::connect(OpenJTalkDicBuildDirEdit, SIGNAL(editingFinished()), SettingDlg, SLOT(OpenJTalkDicBuildDirEditFinished()));


public slots:

private:
    Ui::SettingDlg *ui;
    void SetVoiceName();
};

#endif // SETTINGDLG_H
