
#pragma once

#include <fstream>
#include <iostream>
#include <locale>
#include <codecvt>

#include "rapidjson/document.h"
#include "rapidjson/encodings.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/error/en.h"

using namespace std;
using namespace rapidjson;

// 로그 레벨
#define LEVEL_NOLOG		-1
#define LEVEL_DEFAULT	0
#define LEVEL_CRITICAL	1
#define LEVEL_ERROR		2
#define LEVEL_WARNING	3
#define LEVEL_INFO		4
#define LEVEL_DEBUG		5
#define LEVEL_SCRIPT	6

// 각 엑셀을 대표하는 이름
#define DEFAULT_TESTCASE		L"테스트케이스"
#define CHECKLIST_MAIN			L"주계약"
#define CHECKLIST_SUB			L"특약"
#define CHECKLIST_ANNUITY		L"연금전환특약"

// 각 엑셀을 읽을 때 기준이 되는 Cell(데이터 판단 기준)
#define RUNMAP_HEADER		L"A2"
//#define CODEMAP_HEADER		L"A2"
#define PRODUCTMAP_HEADER	L"A3"
#define EXCELMAP_HEADER		L"A2"
#define TESTCASE_HEADER		L"A2"
#define LEADCODE_HEADER		L"A2"

// 실행맵 병행검증 시트(다른 엑셀 파일에 추가하기 위해 미리 만들어둔 시트)
#define DEFAULT_SHEET_TOTAL		L"perfectwin"

// 실행맵 Input, Output 컬럼 정보
#define MAP_COL_INDEX			0
#define MAP_COL_SOURCEFILE		1
#define MAP_COL_SOURCESHEET		2
#define MAP_COL_SOURCECELL		3
#define MAP_COL_RULETYPE		4
#define MAP_COL_FUNCTION		5
#define MAP_COL_RANGEYN			6
#define MAP_COL_TARGETFILE		7
#define MAP_COL_TARGETSHEET		8
#define MAP_COL_ID				9
#define MAP_COL_TARGETCELL		10

// 실행맵 Service 시트 컬럼 정보
#define SERVICEMAP_TYPE				0
#define SERVICEMAP_NAME				1
#define SERVICEMAP_SERVER			2
#define SERVICEMAP_ID1				3
#define SERVICEMAP_INPUT_HEADER		4
#define SERVICEMAP_OUTPUT_HEADER	5

// 실행맵 초기오픈 시트 컬럼 정보
#define OPENMAP_TYPE	0
#define OPENMAP_NAME	1
#define OPENMAP_FILE	2
#define OPENMAP_SHEET	3
#define OPENMAP_KEY		4

// 실행맵 기본 시트 컬럼 정보
#define DEFAULTMAP_CODE			0
#define DEFAULTMAP_SHEETIN		1
#define DEFAULTMAP_SHEETOUT		2
#define DEFAULTMAP_MACROINIT	3
#define DEFAULTMAP_MACRORUN		4
#define DEFAULTMAP_MACROAUTORUN	5

// 실행맵 체크리스트 입력 시트 컬럼 정보
#define CHECKLISTMAP_CODE		0
#define CHECKLISTMAP_TYPE		1
#define CHECKLISTMAP_NAME		2
#define CHECKLISTMAP_SUBINFO	5

// 실행맵 특약별파일명 컬럼 정보
#define EXCELCODE_CODE		0
#define EXCELCODE_FILE		1
#define EXCELCODE_CHECKLIST	2

// 실행맵 Range 시트 컬럼 정보
#define RANGEMAP_CODE			0
#define RANGEMAP_CHECKLIST		1
#define RANGEMAP_SHEET			2
#define RANGEMAP_ADDRESS		3

// 전문 Input, Output을 나타내는 이름
#define TRAN_INPUT		L"input"
#define TRAN_OUTPUT		L"output"

