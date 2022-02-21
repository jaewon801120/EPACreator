
// EPACreatorDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "EPACreator.h"
#include "EPACreatorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CEPACreatorDlg 대화 상자



CEPACreatorDlg::CEPACreatorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EPACREATOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_jvm = NULL;

	m_strDirPath = L"";
}

void CEPACreatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEPACreatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CEPACreatorDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CEPACreatorDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CEPACreatorDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CEPACreatorDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CEPACreatorDlg 메시지 처리기

BOOL CEPACreatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CEPACreatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CEPACreatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CEPACreatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

JNIEnv* CEPACreatorDlg::create_vm()
{
	JNIEnv* env = NULL;
	
	return env;
}

void CEPACreatorDlg::OnBnClickedButton1()
{
	JNIEnv* env = NULL;
	JavaVMInitArgs vm_args;
	memset(&vm_args, 0, sizeof(vm_args));
	JavaVMOption options;
	memset(&options, 0, sizeof(options));
	options.optionString = "-Djava.class.path=D:/Download/jar/SHLifeParse/jnitest.jar"; //Path to the java source code
	vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
	vm_args.nOptions = 1;
	vm_args.options = &options;
	vm_args.ignoreUnrecognized = JNI_TRUE;

	int ret = JNI_CreateJavaVM(&m_jvm, (void**)&env, &vm_args);
	if (ret < 0)
		printf("\nUnable to Launch JVM\n");

	if (env == NULL)
		return;

	struct ControlDetail ctrlDetail;
	ctrlDetail.ID = 11;
	strcpy(ctrlDetail.Name, "HR-HW");
	strcpy(ctrlDetail.IP, "10.32.164.133");
	ctrlDetail.Port = 9099;

	printf("Struct Created in C has values:\nID:%d\nName:%s\n IP:%s\nPort:%d\n", ctrlDetail.ID, ctrlDetail.Name, ctrlDetail.IP, ctrlDetail.Port);

	/********************************************************/
	struct WorkOrder WO[2];
	strcpy(WO[0].sumSerialId, "2000");
	strcpy(WO[0].accessNumber, "2878430");
	strcpy(WO[0].actionType, "04");
	strcpy(WO[0].effectiveDate, "25-12-2007 12:20:30 PM");
	strcpy(WO[0].fetchFlag, "0");
	strcpy(WO[0].reason, "Executed Successfully");
	strcpy(WO[0].accessSource, "PMS");
	strcpy(WO[1].sumSerialId, "1000");
	strcpy(WO[1].accessNumber, "2878000");
	strcpy(WO[1].actionType, "T4");
	strcpy(WO[1].effectiveDate, "25-12-2007 11:20:30 PM");
	strcpy(WO[1].fetchFlag, "0");
	strcpy(WO[1].reason, "");
	strcpy(WO[1].accessSource, "RMS");


	jclass clsH = NULL;
	jclass clsC = NULL;
	jclass clsW = NULL;
	jclass clsR = NULL;
	jmethodID midMain = NULL;
	jmethodID midCalling = NULL;
	jmethodID midDispStruct = NULL;
	jmethodID midDispStructArr = NULL;
	jmethodID midRetObjFunc = NULL;
	jmethodID midCtrlDetConst = NULL;
	jmethodID midWoConst = NULL;

	jobject jobjDet = NULL;
	jobject jobjRetData = NULL;
	jobjectArray jobjWOArr = NULL;

	//Obtaining Classes
	clsH = env->FindClass("SHLife/jnitest");
	clsC = env->FindClass("SHLife/ControlNEDetail");
	clsW = env->FindClass("SHLife/WorkOrder");

	//Obtaining Method IDs
	if (clsH != NULL)
	{
		midMain = env->GetStaticMethodID(clsH, "main", "([Ljava/lang/String;)V");
		midCalling = env->GetStaticMethodID(clsH, "TestCall", "(Ljava/lang/String;)V");
		midDispStruct = env->GetStaticMethodID(clsH, "DisplayStruct", "(LSHLife/ControlNEDetail;)I");
		midDispStructArr = env->GetStaticMethodID(clsH, "DisplayStructArray", "([LSHLife/WorkOrder;)V");
		midRetObjFunc = env->GetStaticMethodID(clsH, "ReturnObjFunc", "()Ljava/lang/Object;");
	}
	else
	{
		printf("\nUnable to find the requested class\n");
	}

	if (clsC != NULL)
	{
		//Get constructor ID for ControlDetail
		midCtrlDetConst = env->GetMethodID(clsC, "<init>", "(ILjava/lang/String;Ljava/lang/String;I)V");
	}
	else
	{
		printf("\nUnable to find the requested class\n");
	}

	if (clsW != NULL)
	{
		//Get Constructor ID for WorkOrder
		midWoConst = env->GetMethodID(clsW, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	}
	else
	{
		printf("\nUnable to find the requested class\n");
	}

	/************************************************************************/
	/* Now we will call the functions using the their method IDs			*/
	/************************************************************************/
	if (midMain != NULL)
		env->CallStaticVoidMethod(clsH, midMain, NULL); //Calling the main method.

	if (midCalling != NULL)
	{
		jstring StringArg = env->NewStringUTF("\nTestCall:Called from the C Program\n");
		//Calling another static method and passing string type parameter
		env->CallStaticVoidMethod(clsH, midCalling, StringArg);
	}

	printf("\nGoing to Call DisplayStruct\n");
	if (midDispStruct != NULL)
	{
		if (clsC != NULL && midCtrlDetConst != NULL)
		{
			jstring StringArgName = env->NewStringUTF(ctrlDetail.Name);
			jstring StringArgIP = env->NewStringUTF(ctrlDetail.IP);

			//Creating the Object of ControlDetail.
			jobjDet = env->NewObject(clsC, midCtrlDetConst, (jint)ctrlDetail.ID, StringArgName, StringArgIP, (jint)ctrlDetail.Port);
		}

		if (jobjDet != NULL && midDispStruct != NULL)
			env->CallStaticIntMethod(clsH, midDispStruct, jobjDet); //Calling the method and passing ControlDetail Object as parameter
	}

	//Calling a function from java and passing Structure array to it.
	printf("\n\nGoing to call DisplayStructArray From C\n\n");
	if (midDispStructArr != NULL)
	{
		if (clsW != NULL && midWoConst != NULL)
		{
			//Creating the Object Array that will contain 2 structures.
			jobjWOArr = (jobjectArray)env->NewObjectArray(2, clsW, env->NewObject(clsW, midWoConst, env->NewStringUTF(""), env->NewStringUTF(""), env->NewStringUTF(""),
				env->NewStringUTF(""), env->NewStringUTF(""), env->NewStringUTF(""), env->NewStringUTF("")));
			//Initializing the Array
			for (int i = 0; i < 2; i++)
			{
				env->SetObjectArrayElement(jobjWOArr, i, env->NewObject(clsW, midWoConst, env->NewStringUTF(WO[i].sumSerialId),
					env->NewStringUTF(WO[i].accessNumber),
					env->NewStringUTF(WO[i].actionType),
					env->NewStringUTF(WO[i].effectiveDate),
					env->NewStringUTF(WO[i].fetchFlag),
					env->NewStringUTF(WO[i].reason),
					env->NewStringUTF(WO[i].accessSource)));
			}
		}
		//Calling the Static method and passing the Structure array to it.
		if (jobjWOArr != NULL && midDispStructArr != NULL)
			env->CallStaticVoidMethod(clsW, midDispStructArr, jobjWOArr);
	}

	//Calling a Static function that return an Object
	if (midRetObjFunc != NULL)
	{
		//Calling the function and storing the return object into jobject type variable
		//Returned object is basically a structure having two fields (string and integer)
		jobjRetData = (jobject)env->CallStaticObjectMethod(clsH, midRetObjFunc, NULL);
		//Get the class of object
		clsR = env->GetObjectClass(jobjRetData);
		//Obtaining the Fields data from the returned object
		jint nRet = env->GetIntField(jobjRetData, env->GetFieldID(clsR, "returnValue", "I"));
		jstring jstrLog = (jstring)env->GetObjectField(jobjRetData, env->GetFieldID(clsR, "Log", "Ljava/lang/String;"));
		const char* pLog = env->GetStringUTFChars(jstrLog, 0);

		printf("\n\nValues Returned from Object are:\nreturnValue=%d\nLog=%s", nRet, pLog);
		//After using the String type data release it.
		env->ReleaseStringUTFChars(jstrLog, pLog);
	}

	//Release resources.
	int n = m_jvm->DestroyJavaVM();
}


void CEPACreatorDlg::OnBnClickedButton2()
{
	CString strCommandLine = L"";
	GetDlgItemText(IDC_EDT_COMMANDLINE, strCommandLine);

	HANDLE hOutput, hProcess;
	for (INT_PTR i = 0; i < 10; i++)
	{
		hProcess = SpawnAndRedirect(strCommandLine, &hOutput, NULL);
		if (!hProcess)
			return;
	}

	// (in a real program, you would put the following in a separate thread so your GUI thread is not blocked)
	//CHAR buffer[1024];
	//DWORD read;
	//while (ReadFile(hOutput, buffer, 1024, &read, NULL))
	//{
	//	buffer[read] = '\0';
	//	//TRACE(buffer);
	//}

	CloseHandle(hOutput);
	CloseHandle(hProcess);

	//AfxMessageBox((CString)buffer);
}

HANDLE CEPACreatorDlg::SpawnAndRedirect(CString strCommandLine, HANDLE* hStdOutputReadPipe, LPCTSTR lpCurrentDirectory)
{
	HANDLE hStdOutputWritePipe, hStdOutput, hStdError;
	CreatePipe(hStdOutputReadPipe, &hStdOutputWritePipe, NULL, 0);      // create a non-inheritable pipe
	DuplicateHandle(GetCurrentProcess(), hStdOutputWritePipe,
		GetCurrentProcess(), &hStdOutput,     // duplicate the "write" end as inheritable stdout
		0, TRUE, DUPLICATE_SAME_ACCESS);
	DuplicateHandle(GetCurrentProcess(), hStdOutput,
		GetCurrentProcess(), &hStdError,      // duplicate stdout as inheritable stderr
		0, TRUE, DUPLICATE_SAME_ACCESS);
	CloseHandle(hStdOutputWritePipe);                                                         // no longer need the non-inheritable "write" end of the pipe

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);       // (this is bad on a GUI app)
	si.hStdOutput = hStdOutput;
	si.hStdError = hStdError;
	si.wShowWindow = SW_HIDE;                                           // IMPORTANT: hide subprocess console window
	//TCHAR commandLineCopy[] = _T("atprogram -t atmelice -s J41800090028 -i updi -d attiny1616 program -f D:\project\program.elf");
	//TCHAR commandLineCopy[1024];                                      // CreateProcess requires a modifiable buffer
	//_tcscpy(commandLineCopy, commandLine);
	if (!CreateProcess(NULL, (LPWSTR)strCommandLine.GetBuffer(), NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE, NULL, lpCurrentDirectory, &si, &pi))
	{
		CloseHandle(hStdOutput);
		CloseHandle(hStdError);
		CloseHandle(*hStdOutputReadPipe);
		*hStdOutputReadPipe = INVALID_HANDLE_VALUE;

		return NULL;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitCode;
	if (GetExitCodeProcess(pi.hProcess, &exitCode))
	{
		switch(exitCode)
		{
		case STILL_ACTIVE:
			break;
		default:
			break;
		}
	}

	CloseHandle(pi.hThread);
	CloseHandle(hStdOutput);
	CloseHandle(hStdError);

	return pi.hProcess;
}

void CEPACreatorDlg::OnBnClickedButton3()
{
	JNIEnv* env = NULL;
	JavaVMInitArgs vm_args;
	memset(&vm_args, 0, sizeof(vm_args));
	JavaVMOption options;
	memset(&options, 0, sizeof(options));
	options.optionString = "-Djava.class.path=D:/Download/jar/SHLifeParse/NexacroParser.jar"; //Path to the java source code
	vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
	vm_args.nOptions = 1;
	vm_args.options = &options;
	vm_args.ignoreUnrecognized = JNI_TRUE;

	int ret = JNI_CreateJavaVM(&m_jvm, (void**)&env, &vm_args);
	if (ret < 0)
		printf("\nUnable to Launch JVM\n");

	if (env == NULL)
		return;

	char* pchSSV = CUtility::ReadFileChar(L"D:\\병행검증솔루션\\Project\\신한오렌지\\PerfecTwin\\case\\nexacro\\sni\\ssv.dat");
	if (pchSSV == NULL)
		return;

	jclass clsN = NULL;
	jmethodID funcData = NULL;

	clsN = env->FindClass("com/lgcns/testpilot/common/mapping/parser/nexacro/NexacroParserTest");

	if (clsN != NULL)
	{
		funcData = env->GetStaticMethodID(clsN, "getDatafromSSV", "(Ljava/lang/String;)Ljava/lang/String;");
		if (funcData != NULL)
		{
			jstring StringData = env->NewStringUTF(pchSSV);
			jstring returnString = (jstring)env->CallStaticObjectMethod(clsN, funcData, StringData);
			const char* pchData = env->GetStringUTFChars(returnString, 0);

			CUtility::WriteFileData(L"D:\\병행검증솔루션\\Project\\신한오렌지\\PerfecTwin\\case\\nexacro\\sni\\ssv_parse.dat", (char*)pchData);

			env->ReleaseStringUTFChars(returnString, pchData);

			//jbyteArray returnByte = (jbyteArray)env->CallStaticObjectMethod(clsN, funcData, StringData);
			//int returnLength = env->GetArrayLength(returnByte);
			//std::string data;
			//data.reserve(returnLength);
			//env->GetByteArrayRegion(returnByte, 0, returnLength, reinterpret_cast<jbyte*>(&data[0]));
			//env->DeleteLocalRef(returnByte);

			//jobject objReturn = (jobject)env->CallStaticObjectMethod(clsN, funcData, StringData);
			//jclass clsR = NULL;
			//clsR = env->GetObjectClass(objReturn);
			//jfieldID fldRet = env->GetFieldID(clsR, "value", "Ljava/lang/String;");
			//jstring jstrLog = (jstring)env->GetCharField(objReturn, fldRet);
			//const char* pLog = env->GetStringUTFChars(jstrLog, 0);

			//env->ReleaseStringUTFChars(jstrLog, pLog);
		}
	}

	delete[] pchSSV;

	int n = m_jvm->DestroyJavaVM();
}

void CEPACreatorDlg::OnBnClickedButton4()
{
	//m_strDirPath = L"D:\\workspace_c\\perfectwin-excel\\epa\\EPACreator\\x64\\Debug\\";
	m_strDirPath = L"..\\";

	CString strHarPath = m_strDirPath + L"test.har";
	FILE* fp = NULL;
	fopen_s(&fp, (CStringA)strHarPath, "r");
	if (!fp)
	{
		//CUtility::LogWrite(LEVEL_ERROR, L"InitMessageSetting / message.json 파일 읽기 실패");
		return;
	}

	fseek(fp, 0, SEEK_END);
	int nFileSize = ftell(fp);

	char* buffer = new char[nFileSize + 1];
	ZeroMemory(buffer, nFileSize + 1);

	FileReadStream fs(fp, buffer, sizeof(buffer));
	Document document;
	document.ParseStream(fs);
	fclose(fp);

	if (document.HasParseError()) {
		//CString strError = ConvertMultibyteToUnicode((char*)GetParseError_En(document.GetParseError()));

		//CString strLog = L"";
		//strLog.Format(L"InitMessageSetting / Error offset %u: %s", static_cast<unsigned>(document.GetErrorOffset()), strError);
		//LogWrite(LEVEL_ERROR, strLog);

		return;
	}

	const Value& a = document["entries"];	// Using a reference for consecutive access is handy and faster.
	assert(a.IsArray());
	for (SizeType i = 0; i < a.Size(); i++)	// rapidjson uses SizeType instead of size_t.
	{

	}

	//for (auto& v : document.GetArray())
	//{
	//	if (v.GetType() == kObjectType)
	//	{
	//		const Value& vCode = v["code"];
	//		CString strCode = ConvertUTF8ToUnicode((char*)vCode.GetString());
	//		const Value& vMessage = v["message"];
	//		CString strMessage = ConvertUTF8ToUnicode((char*)vMessage.GetString());

	//		m_mapMessage.SetAt(strCode, strMessage);
	//	}
	//}

}
