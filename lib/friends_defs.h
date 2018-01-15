/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_DEFS_H
#define FRIENDS_DEFS_H

#include <stddef.h>

#include "friends_config.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

/* Core Data types */
struct friendsMemory;
typedef struct friendsMemory friendsMemory;

typedef ptrdiff_t friendsSize;

/* Data Types */
struct friendsPark;
struct friendsData;
struct friendsAtomData;
struct friendsVariableData;
struct friendsPropositionData;
struct friendsMatchData;

struct friendsDataList;
struct friendsDataSet;
struct friendsDataSetNode;

struct friendsParser;
struct friendsLineColumn;

struct friendsDataFunctions;

struct friendsStringList;

typedef struct friendsPark            friendsPark;
typedef struct friendsData            friendsData;
typedef struct friendsAtomData        friendsAtomData;
typedef struct friendsVariableData    friendsVariableData;
typedef struct friendsPropositionData friendsPropositionData;
typedef struct friendsMatchData       friendsMatchData;
typedef struct friendsDataList        friendsDataList;
typedef struct friendsDataSet         friendsDataSet;
typedef struct friendsDataSetNode     friendsDataSetNode;

typedef struct friendsToken           friendsToken;
typedef struct friendsParser          friendsParser;
typedef struct friendsLineColumn      friendsLineColumn;

typedef struct friendsDataFunctions   friendsDataFunctions;
typedef struct friendsStringList      friendsStringList;

/**
 * @typedef friendsPark
 * @brief 必要なデータを保存するのです。
 */
/**
 * @typedef friendsObject
 * @brief データを保存するのです。
 */
/**
 * @typedef friendsAtomData
 * @brief アトムデータを保存するのです。
 */
/**
 * @typedef friendsVariableData
 * @brief 変数データを保存するのです。
 */
/**
 * @typedef friendsPropositionData
 * @brief 命題データを保存するのです。
 */
/**
 * @typedef friendsPark
 * @brief 全体を取りまとめるパークを保存するのです。
 */
/**
 * @typedef friendsVariableMatch
 * @brief マッチングした情報を保存するのです。
 */
/**
 * @typedef friendsParser
 * @brief Friends 言語を読むときに使う情報を保存するのです。
 */
/**
 * @typedef friendsLineColumn
 * @brief Friends 言語を読むときに使う行と列の情報を保存するのです。
 */
/**
 * @typedef friendsDataList
 * @brief データの双方向リンクを実現するのです。
 */
/**
 * @typedef friendsDataSet
 * @brief データの連想配列（のようなもの）を実現するのです。
 */
/**
 * @typedef friendsToken
 * @brief Frineds 言語を分断したトークンのリストを保存するのです。
 */
/**
 * @typedef friendsStringList
 * @brief 文字列のリストなのです。
 */

/**
 * @typedef friendsHash
 * @brief データを識別するための値を保存する型なのです。
 */
#if defined(HAVE_UINT32_T)
typedef uint32_t friendsHash;
#else
typedef unsigned int friendsHash;
#endif

/**
 * @typedef friendsChar
 * @brief Friends 言語で使う文字の型なのです。
 *
 * UTF-8 モードなら 1 バイト、UTF-16 モードなら 2 バイトになるのです。
 *
 * もし、2バイト整数が使えない時は、UTF-8 モードを使うのです。
 *
 * 普通は、UTF-16 モードの方が速いのです。メモリの使用量は、UTF-8 モー
 * ドの方が多くなるのです。
 */
/**
 * @macro FRIENDS_MAX_CHAR
 * @brief 文字1文字あたりに使う、`friendsChar` の配列要素数なのです。
 */
/* UTF-8 */
#if defined(FRIENDS_USE_UTF8_INTERNAL)
#define FRIENDS_MAX_CHAR 4
typedef unsigned char friendsChar;

#else

/* UTF-16 */
#define FRIENDS_MAX_CHAR 2

