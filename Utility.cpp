
#include "pch.h"
#include "Utility.h"
#include "EPACreator.h"
#include "Base64.h"

#pragma comment(lib, "Rpcrt4.lib")

CString CUtility::m_strRunType;
CString CUtility::m_strRootPath;
CString CUtility::m_strExePath;
CString CUtility::m_strLogPath;
CString CUtility::m_strJSPath;
CString CUtility::m_strCheckListPath;
CString CUtility::m_strIniFilePath;
CString CUtility::m_strServicePath;
BOOL CUtility::m_bUseJS = FALSE;
CString CUtility::m_strRuleFile = L"";
BOOL CUtility::m_bDataLog = FALSE;
BOOL CUtility::m_bHTTPLog = FALSE;
BOOL CUtility::m_bTraceLog = FALSE;
BOOL CUtility::m_bCloseMode = FALSE;
BOOL CUtility::m_bSaveAs = FALSE;
BOOL CUtility::m_bCache = FALSE;
BOOL CUtility::m_bClearFolder = FALSE;
CMapStringToString CUtility::m_mapMessage;

CRITICAL_SECTION CUtility::__log_lock__;
ofstream* CUtility::m_pStream;
INT_PTR CUtility::m_nLogLevel = LEVEL_ERROR;

CMapStringToString CUtility::m_mapPath;
CMapStringToString CUtility::m_mapFilePath;
CMapVariantMap CUtility::m_mapList;

// cache 사용 시 각 체크리스트 모든 데이터를 저장(subcode, 데이터)
CMap2StringArray CUtility::m_mapCheckListArray;

BOOL CUtility::m_bShowMessage = TRUE;

CMapStringToPtr CUtility::m_mapFont;


CUtility::CUtility()
{
}

CUtility::~CUtility()
{
}

void CUtility::Release()
{
	POSITION pos = m_mapFont.GetStartPosition();
	CFont* pFont = NULL;
	CString strT = L"";
	while (pos)
	{
		m_mapFont.GetNextAssoc(pos, strT, (void*&)pFont);
		if (pFont != NULL)
			delete pFont;
	}
	m_mapFont.RemoveAll();

	RemoveMapVariantMap(&m_mapList);

	if (m_pStream != NULL)
	{
		m_pStream->close();
		delete m_pStream;
	}

	DeleteCriticalSection(&__log_lock__);
}

CString CUtility::ReadFileData(CString strFilePath)
{
	CString strData = _T("");

	FILE* p_file = NULL; // 파일 포인터 선언
	//errno_t err = _tfopen_s(&p_file, strFilePath, _T("rt,ccs=UTF-8"));
	errno_t err = _tfopen_s(&p_file, strFilePath, _T("rb"));
	if (err != 0) {
		CString strLog = L"";
		strLog.Format(L"ReadFileData Open Fail(%d)", err);
		LogWrite(LEVEL_ERROR, strLog);

		return _T("");
	}

	fseek(p_file, 0, SEEK_END);
	int nFileSize = ftell(p_file);

	//wchar_t* str = new wchar_t[nFileSize];
	char* str = new char[nFileSize+1];
	ZeroMemory(str, nFileSize + 1);
	fseek(p_file, 0, SEEK_SET);
	size_t result = fread_s(str, nFileSize, 1, nFileSize, p_file);
	if (result != nFileSize) {
		return _T("");
	}

	fclose(p_file);

	strData = ConvertUTF8ToUnicode(str);

	delete[] str;

	//CFile fFile;
	//if (fFile.Open(strFilePath, CFile::modeRead | CFile::typeBinary) == FALSE)
	//{
	//	return _T("");
	//}

	//UINT nFileSize = (UINT)fFile.GetLength();
	//char* chSendData = new char[nFileSize + 1];
	//ZeroMemory(chSendData, nFileSize + 1);

	//UINT nLength = fFile.Read(chSendData, nFileSize);

	//strData = ConvertUTF8ToUnicode(chSendData);
	//delete[] chSendData;

	//fFile.Close();




	//CStdioFile stdioFile;
	//if (stdioFile.Open(strFilePath, CFile::modeRead))
	//{
	//	CString strTemp = _T("");
	//	while (stdioFile.ReadString(strTemp))
	//	{
	//		strData += strTemp;
	//	}

	//	stdioFile.Close();
	//}

	return strData;
}

BOOL CUtility::ReadFileData(CString strFilePath, char* chReadData)
{
	CFile fFile;
	if (fFile.Open(strFilePath, CFile::modeRead | CFile::typeBinary) == FALSE)
	{
		CUtility::ShowMessageBox(LEVEL_WARNING, L"전문 파일를 여는 중 오류가 발생하였습니다.");

		return FALSE;
	}

	UINT nFileSize = (UINT)fFile.GetLength();
	chReadData = new char[nFileSize + 1];
	ZeroMemory(chReadData, nFileSize + 1);

	UINT nLength = fFile.Read(chReadData, nFileSize);

	fFile.Close();

	return TRUE;
}

char* CUtility::ReadFileChar(CString strFilePath)
{
	CString strData = _T("");

	FILE* p_file = NULL; // 파일 포인터 선언
	//errno_t err = _tfopen_s(&p_file, strFilePath, _T("rt,ccs=UTF-8"));
	errno_t err = _tfopen_s(&p_file, strFilePath, _T("rb"));
	if (err != 0) {
		CString strLog = L"";
		strLog.Format(L"ReadFileData Open Fail(%d)", err);
		LogWrite(LEVEL_ERROR, strLog);

		return NULL;
	}

	fseek(p_file, 0, SEEK_END);
	int nFileSize = ftell(p_file);

	//wchar_t* str = new wchar_t[nFileSize];
	char* str = new char[nFileSize + 1];
	ZeroMemory(str, nFileSize + 1);
	fseek(p_file, 0, SEEK_SET);
	size_t result = fread_s(str, nFileSize, 1, nFileSize, p_file);
	if (result != nFileSize) {
		fclose(p_file);
		return NULL;
	}

	fclose(p_file);

	return str;
}

// base64 인코딩 전 원문이 text이면 utf8로 변경 후 base64하는 것이 좋음.
// java, web 등이 모두 utf8을 base64하는 형식이라 맞춰주는 것이 편함
// 하지만 원문이 binary 이면 그대로 base64해야 함.
// 주의할 것은 binary 데이터에 0x00이 있을 수 있으므로 base64 할 때 문자열 길이를 구해서 하지않고 파일 길이를 이용하여 인코딩해야함
CString CUtility::ReadFileDataToBase64(CString strFilePath)
{
	CFile fFile;
	if (fFile.Open(strFilePath, CFile::modeRead | CFile::typeBinary) == FALSE)
	{
		CString strLog = L"";
		strLog.Format(L"파일을 여는 중 오류가 발생하였습니다.\r\n\r\n파일명 : %s", strFilePath);
		CUtility::ShowMessageBox(LEVEL_ERROR, strLog);
		return L"";
	}

	UINT nFileSize = (UINT)fFile.GetLength();
	char* chSendData = new char[nFileSize + 1];
	ZeroMemory(chSendData, nFileSize + 1);

	UINT nLength = fFile.Read(chSendData, nFileSize);

	fFile.Close();

	if (chSendData[0] != '%' || chSendData[1] != 'P' || chSendData[2] != 'D' || chSendData[3] != 'F')
	{
		delete[] chSendData;
		CUtility::ShowMessageBox(LEVEL_ERROR, L"파일 형식이 PDF가 아니거나 문서보안으로 인해 암호화되어 있습니다. 다시 확인해주시기 바랍니다.");
		return L"";
	}

	CBase64 base64;
	CString strData = base64.base64encode(chSendData, nFileSize);
	delete[] chSendData;

	return strData;

	// 아래는 Decode Code
	//CFile fFile;
	//if (fFile.Open(L"C:\\EPA\\PDF\\Base64.txt", CFile::modeRead | CFile::typeBinary) == FALSE)
	//{
	//	return;
	//}

	//UINT nFileSize = (UINT)fFile.GetLength();
	//char* chSendData = new char[nFileSize + 1];
	//ZeroMemory(chSendData, nFileSize + 1);

	//UINT nLength = fFile.Read(chSendData, nFileSize);

	//fFile.Close();

	//CBase64 base;
	//int nRet = 0;
	//unsigned char* psz = base.base64decode((unsigned char*)(const char*)chSendData, nFileSize, nRet);
	//CString strData = L"";
	//CUtility::WriteFileData(L"C:\\EPA\\PDF\\Base64.pdf", (char*)psz, nRet);
	//delete psz;
}

