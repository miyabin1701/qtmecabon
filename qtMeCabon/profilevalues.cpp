/**
 * @file profilevalues.cpp
 * @brief 初期設定データのハンドリング
 * @author m.kawakami
 * @date 23/10/??
 */

#include "profilevalues.h"
//#include <QTextCodec>
#include <QStringEncoder>
#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QMessageBox>
#include <qtmetamacros.h>


extern bool isExistFile( QString path );


static QString szrecentfile[8] =
{   "recentFilename0",
    "recentFilename1",
    "recentFilename2",
    "recentFilename3",
    "recentFilename4",
    "recentFilename5",
    "recentFilename6",
    "recentFilename7"
};


static QString szrecentidx[8] =
{    "recentFileLine0",
     "recentFileLine1",
     "recentFileLine2",
     "recentFileLine3",
     "recentFileLine4",
     "recentFileLine5",
     "recentFileLine6",
     "recentFileLine7"
};


/**
 * @fn
 * profileValuesのコンストラクタ
 * @brief profileValuesクラスのコンストラクタ
 * @return なし
 * @sa
 * @detail profileValuesクラスのコンストラクタ　プロファイルデータを.iniファイルから取得する
 */
profileValues::profileValues()
{   QString iniFile;
    QString defMecabPath;
    QString defSystemDictionaryFolder;
    QString defUserDictionaryPath;
    QString defUserDictionaryCsvPath;
    QString defSystemDictionaryBuildFolder;
//    QString defOpenJTalkDicPath;
//    QString defOpenJTalkDicBuildDir;

    // ディレクトリの絶対パス
    //    QApplication::applicationDirPath();
    // 実行ファイルの絶対パス
    //    QApplication::applicationFilePath();
    // image for exe file dir
    // qtmecabon-+-qtmecabon.exe
    //           \ qtmecabon.ini            <- これが無かったら、以下のイメージでデフォルトセット
    //           \ libmecab64.dll
    //           \ model---metas.json       <- voicevox のmodelフォルダー
    //           +-mecab-0.996---bin-----libmecab.lib (32bit版)
    //           \                       mecab-dict-index.exe etc  (32bit版)
    //           +-work-+--final---dicrc    <- ここがsystem dictionary mecab&openJtalk
    //                  \          sys.dic
    //                  \          unk.dic etc
    //                  +--seed----dicrc
    //                             *.csv
    //                             model
    //                             corpus corpus.1~8
    //                             *.def
    //                             matrix.* etc

    fgRestart = false;
    iniFile = QApplication::applicationDirPath() + "\\qtMeCabon.ini";
    if ( !QFile::exists( iniFile )) // iniファイルが無ければ
    {
//        qDebug() << QString("defset defMecabPath:[%1]").arg(defMecabPath);
//        qDebug() << QString("defset defSystemDictionaryFolder:[%1]").arg(defSystemDictionaryFolder);
//        qDebug() << QString("defset defSystemDictionaryBuildFolder:[%1]").arg(defSystemDictionaryBuildFolder);
        defMecabPath = QApplication::applicationDirPath() + "/mecab-0.996/bin/mecab.exe";
//        qDebug() << QString("defset defMecabPath:[%1]").arg(defMecabPath);
        defSystemDictionaryFolder = QApplication::applicationDirPath() + "/work/final";
//        qDebug() << QString("defset defSystemDictionaryFolder:[%1]").arg(defSystemDictionaryFolder);
        defSystemDictionaryBuildFolder = QApplication::applicationDirPath() + "/work";
//        qDebug() << QString("defset defSystemDictionaryBuildFolder:[%1]").arg(defSystemDictionaryBuildFolder);
//        defOpenJTalkDicPath = QApplication::applicationDirPath() + "\\work\\final";
//        defOpenJTalkDicBuildDir = QApplication::applicationDirPath() + "\\work";
//        defUserDictionaryPath = QApplication::applicationDirPath() + "";
//        defUserDictionaryCsvPath = QApplication::applicationDirPath() + "";
    }
    mpSettings = new QSettings( "qtMeCabon.ini", QSettings::IniFormat );
    //    mpSettings->setIniCodec( QTextCodec::codecForName( "UTF-8" )); QT6でQTextCodecは削除された
    mpSettings->beginGroup( "Operation" );
    oper.ttsSelector = ( enum TTSNAME )mpSettings->value( "ttsSelector", 0 ).toInt();
    qDebug() << "profileValue( " << oper.ttsSelector << " )";
    oper.displayOutputOffThenPouse = mpSettings->value( "displayOutputOffThenPouse", false ).toBool();
    oper.soundOutputSkip = mpSettings->value( "soundOutputSkip", false ).toBool();
    oper.symbolSpeakingOff = mpSettings->value( "symbolSpeakingOff", false ).toBool();
    oper.idisplayMagnify = mpSettings->value( "idisplayMagnify", 1 ).toInt();
    oper.iAppVolume = mpSettings->value( "idAppVolume", 50 ).toInt();
    for ( int i = 0; i < lengthof( oper.recent ); i++ )
    {   oper.recent[i].pwszCurrentPlayFile = new ( QString )( mpSettings->value( szrecentfile[i], "" ).toString());
        oper.recent[i].iCurrentPlayFileLine = mpSettings->value( szrecentidx[i], 1 ).toInt();
    }
    oper.iSizeMainwindowHight = mpSettings->value( "SizeMainwindowHight", 0 ).toInt();
    oper.iSizeMainwindowWidth = mpSettings->value( "SizeMainwindowWidth", 0 ).toInt();
    oper.iSizeCorpaswindowHight = mpSettings->value( "SizeCorpaswindowHight", 0 ).toInt();
    oper.iSizeCorpaswindowWidth = mpSettings->value( "SizeCorpaswindowWidth", 0 ).toInt();
    oper.iSizeOutputwindowHight = mpSettings->value( "SizeOutputwindowHight", 0 ).toInt();
    oper.iSizeOutputwindowWidth = mpSettings->value( "SizeOutputwindowWidth", 0 ).toInt();
    mpSettings->endGroup();

    mpSettings->beginGroup( "other" );
    other.selBakeColorBlack = mpSettings->value( "selBakeColorBlack", false ).toBool();
    other.infoWndLineMaxLimmit = mpSettings->value( "infoWndLineMaxLimmit", 1000 ).toInt();
    other.pwszRamDiskPath = new ( QString )( mpSettings->value( "pwszRamDiskPath", "" ).toString());
    other.pwszEdtorPath = new ( QString )( mpSettings->value( "pwszEdtorPath", "" ).toString());
    other.pwszEdtorTagjumpCommand = new ( QString )( mpSettings->value( "pwszEdtorTagjumpCommand", "" ).toString());
    mpSettings->endGroup();

    mpSettings->beginGroup( "mecab" );
    mecab.pwszMecabPath = new ( QString )( mpSettings->value( "pwszMecabPath", defMecabPath ).toString());
    mecab.pwszSystemDictionaryFolder = new ( QString )( mpSettings->value( "pwszSystemDictionaryFolder", defSystemDictionaryFolder ).toString());
    mecab.pwszUserDictionaryPath = new ( QString )( mpSettings->value( "pwszUserDictionaryPath", "" ).toString());
    mecab.pwszUserDictionaryCsvPath = new ( QString )( mpSettings->value( "pwszUserDictionaryCsvPath", "" ).toString());
    mecab.pwszSystemDictionaryBuildFolder = new ( QString )( mpSettings->value( "pwszSystemDictionaryBuildFolder", defSystemDictionaryBuildFolder ).toString());
    mecab.chSetDictionary = new ( QString )( mpSettings->value( "chSetDictionary", "" ).toString());
    mecab.chSetDonCsvDictionary = new ( QString )( mpSettings->value( "chSetDonCsvDictionary", "" ).toString());
    mpSettings->endGroup();

    mpSettings->beginGroup( "sapi" );
    sapi.iRate = mpSettings->value( "iRate", 0 ).toInt();
    sapi.iVolume = mpSettings->value( "iVolume", 0 ).toInt();
    sapi.pwszVoiceName = new ( QString )( mpSettings->value( "pwszVoiceName", "" ).toString());
    sapi.iMuonZan = mpSettings->value( "iMuonZan", 0 ).toInt();
    sapi.pwszVoicevox4sapiRunPath = new ( QString )( mpSettings->value( "pwszVoicevox4sapiRunPath", "" ).toString());
    sapi.iOutputMode = mpSettings->value( "iOutputMode", 0 ).toInt();
    mpSettings->endGroup();

    mpSettings->beginGroup( "aqtk" );
    aqtk.iSpeed = mpSettings->value( "iSpeed", 0 ).toInt();
    aqtk.iStep = mpSettings->value( "iStep", 0 ).toInt();
    aqtk.iHigh = mpSettings->value( "iHigh", 0 ).toInt();
    aqtk.iAccent = mpSettings->value( "iAccent", 0 ).toInt();
    aqtk.iStep2 = mpSettings->value( "iStep2", 0 ).toInt();
    aqtk.iVolume = mpSettings->value( "iVolume", 0 ).toInt();
    aqtk.iVoiceIdx = mpSettings->value( "iVoiceIdx", 0 ).toInt();
    mpSettings->endGroup();

    mpSettings->beginGroup( "voicevox" );
    vvox.pwszOpenJTalkDicPath = new ( QString )( mpSettings->value( "pwszOpenJTalkDicPath", defSystemDictionaryFolder ).toString());
    vvox.pwszOpenJTalkDicBuildDir = new ( QString )( mpSettings->value( "pwszOpenJTalkDicBuildDir", defSystemDictionaryBuildFolder ).toString());
    vvox.ispeaker_id  = mpSettings->value( "speaker_id", 0 ).toInt();
    mpSettings->endGroup();

    updateCorpusFile();
#if 0
#endif
}


