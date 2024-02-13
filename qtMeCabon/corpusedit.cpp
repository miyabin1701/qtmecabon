/**
 * @file corpusedit.cpp
 * @brief サブ（インフォメーション）ウインドウのテキストエディタのサブクラス　フォントの拡大縮小、ポップアップメニューハンドラ
 * @author m.kawakami
 * @date 23/10/??
 */

#include "corpusedit.h"
#include <QMenu>
#include <QMessageBox>
#include "profilevalues.h"
#include <io.h>
#include "messagebuff.h"
#include <QTextBlock>
#include "soundplaystate.h"
#include "mecabif.h"
#include "playlinevvoxcore.h"
#include <QFileDialog>
#include <QFont>
#include <QApplication>
//#include <CString>
//#include "mainwindow.h"
#include <QMainWindow>


extern profileValues   *pprof;


/**
 * @fn
 * corpusEditクラスコンストラクタ
 * @brief corpusEditクラスコンストラクタ
 * @param (*parent) 親ウィジェット
 * @return なし
 * @sa
 * @detail エディットの上書きモード設定、フォント（固定幅）設定、ビルドスレッドポインタクリア
 */
corpusEdit::corpusEdit(QWidget *parent)
{   pbuildthred = NULL;
    setOverwriteMode( true );
    QFont qt_font = QFont( "ＭＳ ゴシック" );        // # fontを生成
//    qt_font.setStyleHint( QFont::Helvetica );   // # 使用できない場合はHelveticaにする
    setFont( qt_font );                           // # fontを設定
    if ( pprof->fgRestart != false )
    {   connect( this, SIGNAL(sigbuildSystemDictionary()), this, SLOT(fnbuildSystemDictionary()));
        emit sigbuildSystemDictionary();
//        pprof->fgRestart = false;             // ここでは早すぎなので、実際のビルドが終わった際に行う
    }
}


/**
 * @fn
 * ポップっプメニュー表示前設定
 * @brief ポップっプメニュー表示前設定
 * @param (*event) イベント
 * @return なし
 * @sa
 * @detail 独自ポップアップメニュー、セパレータの追加
 */
void corpusEdit::contextMenuEvent(QContextMenuEvent *event)
{   QAction* point;
    QMenu *menu = createStandardContextMenu();

    sarchCsvDictionary = new QAction( tr( "CSV辞書検索" ), this );
    connect( sarchCsvDictionary, SIGNAL(triggered()), this, SLOT(fnsarchCsvDictionary()));
    connect( menu, SIGNAL(aboutToShow()), this, SLOT(updateMenuItem()));
    QAction *tagJumpEditor = new QAction( tr( "エディター タグジャンプ" ), this );
    connect( tagJumpEditor, SIGNAL(triggered()), this, SLOT(fntagJumpEditor()));
    QAction *appendCorpus = new QAction( tr( "+corpus" ), this );
    connect( appendCorpus, SIGNAL(triggered()), this, SLOT(fnappendCorpus()));
    buildSystemDictionary = new QAction( tr( "システム辞書ビルド" ), this );
    connect( buildSystemDictionary, SIGNAL(triggered()), this, SLOT(fnbuildSystemDictionary()));
    QAction *saveas = new QAction( tr( "名前を付けて保存" ), this );
    connect( saveas, SIGNAL(triggered()), this, SLOT(fnsaveas()));

//    point = menu->actions().at(8);
//    menu->addActions(point, sarchCsvDictionary);
    menu->addAction(sarchCsvDictionary);
    menu->addAction(appendCorpus);
    menu->addAction(tagJumpEditor);
    menu->addAction(buildSystemDictionary);
    menu->addAction(saveas);
    point = menu->actions().at(9);
    menu->insertSeparator(point);
    point = menu->actions().at(13);
    menu->insertSeparator(point);
    point = menu->actions().at(15);
    menu->insertSeparator(point);
//    point = menu->actions().at(11);
//    menu->addActions(point, buildSystemDictionary);
//    menu->addAction(buildSystemDictionary);

    menu->exec( event->globalPos());
    delete menu;
}


/**
 * @fn
 * 単一.csvファイル内の文字列検索
 * @brief 単一.csvファイル内の文字列検索
 * @param (*pszStr) 検索文字列
 * @param (*oszfname) ファイルパス
 * @return なし
 * @sa
 * @detail 一つのファイル内で行頭文字列を検索する。（.csv辞書の語彙は行頭に在るので行頭文字列のみの検索としている）
 * 複数の一致が在る。一致した行をメッセージキューにストア
 */