// 상품 파일 컬럼 정보
#define PRODUCTMAP_TYPE				0
#define PRODUCTMAP_PRODUCT_TYPE		1
#define PRODUCTMAP_REPORT_TYPE		2
#define PRODUCTMAP_PRODUCT_CODE		3
#define PRODUCTMAP_PRODUCT_NAME		4
#define PRODUCTMAP_CHECK_GROUP		5
#define PRODUCTMAP_CHECK_NAME		6
#define PRODUCTMAP_CHECK_CODE		7
#define PRODUCTMAP_CURRENCY			8
#define PRODUCTMAP_MAIN_CODE		9
#define PRODUCTMAP_SUB_CODE			10
#define PRODUCTMAP_PS_CODE			11
#define PRODUCTMAP_CHANGE_CODE		12
#define PRODUCTMAP_INCLUDEFROM		13
#define PRODUCTMAP_INCLUDETO		88
#define PRODUCTMAP_DTO_CODE			89
#define PRODUCTMAP_SCN_ORDER		90
#define PRODUCTMAP_CURRENCY_SUB		91

// 테스트케이스 파일 컬럼 정보
#define TESTCASE_GROUP_NO		0
#define TESTCASE_SUB_CODE		3
#define TESTCASE_JOIN_AGE		25
#define TESTCASE_ANNUITY_AGE	58
#define TESTCASE_PDF_FILE		96

// 시스템구분
#define SYSTEM_COCO		L"COCO"
#define SYSTEM_TLC		L"TLC"
#define SYSTEM_BA		L"BA"
#define SYSTEM_PRODUCT	L"상품설명서"
#define SYSTEM_PRODUCTPDF	L"상품설명서PDF"

// Message.json 정의
#define MESSAGE_ADDRESS_INVALID		L"ADDRESS_INVALID"
#define MESSAGE_SOURCECELL			L"SOURCECELL"
#define MESSAGE_SOURCEFILE			L"SOURCEFILE"
#define MESSAGE_SCRIPT_NOTFOUND		L"SCRIPT_NOTFOUND"
#define MESSAGE_GROUP_NOTFOUND		L"GROUP_NOTFOUND"
#define MESSAGE_RANGE_INVALID		L"RANGE_INVALID"
#define MESSAGE_SOURCECOUNT_INVALID	L"SOURCECOUNT_INVALID"
#define MESSAGE_RANGE_EMPTY			L"RANGE_EMPTY"
#define MESSAGE_SHEETADD_ERROR		L"SHEETADD_ERROR"
#define MESSAGE_MULTI_INVALID		L"MULTI_INVALID"
#define MESSAGE_CHECKLISTCODE_EMPTY	L"CHECKLISTCODE_EMPTY"
#define MESSAGE_MULTI_JS_INVALID	L"MULTI_JS_INVALID"
#define MESSAGE_MAIN_NOTFOUND		L"MAIN_NOTFOUND"

// 신한라이프 Custom 항목
#define SUB_EXCEPTPAY		L"납입면제특약"
#define SUB_KEEPTRUTH		L"진심을품은특약"
#define SUB_KEEPTRUTH_RENEW	L"진심을품은특약(갱신형)"
#define SUB_KEEPTRUTH_NOTRENEW	L"진심을품은특약(비갱신형)"
#define SUB_ADDTRUTH		L"진심을The한특약(비갱신형)"
#define SUB_ALLPAY			L"올페이특약(100%)"
#define SUB_ALLPAY50		L"올페이특약(50%)"
#define SUB_ALLPAYPLUS		L"올페이Plus특약(100%)"
#define SUB_ALLPAYPLUS50	L"올페이Plus특약(50%)"

typedef CArray	<INT_PTR, INT_PTR&> CArrayInt;
typedef CArray	<CStringArray*, CStringArray*> CArrayStringArray;
typedef CArray	<CArrayStringArray*, CArrayStringArray*> CArray2StringArray;
typedef CMap	<CString, LPCTSTR, CStringArray*, CStringArray*> CMapStringArray;
typedef CMap	<CString, LPCTSTR, VARIANT, VARIANT> CMapVariant;
typedef CMap	<CString, LPCTSTR, CMapVariant*, CMapVariant*> CMapVariantMap;
typedef CMap	<CString, LPCTSTR, CArrayStringArray*, CArrayStringArray*> CMap2StringArray;
typedef CMap	<CString, LPCTSTR, CMapStringArray*, CMapStringArray*> CMapMapStringArray;

