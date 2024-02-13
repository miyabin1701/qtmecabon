/********************************************************************************
** Form generated from reading UI file 'SettingDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGDIALOG_H
#define UI_SETTINGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QTabWidget *tabWidget;
    QWidget *MeCAb;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *EditMeCabPath;
    QPushButton *refMeCabPath;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *EditSysDicPath;
    QPushButton *refSysDicPath;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *EditMeCabUserDicPath;
    QPushButton *refMeCabUserDicPath;
    QWidget *horizontalLayoutWidget_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *EditMeCabUserCsvDicPath;
    QPushButton *refMeCabUserCsvDicPath;
    QWidget *horizontalLayoutWidget_5;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QLineEdit *EditSysDicBildFolder;
    QPushButton *refSysDicBildFolder;
    QWidget *horizontalLayoutWidget_7;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_7;
    QComboBox *comboBox;
    QLabel *label_6;
    QComboBox *comboBox_2;
    QWidget *SAPI;
    QWidget *horizontalLayoutWidget_6;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_11;
    QDial *dialrate;
    QSpacerItem *horizontalSpacer_5;
    QLabel *label_10;
    QDial *dialVolume;
    QSpacerItem *horizontalSpacer_4;
    QWidget *horizontalLayoutWidget_12;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_12;
    QComboBox *comboBoxVoiceSel;
    QWidget *horizontalLayoutWidget_13;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_13;
    QLineEdit *EditLestMuteTime;
    QLabel *label_14;
    QWidget *horizontalLayoutWidget_14;
    QHBoxLayout *horizontalLayout_14;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_16;
    QLineEdit *EditVoicevox_runPath;
    QPushButton *refVoicevox_runPathButton;
    QWidget *horizontalLayoutWidget_16;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_18;
    QComboBox *comboBoxIfMode;
    QLabel *label_15;
    QLabel *label_17;
    QWidget *AQTK10;
    QWidget *horizontalLayoutWidget_11;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_19;
    QComboBox *comboBox_voiceSel;
    QWidget *horizontalLayoutWidget_20;
    QHBoxLayout *horizontalLayout_20;
    QLabel *label_26;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_21;
    QDial *dial_Aqtk10Ontei;
    QLabel *label_20;
    QDial *dial_Aqtk10Speed;
    QLabel *label_22;
    QDial *dial_Aqtk10High;
    QLabel *label_23;
    QDial *dial_Aqtk10Accent;
    QLabel *label_24;
    QDial *dial_Aqtk10Ontei2;
    QLabel *label_25;
    QDial *dial_Aqtk10Volume;
    QWidget *JTALK;
    QWidget *other;
    QWidget *horizontalLayoutWidget_21;
    QHBoxLayout *horizontalLayout_21;
    QSpacerItem *horizontalSpacer_14;
    QCheckBox *checkBoxEditWndColor;
    QSpacerItem *horizontalSpacer_15;
    QWidget *horizontalLayoutWidget_15;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_28;
    QLineEdit *EditTextEditorPath;
    QPushButton *refTextEditorPath;
    QWidget *horizontalLayoutWidget_17;
    QHBoxLayout *horizontalLayout_18;
    QLabel *label_29;
    QLineEdit *EditEdtorOption;
    QWidget *horizontalLayoutWidget_10;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_27;
    QComboBox *ttsSelector;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(554, 494);
        tabWidget = new QTabWidget(Dialog);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(10, 10, 541, 421));
        MeCAb = new QWidget();
        MeCAb->setObjectName("MeCAb");
        horizontalLayoutWidget = new QWidget(MeCAb);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(0, 10, 531, 41));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(horizontalLayoutWidget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        EditMeCabPath = new QLineEdit(horizontalLayoutWidget);
        EditMeCabPath->setObjectName("EditMeCabPath");

        horizontalLayout->addWidget(EditMeCabPath);

        refMeCabPath = new QPushButton(horizontalLayoutWidget);
        refMeCabPath->setObjectName("refMeCabPath");

        horizontalLayout->addWidget(refMeCabPath);

        horizontalLayoutWidget_2 = new QWidget(MeCAb);
        horizontalLayoutWidget_2->setObjectName("horizontalLayoutWidget_2");
        horizontalLayoutWidget_2->setGeometry(QRect(0, 50, 531, 41));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(horizontalLayoutWidget_2);
        label_2->setObjectName("label_2");

        horizontalLayout_2->addWidget(label_2);

        EditSysDicPath = new QLineEdit(horizontalLayoutWidget_2);
        EditSysDicPath->setObjectName("EditSysDicPath");

        horizontalLayout_2->addWidget(EditSysDicPath);

        refSysDicPath = new QPushButton(horizontalLayoutWidget_2);
        refSysDicPath->setObjectName("refSysDicPath");

        horizontalLayout_2->addWidget(refSysDicPath);

        horizontalLayoutWidget_3 = new QWidget(MeCAb);
        horizontalLayoutWidget_3->setObjectName("horizontalLayoutWidget_3");
        horizontalLayoutWidget_3->setGeometry(QRect(0, 90, 531, 41));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(horizontalLayoutWidget_3);
        label_3->setObjectName("label_3");
        label_3->setEnabled(false);

        horizontalLayout_3->addWidget(label_3);

        EditMeCabUserDicPath = new QLineEdit(horizontalLayoutWidget_3);
        EditMeCabUserDicPath->setObjectName("EditMeCabUserDicPath");
        EditMeCabUserDicPath->setEnabled(false);

        horizontalLayout_3->addWidget(EditMeCabUserDicPath);

        refMeCabUserDicPath = new QPushButton(horizontalLayoutWidget_3);
        refMeCabUserDicPath->setObjectName("refMeCabUserDicPath");
        refMeCabUserDicPath->setEnabled(false);

        horizontalLayout_3->addWidget(refMeCabUserDicPath);

        horizontalLayoutWidget_4 = new QWidget(MeCAb);
        horizontalLayoutWidget_4->setObjectName("horizontalLayoutWidget_4");
        horizontalLayoutWidget_4->setGeometry(QRect(0, 130, 531, 41));
        horizontalLayout_4 = new QHBoxLayout(horizontalLayoutWidget_4);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(horizontalLayoutWidget_4);
        label_4->setObjectName("label_4");
        label_4->setEnabled(false);

        horizontalLayout_4->addWidget(label_4);

        EditMeCabUserCsvDicPath = new QLineEdit(horizontalLayoutWidget_4);
        EditMeCabUserCsvDicPath->setObjectName("EditMeCabUserCsvDicPath");
        EditMeCabUserCsvDicPath->setEnabled(false);

        horizontalLayout_4->addWidget(EditMeCabUserCsvDicPath);

        refMeCabUserCsvDicPath = new QPushButton(horizontalLayoutWidget_4);
        refMeCabUserCsvDicPath->setObjectName("refMeCabUserCsvDicPath");
        refMeCabUserCsvDicPath->setEnabled(false);

        horizontalLayout_4->addWidget(refMeCabUserCsvDicPath);

        horizontalLayoutWidget_5 = new QWidget(MeCAb);
        horizontalLayoutWidget_5->setObjectName("horizontalLayoutWidget_5");
        horizontalLayoutWidget_5->setGeometry(QRect(0, 170, 531, 41));
        horizontalLayout_5 = new QHBoxLayout(horizontalLayoutWidget_5);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(horizontalLayoutWidget_5);
        label_5->setObjectName("label_5");

        horizontalLayout_5->addWidget(label_5);

        EditSysDicBildFolder = new QLineEdit(horizontalLayoutWidget_5);
        EditSysDicBildFolder->setObjectName("EditSysDicBildFolder");

        horizontalLayout_5->addWidget(EditSysDicBildFolder);

        refSysDicBildFolder = new QPushButton(horizontalLayoutWidget_5);
        refSysDicBildFolder->setObjectName("refSysDicBildFolder");

        horizontalLayout_5->addWidget(refSysDicBildFolder);

        horizontalLayoutWidget_7 = new QWidget(MeCAb);
        horizontalLayoutWidget_7->setObjectName("horizontalLayoutWidget_7");
        horizontalLayoutWidget_7->setGeometry(QRect(0, 210, 531, 41));
        horizontalLayout_7 = new QHBoxLayout(horizontalLayoutWidget_7);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(horizontalLayoutWidget_7);
        label_7->setObjectName("label_7");
        label_7->setEnabled(false);

        horizontalLayout_7->addWidget(label_7);

        comboBox = new QComboBox(horizontalLayoutWidget_7);
        comboBox->setObjectName("comboBox");
        comboBox->setEnabled(false);

        horizontalLayout_7->addWidget(comboBox);

        label_6 = new QLabel(horizontalLayoutWidget_7);
        label_6->setObjectName("label_6");
        label_6->setEnabled(false);

        horizontalLayout_7->addWidget(label_6);

        comboBox_2 = new QComboBox(horizontalLayoutWidget_7);
        comboBox_2->setObjectName("comboBox_2");
        comboBox_2->setEnabled(false);

        horizontalLayout_7->addWidget(comboBox_2);

        tabWidget->addTab(MeCAb, QString());
        SAPI = new QWidget();
        SAPI->setObjectName("SAPI");
        horizontalLayoutWidget_6 = new QWidget(SAPI);
        horizontalLayoutWidget_6->setObjectName("horizontalLayoutWidget_6");
        horizontalLayoutWidget_6->setGeometry(QRect(0, 20, 531, 80));
        horizontalLayout_6 = new QHBoxLayout(horizontalLayoutWidget_6);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);

        label_11 = new QLabel(horizontalLayoutWidget_6);
        label_11->setObjectName("label_11");

        horizontalLayout_6->addWidget(label_11);

        dialrate = new QDial(horizontalLayoutWidget_6);
        dialrate->setObjectName("dialrate");

        horizontalLayout_6->addWidget(dialrate);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);

        label_10 = new QLabel(horizontalLayoutWidget_6);
        label_10->setObjectName("label_10");

        horizontalLayout_6->addWidget(label_10);

        dialVolume = new QDial(horizontalLayoutWidget_6);
        dialVolume->setObjectName("dialVolume");

        horizontalLayout_6->addWidget(dialVolume);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        horizontalLayoutWidget_12 = new QWidget(SAPI);
        horizontalLayoutWidget_12->setObjectName("horizontalLayoutWidget_12");
        horizontalLayoutWidget_12->setGeometry(QRect(0, 100, 531, 51));
        horizontalLayout_12 = new QHBoxLayout(horizontalLayoutWidget_12);
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        horizontalLayout_12->setContentsMargins(0, 0, 0, 0);
        label_12 = new QLabel(horizontalLayoutWidget_12);
        label_12->setObjectName("label_12");

        horizontalLayout_12->addWidget(label_12);

        comboBoxVoiceSel = new QComboBox(horizontalLayoutWidget_12);
        comboBoxVoiceSel->setObjectName("comboBoxVoiceSel");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboBoxVoiceSel->sizePolicy().hasHeightForWidth());
        comboBoxVoiceSel->setSizePolicy(sizePolicy);

        horizontalLayout_12->addWidget(comboBoxVoiceSel);

        horizontalLayoutWidget_13 = new QWidget(SAPI);
        horizontalLayoutWidget_13->setObjectName("horizontalLayoutWidget_13");
        horizontalLayoutWidget_13->setGeometry(QRect(0, 150, 531, 54));
        horizontalLayout_13 = new QHBoxLayout(horizontalLayoutWidget_13);
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        horizontalLayout_13->setContentsMargins(0, 0, 0, 0);
        label_13 = new QLabel(horizontalLayoutWidget_13);
        label_13->setObjectName("label_13");

        horizontalLayout_13->addWidget(label_13);

        EditLestMuteTime = new QLineEdit(horizontalLayoutWidget_13);
        EditLestMuteTime->setObjectName("EditLestMuteTime");

        horizontalLayout_13->addWidget(EditLestMuteTime);

        label_14 = new QLabel(horizontalLayoutWidget_13);
        label_14->setObjectName("label_14");
        QFont font;
        font.setPointSize(10);
        label_14->setFont(font);

        horizontalLayout_13->addWidget(label_14);

        horizontalLayoutWidget_14 = new QWidget(SAPI);
        horizontalLayoutWidget_14->setObjectName("horizontalLayoutWidget_14");
        horizontalLayoutWidget_14->setGeometry(QRect(0, 240, 531, 56));
        horizontalLayout_14 = new QHBoxLayout(horizontalLayoutWidget_14);
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        horizontalLayout_14->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName("horizontalLayout_15");
        label_16 = new QLabel(horizontalLayoutWidget_14);
        label_16->setObjectName("label_16");
        label_16->setFont(font);

        horizontalLayout_15->addWidget(label_16);

        EditVoicevox_runPath = new QLineEdit(horizontalLayoutWidget_14);
        EditVoicevox_runPath->setObjectName("EditVoicevox_runPath");

        horizontalLayout_15->addWidget(EditVoicevox_runPath);

        refVoicevox_runPathButton = new QPushButton(horizontalLayoutWidget_14);
        refVoicevox_runPathButton->setObjectName("refVoicevox_runPathButton");

        horizontalLayout_15->addWidget(refVoicevox_runPathButton);


        horizontalLayout_14->addLayout(horizontalLayout_15);

        horizontalLayoutWidget_16 = new QWidget(SAPI);
        horizontalLayoutWidget_16->setObjectName("horizontalLayoutWidget_16");
        horizontalLayoutWidget_16->setGeometry(QRect(0, 320, 531, 51));
        horizontalLayout_16 = new QHBoxLayout(horizontalLayoutWidget_16);
        horizontalLayout_16->setObjectName("horizontalLayout_16");
        horizontalLayout_16->setContentsMargins(0, 0, 0, 0);
        label_18 = new QLabel(horizontalLayoutWidget_16);
        label_18->setObjectName("label_18");

        horizontalLayout_16->addWidget(label_18);

        comboBoxIfMode = new QComboBox(horizontalLayoutWidget_16);
        comboBoxIfMode->setObjectName("comboBoxIfMode");
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboBoxIfMode->sizePolicy().hasHeightForWidth());
        comboBoxIfMode->setSizePolicy(sizePolicy1);

        horizontalLayout_16->addWidget(comboBoxIfMode);

        label_15 = new QLabel(SAPI);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(0, 200, 531, 29));
        label_15->setAlignment(Qt::AlignCenter);
        label_17 = new QLabel(SAPI);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(0, 300, 531, 21));
        label_17->setFont(font);
        label_17->setAlignment(Qt::AlignCenter);
        tabWidget->addTab(SAPI, QString());
        AQTK10 = new QWidget();
        AQTK10->setObjectName("AQTK10");
        horizontalLayoutWidget_11 = new QWidget(AQTK10);
        horizontalLayoutWidget_11->setObjectName("horizontalLayoutWidget_11");
        horizontalLayoutWidget_11->setGeometry(QRect(0, 10, 531, 51));
        horizontalLayout_11 = new QHBoxLayout(horizontalLayoutWidget_11);
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        horizontalLayout_11->setContentsMargins(0, 0, 0, 0);
        label_19 = new QLabel(horizontalLayoutWidget_11);
        label_19->setObjectName("label_19");

        horizontalLayout_11->addWidget(label_19);

        comboBox_voiceSel = new QComboBox(horizontalLayoutWidget_11);
        comboBox_voiceSel->setObjectName("comboBox_voiceSel");
        sizePolicy1.setHeightForWidth(comboBox_voiceSel->sizePolicy().hasHeightForWidth());
        comboBox_voiceSel->setSizePolicy(sizePolicy1);

        horizontalLayout_11->addWidget(comboBox_voiceSel);

        horizontalLayoutWidget_20 = new QWidget(AQTK10);
        horizontalLayoutWidget_20->setObjectName("horizontalLayoutWidget_20");
        horizontalLayoutWidget_20->setGeometry(QRect(0, 60, 531, 51));
        horizontalLayout_20 = new QHBoxLayout(horizontalLayoutWidget_20);
        horizontalLayout_20->setObjectName("horizontalLayout_20");
        horizontalLayout_20->setContentsMargins(0, 0, 0, 0);
        label_26 = new QLabel(horizontalLayoutWidget_20);
        label_26->setObjectName("label_26");
        label_26->setAlignment(Qt::AlignCenter);

        horizontalLayout_20->addWidget(label_26);

        gridLayoutWidget = new QWidget(AQTK10);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(0, 110, 531, 271));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_21 = new QLabel(gridLayoutWidget);
        label_21->setObjectName("label_21");
        label_21->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_21, 0, 2, 1, 1);

        dial_Aqtk10Ontei = new QDial(gridLayoutWidget);
        dial_Aqtk10Ontei->setObjectName("dial_Aqtk10Ontei");

        gridLayout->addWidget(dial_Aqtk10Ontei, 0, 1, 1, 1);

        label_20 = new QLabel(gridLayoutWidget);
        label_20->setObjectName("label_20");
        label_20->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_20, 0, 0, 1, 1);

        dial_Aqtk10Speed = new QDial(gridLayoutWidget);
        dial_Aqtk10Speed->setObjectName("dial_Aqtk10Speed");

        gridLayout->addWidget(dial_Aqtk10Speed, 0, 3, 1, 1);

        label_22 = new QLabel(gridLayoutWidget);
        label_22->setObjectName("label_22");
        label_22->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_22, 1, 0, 1, 1);

        dial_Aqtk10High = new QDial(gridLayoutWidget);
        dial_Aqtk10High->setObjectName("dial_Aqtk10High");

        gridLayout->addWidget(dial_Aqtk10High, 1, 1, 1, 1);

        label_23 = new QLabel(gridLayoutWidget);
        label_23->setObjectName("label_23");
        label_23->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_23, 1, 2, 1, 1);

        dial_Aqtk10Accent = new QDial(gridLayoutWidget);
        dial_Aqtk10Accent->setObjectName("dial_Aqtk10Accent");

        gridLayout->addWidget(dial_Aqtk10Accent, 1, 3, 1, 1);

        label_24 = new QLabel(gridLayoutWidget);
        label_24->setObjectName("label_24");
        label_24->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_24, 2, 0, 1, 1);

        dial_Aqtk10Ontei2 = new QDial(gridLayoutWidget);
        dial_Aqtk10Ontei2->setObjectName("dial_Aqtk10Ontei2");

        gridLayout->addWidget(dial_Aqtk10Ontei2, 2, 1, 1, 1);

        label_25 = new QLabel(gridLayoutWidget);
        label_25->setObjectName("label_25");
        label_25->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_25, 2, 2, 1, 1);

        dial_Aqtk10Volume = new QDial(gridLayoutWidget);
        dial_Aqtk10Volume->setObjectName("dial_Aqtk10Volume");

        gridLayout->addWidget(dial_Aqtk10Volume, 2, 3, 1, 1);

        tabWidget->addTab(AQTK10, QString());
        JTALK = new QWidget();
        JTALK->setObjectName("JTALK");
        tabWidget->addTab(JTALK, QString());
        other = new QWidget();
        other->setObjectName("other");
        horizontalLayoutWidget_21 = new QWidget(other);
        horizontalLayoutWidget_21->setObjectName("horizontalLayoutWidget_21");
        horizontalLayoutWidget_21->setGeometry(QRect(0, 20, 531, 41));
        horizontalLayout_21 = new QHBoxLayout(horizontalLayoutWidget_21);
        horizontalLayout_21->setObjectName("horizontalLayout_21");
        horizontalLayout_21->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_21->addItem(horizontalSpacer_14);

        checkBoxEditWndColor = new QCheckBox(horizontalLayoutWidget_21);
        checkBoxEditWndColor->setObjectName("checkBoxEditWndColor");

        horizontalLayout_21->addWidget(checkBoxEditWndColor);

        horizontalSpacer_15 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_21->addItem(horizontalSpacer_15);

        horizontalLayoutWidget_15 = new QWidget(other);
        horizontalLayoutWidget_15->setObjectName("horizontalLayoutWidget_15");
        horizontalLayoutWidget_15->setGeometry(QRect(0, 70, 531, 36));
        horizontalLayout_17 = new QHBoxLayout(horizontalLayoutWidget_15);
        horizontalLayout_17->setObjectName("horizontalLayout_17");
        horizontalLayout_17->setContentsMargins(0, 0, 0, 0);
        label_28 = new QLabel(horizontalLayoutWidget_15);
        label_28->setObjectName("label_28");

        horizontalLayout_17->addWidget(label_28);

        EditTextEditorPath = new QLineEdit(horizontalLayoutWidget_15);
        EditTextEditorPath->setObjectName("EditTextEditorPath");

        horizontalLayout_17->addWidget(EditTextEditorPath);

        refTextEditorPath = new QPushButton(horizontalLayoutWidget_15);
        refTextEditorPath->setObjectName("refTextEditorPath");

        horizontalLayout_17->addWidget(refTextEditorPath);

        horizontalLayoutWidget_17 = new QWidget(other);
        horizontalLayoutWidget_17->setObjectName("horizontalLayoutWidget_17");
        horizontalLayoutWidget_17->setGeometry(QRect(0, 120, 531, 80));
        horizontalLayout_18 = new QHBoxLayout(horizontalLayoutWidget_17);
        horizontalLayout_18->setObjectName("horizontalLayout_18");
        horizontalLayout_18->setContentsMargins(0, 0, 0, 0);
        label_29 = new QLabel(horizontalLayoutWidget_17);
        label_29->setObjectName("label_29");

        horizontalLayout_18->addWidget(label_29);

        EditEdtorOption = new QLineEdit(horizontalLayoutWidget_17);
        EditEdtorOption->setObjectName("EditEdtorOption");

        horizontalLayout_18->addWidget(EditEdtorOption);

        tabWidget->addTab(other, QString());
        horizontalLayoutWidget_10 = new QWidget(Dialog);
        horizontalLayoutWidget_10->setObjectName("horizontalLayoutWidget_10");
        horizontalLayoutWidget_10->setGeometry(QRect(10, 440, 531, 41));
        horizontalLayout_10 = new QHBoxLayout(horizontalLayoutWidget_10);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        horizontalLayout_10->setContentsMargins(0, 0, 0, 0);
        label_27 = new QLabel(horizontalLayoutWidget_10);
        label_27->setObjectName("label_27");

        horizontalLayout_10->addWidget(label_27);

        ttsSelector = new QComboBox(horizontalLayoutWidget_10);
        ttsSelector->setObjectName("ttsSelector");

        horizontalLayout_10->addWidget(ttsSelector);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer);

        okButton = new QPushButton(horizontalLayoutWidget_10);
        okButton->setObjectName("okButton");

        horizontalLayout_10->addWidget(okButton);


        retranslateUi(Dialog);
        QObject::connect(okButton, SIGNAL(clicked()), Dialog, SLOT(OkButtonClicked()));
        QObject::connect(ttsSelector, SIGNAL(currentIndexChanged(int)), Dialog, SLOT(ttsSellChanged()));
        QObject::connect(refMeCabPath, SIGNAL(clicked()), Dialog, SLOT(MeCabPathRefButtonClickes()));
        QObject::connect(refSysDicPath, SIGNAL(clicked()), Dialog, SLOT(MeCabSysDicRefButtonClicked()));
        QObject::connect(refSysDicBildFolder, SIGNAL(clicked()), Dialog, SLOT(MeCabSisDicBuildButtonClicked()));
        QObject::connect(comboBoxVoiceSel, SIGNAL(currentIndexChanged(int)), Dialog, SLOT(sapiVoiceSelectCmbBoxSelChange()));
        QObject::connect(refVoicevox_runPathButton, SIGNAL(clicked()), Dialog, SLOT(VoiceVoxRunexePasRefButtonClicked()));
        QObject::connect(comboBoxIfMode, SIGNAL(currentIndexChanged(int)), Dialog, SLOT(SapiOutputMoideCmbSelChange()));
        QObject::connect(dialrate, SIGNAL(valueChanged(int)), Dialog, SLOT(SapiRateDialChanged()));
        QObject::connect(dialVolume, SIGNAL(valueChanged(int)), Dialog, SLOT(SapiVolumeDialChanged()));
        QObject::connect(EditLestMuteTime, SIGNAL(editingFinished()), Dialog, SLOT(SapiMuonZanChanged()));
        QObject::connect(comboBox_voiceSel, SIGNAL(currentIndexChanged(int)), Dialog, SLOT(Aqtk10VoiceSelCmbChanged()));
        QObject::connect(dial_Aqtk10Ontei, SIGNAL(valueChanged(int)), Dialog, SLOT(Aqtk10VoiceSelCmbChanged()));
        QObject::connect(dial_Aqtk10High, SIGNAL(valueChanged(int)), Dialog, SLOT(Aqtk10VoiceHighDialChenged()));
        QObject::connect(dial_Aqtk10Ontei2, SIGNAL(valueChanged(int)), Dialog, SLOT(Aqtk10VoiceStep2DialChanged()));
        QObject::connect(dial_Aqtk10Speed, SIGNAL(valueChanged(int)), Dialog, SLOT(Aqtk10VoiceStepDialChanged()));
        QObject::connect(dial_Aqtk10Accent, SIGNAL(valueChanged(int)), Dialog, SLOT(Aqtk10AccentDilaChanged()));
        QObject::connect(dial_Aqtk10Volume, SIGNAL(valueChanged(int)), Dialog, SLOT(Aqtk10OutputVolumeDialChanged()));
        QObject::connect(checkBoxEditWndColor, SIGNAL(stateChanged(int)), Dialog, SLOT(OtherTextBackColorCheckBoxChanged()));
        QObject::connect(refTextEditorPath, SIGNAL(clicked()), Dialog, SLOT(OtherTextEditorRefButtonClicked()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("Dialog", "MeCab\343\203\221\343\202\271", nullptr));
        refMeCabPath->setText(QCoreApplication::translate("Dialog", "\345\217\202\347\205\247", nullptr));
        label_2->setText(QCoreApplication::translate("Dialog", "\343\202\267\343\202\271\343\203\206\343\203\240\350\276\236\346\233\270\343\203\221\343\202\271", nullptr));
        refSysDicPath->setText(QCoreApplication::translate("Dialog", "\345\217\202\347\205\247", nullptr));
        label_3->setText(QCoreApplication::translate("Dialog", "\343\203\246\343\203\274\343\202\266\350\276\236\346\233\270\343\203\221\343\202\271", nullptr));
        refMeCabUserDicPath->setText(QCoreApplication::translate("Dialog", "\345\217\202\347\205\247", nullptr));
        label_4->setText(QCoreApplication::translate("Dialog", "\343\203\246\343\203\274\343\202\266\343\203\274CSV\350\276\236\346\233\270\343\203\221\343\202\271", nullptr));
        refMeCabUserCsvDicPath->setText(QCoreApplication::translate("Dialog", "\345\217\202\347\205\247", nullptr));
        label_5->setText(QCoreApplication::translate("Dialog", "\343\202\267\343\202\271\343\203\206\343\203\240\350\276\236\346\233\270\346\247\213\347\257\211\343\203\225\343\202\251\343\203\253\343\203\200\343\203\274", nullptr));
        refSysDicBildFolder->setText(QCoreApplication::translate("Dialog", "\345\217\202\347\205\247", nullptr));
        label_7->setText(QCoreApplication::translate("Dialog", "\350\276\236\346\233\270\346\226\207\345\255\227\343\202\273\343\203\203\343\203\210", nullptr));
        label_6->setText(QCoreApplication::translate("Dialog", "csv\346\226\207\345\255\227\343\202\273\343\203\203\343\203\210", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(MeCAb), QCoreApplication::translate("Dialog", "MeCab", nullptr));
        label_11->setText(QCoreApplication::translate("Dialog", "rate", nullptr));
        label_10->setText(QCoreApplication::translate("Dialog", "volume", nullptr));
        label_12->setText(QCoreApplication::translate("Dialog", "voice", nullptr));
        label_13->setText(QCoreApplication::translate("Dialog", "\347\204\241\351\237\263\346\256\213", nullptr));
        label_14->setText(QCoreApplication::translate("Dialog", "ms \345\244\211\346\217\233\346\257\216\343\201\256\346\234\253\345\260\276\347\204\241\351\237\263\346\231\202\351\226\223\346\234\200\345\244\247\n"
"max1000ms", nullptr));
        label_16->setText(QCoreApplication::translate("Dialog", "VOICEVOX\n"
"run.exe\343\203\221\343\202\271", nullptr));
        refVoicevox_runPathButton->setText(QCoreApplication::translate("Dialog", "\345\217\202\347\205\247", nullptr));
        label_18->setText(QCoreApplication::translate("Dialog", "\345\207\272\345\212\233\343\203\242\343\203\274\343\203\211", nullptr));
        label_15->setText(QCoreApplication::translate("Dialog", "VOICEVOX\343\201\256\351\237\263\345\243\260\344\275\277\347\224\250\346\231\202\343\201\257\350\250\255\345\256\232\343\201\227\343\201\246\343\201\217\343\201\240\343\201\225\343\201\204\357\274\201", nullptr));
        label_17->setText(QCoreApplication::translate("Dialog", "\345\207\272\345\212\233\343\203\242\343\203\274\343\203\211\343\201\257\343\200\201\351\201\251\345\210\207\343\201\252sapi\343\201\270\343\201\256\343\203\207\343\203\274\343\202\277\343\201\256\346\270\241\343\201\227\346\226\271\343\201\214\347\242\272\347\216\207\343\201\227\343\201\246\343\201\204\343\201\252\343\201\204\343\201\237\343\202\201\343\201\256\343\203\206\343\202\271\343\203\210\350\250\255\345\256\232\343\201\247\343\201\231\343\200\202", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(SAPI), QCoreApplication::translate("Dialog", "SAPI5.4", nullptr));
        label_19->setText(QCoreApplication::translate("Dialog", "\343\200\200\345\243\260\351\201\270\346\212\236\343\200\200", nullptr));
        label_26->setText(QCoreApplication::translate("Dialog", "\344\273\245\344\270\213\350\250\255\345\256\232\343\201\257\345\243\260\351\201\270\346\212\236\343\201\247\345\244\211\343\202\217\343\202\212\343\201\276\343\201\231\357\274\201", nullptr));
        label_21->setText(QCoreApplication::translate("Dialog", "\351\237\263\347\250\213", nullptr));
        label_20->setText(QCoreApplication::translate("Dialog", "\351\200\237\345\272\246", nullptr));
        label_22->setText(QCoreApplication::translate("Dialog", "\351\253\230\343\201\225", nullptr));
        label_23->setText(QCoreApplication::translate("Dialog", "\343\202\242\343\202\257\343\202\273\343\203\263\343\203\210", nullptr));
        label_24->setText(QCoreApplication::translate("Dialog", "\351\237\263\347\250\213\357\274\222", nullptr));
        label_25->setText(QCoreApplication::translate("Dialog", "volume", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(AQTK10), QCoreApplication::translate("Dialog", "AQTK10", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(JTALK), QCoreApplication::translate("Dialog", "JTALK", nullptr));
        checkBoxEditWndColor->setText(QCoreApplication::translate("Dialog", "\343\203\206\343\202\255\343\202\271\343\203\210\350\241\250\347\244\272\350\203\214\346\231\257\350\211\262\351\273\222/\346\226\207\345\255\227\350\211\262\347\231\275", nullptr));
        label_28->setText(QCoreApplication::translate("Dialog", "text editor", nullptr));
        refTextEditorPath->setText(QCoreApplication::translate("Dialog", "\345\217\202\347\205\247", nullptr));
        label_29->setText(QCoreApplication::translate("Dialog", "editor option\n"
"line:%d\n"
"file:%f", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(other), QCoreApplication::translate("Dialog", "other", nullptr));
        label_27->setText(QCoreApplication::translate("Dialog", "TTS\351\201\270\346\212\236", nullptr));
        ttsSelector->setCurrentText(QString());
        okButton->setText(QCoreApplication::translate("Dialog", "OK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGDIALOG_H