void corpusEdit::grep( char *pszStr, wchar_t *oszfname )
{	FILE *fp;
    char	szLine[MAX_LINE], szUni[MAX_LINE], *pcr;
    int len, iNoLine;   //, lenLineNo;
    BOOL fgFound;
    QString msg;
    messageBuff msgBuff;

    if (( fp = _wfopen( oszfname, L"rt" )) == NULL )
    {	return;
    }
    len = ( int )strlen( pszStr );  // utf8
    for ( fgFound = FALSE, iNoLine = 1; fgets( szLine, lengthof( szLine ), fp ) != NULL; iNoLine++ )
    {	if ( strncmp( pszStr, szLine, len ) == 0 )
        {	if ( fgFound == FALSE )
            {   msg = QString::fromWCharArray( oszfname );
//                msg += u"\n";
                msgBuff.enterMessage( msg );
                fgFound = TRUE;
            }
            if (( pcr = strchr( szLine, '\n' )) != NULL )
            {   *pcr = '\0';
            }
            sprintf_s( szUni, sizeof( szUni ), "%d %s", iNoLine, szLine );
            msg = QString::fromUtf8( szUni );
            msgBuff.enterMessage( msg );
    }	}
    fclose( fp );
}


/**
 * @fn
 * *.csvファイル内の文字列検索
 * @brief *.csvファイル内の文字列検索
 * @return なし
 * @sa
 * @detail エディット内の選択文字列（部分一致を防ぐため末尾に','をなければ付加）を、全.csvファイルから検索する。
 */
void corpusEdit::fnsarchCsvDictionary()
{   wchar_t szSysDicDir[_MAX_PATH];
    char szSarchStr[MAX_LINE / 2];
    struct _wfinddata64_t c_file;
    intptr_t fh;
    QTextCursor csr = textCursor();     // QTextCursorのインスタンスの取得

    if ( !csr.hasSelection())
    {   QMessageBox::warning(this, tr("MessageBox"), tr("検索単語を選択してください！"));
        return;
    }
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QString selected_text = csr.selectedText(); // 選択されたテキストを取得。
    selected_text += ",";                       // 文字列末尾にカンマ追加
    selected_text.replace( ",,", "," );         // 元々カンマが在ったときのため
    strcpy(( char * )szSarchStr, ( char * )selected_text.toUtf8().toStdString().c_str());
    QString dicBuildDir = *( pprof->mecab.pwszSystemDictionaryBuildFolder );
    wcscpy_s( szSysDicDir, lengthof( szSysDicDir ), ( wchar_t * )dicBuildDir.toStdU16String().c_str());
    wcscat_s( szSysDicDir, lengthof( szSysDicDir ), L"\\seed\\*.csv" );
    if (( fh = _wfindfirst64( szSysDicDir, &c_file )) == -1L )
    {	QMessageBox::warning(this, tr("MessageBox"), tr("*.csv file not found!"));
        QApplication::restoreOverrideCursor();
        return;
    }
    do
    {   wcscpy_s( szSysDicDir, lengthof( szSysDicDir ), ( wchar_t * )dicBuildDir.toStdU16String().c_str());
        wcscat_s( szSysDicDir, lengthof( szSysDicDir ), L"\\seed\\" );
        wcscat_s( szSysDicDir, lengthof( szSysDicDir ), c_file.name );
        grep( szSarchStr, szSysDicDir );
    } while ( _wfindnext64( fh, &c_file ) != -1 );
    _findclose( fh );
    QApplication::restoreOverrideCursor();
}


/**
 * @fn
 * *.csvの文字列検索で得られたメッセージから、該当ファイルの該当行にタグジャンプする
 * @brief *.csvの文字列検索で得られたメッセージから、該当ファイルの該当行にタグジャンプする
 * @return なし
 * @sa
 * @detail エディット内に表示された検索行から、設定エディタでその部分を開けるようにタグジャンプする
 */