enum CHARACTER_ENCODING
{
	ANSI,
	Unicode,
	Unicode_big_endian,
	UTF8_with_BOM,
	UTF8_without_BOM
};


class CUtility
{
public:
	CUtility();
	~CUtility();

protected:
	static CString m_strRunType;
	static CString m_strRootPath;
	static CString m_strExePath;
	static CString m_strLogPath;
	static CString m_strJSPath;
	static CString m_strCheckListPath;
	static CString m_strIniFilePath;
	static CString m_strRuleFile;
	static CString m_strServicePath;
	static BOOL m_bDataLog;
	static BOOL m_bHTTPLog;
	static BOOL m_bTraceLog;
	static BOOL m_bCloseMode;
	static BOOL m_bUseJS;
	static BOOL m_bSaveAs;
	static BOOL m_bCache;
	static BOOL m_bClearFolder;
	static CMapStringToString m_mapMessage;

	static CRITICAL_SECTION __log_lock__;
	static ofstream* m_pStream;
	static INT_PTR m_nLogLevel;

	static CMapStringToString m_mapPath;
	static CMapStringToString m_mapFilePath;
	static CMapVariantMap m_mapList;

	static CMap2StringArray m_mapCheckListArray;

	static BOOL m_bShowMessage;

	static CMapStringToPtr m_mapFont;

public:
	static CString ReadFileData(CString strFilePath);
	static BOOL ReadFileData(CString strFilePath, char* chReadData);
	static char* ReadFileChar(CString strFilePath);
	static CString ReadFileDataToBase64(CString strFilePath);
	static BOOL WriteFileData(CString strFilePath, CString strFileContents);
	static BOOL WriteFileData(CString strFilePath, char* chData, int nLen = -1);
	static void ReadWriteFileData(CString strFilePath, CString strDestFilePath);
	static void FileCopy(CString strFilePath, CString strDestFilePath);
	static void FileCopy(const char* InputFilename, const char* OutputFilename);

	static char* ConvertUnicodeToMultybyte(CString strUnicode);
	static CString ConvertMultibyteToUnicode(char* pMultibyte);
	static char* ConvertUnicodeToUTF8(CString strUnicode);
	static CString ConvertUTF8ToUnicode(char* pUTF8);
	static CHARACTER_ENCODING get_text_file_encoding(CString filename);
	static CString GetUUID();
	static BOOL MakeDirectory(CString strPath);
	static BOOL InitConfig();
	static CString GetRunType();
	static void SetRunType(CString strRunType);
	static void SetPath(CString strKey, CString strPath);
	static CString GetPath(CString strKey);
	static void SetFilePath(CString strKey, CString strPath);
	static CString GetFilePath(CString strKey);
	static void SetMap(CString strMapName, CString strKey, VARIANT vValue);
	static VARIANT GetMap(CString strMapName, CString strKey);
	static void RemoveMap(CString strMapName, CString strKey);
	static BOOL ReadPath(CString strSection);
	static BOOL InitLogSetting();
	static void SetLogLevel(INT_PTR nLevel);
	static void Release();
	static void LogWrite(INT_PTR nLevel, CString strLogData);
	static BOOL InitMessageSetting();
	static int ShowMessageBox(INT_PTR nLevel, CString strDetailMessage, CString strCode = L"", UINT nType = MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL | MB_SETFOREGROUND | MB_TOPMOST);
	static int MsgBox(CString strDetailMessage, UINT nType);

	static CString GetRootPath();
	static void SetRootPath(CString strPath);
	static CString GetExePath();
	static void SetExePath(CString strPath);
	static CString GetLogPath();
	static void SetLogPath(CString strPath);
	static CString GetJSPath();
	static void SetJSPath(CString strPath);
	static CString GetCheckListPath();
	static void SetCheckListPath(CString strPath);
	static CString GetIniFilePath();
	static void SetIniFilePath(CString strPath);
	static CString GetRuleFile();
	static void SetRuleFile(CString strFile);
	static CString GetServicePath();
	static void SetServicePath(CString strPath);
	static CString ReadIniData(CString strSection, CString strKey);