BOOL CUtility::WriteFileData(CString strFilePath, CString strFileContents)
{
	FILE* p_file = NULL;
	errno_t err = _tfopen_s(&p_file, strFilePath, _T("wb"));
	if (err != 0) {
		CString strLog = L"";
		strLog.Format(L"File Open Fail(%d)\r\n\r\n(%s)", err, strFilePath);
		LogWrite(LEVEL_ERROR, strLog);

		return FALSE;
	}

	char* pchContents = ConvertUnicodeToUTF8(strFileContents);
	size_t nLen = strlen(pchContents);
	fwrite(pchContents, 1, nLen, p_file);
	fclose(p_file);

	delete[] pchContents;

	return TRUE;
}

BOOL CUtility::WriteFileData(CString strFilePath, char* chData, int nLen/*=-1*/)
{
	FILE* fFile = NULL;
	fopen_s(&fFile, (CStringA)strFilePath, "wb");
	if (fFile == NULL)
	{
		CString strLog = L"";
		strLog.Format(L"File Open Fail\r\n\r\n(%s)", strFilePath);
		CUtility::ShowMessageBox(LEVEL_ERROR, strLog);

		return FALSE;
	}

	if (nLen == -1)
		fwrite(chData, strlen(chData), 1, fFile);
	else
		fwrite(chData, nLen, 1, fFile);

	fclose(fFile);

	return TRUE;
}

void CUtility::ReadWriteFileData(CString strFilePath, CString strDestFilePath)
{
	FILE* fp = NULL;
	errno_t err = _tfopen_s(&fp, strFilePath, _T("rb"));
	if (err != 0) {
		return;
	}

	FILE* fp_dest = NULL;
	err = _tfopen_s(&fp_dest, strDestFilePath, _T("wb"));
	if (err != 0) {
		return;
	}

	size_t total = 0;
	char* buffer = new char[1024];
	while (feof(fp) == 0)
	{
		ZeroMemory(buffer, 1024);

		size_t count = fread(buffer, sizeof(char), 1024, fp);

		fwrite(buffer, sizeof(char), count, fp_dest);

		total += count;
	}

	fclose(fp);
	fclose(fp_dest);
}

void CUtility::FileCopy(CString strFilePath, CString strDestFilePath)
{
	CStringA strIn = (CStringA)strFilePath;
	char* pchIn = strIn.GetBuffer();
	CStringA strOut = (CStringA)strDestFilePath;
	char* pchOut = strOut.GetBuffer();

	FILE* fp = fopen(pchIn, "rb");
	FILE* fpOut = fopen(pchOut, "wb");

	while (!feof(fp))
	{
		char buffer[1024] = { 0 };
		fgets(buffer, sizeof(buffer), fp);
		fputs(buffer, fpOut);
	}

	fclose(fp);
	fclose(fpOut);
}

void CUtility::FileCopy(const char* InputFilename, const char* OutputFilename)
{
	const int BUF_LEN = 4096;
	char buffer[BUF_LEN];
	fstream fs_in, fs_out;

	fs_in.open(InputFilename, fstream::in | fstream::binary);
	fs_out.open(OutputFilename, fstream::out | fstream::binary);

	while (!fs_in.eof())
	{
		fs_in.read(buffer, BUF_LEN);
		fs_out.write(buffer, fs_in.gcount());
	}

	fs_in.close();
	fs_out.close();
}

// CString 은 유니코드, CStringA 는 멀티바이트
// 멀티바이트는 한글 2자, 영문 1자, utf8 은 한글 3자, 영문 1자
// 멀티바이트를 utf8로 인코딩하기 위해서 유니코드를 거쳐야함
// 파일을 읽어서 변경하려면 파일은 bytecode이기 때문에 문자열 마지막 널문자가 끼어있기 때문에
// 변환하려는 길이로 filesize를 줘야함, 문자열길이로 주면 널문자까지 밖에 길이가 나오지 않음
char* CUtility::ConvertUnicodeToMultybyte(CString strUnicode)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);

	char* pMultibyte = new char[nLen];
	memset(pMultibyte, 0x00, (nLen) * sizeof(char));

	WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, pMultibyte, nLen, NULL, NULL);

	return pMultibyte;
}

CString CUtility::ConvertMultibyteToUnicode(char* pMultibyte)
{
	int nLen = (int)strlen(pMultibyte) + 1;

	WCHAR* pWideChar = new WCHAR[nLen];
	memset(pWideChar, 0x00, (nLen) * sizeof(WCHAR));

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pMultibyte, -1, pWideChar, nLen);

	CString strUnicode;
	strUnicode.Format(_T("%s"), pWideChar);

	delete[] pWideChar;

	return strUnicode;
}

char* CUtility::ConvertUnicodeToUTF8(CString strUnicode)
{
	int nLen = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);

	char* pMultibyte = new char[nLen];
	memset(pMultibyte, 0x00, (nLen) * sizeof(char));

	WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, pMultibyte, nLen, NULL, NULL);

	return pMultibyte;
}

CString CUtility::ConvertUTF8ToUnicode(char* pUTF8)
{
	int nLen = (int)strlen(pUTF8);

	int nDestLen = MultiByteToWideChar(CP_UTF8, 0, pUTF8, -1, NULL, NULL);

	WCHAR* pWideChar = new WCHAR[nDestLen];
	ZeroMemory(pWideChar, (nDestLen) * sizeof(WCHAR));

	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, -1, pWideChar, nDestLen);

	CString strUnicode;
	strUnicode.Format(_T("%s"), pWideChar);

	delete[] pWideChar;

	return strUnicode;
}

CHARACTER_ENCODING CUtility::get_text_file_encoding(CString filename)
{
	CHARACTER_ENCODING encoding;

	unsigned char uniTxt[] = { 0xFF, 0xFE };// Unicode file header
	unsigned char endianTxt[] = { 0xFE, 0xFF };// Unicode big endian file header
	unsigned char utf8Txt[] = { 0xEF, 0xBB };// UTF_8 file header

	DWORD dwBytesRead = 0;
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = NULL;
		CloseHandle(hFile);

		return ANSI;
	}

	BYTE* lpHeader = new BYTE[2];
	ReadFile(hFile, lpHeader, 2, &dwBytesRead, NULL);
	CloseHandle(hFile);

	if (lpHeader[0] == uniTxt[0] && lpHeader[1] == uniTxt[1])// Unicode file
		encoding = CHARACTER_ENCODING::Unicode;
	else if (lpHeader[0] == endianTxt[0] && lpHeader[1] == endianTxt[1])//  Unicode big endian file
		encoding = CHARACTER_ENCODING::Unicode_big_endian;
	else if (lpHeader[0] == utf8Txt[0] && lpHeader[1] == utf8Txt[1])// UTF-8 file
		encoding = CHARACTER_ENCODING::UTF8_with_BOM;
	else
		encoding = CHARACTER_ENCODING::ANSI;   //Ascii

	delete[]lpHeader;
	return encoding;
}

CString CUtility::GetUUID()
{
	CString strUUID = L"";

	UUID uuid;
	if (UuidCreate(&uuid) == RPC_S_OK)
	{
		char* szUuid;
		if (UuidToStringA(&uuid, (RPC_CSTR*)&szUuid) == RPC_S_OK)
		{
			strUUID = szUuid;
			strUUID.MakeLower();
			RpcStringFreeA((RPC_CSTR*)&szUuid);
		}
	}

	return strUUID;
}