#if defined(HAVE_UINT16_T)
typedef uint16_t friendsChar;
#else
#error "Use UTF-8 mode"
#endif
#endif

/**
 * @brief データの型を表すのです。
 */
enum friendsType
{
  friendsInvalidType,  /*!< 何もデータが入っていないのです */
  friendsAtom,         /*!< アトム */
  friendsVariable,     /*!< 変数 */
  friendsProposition,  /*!< 命題 */

  friendsListType,  /*!< リスト */
  friendsSetType,   /*!< セット */

  friendsMatch, /*!< 一致情報 */

  friendsLastType, /*!< 最後の型 */
};
typedef enum friendsType friendsType;

/**
 * @brief アトムの型を表すのです。
 */
enum friendsAtomType
{
  friendsTextAtom,    /*!< 文字列 */
  friendsNumericAtom, /*!< 数値 */
  friendsNextAtom,    /*!< 「次」 */

  friendsLastAtomType, /*!< 最後の型 */
};
typedef enum friendsAtomType friendsAtomType;

/**
 * @brief 命題データの扱いを表すのです。
 */
enum friendsPropositionMode
{
  friendsPropositionNormal,  /*!< ふつうの */
  friendsPropositionStop,    /*!< たーのしー！ */
  friendsPropositionQuery,   /*!< なんだっけ？ */
};
typedef enum friendsPropositionMode friendsPropositionMode;

/**
 * @brief 真偽値を表すのです。
 *
 * 比較する時は列挙値と比較するのです。
 */
enum friendsBool
{
  friendsFalse = 0,
  friendsTrue  = 1,
};
typedef int friendsBool;

struct friendsFile;
typedef struct friendsFile friendsFile;

/**
 * @brief Friends 言語のエラーを表すのです。
 */
enum friendsError
{
  friendsNoError = 0,

  /* Client Notifications */
  friendsErrorBye,           /*!< さようなら */

  /* Friends System */
  friendsErrorInvalidType,  /*!< データの型が正しくない（値の場合は INVAL） */
  friendsErrorValidType,
                    /*!< 有効なデータ型になっている（未設定のデータが必要） */
  friendsErrorInvalidPark,   /*!< 異なるパークに所属している */

  friendsErrorListIndex,     /*!< リストのインデックスが範囲外 */
  friendsErrorTooNested,     /*!< ネストが深すぎる */
  friendsErrorSetDuplicated, /*!< セットの要素が重複している */

  friendsErrorNoArgument,    /*!< 命題の引数がない */
  friendsErrorSyntax,        /*!< 文法エラー */

  friendsErrorUnderscore,    /*!< 下線のあとの文字が不正 */