void corpusEdit::fntagJumpEditor()
{	int LineIdx;
    INT_PTR stat;
    bool    ok;
    QString line, path, option;
    QTextCursor csr = textCursor();     // QTextCursorのインスタンスの取得
                                        // カーソル行データ取得
    QTextBlock block = document()->findBlockByNumber( LineIdx = csr.blockNumber());
//    qDebug() << QString("fntagJumpEditor::%1").arg(LineIdx);
    if ( !block.isValid())              //ブロックが無効なら何もしない
    {   return;
    }
    line = block.text();                // 選択された行を取得。
//    qDebug() << QString("block.text()::%1").arg(line);
    line = line.left( line.indexOf( u' ' ));
//    qDebug() << QString("block.text()::%1").arg(line);
/*    QString line0 = "26325";
    QString line1 = "26325 ";
    QString line2 = "26325 欲しい";       // NG
    long num0 = line0.toULong( &ok );   // 数字列のみでないとエラーらしいｗ
    qDebug() << QString("%3.toULong( %1 )::%2").arg(ok).arg(num0).arg(line0);
    long num1 = line1.toULong( &ok );
    qDebug() << QString("%3.toULong( %1 )::%2").arg(ok).arg(num1).arg(line1);
    long num2 = line2.toULong( &ok );
    qDebug() << QString("%3.toULong( %1 )::%2").arg(ok).arg(num2).arg(line2);
*/
//    long num = line.toULong( &ok );     // 数字列のみでないとエラーらしいｗ
//    qDebug() << QString("line.toULong( %1 )::%2").arg(ok).arg(num);
    line.setNum( line.toULong( &ok ));    // 数字列のみでないとエラーらしいｗ
    if ( ok != true )
    {   QMessageBox::warning(this, tr("MessageBox"), tr("行番号が取得できませんでした。！"));
        return;                         // 99999 単語,,,,,,,,,,
    }
    for ( LineIdx--; LineIdx >= 0; LineIdx-- )
    {   block = document()->findBlockByNumber( LineIdx );
        if ( !block.isValid())          //ブロックが無効なら何もしない
        {   return;
        }
        path = block.text();            // 選択された行を取得。
        if (( path[0] < u'0' ) || ( u'9' < path[0] ))
        {	break;
    }	}
    if ( LineIdx < 0 )
    {	return;
    }
    path = path.trimmed();              // file path
    option = *( pprof->other.pwszEdtorTagjumpCommand );
    option.replace( QString( "%l" ), line );
    option.replace( QString( "%f" ), path );
    //	wsprintf( option, L"-n%d %s", LineNo, selLine );
    stat = ( INT_PTR )ShellExecute(
        NULL,                                                   // 親ウィンドウのハンドル
        L"open",                                                // 操作
        pprof->other.pwszEdtorPath->toStdWString().c_str(),		// 操作対象のファイル
        option.toStdWString().c_str(),							// 操作のパラメータ
        NULL,                                                   // 既定のディレクトリ
        SW_SHOW	);                                              // 表示状態
    if ( stat < 32 )
    {    QMessageBox::warning(this, tr("MessageBox"), tr("エディターを起動できませんでした。！"));
    }
}


/**
 * @fn
 * 行フォーマットが、corpusフォーマットかチェックする
 * @brief 行フォーマットが、corpusフォーマットかチェックする
 * @return なし
 * @sa
 * @detail 行フォーマットが以下の条件かチェックする
 * "EOS\n"ならtrue
 * 空行ならfalse
 * 先頭が空白ならfalse
 * 最初の区切り文字がTABでなければ,TABより','が先に現れたらfalse
 * その後','が10個でなければfalse
 */
bool corpusEdit::formatCheckLine( QString lineText )
{	int		idx, idx2;

    if ( lineText == "\n" )			// 空行なら
    {	return ( false );
    }
    if ( lineText == "EOS\n" )		// EOSの行なら
    {	return ( true );
    }
    if ( lineText[0] == ' ' )		// 先頭が空白なら
    {	return ( false );
    }
    if (( idx = lineText.indexOf( "\t" )) < 0 )
    {	return ( false );           // TABがなければ
    }
    idx2 = lineText.indexOf( "," );
    if ( idx > idx2 )				// ','がTABより先に出てきたら
    {	return ( false );
    }
    idx = lineText.count( "\t" );
    if ( idx > 1 )					// TABが複数現れたら
    {	return ( false );
    }
    idx = lineText.count( "," );	// ','が１０個か？
    return (( idx >= 10 )? true: false );
}