BOOL CUtility::MakeDirectory(CString strPath)
{
	CString strLog = L"";
	strLog.Format(L"MakeDirectory / Start / %s", strPath);
	LogWrite(LEVEL_DEBUG, strLog);

	LPCTSTR lpszPath = strPath;
	TCHAR szPathBuffer[MAX_PATH] = { 0, };

	size_t len = _tcslen(lpszPath);

	for (size_t i = 0; i < len; i++)
	{
		szPathBuffer[i] = *(lpszPath + i);
		if (szPathBuffer[i] == _T('\\') || szPathBuffer[i] == _T('/'))
		{
			szPathBuffer[i + 1] = NULL;
			if (!PathFileExists(szPathBuffer))
			{
				if (!::CreateDirectory(szPathBuffer, NULL))
				{
					if (GetLastError() != ERROR_ALREADY_EXISTS)
					{
						LogWrite(LEVEL_ERROR, L"MakeDirectory / End / FAIL");

						return FALSE;
					}
				}
			}
		}
	}

	LogWrite(LEVEL_DEBUG, L"MakeDirectory / End");

	return TRUE;
}

BOOL CUtility::InitConfig()
{
	char chJSUse[256] = { 0, };
	GetPrivateProfileStringA("PERFECTWIN", "USEJS", "", chJSUse, 256, (CStringA)m_strIniFilePath);
	CString strJSUse = (CString)chJSUse;
	if (strJSUse == L"1")
		m_bUseJS = TRUE;

	char chRuleFile[256] = { 0, };
	GetPrivateProfileStringA("PERFECTWIN", "RULEFILE", "", chRuleFile, 256, (CStringA)CUtility::GetIniFilePath());
	m_strRuleFile = (CString)chRuleFile;

	char chLogLevel[256] = { 0, };
	GetPrivateProfileStringA("PERFECTWIN", "LOGLEVEL", "", chLogLevel, 256, (CStringA)m_strIniFilePath);
	m_nLogLevel = atoi(chLogLevel);

	char chDataLog[256] = { 0, };
	GetPrivateProfileStringA("PERFECTWIN", "DATALOG", "", chDataLog, 256, (CStringA)m_strIniFilePath);
	CString strDataLog = (CString)chDataLog;
	if (strDataLog == L"1")
		m_bDataLog = TRUE;

	char chHTTPLog[256] = { 0, };
	GetPrivateProfileStringA("PERFECTWIN", "HTTPLOG", "", chHTTPLog, 256, (CStringA)m_strIniFilePath);
	CString strHTTPLog = (CString)chHTTPLog;
	if (strHTTPLog == L"1")
		m_bHTTPLog = TRUE;

	char chTraceLog[256] = { 0, };
	GetPrivateProfileStringA("PERFECTWIN", "TRACELOG", "", chTraceLog, 256, (CStringA)m_strIniFilePath);
	CString strTraceLog = (CString)chTraceLog;
	if (strTraceLog == L"1")
		m_bTraceLog = TRUE;

	//char chCloseMode[256] = { 0, };
	//GetPrivateProfileStringA("PERFECTWIN", "CLOSEMODE", "", chCloseMode, 256, (CStringA)m_strIniFilePath);
	//CString strCloseMode = (CString)chCloseMode;
	//if (strCloseMode == L"1")
	//	m_bCloseMode = TRUE;

	//char chSaveAs[256] = { 0, };
	//GetPrivateProfileStringA("PERFECTWIN", "SAVEAS", "", chSaveAs, 256, (CStringA)m_strIniFilePath);
	//CString strSaveAs= (CString)chSaveAs;
	//if (strSaveAs == L"1")
	//	m_bSaveAs = TRUE;

	char chCache[256] = { 0, };
	GetPrivateProfileStringA("PERFECTWIN", "CACHE", "", chCache, 256, (CStringA)CUtility::GetIniFilePath());
	CString strCache = (CString)chCache;
	if (strCache != L"0")
		m_bCache = TRUE;

	char chClear[256] = { 0, };
	GetPrivateProfileStringA("PERFECTWIN", "CLEAR", "", chClear, 256, (CStringA)CUtility::GetIniFilePath());
	CString strClear = (CString)chClear;
	if (strClear != L"0")
		m_bClearFolder = TRUE;

#ifdef _DEBUG
	m_bClearFolder = FALSE;
#endif

	LogWrite(LEVEL_DEFAULT, L"USEJS : " + strJSUse);
	LogWrite(LEVEL_DEFAULT, L"RULEFILE : " + m_strRuleFile);
	LogWrite(LEVEL_DEFAULT, L"DATALOG : " + strDataLog);
	LogWrite(LEVEL_DEFAULT, L"HTTPLOG : " + strHTTPLog);
	LogWrite(LEVEL_DEFAULT, L"TRACELOG : " + strTraceLog);
	LogWrite(LEVEL_DEFAULT, L"CACHE : " + strCache);
	LogWrite(LEVEL_DEFAULT, L"CLEAR : " + strClear);

	//ReadPath(L"PATH");
	//ReadPath(L"FILE");strHTTPLog
	return TRUE;
}

CString CUtility::GetRunType()
{
	return m_strRunType;
}

void CUtility::SetRunType(CString strRunType)
{
	m_strRunType = strRunType;
}

BOOL CUtility::ReadPath(CString strSection)
{
	TCHAR s_section[4096] = { 0 }; //섹션 아래의 모든 값을 저장할 버퍼
	TCHAR s_key[256] = { 0x00, }; //한 줄을 읽어 저장할 버퍼(키)
	TCHAR s_data[256] = { 0x00, }; //한 줄을 읽어 저장할 버퍼(값)
	int key_pos = 0; //한 줄의 키의 인덱스 위치
	int data_pos = 0; //한 줄의 값의 인덱스 위치

	//nSize = 그 섹션의 모든 값 하나하나의 수.
	DWORD nSize = GetPrivateProfileSection(strSection, s_section, 4096, m_strIniFilePath);
	BOOL bSection = false; // 한 줄을 모두 확인했으면 true하여, pos를 0으로 초기화해서 다음 줄을 읽을 준비
	bool bEqul = false; //'='으로 키와 값을 분리. =지나면 true해서 구분

	//i는 모든 섹션의 인덱스 위치이고, pos는 한 줄의 인덱스 위치
	for (int i = 0; i < (int)nSize; i++)
	{
		if (s_section[i] != '\0') //문자가 있을 경우
		{
			if (s_section[i] != '=')
			{
				if (bSection) {
					key_pos = 0;
					data_pos = 0;
					bSection = FALSE;
				}

				if (bEqul == false) //=이전 문자는 바로 키
				{
					memcpy(s_key + key_pos, s_section + i, sizeof(TCHAR)); //=이전값인 키를 저장
					key_pos++;
				}
				else  //=이후 문자는 바로 값
				{
					memcpy(s_data + data_pos, s_section + i, sizeof(TCHAR));
					data_pos++;
				}
			}
			else //문자가 =인 경우
				bEqul = true;
		}
		else //없을경우 ( 키의 끝 / 섹터의 끝)
		{
			//s_key 초기화
			s_key[i] = '\0';
			s_data[i] = '\0';

			CString strPath = L"";
			strPath.Format(L"%s%s", GetRootPath(), s_data);

			if (strSection == L"PATH")
				m_mapPath.SetAt(s_key, strPath);
			else if (strSection == L"FILE")
				m_mapFilePath.SetAt(s_key, strPath);

			memset(s_key, 0x00, sizeof(s_key));
			memset(s_data, 0x00, sizeof(s_data));

			bSection = true;
			bEqul = false;
		}
	}

	return TRUE;
}

void CUtility::SetPath(CString strKey, CString strPath)
{
	m_mapPath.SetAt(strKey, strPath);
}

CString CUtility::GetPath(CString strKey)
{
	CString strPath = L"";
	m_mapPath.Lookup(strKey, strPath);

	return strPath;
}

void CUtility::SetFilePath(CString strKey, CString strPath)
{
	m_mapFilePath.SetAt(strKey, strPath);
}