	static BOOL GetUseJS();
	static void SetUseJS(BOOL bUseJS);
	static BOOL GetDataLog();
	static void SetDataLog(BOOL bLog);
	static BOOL GetHTTPLog();
	static void SetHTTPLog(BOOL bLog);
	static BOOL GetTraceLog();
	static void SetTraceLog(BOOL bLog);
	static BOOL GetCloseMode();
	static void SetCloseMode(BOOL bDataLog);
	static BOOL GetSaveAs();
	static void SetSaveAs(BOOL bSaveAs);
	static BOOL GetCache();
	static void SetCache(BOOL bCache);
	static BOOL GetClearFolder();
	static void SetClearFolder(BOOL bClearFolder);

	static CString GetVariantString(VARIANT vValue);
	static CString GetScaleData(CString strScale, CString strData);
	static void RemoveArrayStringArray(CArrayStringArray* pRemove);
	static void RemoveArray2StringArray(CArray2StringArray* pRemove);
	static void RemoveMapStringArray(CMapStringArray* pRemove);
	static void RemoveMap2StringArray(CMap2StringArray* pRemove);
	static void RemoveMapMapStringArray(CMapMapStringArray* pRemove);
	static void RemoveMapVariantMap(CMapVariantMap* pMap);
	static void CopyArrayArray(CArrayStringArray* pArrSource, CArrayStringArray* pArrDest);
	static INT_PTR GetTokensByString(CString strData, CString strDel, CStringArray& arrRet, BOOL bAllowEmpty);
	static INT_PTR GetTokensByChar(CString strData, CHAR chSep, CStringArray& arrRet, BOOL bAllowEmpty);
	static INT_PTR StringToIndex(CString strCol);
	static CString IndexToString(INT_PTR nRow, INT_PTR nCol);
	static void Trace(CString strType, CString strData);
	static INT_PTR SendCopyData(CString strTarget, CString strSendData, INT_PTR nMessage);
	static BOOL IsNumber(CString strInput);
	static void SetStringToArrayData(CArrayStringArray* pArrayCase, INT_PTR nCaseIndex, CString strCell, CString strData, INT_PTR nStartCol = 0);
	static CString GetStringToArrayData(CArrayStringArray* pArrayCase, INT_PTR nCaseIndex, CString strCell, INT_PTR nStartCol = 0);
	static CString GetStringToArrayData(CArrayStringArray* pArrayCase, INT_PTR nCaseIndex, INT_PTR nColumn, INT_PTR nStartCol = 0);
	static CString GetDataFromArray(CStringArray* pArrayCase, CString strCell, INT_PTR nStartCol = 0);
	static CStringArray* GetArrayFrom2ArrayByCol(CArrayStringArray* pArray, INT_PTR nColumn, INT_PTR nStartRow = 0, INT_PTR nStartCol = 0);
	static CStringArray* GetArrayFrom2ArrayByStartEnd(CArrayStringArray* pArray, INT_PTR nCaseIndex, INT_PTR nStartCol, INT_PTR nEndCol, INT_PTR nStandardCol = 0);
	static CStringArray* GetArrayFrom2ArrayByRange(CArrayStringArray* pArray, INT_PTR nCaseIndex, CString strStartCol, CString strEndCol, INT_PTR nStandardCol = 0);
	static CString GetFindFileName(CString strSearchDir, CString strSearchKey);
	static CStringArray* FindMapFromString(CMapStringArray& mapFind, CString strFind, INT_PTR nStandard);
	static CStringArray* FindArrayFromString(CArrayStringArray& arrFind, CString strFind, INT_PTR nStandard);
	static INT_PTR FindIndexFromString(CStringArray& arrFind, CString strFind);
	static INT_PTR FindIndexFromString(CArrayStringArray& arrFind, CString strFind, INT_PTR nStandard);
	static BOOL IsValidCell(CString strCell);
	static BOOL IsValidCell(CStringArray* pArray);
	static CString GetFixData(CString strData);
	static CArrayStringArray* GetCheckListArray(CString strKey);
	static void SetCheckListArray(CString strKey, CArrayStringArray* pArray);
	static void RemoveCheckListArray();
	static BOOL LoadResourceImage(CImage* pImg, int nResourceID, LPCTSTR pType);
	static void SetShowMessage(BOOL bValue);
	static CFont* ConvertLOGFontToCFont(LOGFONT lFont);
};