/**
 * @fn
 * profileValuesのデストラクタ
 * @brief profileValuesクラスのデストラクタ
 * @return なし
 * @sa
 * @detail profileValuesクラスのデストラクタ　プロファイルデータを.iniファイルへ書き込む
 */
profileValues::~profileValues()
{

//    QSettings settings( "qtMeCabon.ini", QSettings::IniFormat );
//    mpSettings->setIniCodec( QTextCodec::codecForName( "UTF-8" ));
//    mpSettings->setIniCodec( "UTF-8" );


    mpSettings->beginGroup( "Operation" );
    mpSettings->setValue( "ttsSelector", oper.ttsSelector );
    qDebug() << "~profileValue( " << oper.ttsSelector << " )";
    mpSettings->setValue( "displayOutputOffThenPouse", oper.displayOutputOffThenPouse );
    mpSettings->setValue( "soundOutputSkip", oper.soundOutputSkip );
    mpSettings->setValue( "symbolSpeakingOff", oper.symbolSpeakingOff );
    mpSettings->setValue( "idisplayMagnify", oper.idisplayMagnify );
    mpSettings->setValue( "idAppVolume", oper.iAppVolume );
    for ( int i = 0; i < lengthof( oper.recent ); i++ )
    {   mpSettings->setValue( szrecentfile[i], *( oper.recent[i].pwszCurrentPlayFile ));
        mpSettings->setValue( szrecentidx[i], oper.recent[i].iCurrentPlayFileLine );
    }
    mpSettings->setValue( "SizeMainwindowHight", oper.iSizeMainwindowHight );
    mpSettings->setValue( "SizeMainwindowWidth", oper.iSizeMainwindowWidth );
    mpSettings->setValue( "SizeCorpaswindowHight", oper.iSizeCorpaswindowHight );
    mpSettings->setValue( "SizeCorpaswindowWidth", oper.iSizeCorpaswindowWidth );
    mpSettings->setValue( "SizeOutputwindowHight", oper.iSizeOutputwindowHight );
    mpSettings->setValue( "SizeOutputwindowWidth", oper.iSizeOutputwindowWidth );
    mpSettings->setValue( "pwszEdtorPath", *( other.pwszEdtorPath ));
    mpSettings->endGroup();

    mpSettings->beginGroup( "other" );
    mpSettings->setValue( "selBakeColorBlack", other.selBakeColorBlack );
    mpSettings->setValue( "infoWndLineMaxLimmit", other.infoWndLineMaxLimmit );

    mpSettings->setValue( "pwszRamDiskPath", *( other.pwszRamDiskPath ));
    mpSettings->setValue( "pwszEdtorPath", *( other.pwszEdtorPath ));
    mpSettings->setValue( "pwszEdtorTagjumpCommand", *( other.pwszEdtorTagjumpCommand ));
    mpSettings->endGroup();

    mpSettings->beginGroup( "mecab" );
    mpSettings->setValue( "pwszMecabPath", *( mecab.pwszMecabPath ));
    mpSettings->setValue( "pwszSystemDictionaryFolder", *( mecab.pwszSystemDictionaryFolder ));
    mpSettings->setValue( "pwszUserDictionaryPath", *( mecab.pwszUserDictionaryPath ));
    mpSettings->setValue( "pwszUserDictionaryCsvPath", *( mecab.pwszUserDictionaryCsvPath ));
    mpSettings->setValue( "pwszSystemDictionaryBuildFolder", *( mecab.pwszSystemDictionaryBuildFolder ));
    mpSettings->setValue( "chSetDictionary", *( mecab.chSetDictionary ));
    mpSettings->setValue( "chSetDonCsvDictionary", *( mecab.chSetDonCsvDictionary ));
    mpSettings->endGroup();

    mpSettings->beginGroup( "sapi" );
    mpSettings->setValue( "iRate", sapi.iRate );
    mpSettings->setValue( "iVolume", sapi.iVolume );
    mpSettings->setValue( "pwszVoiceName", *( sapi.pwszVoiceName ));
    mpSettings->setValue( "iMuonZan", sapi.iMuonZan );
    mpSettings->setValue( "pwszVoicevox4sapiRunPath", *( sapi.pwszVoicevox4sapiRunPath ));
    mpSettings->setValue( "iOutputMode", sapi.iOutputMode );
    mpSettings->endGroup();

    mpSettings->beginGroup( "aqtk" );
    mpSettings->setValue( "iSpeed", aqtk.iSpeed );
    mpSettings->setValue( "iStep", aqtk.iStep );
    mpSettings->setValue( "iHigh", aqtk.iHigh );
    mpSettings->setValue( "iAccent", aqtk.iAccent );
    mpSettings->setValue( "iStep2", aqtk.iStep2 );
    mpSettings->setValue( "iVolume", aqtk.iVolume );
    mpSettings->setValue( "iVoiceIdx", aqtk.iVoiceIdx );
    mpSettings->endGroup();

    mpSettings->beginGroup( "voicevox" );
    mpSettings->setValue( "pwszOpenJTalkDicPath", *( vvox.pwszOpenJTalkDicPath ));
    mpSettings->setValue( "pwszOpenJTalkDicBuildDir", *( vvox.pwszOpenJTalkDicBuildDir ));
    mpSettings->setValue( "speaker_id", vvox.ispeaker_id );
    mpSettings->endGroup();

    mpSettings->sync();
    delete mpSettings;
}