CString CUtility::GetFilePath(CString strKey)
{
	CString strPath = L"";
	m_mapFilePath.Lookup(strKey, strPath);

	return strPath;
}

void CUtility::SetMap(CString strMapName, CString strKey, VARIANT vValue)
{
	CMapVariant* pMap = NULL;
	m_mapList.Lookup(strMapName, pMap);
	if (pMap == NULL)
	{
		pMap = new CMapVariant();
		m_mapList.SetAt(strMapName, pMap);
	}

	pMap->SetAt(strKey, vValue);
}

VARIANT CUtility::GetMap(CString strMapName, CString strKey)
{
	VARIANT vValue;
	VariantInit(&vValue);

	CMapVariant* pMap = NULL;
	m_mapList.Lookup(strMapName, pMap);
	if (pMap != NULL)
	{
		pMap->Lookup(strKey, vValue);
	}

	return vValue;
}

void CUtility::RemoveMap(CString strMapName, CString strKey)
{
	CString strValue = L"";

	CMapVariant* pMap = NULL;
	m_mapList.Lookup(strMapName, pMap);
	if (pMap != NULL)
	{
		pMap->RemoveKey(strKey);
	}
}

void CUtility::SetLogLevel(INT_PTR nLevel)
{
	m_nLogLevel = nLevel;
}

