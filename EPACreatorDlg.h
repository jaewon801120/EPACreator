
// EPACreatorDlg.h: 헤더 파일
//

#pragma once

#include <jni.h>


#define  GetMainWnd()	((CEPACreatorDlg*)theApp.m_pMainWnd)

struct ControlDetail
{
	int ID;
	char Name[100];
	char IP[100];
	int Port;
};

struct WorkOrder
{
	char		sumSerialId[20];
	char		accessNumber[18];
	char		actionType[4];
	char		effectiveDate[24];
	char		fetchFlag[2];
	char		reason[456];
	char		accessSource[100];
};

// CEPACreatorDlg 대화 상자
class CEPACreatorDlg : public CDialogEx
{
// 생성입니다.
public:
	CEPACreatorDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EPACREATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

public:
	JavaVM* m_jvm;

	CString m_strDirPath;

public:
	JNIEnv* create_vm();
	afx_msg void OnBnClickedButton2();
	HANDLE SpawnAndRedirect(CString strCommandLine, HANDLE* hStdOutputReadPipe, LPCTSTR lpCurrentDirectory);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