/**
 * @fn
 * アプリケーションの環境変数"tmp"の入れ替え
 * @brief アプリケーションの環境変数"tmp"の入れ替え
 * @param (envName) 環境変数名
 * @param (addStr) 入れ替えるデータ
 * @return なし
 * @sa
 * @detail アプリケーションの環境変数"tmp"の入れ替え　sapiの際wavデータはファイル渡しで煩雑に書き換えるので
 * tmpフォルダーを初期設定項目のramディスク設定と入れ替える
 */
void profileValues::replaceEnv( QString envName, QString addStr )
{
//    char *envvar;
//    size_t requiredSize, requiredSize2;

    if ( addStr.isEmpty())
    {   return;
    }
    while ( 1 )
    {
#if 0       // TMP の入れ替えなので元々入っていたフォルダーはあってはならない
       getenv_s( &requiredSize, NULL, 0, envName.toStdString().c_str());
        if ( requiredSize != 0 )
        {   requiredSize += addStr.length() + 1;
            envvar = ( char * )malloc( requiredSize * sizeof( char ));
            if ( envvar != NULL )
            {   getenv_s( &requiredSize2, envvar, requiredSize, envName.toStdString().c_str());
                strcat( envvar, addStr.toStdString().c_str());
                strcat( envvar, ";" );
                _putenv_s( envName.toStdString().c_str(), envvar );
                free( envvar );
                break;
        }   }
#endif
        _putenv_s( envName.toStdString().c_str(), addStr.toStdString().c_str());
        break;
    }
#if 0       // for debug
    getenv_s( &requiredSize, NULL, 0, envName.toStdString().c_str());
    if ( requiredSize != 0 )
    {   envvar = ( char * )malloc( requiredSize * sizeof( char ));
        if ( envvar != NULL )
        {   getenv_s( &requiredSize2, envvar, requiredSize, envName.toStdString().c_str());
            qDebug() << QString::asprintf("%d[%s]", requiredSize2, envvar );
            free( envvar );
        }
    }
#endif
}