BOOL CUtility::InitLogSetting()
{
	InitializeCriticalSection(&__log_lock__);

	CTime Time = CTime::GetCurrentTime();
	CString strTime = L"";
	strTime.Format(L"%02d%02d%02d", Time.GetHour(), Time.GetMinute(), Time.GetSecond());
	CString strDay;
	strDay.Format(L"%04d%02d%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay());

	m_strLogPath += strDay + L"\\";

	BOOL bDir = MakeDirectory(m_strLogPath);
	if (bDir == FALSE)
	{
		CString strLog = L"";
		strLog.Format(L"InitLogSetting / Directory Create Fail : %s", m_strLogPath);
		LogWrite(LEVEL_ERROR, strLog);

		return FALSE;
	}

	if (m_nLogLevel != -1)
	{
		CString strLogFile = m_strLogPath + L"EPARunner_" + strTime + L".log";
		m_pStream = new ofstream(strLogFile, ios::app | ios::binary);
		if (m_pStream == NULL)
		{
			CString strLog = L"";
			strLog.Format(L"InitLogSetting / LogFile Create Fail : %s", strLogFile);
			LogWrite(LEVEL_ERROR, strLog);

			return FALSE;
		}

		m_pStream->setf(ios::unitbuf);
	}

	return TRUE;
}

void CUtility::LogWrite(INT_PTR nLevel, CString strLogData)
{
	if (nLevel == LEVEL_SCRIPT)
	{
		CUtility::Trace(L"JS", strLogData);

		if (nLevel != LEVEL_NOLOG)
			nLevel = LEVEL_DEBUG;
	}
	else if (nLevel < LEVEL_INFO && nLevel != LEVEL_DEFAULT)
	{
		CUtility::Trace(L"ERROR", strLogData);
	}

	if (m_pStream == NULL)
	{
		return;
	}

	if (nLevel == LEVEL_NOLOG)
	{
		return;
	}

	if (m_nLogLevel >= nLevel)
	{
		CTime Time = CTime::GetCurrentTime();

		CString strHead = L"";

		switch (nLevel)
		{
		case LEVEL_DEFAULT:	strHead = L"기본"; break;
		case LEVEL_CRITICAL:strHead = L"위험"; break;
		case LEVEL_ERROR:	strHead = L"오류"; break;
		case LEVEL_WARNING:	strHead = L"경고"; break;
		case LEVEL_INFO:	strHead = L"정보"; break;
		case LEVEL_DEBUG:	strHead = L"개발"; break;
		}

		SYSTEMTIME st;
		GetSystemTime(&st);

		CString strMili = L"";
		strMili.Format(L":%03d", st.wMilliseconds);

		CString strTime = Time.Format(L"%Y-%m-%d %H:%M:%S") + strMili;

		CString strLog = L"";
		strLog.Format(L"[%s|%s] %s", strHead, strTime, strLogData);

		EnterCriticalSection(&__log_lock__);

		*m_pStream << (CStringA)strLog << "\r\n";

		LeaveCriticalSection(&__log_lock__);
	}
}

BOOL CUtility::InitMessageSetting()
{
	LogWrite(LEVEL_DEBUG, L"InitMessageSetting / Start");

	CString strMessagePath = m_strExePath + L"message.json";
	FILE* fp = NULL;
	fopen_s(&fp, (CStringA)strMessagePath, "r");
	if (!fp)
	{
		LogWrite(LEVEL_ERROR, L"InitMessageSetting / message.json 파일 읽기 실패");
		return FALSE;
	}

	char buffer[65536];
	FileReadStream fs(fp, buffer, sizeof(buffer));
	Document document;
	document.ParseStream(fs);
	if (document.HasParseError()) {
		fclose(fp);

		CString strError = ConvertMultibyteToUnicode((char*)GetParseError_En(document.GetParseError()));

		CString strLog = L"";
		strLog.Format(L"InitMessageSetting / Error offset %u: %s", static_cast<unsigned>(document.GetErrorOffset()), strError);
		LogWrite(LEVEL_ERROR, strLog);

		return FALSE;
	}

	fclose(fp);

	if (document.IsArray() == FALSE)
	{
		LogWrite(LEVEL_ERROR, L"InitMessageSetting / message.json 배열 아님");
		return FALSE;
	}

	for (auto& v : document.GetArray())
	{
		if (v.GetType() == kObjectType)
		{
			const Value& vCode = v["code"];
			CString strCode = ConvertUTF8ToUnicode((char*)vCode.GetString());
			const Value& vMessage = v["message"];
			CString strMessage = ConvertUTF8ToUnicode((char*)vMessage.GetString());

			m_mapMessage.SetAt(strCode, strMessage);
		}
	}

	LogWrite(LEVEL_DEBUG, L"InitMessageSetting / End");

	return TRUE;
}

int CUtility::ShowMessageBox(INT_PTR nLevel, CString strDetailMessage, CString strCode/*=L""*/, UINT nType/*=MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL|MB_SETFOREGROUND|MB_TOPMOST*/)
{
	CString strMessage = L"";
	if (strCode != L"")
	{
		CString strTemp = L"";
		m_mapMessage.Lookup(strCode, strTemp);
		strMessage = strTemp + L"\r\n\r\n항목 : " + strDetailMessage;
	}
	else
	{
		strMessage = strDetailMessage;
	}

	CString strLog = L"ShowMessageBox / " + strMessage;
	CUtility::LogWrite(nLevel, strLog);

	if (nLevel < LEVEL_INFO)
	{
		CUtility::Trace(L"ERROR", strMessage);
	}

	//GetMainWnd()->m_pProcess->AddErrorUserMsg(strMessage);

	if (m_bShowMessage == FALSE)
		return IDOK;

	if (nLevel == LEVEL_CRITICAL || nLevel == LEVEL_ERROR)
	{
		nType ^= MB_ICONQUESTION;
		nType ^= MB_ICONINFORMATION;
		nType ^= MB_ICONQUESTION;
		nType |= MB_ICONERROR;
	}

	if (GetMainWnd()->IsIconic())
	{
		GetMainWnd()->ShowWindow(SW_RESTORE);
	}

	GetMainWnd()->BringWindowToTop();
	GetMainWnd()->SetForegroundWindow();

	return AfxMessageBox(strMessage, nType);
}

int CUtility::MsgBox(CString strDetailMessage, UINT nType/*=MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL|MB_SETFOREGROUND|MB_TOPMOST*/)
{
	CString strMessage = strDetailMessage;

	CString strLog = L"MsgBox / " + strMessage;

	CUtility::LogWrite(LEVEL_INFO, strLog);
	CUtility::Trace(L"ERROR", strLog);

	if (GetMainWnd()->IsIconic())
	{
		GetMainWnd()->ShowWindow(SW_RESTORE);
	}

	GetMainWnd()->BringWindowToTop();
	GetMainWnd()->SetForegroundWindow();

	return AfxMessageBox(strMessage, nType);
}

CString CUtility::GetRootPath()
{
	return m_strRootPath;
}

void CUtility::SetRootPath(CString strPath)
{
	m_strRootPath = strPath;
}

CString CUtility::GetExePath()
{
	return m_strExePath;
}

void CUtility::SetExePath(CString strPath)
{
	m_strExePath = strPath;
}

CString CUtility::GetLogPath()
{
	return m_strLogPath;
}

void CUtility::SetLogPath(CString strPath)
{
	m_strLogPath = strPath;
}

CString CUtility::GetJSPath()
{
	return m_strJSPath;
}

void CUtility::SetJSPath(CString strPath)
{
	m_strJSPath = strPath;
}

CString CUtility::GetCheckListPath()
{
	return m_strCheckListPath;
}

void CUtility::SetCheckListPath(CString strPath)
{
	m_strCheckListPath = strPath;
}

CString CUtility::GetIniFilePath()
{
	return m_strIniFilePath;
}

void CUtility::SetIniFilePath(CString strPath)
{
	m_strIniFilePath = strPath;
}

CString CUtility::GetRuleFile()
{
	return m_strRuleFile;
}

void CUtility::SetRuleFile(CString strFile)
{
	m_strRuleFile = strFile;
}

CString CUtility::GetServicePath()
{
	return m_strServicePath;
}

void CUtility::SetServicePath(CString strPath)
{
	m_strServicePath = strPath;
}

CString CUtility::ReadIniData(CString strSection, CString strKey)
{
	wchar_t chPath[MAX_PATH] = { 0 };

	GetPrivateProfileString(strSection, strKey, L"", chPath, MAX_PATH, m_strIniFilePath);
	
	return (CString)chPath;
}

BOOL CUtility::GetUseJS()
{
	return m_bUseJS;
}

void CUtility::SetUseJS(BOOL bUseJS)
{
	m_bUseJS = bUseJS;
}

BOOL CUtility::GetDataLog()
{
	return m_bDataLog;
}

void CUtility::SetDataLog(BOOL bLog)
{
	m_bDataLog = bLog;
}

BOOL CUtility::GetHTTPLog()
{
	return m_bHTTPLog;
}

void CUtility::SetHTTPLog(BOOL bLog)
{
	m_bHTTPLog = bLog;
}

BOOL CUtility::GetTraceLog()
{
	return m_bTraceLog;
}

void CUtility::SetTraceLog(BOOL bLog)
{
	m_bTraceLog = bLog;
}

BOOL CUtility::GetCloseMode()
{
	return m_bCloseMode;
}

void CUtility::SetCloseMode(BOOL bCloseMode)
{
	m_bCloseMode = bCloseMode;
}

BOOL CUtility::GetSaveAs()
{
	return m_bSaveAs;
}

void CUtility::SetSaveAs(BOOL bSaveAs)
{
	m_bSaveAs = bSaveAs;
}

BOOL CUtility::GetCache()
{
	return m_bCache;
}

void CUtility::SetCache(BOOL bCache)
{
	m_bCache = bCache;
}

BOOL CUtility::GetClearFolder()
{
	return m_bClearFolder;
}

void CUtility::SetClearFolder(BOOL bClearFolder)
{
	m_bClearFolder = bClearFolder;
}

CString CUtility::GetVariantString(VARIANT vValue)
{
	CComVariant resultBSTR;
	if (SUCCEEDED(VariantChangeType(&resultBSTR, &vValue, 0, VT_BSTR)))
	{
		return (CString)resultBSTR.bstrVal;
	}

	CString strValue = L"";
	switch (vValue.vt) {
	case VT_I2:
		strValue.Format(L"%d", vValue.iVal);

		break;
	case VT_I4:
		strValue.Format(L"%d", vValue.lVal);

		break;
	case VT_R4:
	{
		strValue.Format(L"%f", vValue.fltVal);
		strValue.TrimRight(L"0");
		strValue.TrimRight(L".");

		break;
	}
	case VT_R8:
	{
		strValue.Format(L"%f", vValue.dblVal);
		strValue.TrimRight(L"0");
		strValue.TrimRight(L".");

		break;
	}
	case VT_CY:
		strValue.Format(L"%d", vValue.cyVal);

		break;
	case VT_DATE:
		strValue.Format(L"%f", vValue.date);

		break;
	case VT_BSTR:
		strValue.Format(L"%s", vValue.bstrVal);

		break;
	case VT_DISPATCH:
		strValue.Format(L"%d", vValue.pdispVal);

		break;
	case VT_ERROR:
		strValue.Format(L"%d", vValue.scode);

		break;
	case VT_BOOL:
		strValue.Format(L"%d", vValue.boolVal);

		break;
	case VT_VARIANT:
		strValue.Format(L"%d", vValue.pvarVal);

		break;
	case VT_UNKNOWN:
		strValue.Format(L"%d", vValue.punkVal);

		break;
	case VT_UI1:
		strValue.Format(L"%d", vValue.bVal);

		break;
	case VT_DECIMAL:
		strValue.Format(L"%d", vValue.decVal);

		break;
	case VT_I1:
		strValue.Format(L"%d", vValue.cVal);

		break;
	case VT_UI2:
		strValue.Format(L"%d", vValue.uiVal);

		break;
	case VT_UI4:
		strValue.Format(L"%d", vValue.ulVal);

		break;
	case VT_I8:
		strValue.Format(L"%f", (double)vValue.llVal);

		break;
	case VT_UI8:
		strValue.Format(L"%f", (double)vValue.ullVal);

		break;
	case VT_INT:
		strValue.Format(L"%d", vValue.intVal);

		break;
	case VT_UINT:
		strValue.Format(L"%d", vValue.uintVal);

	case VT_HRESULT:
		strValue.Format(L"%d", vValue.iVal);

		break;
	case VT_VOID:
		strValue.Format(L"%d", vValue.byref);

		break;
	case VT_LPSTR:
		strValue.Format(L"%s", vValue.pcVal);

		break;
	case VT_LPWSTR:
		strValue.Format(L"%s", vValue);

		break;
	case VT_EMPTY:
		strValue = L"";

		break;
	default:
		strValue.Format(L"%d", vValue.lVal);

		break;
	}

	return strValue;
}

CString CUtility::GetScaleData(CString strScale, CString strData)
{
	CString strDigit = strScale.Left(1);

	int nScale = strScale.GetLength();
	CString strTemp = strScale.Mid(1, nScale - 1);

	CString strMath = strScale.Right(1);

	int nMath = 0;
	if (strMath == L"+")
		nMath = 1;
	else if (strMath == L"-")
		nMath = 2;
	else
		strMath = L"";

	CString strLength = L"";
	if (nMath == 0)
		strLength = strScale.Mid(1, nScale - 1);
	else
		strLength = strScale.Mid(1, nScale - 2);

	CString strFormat = L"%.0f";
	BOOL bDecimal = TRUE;
	if (strDigit == L"-")
	{
		bDecimal = FALSE;
		strFormat.Format(L"%%.%sf", strLength);
	}

	INT_PTR nPow = _wtoi(strLength);
	INT_PTR nDigit = (INT_PTR)pow(10, nPow);

	DOUBLE dResult = 0;
	DOUBLE dData = _wtof(strData);
	switch (nMath)
	{
	case 0:
		if (bDecimal == TRUE)
		{
			dResult = round(dData / nDigit) * nDigit;
		}
		else
		{
			dResult = round(dData * nDigit) / nDigit;
			
		}
		break;
	case 1:
		if (bDecimal == TRUE)
		{
			dResult = ceil(dData / nDigit) * nDigit;
		}
		else
		{
			dResult = ceil(dData * nDigit) / nDigit;
		}
		break;
	case 2:
		if (bDecimal == TRUE)
		{
			dResult = floor(dData / nDigit) * nDigit;
		}
		else
		{
			dResult = floor(dData * nDigit) / nDigit;
		}
		break;
	default:
		break;
	}

	CString strReturn = L"";
	strReturn.Format(strFormat, dResult);

	return strReturn;
}

void CUtility::RemoveArrayStringArray(CArrayStringArray* pRemove)
{
	if (pRemove == NULL)
		return;

	INT_PTR nRemove = pRemove->GetCount();
	for (INT_PTR m = 0; m < nRemove; m++)
	{
		CStringArray* pArray = pRemove->GetAt(m);
		if (pArray != NULL)
		{
			pArray->RemoveAll();
			delete pArray;
		}
		pArray = NULL;
	}
	pRemove->RemoveAll();
}

void CUtility::RemoveArray2StringArray(CArray2StringArray* pRemove)
{
	if (pRemove == NULL)
		return;

	INT_PTR nRemove = pRemove->GetCount();
	for (INT_PTR m = 0; m < nRemove; m++)
	{
		CArrayStringArray* pArray = pRemove->GetAt(m);
		if (pArray != NULL)
		{
			RemoveArrayStringArray(pArray);

			pArray->RemoveAll();
			delete pArray;
		}
		pArray = NULL;
	}
	pRemove->RemoveAll();
}

void CUtility::RemoveMapStringArray(CMapStringArray* pRemove)
{
	if (pRemove == NULL)
		return;

	CStringArray* pArray = NULL;
	CString strKey = L"";
	POSITION pos = pRemove->GetStartPosition();
	while (pos)
	{
		pRemove->GetNextAssoc(pos, strKey, pArray);
		if (pArray != NULL)
		{
			pArray->RemoveAll();
			delete pArray;
		}
		pArray = NULL;
	}
	pRemove->RemoveAll();
}

void CUtility::RemoveMap2StringArray(CMap2StringArray* pRemove)
{
	if (pRemove == NULL)
		return;

	CArrayStringArray* pArray = NULL;
	CString strKey = L"";
	POSITION pos = pRemove->GetStartPosition();
	while (pos)
	{
		pRemove->GetNextAssoc(pos, strKey, pArray);
		if (pArray != NULL)
		{
			INT_PTR nCount = pArray->GetCount();
			for (INT_PTR i = 0; i < nCount; i++)
			{
				CStringArray* pArr = pArray->GetAt(i);
				if (pArr != NULL)
				{
					pArr->RemoveAll();
					delete pArr;
				}
				pArr = NULL;
			}
			pArray->RemoveAll();
			delete pArray;
		}
		pArray = NULL;
	}
	pRemove->RemoveAll();
}

void CUtility::RemoveMapMapStringArray(CMapMapStringArray* pRemove)
{
	if (pRemove == NULL)
		return;

	CMapStringArray* pMap = NULL;
	CString strKey = L"";
	POSITION pos = pRemove->GetStartPosition();
	while (pos)
	{
		pRemove->GetNextAssoc(pos, strKey, pMap);
		if (pMap != NULL)
		{
			CStringArray* pArray = NULL;
			CString strKey2 = L"";
			POSITION pos2 = pMap->GetStartPosition();
			while (pos2)
			{
				pMap->GetNextAssoc(pos2, strKey2, pArray);
				if (pArray != NULL)
				{
					pArray->RemoveAll();
					delete pArray;
				}
				pArray = NULL;
			}
			pMap->RemoveAll();
			delete pMap;
			pMap = NULL;
		}
	}
	pRemove->RemoveAll();
}

void CUtility::RemoveMapVariantMap(CMapVariantMap* pMap)
{
	if (pMap == NULL)
		return;

	CMapVariant* pVariantMap = NULL;
	CString strKey = L"";
	POSITION pos = pMap->GetStartPosition();
	while (pos)
	{
		pMap->GetNextAssoc(pos, strKey, pVariantMap);
		if (pVariantMap != NULL)
		{
			pVariantMap->RemoveAll();
			delete pVariantMap;
		}
		pVariantMap = NULL;
	}
	pMap->RemoveAll();
}

void CUtility::CopyArrayArray(CArrayStringArray* pArrSource, CArrayStringArray* pArrDest)
{
	if (pArrSource == NULL || pArrDest == NULL)
		return;

	INT_PTR nCount = pArrSource->GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		CStringArray* pArray = pArrSource->GetAt(i);
		if (pArray == NULL)
			continue;

		CStringArray* pNew = new CStringArray();
		pNew->Copy(*pArray);

		pArrDest->Add(pNew);
	}
}

