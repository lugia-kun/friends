/**
 * @file friends.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_DEFS_H
#define FRIENDS_DEFS_H

#include <stddef.h>

#include "friends_config.h"

/* Core Data types */
struct friendsMemoryT;
typedef struct friendsMemoryT friendsMemory;

/* Data Types */
struct friendsParkT;
struct friendsDataT;
struct friendsAtomDataT;
struct friendsVariableDataT;
struct friendsPropositionDataT;
struct friendsMatchDataT;

struct friendsDataListT;
struct friendsDataSetT;
struct friendsDataSetNodeT;

struct friendsParserT;
struct friendsLineColumnT;

typedef struct friendsParkT            friendsPark;
typedef struct friendsDataT            friendsData;
typedef struct friendsAtomDataT        friendsAtomData;
typedef struct friendsVariableDataT    friendsVariableData;
typedef struct friendsPropositionDataT friendsPropositionData;
typedef struct friendsMatchDataT       friendsMatchData;
typedef struct friendsDataListT        friendsDataList;
typedef struct friendsDataSetT         friendsDataSet;
typedef struct friendsDataSetNodeT     friendsDataSetNode;

typedef struct friendsTokenDataT       friendsTokenData;
typedef struct friendsParserT          friendsParser;
typedef struct friendsLineColumnT      friendsLineColumn;

/**
 * @typedef friendsPark
 * @brief 必要なデータを保存するのです。
 */
/**
 * @typedef friendsData
 * @brief データを保存するのです。
 *
 * この型には、双方向リンク機能があるのです。
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
 * @typedef friendsHash
 * @brief データを識別するための値を保存する型なのです。
 */
typedef unsigned int  friendsHash;

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

#if defined(HAVE_USHORT_T) && USHORT_T_SIZE == 2
typedef unsigned short   friendsChar;
#elif defined(HAVE_UINT16_T) && UINT16_T_SIZE == 2
typedef uint16_t         friendsChar;
#else
#error "Invalid configuration. Use UTF-8 mode."
#endif
#endif

/**
 * @brief データの型を表すのです。
 */
typedef enum {
  friendsInvalidType,  /*!< 何もデータが入っていないのです */
  friendsAtom,         /*!< アトム */
  friendsVariable,     /*!< 変数 */
  friendsProposition,  /*!< 命題 */

  friendsList,  /*!< リスト */
  friendsSet,   /*!< セット */

  friendsMatch, /*!< 一致情報 */

  friendsToken, /*!< パーサートークン */

  friendsLastType, /*!< 最後の型 */
} friendsType;

/**
 * @brief アトムの型を表すのです。
 */
typedef enum {
  friendsTextAtom,    /*!< 文字列 */
  friendsNumericAtom, /*!< 数値 */
  friendsNextAtom,    /*!< 「次」 */

  friendsLastAtomType, /*!< 最後の型 */
} friendsAtomType;

/**
 * @brief 命題データの扱いを表すのです。
 */
typedef enum {
  friendsPropositionNormal,  /*!< ふつうの */
  friendsPropositionStop,    /*!< たーのしー！ */
  friendsPropositionQuery,   /*!< なんだっけ？ */
} friendsPropositionMode;

/**
 * @brief 真偽値を表すのです。
 *
 * 比較する時は列挙値と比較するのです。
 */
typedef enum {
  friendsFalse = 0,
  friendsTrue  = 1,
} friendsBool;

struct friendsFileT;
typedef struct friendsFileT friendsFile;

/**
 * @brief Friends 言語のエラーを表すのです。
 */
typedef enum {
  friendsNoError = 0,

  /* Friends System */
  friendsErrorInvalidType,  /*!< データの型が正しくない（値の場合は INVAL） */
  friendsErrorValidType,
                    /*!< 有効なデータ型になっている（未設定のデータが必要） */

  friendsErrorListIndex,     /*!< リストのインデックスが範囲外 */
  friendsErrorTooNested,     /*!< ネストが深すぎる */

  friendsErrorNoArgument,    /*!< 命題の引数がない */

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
} friendsError;

typedef enum {
  friendsErrorLevelDebug = 0,
  friendsErrorLevelInfo,
  friendsErrorLevelWarning,
  friendsErrorLevelError,
  friendsErrorLevelFatal,
  friendsErrorLevelAssert,
} friendsErrorLevel;

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
typedef enum {
  friendsDataEqual   = 0x01, /*!< a == b */
  friendsDataGreater = 0x02, /*!< a >  b */
  friendsDataLess    = 0x04, /*!< a <  b */
  friendsDataNotComparable = 0x00, /*!< 比較できない */
  friendsDataNotEqual = friendsDataGreater | friendsDataLess,
  /*!< a != b */

  friendsDataCompareSpecialsMask = 0x0f, /*!< 特殊な比較をマスクするための値 */
  friendsDataDifferentType = 0x08, /*!< typeof(a) != typeof(b) */
  friendsDataInAnotherPark = 0x10, /*!< park(a) != park(b) */
  friendsDataSetEqual = 0x20, /*!< 違うデータではあるが、friendsSet の中では
                                   等しいものとして扱う */

  friendsDataLastCompareResult = 0xff,
} friendsDataCompareResult;

/**
 * @brief メモリを解放するための関数の形式なのです。
 */
typedef void friendsPointerDeleter(void *);

/**
 * @brief データの比較関数の形式なのです。
 */
typedef friendsDataCompareResult friendsDataCompareFunc(const void *,
                                                        const void *);

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
struct friendsCodeSetT {
  friendsOneCharEncoder *one_enc; /*!< 1文字エンコーダなのです。 */
  friendsCharEncoder *enc; /*!< エンコーダなのです。 */
  friendsCharDecoder *dec; /*!< デコーダなのです。 */
  friendsEncodingMaxChar *max; /*!< 1文字の最大バイト数なのです。 */
  const char *name;        /*!< 文字コードの名前なのです。 */
};
typedef struct friendsCodeSetT friendsCodeSet;

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
 * @note __FILE__ はコンパイラの指定によって相対パスだったり絶対パスだっ
 *       たりするけど、CMake が生成する Makefile では絶対パスになるよう
 *       なのです。
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

#endif