  /* POSIX System Call Errors */
  friendsError2BIG,          /*!< 引き数リストが長過ぎる */
  friendsErrorACCES,         /*!< 許可がない */
  friendsErrorADDRINUSE,     /*!< アドレスがすでに使用されている */
  friendsErrorADDRNOTAVAIL,  /*!< アドレスが使用できない */
  friendsErrorAFNOSUPPORT,   /*!< アドレスファミリーがサポートされていない */
  friendsErrorAGAIN,         /*!< リソースが一時的に利用不可 */
  friendsErrorALREADY,       /*!< 接続が既に処理中である */
  friendsErrorBADE,          /*!< 不正なやり取り (exchange) である */
  friendsErrorBADF,          /*!< ファイルディスクリプターが不正である */
  friendsErrorBADFD,        /*!< ファイルディスクリプターが不正な状態である */
  friendsErrorBADMSG,        /*!< メッセージが不正である */
  friendsErrorBADR,          /*!< 不正なリクエストディスクリプター */
  friendsErrorBADRQC,        /*!< 不正なリクエストコード */
  friendsErrorBADSLT,        /*!< 不正なスロット */
  friendsErrorBUSY,          /*!< リソースが使用中である */
  friendsErrorCANCELED,      /*!< 操作がキャンセルされた */
  friendsErrorCHILD,         /*!< 子プロセスが無い */
  friendsErrorCHRNG,         /*!< チャンネル番号が範囲外である */
  friendsErrorCOMM,          /*!< 送信時に通信エラーが発生した */
  friendsErrorCONNABORTED,   /*!< 接続が中止された */
  friendsErrorCONNREFUSED,   /*!< 接続が拒否された */
  friendsErrorCONNRESET,     /*!< 接続がリセットされた */
  friendsErrorDEADLK,        /*!< リソースのデッドロックを回避した */
  friendsErrorDEADLOCK,      /*!< EDEADLK の同義語 */
  friendsErrorDESTADDRREQ,   /*!< 宛先アドレスが必要である */
  friendsErrorDOM,      /*!< 数学関数で引き数が領域外である (out of domain) */
  friendsErrorDQUOT,         /*!< ディスククォータ (quota) を超過した */
  friendsErrorEXIST,         /*!< ファイルが存在する */
  friendsErrorFAULT,         /*!< アドレスが不正である */
  friendsErrorFBIG,          /*!< ファイルが大き過ぎる */
  friendsErrorHOSTDOWN,      /*!< ホストがダウンしている */
  friendsErrorHOSTUNREACH,   /*!< ホストに到達不能である */
  friendsErrorIDRM,          /*!< 識別子が削除された */
  friendsErrorILSEQ,         /*!< 不正なバイト列 */
  friendsErrorINPROGRESS,    /*!< 操作が実行中である */
  friendsErrorINTR,          /*!< 関数呼び出しが割り込まれた */
  friendsErrorINVAL,         /*!< 引数が無効である */
  friendsErrorIO,            /*!< 入出力エラー */
  friendsErrorISCONN,        /*!< ソケットが接続されている */
  friendsErrorISDIR,         /*!< ディレクトリである */
  friendsErrorISNAM,         /*!< 名前付きのファイルである */
  friendsErrorL2HLT,         /*!< 停止 (レベル 2) */
  friendsErrorL2NSYNC,       /*!< 同期できていない (レベル 2) */
  friendsErrorL3HLT,         /*!< 停止 (レベル 3) */
  friendsErrorL3RST,         /*!< 停止 (レベル 3) */
  friendsErrorLIBACC,       /*!< 必要な共有ライブラリにアクセスできなかった */
  friendsErrorLIBBAD,       /*!< 壊れた共有ライブラリにアクセスしようとした */
  friendsErrorLIBMAX,       /*!< リンクしようとした共有ライブラリが多過ぎる */
  friendsErrorLIBSCN,        /*!< a.out のライブラリセクションが壊れている */
  friendsErrorLIBEXEC,       /*!< 共有ライブラリを直接実行できなかった */
  friendsErrorLOOP,          /*!< シンボリックリンクの回数が多過ぎる */
  friendsErrorMFILE,         /*!< オープンしているファイルが多過ぎる */
  friendsErrorMLINK,         /*!< リンクが多過ぎる */
  friendsErrorMSGSIZE,       /*!< メッセージが長過ぎる */
  friendsErrorMULTIHOP,      /*!< マルチホップ (multihop) を試みた */
  friendsErrorNAMETOOLONG,   /*!< ファイル名が長過ぎる */
  friendsErrorNETDOWN,       /*!< ネットワークが不通である */
  friendsErrorNETRESET,      /*!< 接続がネットワーク側から中止された */
  friendsErrorNETUNREACH,    /*!< ネットワークが到達不能である */
  friendsErrorNFILE,
                    /*!< システム全体でオープンされているファイルが多過ぎる */
  friendsErrorNOBUFS,        /*!< 使用可能なバッファー空間がない */
  friendsErrorNODATA,
      /*!< ストリームの読み出しキューの先頭に読み出し可能なメッセージがない */
  friendsErrorNODEV,         /*!< そのようなデバイスは無い */
  friendsErrorNOENT,         /*!< そのようなファイルやディレクトリは無い */
  friendsErrorNOEXEC,        /*!< 実行ファイル形式のエラー */
  friendsErrorNOKEY,         /*!< 要求された鍵が利用できない */
  friendsErrorNOLCK,         /*!< 利用できるロックが無い */
  friendsErrorNOLINK,        /*!< リンクが切れている */
  friendsErrorNOMEM,         /*!< 十分な空きメモリー領域が無い */
  friendsErrorNOMSG,         /*!< 要求された型のメッセージが存在しない */
  friendsErrorNONET,         /*!< マシンがネットワーク上にない */
  friendsErrorNOPROTOOPT,    /*!< 指定されたプロトコルが利用できない */
  friendsErrorNOSPC,         /*!< デバイスに空き領域が無い */
  friendsErrorNOSR,          /*!< 指定されたストリームリソースが存在しない */
  friendsErrorNOSTR,         /*!< ストリームではない */
  friendsErrorNOSYS,         /*!< 関数が実装されていない */
  friendsErrorNOTBLK,        /*!< ブロックデバイスが必要である */
  friendsErrorNOTCONN,       /*!< ソケットが接続されていない */
  friendsErrorNOTDIR,        /*!< ディレクトリではない */
  friendsErrorNOTEMPTY,      /*!< ディレクトリが空ではない */
  friendsErrorNOTSOCK,       /*!< ソケットではない */
  friendsErrorNOTSUP,        /*!< 操作がサポートされていない */
  friendsErrorNOTTY,         /*!< I/O 制御操作が適切でない */
  friendsErrorNOTUNIQ,       /*!< 名前がネットワークで一意ではない */
  friendsErrorNXIO,          /*!< そのようなデバイスやアドレスはない */
  friendsErrorOPNOTSUPP,     /*!< ソケットでサポートしていない操作である */