INT_PTR CUtility::GetTokensByString(CString strData, CString strDel, CStringArray& arrRet, BOOL bAllowEmpty)
{
	if (strData == L"")
	{
		return 0;
	}

	int nDelLength = strDel.GetLength();
	while (strData.IsEmpty() == FALSE)
	{
		int nIndex = strData.Find(strDel);
		if (nIndex < 0)
		{
			arrRet.Add(strData);
			break;
		}
		CString strRet = strData.Left(nIndex);
		strRet.Trim();
		if (bAllowEmpty == TRUE || strRet.IsEmpty() == FALSE)
			arrRet.Add(strRet);

		strData = strData.Mid(nIndex + nDelLength);
	}

	return arrRet.GetCount();
}

INT_PTR	CUtility::GetTokensByChar(CString strData, CHAR chSep, CStringArray& arrRet, BOOL bAllowEmpty)
{
	if (strData == L"")
	{
		return 0;
	}

	while (strData.IsEmpty() == FALSE)
	{
		int nIndex = strData.Find(chSep);
		if (nIndex < 0)
		{
			arrRet.Add(strData);
			break;
		}
		CString strRet = strData.Left(nIndex);
		if (bAllowEmpty == TRUE || strRet.IsEmpty() == FALSE)
			arrRet.Add(strRet);

		strData = strData.Mid(nIndex + 1);
	}

	return arrRet.GetCount();
}

INT_PTR CUtility::StringToIndex(CString strCol)
{
	if (strCol == L"")
		return -1;

	INT_PTR ans = 0;
	int nSize = strCol.GetLength();
	for (int i = nSize - 1; i >= 0; i--)
	{
		ans = ans + (strCol.GetAt(i) - L'A' + 1) * (int)pow(26, nSize - i - 1);
	}
	return ans;
}

CString CUtility::IndexToString(INT_PTR nRow, INT_PTR nCol)
{
	CString strIndexToString = L"";
	if (nCol > 26)
		strIndexToString = CString((char)((INT_PTR)('A') + (INT_PTR)((nCol - 1) / 26) - 1));

	strIndexToString = strIndexToString + CString((char)(((INT_PTR)('A') + ((nCol - 1) % 26))));

	if (nRow != -1)
	{
		CString strRow = L"";
		strRow.Format(L"%d", nRow);

		strIndexToString = strIndexToString + strRow;
	}

	return strIndexToString;
}

void CUtility::Trace(CString strType, CString strData)
{
	CString strTrace = L"";
	//if (strType == L"JS")
	//{
	//	strTrace.Format(L"JS : %s", strData);
	//}
	//else
	//{
		strTrace.Format(L"%s : %s", strType, strData);
	//}

	SendCopyData(L"PerfecTwin-Trace", strTrace, 10001);

	//CWnd* pWnd = NULL;
	//pWnd = CWnd::FindWindow(NULL, L"PerfecTwin-Trace");
	//if (pWnd == NULL)
	//	return;

	//int nLength = strData.GetLength();
	//CString strTrace = L"";
	//if (strType == L"JS")
	//{
	//	strTrace = L"[JS] ";
	//}
	//else
	//{
	//	strTrace.Format(L"[%s] / length : %d\r\n", strType, nLength);
	//}

	//int nTraceLen = strTrace.GetLength();
	//TCHAR* pData = new TCHAR[nTraceLen + nLength + 1];
	//ZeroMemory(pData, (nTraceLen + nLength + 1) * sizeof(TCHAR));
	//CopyMemory(pData, strTrace.GetBuffer(0), nTraceLen * sizeof(TCHAR));
	//CopyMemory(pData + nTraceLen, strData, nLength * sizeof(TCHAR));

	//COPYDATASTRUCT copyData;
	//copyData.dwData = 10001;
	//copyData.cbData = nTraceLen + nLength + 1;
	//copyData.lpData = pData;

	//pWnd->SendMessage(WM_COPYDATA, 0, (LPARAM)&copyData);
	//delete[] pData;
	//pData = NULL;
}

