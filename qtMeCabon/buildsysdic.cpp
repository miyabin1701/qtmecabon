/**
 * @file buildsysdic.cpp
 * @brief システム辞書のビルド処理
 * @author m.kawakami
 * @date 21/05/??
 */

#include "buildsysdic.h"
#include "profilevalues.h"
#include <io.h>
#include <QMessageBox>
#include "messagebuff.h"
#include "soundplaystate.h"
#include "mecabif.h"
#include "playlinevvoxcore.h"
#include <fstream>


extern profileValues   *pprof;


/**
 * @fn
 * ビルドsysdicクラスコンストラクタ
 * @brief システム辞書ビルドクラスのコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail 何もしてない
 */
buildSysDic::buildSysDic(QObject *parent)
    : QThread{parent}
{
}


/**
 * @fn
 * 既存Pathのファイル名部分を入れ替える
 * @brief 引数Pathのファイル名部分を入れ替える
 * @param (*pszDest) 入れ替え後のPathを入れるエリア
 * @param (lenpszDest) 入れ替え後のPathを入れるエリアサイズ
 * @param (*pszPath) 入れ替え元のPath
 * @param (*pszfileName) 入れ替えるファイル名
 * @return 入れ替え後のPathへのポインタ
 * @sa
 * @detail 自exeファイルのパスなどから、同一ディレクトリ内のファイルへのPathを生成する
 */
wchar_t *exchgFileName( wchar_t *pszDest, size_t lenpszDest , wchar_t *pszPath, wchar_t *pszfileName )
{	wchar_t	szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFname[_MAX_FNAME], szExt[_MAX_EXT];
    errno_t errno;

    errno = _wsplitpath_s( pszPath, szDrive, lengthof( szDrive ),
                          szDir, lengthof( szDir ), szFname, lengthof( szFname ), szExt, lengthof( szExt ));
//    _wmakepath( pszDest, szDrive, szDir, pszfileName, L"" );
    _wmakepath_s( pszDest, lenpszDest, szDrive, szDir, pszfileName, L"" );
    return ( pszDest );
}


/**
 * @fn
 * 指定されたプロセスを実行し終了ステータスを取得する
 * @brief 指定プロセスを実行し、コンソール出力のキャプチャ、終了時ステータスを取得する
 * @param (*processPath) 実行するプロセスのパス
 * @param (*processOption) 実行するプロセスの起動オプション文字列
 * @param (*execFolder) 実行時のカレントフォルダー
 * @param (*pdwExitCode) プロセスの終了時コードを書き込むポインタ
 * @return エラーコード　起動失敗など
 * @sa
 * @detail チャイルドプロセスを起動し、コンソール出力をキャプチャしメッセージをキューへ出力、などの一連の処理を行う
 */