  friendsErrorOVERFLOW, /*!< 指定されたデータ型に格納するには値が大き過ぎる */
  friendsErrorPERM,          /*!< 操作が許可されていない */
  friendsErrorPFNOSUPPORT,
                        /*!< サポートされていないプロトコルファミリーである */
  friendsErrorPIPE,          /*!< パイプが壊れている */
  friendsErrorPROTO,         /*!< プロトコルエラー */
  friendsErrorPROTONOSUPPORT,   /*!< プロトコルがサポートされていない */
  friendsErrorPROTOTYPE,  /*!< ソケットに指定できないプロトコルタイプである */
  friendsErrorRANGE,         /*!< 結果が大き過ぎる */
  friendsErrorREMCHG,        /*!< リモートアドレスが変わった */
  friendsErrorREMOTE,        /*!< オブジェクトがリモートにある */
  friendsErrorREMOTEIO,      /*!< リモート I/O エラー */
  friendsErrorRESTART,  /*!< システムコールが中断され再スタートが必要である */
  friendsErrorROFS,          /*!< 読み出し専用のファイルシステムである */
  friendsErrorSHUTDOWN,     /*!< 通信相手がシャットダウンされて送信できない */
  friendsErrorSPIPE,         /*!< 無効なシーク */
  friendsErrorSOCKTNOSUPPORT,   /*!< サポートされていないソケット種別である */
  friendsErrorSRCH,          /*!< そのようなプロセスは無い */
  friendsErrorSTALE,         /*!< ファイルハンドルが古い状態になっている */

  friendsErrorSTRPIPE,       /*!< ストリームパイプエラー */
  friendsErrorTIME,          /*!< 時間が経過した */

  friendsErrorTIMEDOUT,      /*!< 操作がタイムアウトした */
  friendsErrorTXTBSY,        /*!< テキストファイルが使用中である */
  friendsErrorUCLEAN,        /*!< Structure needs cleaning */
  friendsErrorUNATCH, /*!< プロトコルのドライバが付与 (attach) されていない */
  friendsErrorUSERS,         /*!< ユーザー数が多過ぎる */
  friendsErrorWOULDBLOCK,    /*!< 操作がブロックされる見込みである */
  friendsErrorXDEV,          /*!< 不適切なリンク */
  friendsErrorXFULL,         /*!< 変換テーブルが一杯である */