INT_PTR CUtility::SendCopyData(CString strTarget, CString strSendData, INT_PTR nMessage)
{
	// 프로그램 가장 마지막에 호출되므로 함수 내 로그 찍으면 안됨
	INT_PTR nRet = 0;

	HWND hTarget = ::FindWindow(NULL, strTarget);
	if (hTarget != NULL)
	{
		CStringA strData = (CStringA)strSendData;
		INT_PTR nDataLen = strData.GetLength() + 1;
		char* chData = new char[nDataLen];
		ZeroMemory(chData, nDataLen);
		strcpy(chData, strData);

		char* p_buffer = NULL;
		if (nDataLen)
		{
			p_buffer = (char*)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, nDataLen);

			if (p_buffer != NULL)
				memcpy(p_buffer, chData, nDataLen);
		}

		COPYDATASTRUCT cpData;

		cpData.dwData = nMessage;
		cpData.cbData = (DWORD)nDataLen;
		cpData.lpData = p_buffer;
		if (IsWindow(hTarget))
		{
			nRet = ::SendMessage(hTarget, WM_COPYDATA, NULL, reinterpret_cast<LPARAM>(&cpData));
		}

		if (p_buffer != NULL)
		{
			::HeapFree(::GetProcessHeap(), 0, p_buffer);
		}

		delete[] chData;
	}

	return nRet;
}

BOOL CUtility::IsNumber(CString strInput)
{
	if (strInput == L"")
		return FALSE;

	int nLength = strInput.GetLength();
	for (int i = 0; i < nLength; i++)
	{
		TCHAR tch = strInput.GetAt(i);
		if (tch < '0' || tch > '9')
			return FALSE;
	}

	return TRUE;
}

void CUtility::SetStringToArrayData(CArrayStringArray* pArrayCase, INT_PTR nCaseIndex, CString strCell, CString strData, INT_PTR nStartCol/*=0*/)
{
	if (pArrayCase != NULL)
	{
		INT_PTR nCaseCount = pArrayCase->GetCount();
		if (nCaseCount > nCaseIndex && nCaseIndex > -1)
		{
			CStringArray* pCase = pArrayCase->GetAt(nCaseIndex);
			if (pCase != NULL)
			{
				INT_PTR nColumn = StringToIndex(strCell) - 1 - nStartCol;
				if (nColumn >= 0 && nColumn < pCase->GetCount())
				{
					pCase->SetAt(nColumn, strData);
				}
			}
		}
	}
}

CString CUtility::GetStringToArrayData(CArrayStringArray* pArrayCase, INT_PTR nCaseIndex, CString strCell, INT_PTR nStartCol/*=0*/)
{
	CString strData = L"";

	if (pArrayCase != NULL)
	{
		INT_PTR nCaseCount = pArrayCase->GetCount();
		if (nCaseCount > nCaseIndex && nCaseIndex > -1)
		{
			CStringArray* pCase = pArrayCase->GetAt(nCaseIndex);
			if (pCase != NULL)
			{
				INT_PTR nColumn = StringToIndex(strCell) - 1 - nStartCol;
				if (nColumn >= 0 && nColumn < pCase->GetCount())
				{
					strData = pCase->GetAt(nColumn);
				}
			}
		}
	}

	return strData;
}

CString CUtility::GetStringToArrayData(CArrayStringArray* pArrayCase, INT_PTR nCaseIndex, INT_PTR nColumn, INT_PTR nStartCol/*=0*/)
{
	CString strData = L"";

	if (pArrayCase != NULL)
	{
		INT_PTR nCaseCount = pArrayCase->GetCount();
		if (nCaseCount > nCaseIndex && nCaseIndex > -1)
		{
			CStringArray* pCase = pArrayCase->GetAt(nCaseIndex);
			if (pCase != NULL)
			{
				nColumn = nColumn - nStartCol;
				if (nColumn >= 0 && nColumn < pCase->GetCount())
				{
					strData = pCase->GetAt(nColumn);
				}
			}
		}
	}

	return strData;
}

CString CUtility::GetDataFromArray(CStringArray* pArrayCase, CString strCell, INT_PTR nStartCol/*=0*/)
{
	CString strData = L"";

	if (pArrayCase != NULL)
	{
		INT_PTR nColumn = StringToIndex(strCell) - 1 - nStartCol;
		if (nColumn >= 0 && nColumn < pArrayCase->GetCount())
		{
			strData = pArrayCase->GetAt(nColumn);
		}
	}

	return strData;
}

// CArrayStringArray 에서 특정 index만 추출한 배열 얻기
CStringArray* CUtility::GetArrayFrom2ArrayByCol(CArrayStringArray* pArray, INT_PTR nColumn, INT_PTR nStartRow/*=0*/, INT_PTR nStartCol/*=0*/)
{
	if (pArray == NULL)
		return NULL;

	CStringArray* pColArray = new CStringArray();

	INT_PTR nCount = pArray->GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		CStringArray* pArr = pArray->GetAt(i);
		if (pArr == NULL)
			continue;

		if (nStartRow > i)
			continue;

		INT_PTR nCol = nColumn - nStartCol;
		if (nCol >= 0 && nCol < pArr->GetCount())
		{
			pColArray->Add(pArr->GetAt(nCol));
		}
	}

	if (pColArray->GetCount() == 0)
	{
		delete pColArray;
		pColArray = NULL;
	}

	return pColArray;
}

// CArrayStringArray 에서 특정 index의 시작열부터 끝열까지 추출한 배열 얻기
CStringArray* CUtility::GetArrayFrom2ArrayByStartEnd(CArrayStringArray* pArray, INT_PTR nCaseIndex, INT_PTR nStartCol, INT_PTR nEndCol, INT_PTR nStandardCol/*=0*/)
{
	if (pArray == NULL)
		return NULL;

	INT_PTR nCount = pArray->GetCount();
	if (nCount <= nCaseIndex)
		return NULL;

	CStringArray* pArr = pArray->GetAt(nCaseIndex);
	if (pArr == NULL)
		return NULL;

	CStringArray* pArrResult = new CStringArray();

	nCount = pArr->GetCount();
	for (INT_PTR i = nStartCol; i <= nEndCol; i++)
	{
		pArrResult->Add(pArr->GetAt(i));
	}

	return pArrResult;
}

// CArrayStringArray 에서 특정 index의 시작열부터 끝열까지 추출한 배열 얻기
CStringArray* CUtility::GetArrayFrom2ArrayByRange(CArrayStringArray* pArray, INT_PTR nCaseIndex, CString strStartCol, CString strEndCol, INT_PTR nStandardCol/*=0*/)
{
	if (pArray == NULL)
		return NULL;

	INT_PTR nCount = pArray->GetCount();
	if (nCount <= nCaseIndex)
		return NULL;

	CStringArray* pArr = pArray->GetAt(nCaseIndex);
	if (pArr == NULL)
		return NULL;

	CStringArray* pArrResult = new CStringArray();

	INT_PTR nStartCol = StringToIndex(strStartCol) - 1 - nStandardCol;
	INT_PTR nEndCol = StringToIndex(strEndCol) - 1 - nStandardCol;

	nCount = pArr->GetCount();

	for (INT_PTR i = nStartCol; i <= nEndCol; i++)
	{
		if (i < nCount)
			pArrResult->Add(pArr->GetAt(i));
	}

	return pArrResult;
}