/**
 * @fn
 * 履歴のアップデート
 * @brief 履歴のアップデート
 * @param (FileName) 最新読み込みファイル名
 * @return なし
 * @sa
 * @detail 履歴のアップデート　従来の履歴を検索、同一ファイル名が在れば履歴トップに移動し、行数を更新
 * 従来の履歴に無ければ履歴をシフトし、履歴トップにファイル名、行数を更新
 */
void profileValues::updateRecentFile( QString FileName )
{   QString *swapSaveFileName;
    int i, swapSaveFileLineno;

    for ( i = 0; i < lengthof( oper.recent ); i++ )    // 同じファイルが在るかチェック
    {   if ( FileName == *( oper.recent[i].pwszCurrentPlayFile ))    // 在ったら　リスト先頭に移動　行数更新・・・読み込み時だから不明
        {   if ( i > 0 )    // 先頭でなければ
            {   swapSaveFileName = oper.recent[i].pwszCurrentPlayFile;
                swapSaveFileLineno = oper.recent[i].iCurrentPlayFileLine;
                for ( ; i > 0; i-- )
                {   oper.recent[i].pwszCurrentPlayFile =
                        oper.recent[i - 1].pwszCurrentPlayFile;
                    oper.recent[i].iCurrentPlayFileLine =
                        oper.recent[i - 1].iCurrentPlayFileLine;
                }
                oper.recent[0].pwszCurrentPlayFile = swapSaveFileName;
                oper.recent[0].iCurrentPlayFileLine = swapSaveFileLineno;
            }
//          i = 0;
            return;
        }
    }
//  if ( i >= lengthof( oper.recent ))    // 無かったらリストシフトし先頭に入れる
    {   i = lengthof( oper.recent ) - 1;
        if ( oper.recent[i].pwszCurrentPlayFile != NULL )
        {   free( oper.recent[i].pwszCurrentPlayFile );
        }
        for ( ; i > 0; i-- )
        {   oper.recent[i].pwszCurrentPlayFile =
                oper.recent[i - 1].pwszCurrentPlayFile;
            oper.recent[i].iCurrentPlayFileLine =
                oper.recent[i - 1].iCurrentPlayFileLine;
        }
        oper.recent[0].pwszCurrentPlayFile = new ( QString )( FileName );
        oper.recent[0].iCurrentPlayFileLine = 1;
    }
}