  /* Other */
  friendsErrorUnknown,       /*!< 不明なエラー */
  friendsLastError,          /*!< エラーリストの最後 */
};
typedef enum friendsError friendsError;

enum friendsErrorLevel
{
  friendsErrorLevelDebug = 0,
  friendsErrorLevelInfo,
  friendsErrorLevelWarning,
  friendsErrorLevelError,
  friendsErrorLevelFatal,
  friendsErrorLevelAssert,
};
typedef enum friendsErrorLevel friendsErrorLevel;

enum friendsFixedHashValues {
  friendsHashValueNextAtom = 0x01,
};

/**
 * @brief フレンズのアトムとかの比較結果なのです。
 *
 * `friendsDataNotEqual` は「比較はできるけど、大小は決められず等しく
 * ない」を表すのです。対して、上記の「等しくない」は「比較できるかと
 * か型があってるかとか違っているとかに関わらず、とにかく同じデータで
 * はない」になるのです。
 */
enum friendsDataCompareResult
{
  friendsDataEqual   = 0x01, /*!< a == b */
  friendsDataGreater = 0x02, /*!< a >  b */
  friendsDataLess    = 0x04, /*!< a <  b */
  friendsDataNotComparable = 0x08, /*!< 比較できない */
  friendsDataNotEqual = friendsDataGreater | friendsDataLess,
  /*!< a != b */
  friendsDataDifferentType = 0x10, /*!< typeof(a) != typeof(b) */

  friendsDataCompareSpecialsMask = 0xff, /*!< 特殊な比較をマスクするための値 */
  friendsDataInAnotherPark = 0x100, /*!< park(a) != park(b) */
  friendsDataSetEqual = 0x200, /*!< 違うデータではあるが、friendsSet の中では
                                   等しいものとして扱う */
  friendsDataObjectEqual = 0x400, /*!< 同じオブジェクトを指している */
  friendsDataNullObject = 0x800,  /*!< 一方が NULL オブジェクトである */

  friendsDataLastCompareResult = 0xffff,
};
typedef enum friendsDataCompareResult friendsDataCompareResult;

/**
 * @brief メモリを解放するための関数の形式なのです。
 */
typedef void friendsPointerDeleter(void *);

/**
 * @brief データを削除するときの関数の形式なのです。
 *
 * `friendsObjectDeleter` では、渡されたメモリ空間を解放してはいけないのです。
 */
typedef void *friendsObjectDeleter(void *);

/**
 * @brief データの文字列表現を作成する関数の形式なのです。
 */
typedef friendsError friendsTextCreator(friendsChar **, void *);

/**
 * @brief データの文字列表現を削除する関数の形式なのです。
 */
typedef void friendsTextDeleter(friendsChar *text, void *);

/**
 * @brief データの比較関数の形式なのです。
 */
typedef friendsDataCompareResult
friendsDataCompareFunc(void *, void *);

/**
 * @brief 普通の文字を Friends の文字に変換するエンコーダ関数の形式なのです。
 */
typedef int friendsCharEncoder(friendsChar **, const char *, friendsError *);

/**
 * @brief 普通の文字を Friends の文字に変換するエンコーダ関数の形式なのです。
 */
typedef int friendsOneCharEncoder(friendsChar [FRIENDS_MAX_CHAR],
                                  const char **, friendsError *);

/**
 * @brief Friends の文字を普通の文字に変換するデコーダ関数の形式なのです。
 */
typedef int friendsCharDecoder(char **, const friendsChar *, friendsError *);

/**
 * @brief 普通の文字での1文字のバイト数の最大値を返す関数の形式なのです。
 */