DWORD execProcess( wchar_t *processPath, wchar_t *processOption, wchar_t *execFolder, DWORD *pdwExitCode )
{	STARTUPINFO			tStartupInfo = { 0 };
    PROCESS_INFORMATION	tProcessInfomation = { 0 };
//    HRESULT hResult = NOERROR;
    HANDLE readPipe, writePipe;		// パイプの作成
    SECURITY_ATTRIBUTES sa;
    messageBuff msgBuff;
    char readBuf[1025], linebuf[1025], utf8rem[7], ch;
    DWORD stat, totalLen, len, bufix;
    int lineix;
//    QString msg;
    DWORD err = 0;

    msgBuff.enterMessage( QString::fromWCharArray( execFolder ));
    msgBuff.enterMessage( QString::fromWCharArray( processPath ));
    msgBuff.enterMessage( QString::fromWCharArray( processOption ));
    sa.nLength = sizeof( sa );
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if ( CreatePipe( &readPipe, &writePipe, &sa, 0 ) == 0 )
    {	return( HRESULT_FROM_WIN32( ::GetLastError()));
    }
    tStartupInfo.cb = sizeof( tStartupInfo );
    tStartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    tStartupInfo.hStdOutput = writePipe;
    tStartupInfo.hStdError = writePipe;
    tStartupInfo.wShowWindow = SW_HIDE;
    if ( CreateProcess(                         //	プロセスの起動
            processPath,                        // in LPCTSTR lpApplicationName,
            ( LPTSTR )processOption,            // io LPTSTR lpCommandLine,
            NULL,                               // in LPSECURITY_ATTRIBUTES lpProcessAttributes,
            NULL,                               // in LPSECURITY_ATTRIBUTES lpThreadAttributes,
            TRUE,                               // in BOOL bInheritHandles,
            0,                                  // in DWORD dwCreationFlags,
            NULL,                               // in LPVOID lpEnvironment,
            execFolder,                         // in LPCTSTR lpCurrentDirectory,
            &tStartupInfo,                      // in LPSTARTUPINFO lpStartupInfo,
            &tProcessInfomation ) != 0 )        // out LPPROCESS_INFORMATION lpProcessInformation
    {   for ( lineix = 0, utf8rem[0] = '\0'; ; )
        {	stat = WaitForSingleObject( tProcessInfomation.hProcess, 100 );
            if ( stat != WAIT_TIMEOUT )
            {	break;
            }
            if ( PeekNamedPipe( readPipe, NULL, 0, NULL, &totalLen, NULL ) == 0 )
            {	break;
            }
            for ( ; totalLen > 0; totalLen -= len )
            {   if ( ReadFile( readPipe, &readBuf[0], sizeof( readBuf ) - 1, &len, NULL ) == 0 )
                {	CloseHandle( writePipe );
                    CloseHandle( readPipe );
                    if ( pdwExitCode != NULL )
                    {	::GetExitCodeProcess( tProcessInfomation.hProcess, pdwExitCode );
                    }
                    ::CloseHandle( tProcessInfomation.hProcess );	// ハンドルの解放
                    ::CloseHandle( tProcessInfomation.hThread );
                    return( HRESULT_FROM_WIN32( ::GetLastError()));
                }
                for ( bufix = 0; bufix < len; bufix++ )
                {	ch = readBuf[bufix];
                    if ( lineix == 0 )
                    {
#if 1
                        if (( ch == '\r' ) &&
                            ( readBuf[bufix + 1] == '\n' ))
                        {	ch = '\n';
                            bufix++;
                        }
#endif
                        linebuf[lineix++] = ch;
                        continue;       // linebuf 先頭にCR or LFが在るように区切る
                    }
                    else
                    {	if (( ch != '\r' ) && ( ch != '\n' ) && ( lineix < sizeof( linebuf )))
                        {   linebuf[lineix++] = ch;
                            continue;
                        }
                        else
                        {   //if ( ch == '\r' )
                            //{   linebuf[lineix++] = ch;
                            //}
                            linebuf[lineix] = '\0';
                            msgBuff.enterMessage( QString::fromUtf8( linebuf, lineix ));
                            lineix = 0;
                            bufix--;
        }   }   }   }   }
        if ( lineix > 0 )
        {	linebuf[lineix] = '\0';
            msgBuff.enterMessage( QString::fromUtf8( linebuf, lineix ));
    }   }
    else
    {   err = ::GetLastError();
    }
#if 0
    for ( utf8rem[0] = '\0'; ; )
    {	stat = WaitForSingleObject( tProcessInfomation.hProcess, 100 );
        if ( PeekNamedPipe( readPipe, NULL, 0, NULL, &totalLen, NULL ) == 0 )
        {	break;
        }
        for ( ; totalLen > 0; totalLen -= len )
        {	strcpy_s( readBuf, sizeof( readBuf ) - 1, utf8rem );
            l = ( int )strlen( readBuf );
            if ( ReadFile( readPipe, &readBuf[l], sizeof( readBuf ) - l - 1, &len, NULL ) == 0 )
            {	::CloseHandle( tProcessInfomation.hProcess );	// ハンドルの解放
                ::CloseHandle( tProcessInfomation.hThread );
                CloseHandle( writePipe );
                CloseHandle( readPipe );
                return( HRESULT_FROM_WIN32( ::GetLastError()));
            }
            readBuf[len] = '\0';
            for ( l = len - 1; ; l-- )
            {	if (( readBuf[l] < 0x80 ) || ( 0xbf < readBuf[l] ))	// nbyte code
                {	break;
            }	}
            if (( 0x00 <= readBuf[l] ) && ( readBuf[l] <= 0x7f ))
            {	l++;
            }
            strcpy_s( utf8rem, sizeof( utf8rem ), &readBuf[l]);
            readBuf[l] = '\0';
            msgBuff.enterMessage( QString::fromUtf8( readBuf, l ));
        }
        if ( stat != WAIT_TIMEOUT )
        {	break;
    }	}
#endif
#if 0
    if ( ::WaitForSingleObject(						//	プロセスの終了を待つ
            tProcessInfomation.hProcess,
            INFINITE ) == WAIT_FAILED )
    {	hResult = HRESULT_FROM_WIN32( ::GetLastError());
    }
#endif
    CloseHandle( writePipe );
    CloseHandle( readPipe );
    if ( pdwExitCode != NULL )
    {	::GetExitCodeProcess( tProcessInfomation.hProcess, pdwExitCode );
    }
    ::CloseHandle( tProcessInfomation.hProcess );	// ハンドルの解放
    ::CloseHandle( tProcessInfomation.hThread );
    if ( err == 0 )
    {   return ( S_OK );
    }
    qDebug() << QString("::GetLastError()::%1").arg( err );
    return ( HRESULT_FROM_WIN32( err ));
}