/**
 * @fn
 * エディット内で選択されている部分が、corpusフォーマットかチェックする
 * @brief エディット内で選択されている部分が、corpusフォーマットかチェックする
 * @return なし
 * @sa
 * @detail エディット内で選択されている部分を行単位に分割し、行フォーマットチェックを呼ぶ
 */
bool corpusEdit::formatCheck( QString selected_text )
{	QString lineText;
    int		idx, idx2;

    for ( idx = 0; idx < selected_text.length(); idx = idx2 + 1 )
    {	idx2 = selected_text.indexOf( "\n", idx );
        lineText = selected_text.mid( idx, idx2 - idx + 1 );
        if ( !formatCheckLine( lineText ))
        {	return ( false );
    }   }
    return ( lineText == "EOS\n" );	// 最終行がEOSか？
}


/**
 * @fn
 * エディット内で選択されている部分を、corpusファイルにアペンドする
 * @brief エディット内で選択されている部分を、corpusファイルにアペンドする
 * @return なし
 * @sa
 * @detail 辞書ビルド時に使用するcorpusファイルをアップデートする。注意！正しくcorpusデータを選択する必要が在る。
 */
void corpusEdit::fnappendCorpus()
{   QTextCursor csr = textCursor();                 // QTextCursorのインスタンスの取得

    if ( !csr.hasSelection())
    {   QMessageBox::warning(this, tr("appendCorpus"), tr("Corpusを選択してください！"));
        return;
    }
    QString selected_text = csr.selectedText();		// 選択されたテキストを取得。
    selected_text.replace( QChar(0x2029), '\n' );
    if ( !formatCheck( selected_text ))				// 形式が違う？
    {	if ( QMessageBox::question( this, tr("appendCorpus"),
                tr("コーパスを正しく選択していますか？\nコーパスファイルが破損する可能性が在りますが続行しますか？"),
                        QMessageBox::Cancel | QMessageBox::Ok ) == QMessageBox::Cancel )
        {	return;
    }   }
    QString FileName = pprof->getCorpusPath();
    if( FileName.isEmpty())
    {  return;
    }
    QFile saveFile( FileName );
    saveFile.open( QIODevice::Append | QIODevice::Text );
    saveFile.write( selected_text.toUtf8().toStdString().c_str());
}


/**
 * @fn
 * 辞書ビルド時、多重にビルドメニューを選択出来なくするためにビルドメニューを無効化する
 * @brief 辞書ビルド時、多重にビルドメニューを選択出来なくするためにビルドメニューを無効化する
 * @return なし
 * @sa
 * @detail 辞書ビルド時リエントを防ぐため、ビルドメニューを無効化する
 */
void corpusEdit::updateMenuItem()
{   QTextCursor csr = textCursor();     // QTextCursorのインスタンスの取得。

    sarchCsvDictionary->setEnabled( csr.hasSelection());
    while ( true )
    {   if ( pbuildthred != NULL )
        {   if ( pbuildthred->isRunning())
            {   buildSystemDictionary->setEnabled( false );
                break;
            }
            delete pbuildthred;
            pbuildthred = NULL;
        }
        buildSystemDictionary->setEnabled( true );
        break;
    }
}


/**
 * @fn
 * システム辞書のビルドを行う
 * @brief システム辞書のビルドを行う
 * @return なし
 * @sa
 * @detail システム辞書のビルドを行う。ここで直接ビルドするとチャイルドプロセスの出力キャプチャ文字列が
 * 表示されないため、別のビルドスレッドで行うようにした。このメッセージを抜けないと、タイマーメッセージで表示
 * している文字列が処理できないらしい
 */