CString CUtility::GetFindFileName(CString strSearchDir, CString strSearchKey)
{
	CString strLog = L"";
	strLog.Format(L"GetFindFileName / Search Dir : %s, Search Key : %s", strSearchDir, strSearchKey);
	CUtility::LogWrite(LEVEL_DEBUG, strLog);

	CFileFind fFinder;
	CString strFindFileName = L"";
	CString strFileName = L"";

	BOOL bRet = fFinder.FindFile(strSearchDir + "*.*");
	while (bRet)
	{
		bRet = fFinder.FindNextFile();
		if (fFinder.IsDots() == TRUE)
			continue;

		strFileName = fFinder.GetFileName();
		if (fFinder.IsDirectory() == TRUE)
		{
			continue;
		}
		else if (fFinder.IsArchived() == TRUE)
		{
			if (strFileName.Find(strSearchKey) != -1)
			{
				strFindFileName = strFileName;

				break;
			}
		}
	}

	fFinder.Close();

	CUtility::LogWrite(LEVEL_DEBUG, L"GetFindFileName / End");

	return strFindFileName;
}

CStringArray* CUtility::FindMapFromString(CMapStringArray& mapFind, CString strFind, INT_PTR nStandard)
{
	CString strKey = L"";
	CStringArray* pArray = NULL;
	POSITION pos = mapFind.GetStartPosition();
	while (pos)
	{
		mapFind.GetNextAssoc(pos, strKey, pArray);
		if (pArray != NULL)
		{
			INT_PTR nCount = pArray->GetCount();
			if (nCount > nStandard)
			{
				CString strTemp = pArray->GetAt(nStandard);
				if (strTemp == strFind)
				{
					return pArray;
				}
			}
		}
	}

	return NULL;
}

CStringArray* CUtility::FindArrayFromString(CArrayStringArray& arrFind, CString strFind, INT_PTR nStandard)
{
	INT_PTR nCount = arrFind.GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		CStringArray* pTemp = arrFind.GetAt(i);
		if (pTemp == NULL)
			continue;

		CString strTemp = pTemp->GetAt(nStandard);
		if (strTemp == strFind)
		{
			return pTemp;
		}
	}

	return NULL;
}

INT_PTR CUtility::FindIndexFromString(CStringArray& arrFind, CString strFind)
{
	INT_PTR nCount = arrFind.GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		CString pTemp = arrFind.GetAt(i);
		if (pTemp == strFind)
			return i;
	}

	return -1;
}

INT_PTR CUtility::FindIndexFromString(CArrayStringArray& arrFind, CString strFind, INT_PTR nStandard)
{
	INT_PTR nCount = arrFind.GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		CStringArray* pTemp = arrFind.GetAt(i);
		if (pTemp == NULL)
			continue;

		CString strTemp = pTemp->GetAt(nStandard);
		if (strTemp == strFind)
		{
			return i;
		}
	}

	return -1;
}

// 행과 열로 구성되어 있는지 체크(행:3, 열:A -> A3)
BOOL CUtility::IsValidCell(CString strCell)
{
	if (strCell.GetLength() < 2)
		return FALSE;

	TCHAR tchStart = strCell.GetAt(0);
	if (tchStart < 'A' && tchStart > 'Z')
		return FALSE;

	TCHAR tchEnd = strCell.GetAt(strCell.GetLength() - 1);
	if (tchEnd < '0' || tchEnd > '9')
		return FALSE;

	return TRUE;
}

// 행과 열로 구성되어 있는지 체크(행:3, 열:A -> A3)
BOOL CUtility::IsValidCell(CStringArray* pArray)
{
	if (pArray == NULL)
		return FALSE;

	INT_PTR nCount = pArray->GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		CString strCell = pArray->GetAt(i);
		if (strCell.GetLength() < 2)
			return FALSE;

		TCHAR tchStart = strCell.GetAt(0);
		if (tchStart < 'A' && tchStart > 'Z')
			return FALSE;

		TCHAR tchEnd = strCell.GetAt(strCell.GetLength() - 1);
		if (tchEnd < '0' || tchEnd > '9')
			return FALSE;
	}

	return TRUE;
}

CString CUtility::GetFixData(CString strData)
{
	CString strResult = L"";

	return strResult;
}

CArrayStringArray* CUtility::GetCheckListArray(CString strKey)
{
	CArrayStringArray* pArray = NULL;
	m_mapCheckListArray.Lookup(strKey, pArray);

	return pArray;
}

void CUtility::SetCheckListArray(CString strKey, CArrayStringArray* pArray)
{
	m_mapCheckListArray.SetAt(strKey, pArray);
}

void CUtility::RemoveCheckListArray()
{
	RemoveMap2StringArray(&m_mapCheckListArray);
}

BOOL CUtility::LoadResourceImage(CImage* pImg, int nResourceID, LPCTSTR pType)
{
	BOOL bRet = FALSE;
	IStream* pStream = NULL;

	// 이미 load된것 있으면 해제
	pImg->Destroy();

	if (pType == RT_BITMAP)	// Bitmpa 일때, png / jpg 처럼하면 error 나더군요. ㅠㅠ. 그럴이유도 없겠지만
	{
		pImg->LoadFromResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(nResourceID));

		return TRUE;
	}

	HRSRC hResource = ::FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(nResourceID), pType);

	if (!hResource) return FALSE;

	DWORD nImageSize = ::SizeofResource(AfxGetInstanceHandle(), hResource);

	if (!nImageSize) return FALSE;

	HGLOBAL hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, nImageSize);

	if (hBuffer)
	{
		void* pSource = ::LockResource(::LoadResource(AfxGetInstanceHandle(), hResource));

		if (!pSource) return FALSE;

		void* pDest = ::GlobalLock(hBuffer);

		if (pDest)
		{
			CopyMemory(pDest, pSource, nImageSize);

			if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
			{
				//if (bUseGdiplus)
				//{
				//	// Gdiplus - 이용하는 방식 적용해볼려구요 
				//}
				//else
				{
					if (SUCCEEDED(pImg->Load(pStream)))	//  Gif / Png 일때
					{
						//SetWindowPos(NULL, 0, 0, pImg->GetWidth(), pImg->GetHeight(), SWP_NOMOVE | SWP_NOZORDER);

						bRet = TRUE;
					}
				}

				pStream->Release();
			}

			::GlobalUnlock(hBuffer);
		}

		::GlobalFree(hBuffer);
		hBuffer = NULL;
	}

	if (pImg->IsNull())
		return FALSE;

	if (pImg->GetBPP() == 32)
	{
		CStatic* pImgCtrl = (CStatic*)GetMainWnd()->GetDlgItem(nResourceID);
		//if (strFileName.Right(3).CompareNoCase(_T("png")) == 0)
		{
			unsigned char* pCol = 0;
			long w = pImg->GetWidth();
			long h = pImg->GetHeight();
			for (long y = 0; y < h; y++)
			{
				for (long x = 0; x < w; x++)
				{
					pCol = (unsigned char*)pImg->GetPixelAddress(x, y);
					unsigned char alpha = pCol[3];
					if (alpha != 255)
					{
						pCol[0] = ((pCol[0] * alpha) + 128) >> 8;
						pCol[1] = ((pCol[1] * alpha) + 128) >> 8;
						pCol[2] = ((pCol[2] * alpha) + 128) >> 8;
					}
				}
			}
		}
		pImg->SetHasAlphaChannel(true);
	}

	return TRUE;
}

void CUtility::SetShowMessage(BOOL bValue)
{
	m_bShowMessage = bValue;
}

CFont* CUtility::ConvertLOGFontToCFont(LOGFONT lFont)
{
	CFont* pTmpFont = NULL;
	CString strFont = L"";
	strFont.Format(L"%s;%d;%s;%s;%s", lFont.lfFaceName, lFont.lfHeight, lFont.lfWeight == 700 ? "Bold" : "", lFont.lfItalic == 1 ? "Italic" : "", lFont.lfUnderline == 1 ? "Underline" : "");

	if (m_mapFont.Lookup(strFont, (void*&)pTmpFont))
	{
		return pTmpFont;
	}

	CFont* pFont = new CFont();
	pFont->CreateFontIndirect(&lFont);

	m_mapFont.SetAt(strFont, pFont);

	return pFont;
}