/**
 * @fn
 * コーパスファイルのパス取得
 * @brief コーパスファイルのパス取得
 * @return なし
 * @sa
 * @detail コーパスファイルのパス取得　システム辞書ビルドフォルダーのパスを参照し
 * コーパスファイルの名前を付加する
 */
QString profileValues::getCorpusPath()
{   QString FileName = *( mecab.pwszSystemDictionaryBuildFolder );

    FileName = FileName + u8"/";
    FileName.replace( u8"//", u8"/" );
    return( FileName + u8"seed/corpus" );
}


/**
 * @fn
 * コーパスファイルのアップデート時に履歴コピーを残す
 * @brief コーパスファイルのアップデート時に履歴コピーを残す
 * @return なし
 * @sa
 * @detail コーパスファイルのアップデート時に履歴コピーを残す　現在のコーパスファイルと最新バックアップの日付を比較し
 * 変わっていれば、コーパスファイルの拡張子.1から.MAX_CORPUSBACKUPまで複数のバックアップを残す。
 */
void profileValues::updateCorpusFile()
{   int noBackup;
    QString FileName = getCorpusPath();
    QString srcFile, dstFile;

    {   QFileInfo currentFile( FileName );
        QDateTime currentDate = currentFile.lastModified();
        dstFile = QString( FileName + ".1" );
        QFileInfo backupFile( dstFile );
        QDateTime backupDate = backupFile.lastModified();
        if ( currentDate <= backupDate )    // タイムスタンプ比較
        {   return;
    }   }
    for ( noBackup = MAX_CORPUSBACKUP; noBackup > 0; noBackup-- )
    {   dstFile = QString( FileName + ".%1" ).arg( noBackup );
        if ( QFile::exists( dstFile ))
        {   QFile::remove( dstFile );
        }
        if ( noBackup > 1 )
        {   srcFile = QString( FileName + ".%1" ).arg( noBackup - 1 );
        }
        else
        {   srcFile = FileName;
        }
        QFile::copy( srcFile, dstFile );
    }
}