void corpusEdit::fnbuildSystemDictionary()
{    soundPlayState  plyStat;

    if ( plyStat.getPlayState() != PLAYSTATE::STOP )
    {   QMessageBox::warning(this, tr("MessageBox"), tr("発声をSTOPしてから実行してください！"));
        return;
    }
    if ( QMessageBox::question(this, tr("MessageBox"),
            tr("辞書をビルドしますか？\n辞書ファイル更新のため VOICEVOX(Open-JTalk), Mecab を一旦終了します。"),
//            tr("辞書をビルドしますか？\n辞書ファイル更新のためアプリケーションを再起動します。\n（辞書ファイルのロックを解除するため）"),
                QMessageBox::Cancel | QMessageBox::Ok ) == QMessageBox::Cancel )
    {   return;
    }
#if 0  // 辞書構築の最終(4/4)ステップでchar.binファイルのロックで書き込めなくなるため、ビルド前にロックされていたらエラーを出そうとした
    {   FILE *fp;   // for debug

        if (( fp = fopen( "R:\\release\\work\\final\\char.bin", "r+" )) == NULL )
        {   DWORD err = ::GetLastError();       // 更新オープン出来なければ"char.bin"がロックされているのでメッセージを出して終了
//            qDebug() << QString("permission denied:[%1] R:\\release\\work\\final\\char.bin").arg(err);
            QMessageBox::warning(this, tr("MessageBox"), tr("permission denied:[%1] R:\\release\\work\\final\\char.bin").arg(err));
            return;
        }
        fclose( fp );
    }
#endif
    mecabif::CloseMecabDll();
    PlayLineVvoxCore::unLoadVvoxCoreDll();
//    emit sigreStart();    // dllのみのアンロードではchar.binのロックが解除できなかったため
//                             アプリを再起動し、voicevoxのロード前に辞書を構築してみる
    if ( pbuildthred == NULL )
    {   pbuildthred = new class buildSysDic;
//        QApplication::setOverrideCursor( Qt::WaitCursor );
        pbuildthred->start( QThread::NormalPriority );
//        QApplication::restoreOverrideCursor();    // ウェイトカーソルを出したいが、この関数はビルドスレッドを起動後すぐ抜けるので意味ない
    }
#if 0
//
//  以下　appendPlainTextでエディターへ文字列を追加しているが、表示が更新されないので上記別スレッドでキューへ
//    追加することを試す。この関数はすぐ抜けて、メッセージ処理可能にする　ダイヤログクラスでタイマースロットを実行出来るように
//
#endif
#if 0
//cd D:\Mytool\mecab-0.996\work\seed
//mecab-dict-index -f utf-8 -t utf-8 --model=mecab-ipadic-2.7.0-20070801U8.model
//mecab-cost-train -c 1.0 --old-model=mecab-ipadic-2.7.0-20070801U8.model corpus model
//mecab-dict-gen -o ../final -m model
//cd D:\Mytool\mecab-0.996\work\final
//mecab-dict-index -f utf-8 -t utf-8
    QCursor waitcsr( Qt::WaitCursor );
    wcscpy_s( szExecFolder, lengthof( szExecFolder ), ( pprof->mecab.pwszSystemDictionaryBuildFolder )->toStdWString().c_str()/*szSysDicWork*/ );
    wcscat_s( szExecFolder, lengthof( szExecFolder ), L"\\seed" );
//
//	mecab-dict-index
//
    wcscpy_s( szOption, lengthof( szOption ), szMecabDictIndexOption1 );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-dict-index.exe" );
#if 0
    qDebug() << QString("command [%1] [%2]").arg(szExecCmd).arg(szOption);
#else
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, this, &exitCode ))
        != NOERROR )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセス起動時エラー:%1").arg(stat));
        return;
    }
    if ( exitCode != 0 )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセスリターンコードエラー:%1").arg(exitCode));
        return;
    }
#endif
//
//	mecab-cost-train
//
    wcscpy_s( szOption, lengthof( szOption ), szMecabCostTrainOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-cost-train.exe" );
#if 0
    qDebug() << QString("command [%1] [%2]").arg(szExecCmd).arg(szOption);
#else
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, this, &exitCode ))
        != NOERROR )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセス起動時エラー:%1").arg(stat));
        return;
    }
    if ( exitCode != 0 )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセスリターンコードエラー:%1").arg(exitCode));
        return;
    }
#endif
//
//	mecab-dict-gen
//
    wcscpy_s( szOption, lengthof( szOption ), szMecabDictGenOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-dict-gen.exe" );
#if 0
    qDebug() << QString("command [%1] [%2]").arg(szExecCmd).arg(szOption);
#else
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, this, &exitCode ))
        != NOERROR )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセス起動時エラー:%1").arg(stat));
        return;
    }
    if ( exitCode != 0 )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセスリターンコードエラー:%1").arg(exitCode));
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
#if 0
    qDebug() << QString("command [%1] [%2]").arg(szExecCmd).arg(szOption);
#else
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, this, &exitCode ))
        != NOERROR )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセス起動時エラー:%1").arg(stat));
        return;
    }
    if ( exitCode != 0 )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセスリターンコードエラー:%1").arg(exitCode));
        return;
    }