//#define CHECK_DIE(condition) \
//(condition) ? 0 : die() & std::cerr << __FILE__ << \
//                              "(" << __LINE__ << ") [" << #condition << "] "


/**
 * @fn
 * システム辞書のビルドスレッド
 * @brief システム辞書のビルドのステップを順次実行する
 * @return なし
 * @sa
 * @detail mecabの辞書ビルド手順に基づき、各exeを実行する
 */
void buildSysDic::run()
{
//    static wchar_t *szMecabDictIndexOption1 = ( wchar_t * )L" -f utf-8 -t utf-8 --model=R:/release/work/seed/mecab-ipadic-2.7.0-20070801U8.model";
    static wchar_t *szMecabDictIndexOption1 = ( wchar_t * )L" -f utf-8 -t utf-8 --model=mecab-ipadic-2.7.0-20070801U8.model";
    // R:\release\work\seed
//    static wchar_t *szMecabCostTrainOption = ( wchar_t * )L" -c 1.0 --old-model=R:/release/work/seed/mecab-ipadic-2.7.0-20070801U8.model R:/release/work/seed/corpus R:/release/work/seed/model";
    static wchar_t *szMecabCostTrainOption = ( wchar_t * )L" -c 1.0 --old-model=mecab-ipadic-2.7.0-20070801U8.model corpus model";
//    static wchar_t *szMecabDictGenOption = ( wchar_t * )L" -o ..\\final -m R:/release/work/seed/model";
    static wchar_t *szMecabDictGenOption = ( wchar_t * )L" -o ..\\final -m model";
    static wchar_t *szMecabDictIndexOption2 = ( wchar_t * )L" -f utf-8 -t utf-8";
//    static wchar_t *szMecabTestGenOption = ( wchar_t * )L" < corpus > OrgCorpus.txt";
//    static wchar_t *szMecabAnalizeOption = ( wchar_t * )L" -d ..\\final OrgCorpus.txt > OrgCorpus.result";
//    static wchar_t *szMecabSysEvalOption = ( wchar_t * )L" OrgCorpus.result corpus";
    soundPlayState  plyStat;
    DWORD stat, exitCode;
    wchar_t szOption[MAX_LINE];
    wchar_t szExecCmd[MAX_LINE];
    wchar_t szExecFolder[MAX_LINE];
    QString errmsg;
    messageBuff msgBuff;
    //QTextBlock block = document()->findBlockByNumber(0);
    //  オプション文字列の1文字目に空白必要

    wcscpy_s( szExecFolder, lengthof( szExecFolder ), ( pprof->mecab.pwszSystemDictionaryBuildFolder )->toStdWString().c_str()/*szSysDicWork*/ );
    wcscat_s( szExecFolder, lengthof( szExecFolder ), L"/final/char.bin" );
//    {   DWORD err = ::GetLastError();   // ここで　エラー0確認
//        msgBuff.enterMessage( QString("permission denied:[%1]").arg( err ));
//    }
#if 0   // 辞書構築の最終ステップでchar.binファイルのロックで書き込めなくなるため、ビルド前にロックされていたらエラーを出そうとしたが
        // エラーを出してもロックが外れない。　仕方ないので、アプリを再起動し、voicevox-coreをロードする前にビルドしてみる
    std::ofstream ofs( szExecFolder, std::ios::binary|std::ios::out );
    if ( ofs.fail())                    // char.binにevryoneのアクセス権　フルコントロールにしてもエラー
    {   DWORD err = ::GetLastError();   // 出力時はエラー32パーミッションエラーになる　オープン時にアクセス権指定できるのか？　mecab-dict-indexのチャイルドプロセスにもアクセス権設定必要？
        msgBuff.enterMessage( QString("permission denied:[%1] [%2]").arg( err ).arg( szExecFolder ));
//        std::ifstream ifs( szExecFolder, std::ios::binary );
//        err = ::GetLastError();         // 入力ではエラーにならない
//        msgBuff.enterMessage( QString("permission denied:[%1] [%2]").arg( err ).arg( szExecFolder ));
        return;             // 3/4コマンド時間かかって終わってからエラー出すなら,開始前に出す
    }
    else
    {   ofs.close();        // final\char.bin　のpermission deniedが最後にでていたが、このチェックを入れたことで出なくなった？ 出だした
    }
#endif
    //cd D:\Mytool\mecab-0.996\work\seed
    //mecab-dict-index -f utf-8 -t utf-8 --model=mecab-ipadic-2.7.0-20070801U8.model
    //mecab-cost-train -c 1.0 --old-model=mecab-ipadic-2.7.0-20070801U8.model corpus model
    //mecab-dict-gen -o ../final -m model
    //cd D:\Mytool\mecab-0.996\work\final
    //mecab-dict-index -f utf-8 -t utf-8
//    QCursor waitcsr( Qt::WaitCursor );
    wcscpy_s( szExecFolder, lengthof( szExecFolder ), ( pprof->mecab.pwszSystemDictionaryBuildFolder )->toStdWString().c_str()/*szSysDicWork*/ );
    wcscat_s( szExecFolder, lengthof( szExecFolder ), L"/seed" );
    //
    //	mecab-dict-index
    //
#if 1
    wcscpy_s( szOption, lengthof( szOption ), szMecabDictIndexOption1 );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-dict-index.exe" );
    qDebug() << QString("command [%1] [%2]").arg(szExecCmd).arg(szOption);
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode ))
        != NOERROR )
    {   errmsg = QString("プロセス起動エラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(stat);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
    if ( exitCode != 0 )
    {   errmsg = QString("プロセスリターンコードエラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(exitCode);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
    //
    //	mecab-cost-train
    //
    wcscpy_s( szOption, lengthof( szOption ), szMecabCostTrainOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-cost-train.exe" );
    qDebug() << QString("command [%1] [%2]").arg(szExecCmd).arg(szOption);
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode ))
        != NOERROR )
    {   errmsg = QString("プロセス起動エラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(stat);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
    if ( exitCode != 0 )
    {   errmsg = QString("プロセスリターンコードエラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(exitCode);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
    //
    //	mecab-dict-gen
    //
    wcscpy_s( szOption, lengthof( szOption ), szMecabDictGenOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-dict-gen.exe" );
    qDebug() << QString("command [%1] [%2]").arg(szExecCmd).arg(szOption);
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode ))
        != NOERROR )
    {   errmsg = QString("プロセス起動エラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(stat);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
    if ( exitCode != 0 )
    {   errmsg = QString("プロセスリターンコードエラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(exitCode);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
#endif
    wcscpy_s( szExecFolder, lengthof( szExecFolder ), pprof->mecab.pwszSystemDictionaryBuildFolder->toStdWString().c_str()/*szSysDicWork*/ );
    wcscat_s( szExecFolder, lengthof( szExecFolder ), L"\\final" );
    //
    //	mecab-dict-index
    //
    wcscpy_s( szOption, lengthof( szOption ), szMecabDictIndexOption2 );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-dict-index.exe" );
    qDebug() << QString("command [%1] [%2]").arg(szExecCmd).arg(szOption);
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode ))
        != NOERROR )
    {   errmsg = QString("プロセス起動エラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(stat);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
    if ( exitCode != 0 )
    {   errmsg = QString("プロセスリターンコードエラー[%1]:%2\nwork\finalフォルダーにdicblds4.batを生成しました。このアプリを終了してからdicblds4.batを実行してください！").arg(QString::fromWCharArray(szExecCmd)).arg(exitCode);
        msgBuff.enterMessage( errmsg );
        errmsg = QString( "voicevox-core　ver1.6でファイルのロックが外れて辞書ビルドが成功する予定です。！" );
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        QString batcmd = QString("cd /d %1\n%2 %3\n").arg(szExecFolder).arg(QString::fromWCharArray(szExecCmd)).arg(QString::fromWCharArray(szOption));
        QString FileName = QString::fromWCharArray( szExecFolder, -1 );
        FileName += "\\dicblds4.bat";
        QFile saveFile( FileName );
        saveFile.open( QIODevice::WriteOnly | QIODevice::Text );
        saveFile.write( batcmd.toStdString().c_str());
        return;
    }
#if 0
//
//	mecab-test-gen		以降追加 21/10/26
//
    wcscpy_s( szOption, lengthof( szOption ), szMecabTestGenOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-test-gen.exe" );
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode ))
        != NOERROR )
    {   errmsg = QString("プロセス起動エラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(stat);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
    if ( exitCode != 0 )
    {   errmsg = QString("プロセスリターンコードエラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(exitCode);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
//
//	mecab
//
    wcscpy_s( szOption, lengthof( szOption ), szMecabAnalizeOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode ))
        != NOERROR )
    {   errmsg = QString("プロセス起動エラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(stat);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
    if ( exitCode != 0 )
    {   errmsg = QString("プロセスリターンコードエラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(exitCode);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
//
//	mecab-system-eval
//
    wcscpy_s( szOption, lengthof( szOption ), szMecabSysEvalOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-system-eval.exe" );
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, &exitCode ))
        != NOERROR )
    {   errmsg = QString("プロセス起動エラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(stat);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
    if ( exitCode != 0 )
    {   errmsg = QString("プロセスリターンコードエラー[%1]:%2").arg(QString::fromWCharArray(szExecCmd)).arg(exitCode);
        msgBuff.enterMessage( errmsg );
        pprof->fgRestart = false;
        return;
    }
#endif
    msgBuff.enterMessage( "build finished!" );
    pprof->fgRestart = false;
}