typedef int friendsEncodingMaxChar(void);

/**
 * @typedef friendsCodeSet
 * @brief Friends 内で使う文字コードの変換関数を定義するための型なので
 *        す。
 */
struct friendsCodeSet {
  friendsOneCharEncoder *one_enc; /*!< 1文字エンコーダなのです。 */
  friendsCharEncoder *enc; /*!< エンコーダなのです。 */
  friendsCharDecoder *dec; /*!< デコーダなのです。 */
  friendsEncodingMaxChar *max; /*!< 1文字の最大バイト数なのです。 */
  const char *name;        /*!< 文字コードの名前なのです。 */
};
typedef struct friendsCodeSet friendsCodeSet;


/**
 * @brief 読み取り装置の状態を示すのです。
 *
 * 改行で終わる文字列を渡している限り、friendsLexerUNDERSCORE になるこ
 * とは無いのです。
 */
enum friendsLexerState {
  friendsLexerNORMAL,     /*!< 通常の状態 */
  friendsLexerPAREN,      /*!< () の中 */
  friendsLexerDBRACKET,   /*!< 『』の中 */
  friendsLexerUNDERSCORE, /*!< _ の直後 */
};
typedef enum friendsLexerState friendsLexerState;

/**
 * @brief 引数のうち、使っていないものを示すのです。
 *
 * コンパイラの警告を抑止する効果しか無いので、完成してからつけると良
 * いのです。
 */
#define friendsUnUsed(var) ((void) (var))

/**
 * @brief Friends 言語の入力の続きを読むための関数の形式なのです。
 */
typedef int friendsFillFunc(friendsParser *p, int count, void *arg);

/**
 * @macro friendsStaticThreadLocal
 * @brief 「スレッドローカル」な static 変数を定義するための装飾子なの
 *        です。
 *
 * コンパイラが C11 に対応していると言っているなら、C11 の記法を使うの
 * です。
 *
 * それ以外で、GNU C Compiler を使っているなら、gcc の拡張を使うのです。
 * MSVC を使っているなら、MSVC の拡張を使うのです。
 *
 * 上のどれもダメなら、単に static としておくのです。
 */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define friendsStaticThreadLocal static _Thread_local
#else
#if defined(__GNUC__)
#define friendsStaticThreadLocal static __thread
#elif defined(_MSC_VER)
#define friendsStaticThreadLocal __declspec(thread) static
#else
#define friendsStaticThreadLocal static
#endif
#endif

/**
 * @brief __FILE__ （など）からファイル名だけを取り出すのです。
 * @param name パス名をよこすのです。
 * @return ファイル名を返すのです。
 *
 * スラッシュで終わっていたりとかファイル名を取り出せない時は NULL を
 * 返すのです。
 *
 * 返されるポインタは、渡した名前の一部なのです。
 *
 * @note CMake が生成する Makefile では絶対パスになるのです。
 *
 * @note この関数は表向きにパスを隠せるだけで、解析ツールを使うとパスは
 *       バレバレなので注意するのです。
 */
const char *friendsSourceFile(const char *name);

/**
 * @macro FRIENDS_SOURCE_FILE
 * @brief `friendsSourceFile(__FILE__)` へのショートカットなのです。
 *
 * CMake のオプションで FRIENDS_ENABLE_FULLPATH_SOURCE_NAME を有効にす
 * るとフルパスを返す（__FILE__ を変形せずに返す）のです。
 */
#ifdef FRIENDS_ENABLE_FULLPATH_SOURCE_NAME
#define FRIENDS_SOURCE_FILE __FILE__
#else
#define FRIENDS_SOURCE_FILE (friendsSourceFile(__FILE__))
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define friendsStaticAssert(cond,msg) \
  _Static_assert((cond), (msg))
#else
#define friendsStaticAssert(cond,msg) \
  typedef char friendsStaticAssert_##msg[(cond)?1:-1]
#endif

#endif