#endif
#if 0
//
//	mecab-test-gen		以降追加 21/10/26
//
    wcscpy_s( szOption, lengthof( szOption ), szMecabTestGenOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-test-gen.exe" );
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, this, &exitCode ))
        != NOERROR )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセス起動時エラー:%1").arg(stat));
        return;
    }
    if ( exitCode != 0 )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセスリターンコードエラー:%1").arg(exitCode));
        return;
    }
//
//	mecab
//
    wcscpy_s( szOption, lengthof( szOption ), szMecabAnalizeOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, this, &exitCode ))
        != NOERROR )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセス起動時エラー:%1").arg(stat));
        return;
    }
    if ( exitCode != 0 )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセスリターンコードエラー:%1").arg(exitCode));
        return;
    }
//
//	mecab-system-eval
//
    wcscpy_s( szOption, lengthof( szOption ), szMecabSysEvalOption );
    wcscpy_s( szExecCmd, lengthof( szExecCmd ), pprof->mecab.pwszMecabPath->toStdWString().c_str() /*szMeCabExe*/ );
    exchgFileName( szExecCmd, lengthof( szExecCmd ), szExecCmd, ( wchar_t * )L"mecab-system-eval.exe" );
    if (( stat = execProcess( szExecCmd, szOption, szExecFolder, this, &exitCode ))
        != NOERROR )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセス起動時エラー:%1").arg(stat));
        return;
    }
    if ( exitCode != 0 )
    {	QMessageBox::critical(this, tr("MessageBox"), QString("プロセスリターンコードエラー:%1").arg(exitCode));
        return;
    }
#endif
#endif
}


/**
 * @fn
 * 名前を付けて保存メニュー
 * @brief 名前を付けて保存メニュー
 * @return なし
 * @sa
 * @detail エディットには、辞書未登録語彙が表示されるが、これらを一括して.csvファイルを編集できるように保存できるようにした。
 * ただし、エディットの最大行数（現在５００）指定しているので注意
 */
void corpusEdit::fnsaveas()
{   QString SelFilter = "";
    QString FileName = QFileDialog::getSaveFileName(this, tr("名前を付けて保存"), ".",
                                                    tr("txt document(*.txt)"),
                                                    &SelFilter, QFileDialog::DontUseCustomDirectoryIcons);
    if( FileName.isEmpty())
    {  return;
    }
    QFile saveFile( FileName );
    saveFile.open(QIODevice::WriteOnly | QIODevice::Text);
    saveFile.write( toPlainText().toUtf8().toStdString().c_str());
}


/**
 * @fn
 * マウスホイールイベントハンドラ
 * @brief マウスホイールイベントハンドラ
 * @param (*event) イベントデータ
 * @return なし
 * @sa
 * @detail ctrl+ホイール方向でエディットボックスの文字サイズの拡大、縮小を行う
 */
void corpusEdit::wheelEvent(QWheelEvent *pEevent)
{
    // http://mochiuwiki.e2.valueserver.jp/index.php?title=Qt%E3%81%AE%E3%82%B3%E3%83%B3%E3%83%88%E3%83%AD%E3%83%BC%E3%83%AB_-_%E3%83%9E%E3%82%A6%E3%82%B9
    // マウスホイールの前方 : 正の値
    // マウスホイールの後方 : 負の値
    // QWheelEventクラスのdeltaメソッドの値 : マウスホイールを回した角度の8倍の値
    // 標準的なマウスでは1ステップ15度のため、8 * 15 = 120で除算する
    //    double dSteps = (double)pEevent->angleDelta() / 120.0;
    // VK_SHIFT 	0x10	Shift キー
    // VK_CONTROL	0x11	Ctrl キー
    // VK_MENU  	0x12	ALT キー

    double dSteps = ( int )pEevent->angleDelta().y();
    ushort  ctrlKeyState = 0;
#ifdef Q_OS_WIN
    ctrlKeyState = GetKeyState( VK_CONTROL ) & 0x80;
#endif
    if ( ctrlKeyState )
    {   if ( dSteps > 0 )
        {   zoomIn( 1 );
            pprof->oper.idisplayMagnify++;
        }
        else
        {   zoomOut( 1 );
            pprof->oper.idisplayMagnify--;
    }   }
    else
    {   QPlainTextEdit::wheelEvent( pEevent );
    }
}

