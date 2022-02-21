// Base64.cpp: implementation of the CBase64 class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Base64.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBase64::CBase64()
{

}

CBase64::~CBase64()
{

}

/************************************************************************************************

	buffer size
	 - 3byte�� 6bit�� 4���� ������ ���� ������ �ڽź��� ū 3�� ����� �����.
		ex) 10byte -> 3byte * 3 + 1 byte -> 3byte * 3 + 3byte = 4 * 3 + 4 + 1
		10byte�� 3byte 3���� 1byte�̰�, ������ 1byte�� padding�Ͽ� 3byte�� �����.
		�̴� 12byte�� �Ǹ� encoding �� 16���� ascii�� ��ȯ�ȴ�.
		������ null pointer�� ���Ͽ� 1 �߰�
		����, 17
		nLength = 10 ;
		int nSize = (10 + 3 - 10 % 3) * 8 / 6 + 1 = 17 

	while()
	1.	ù ����Ʈ�� ���� 6��Ʈ
		: szCur[0] >> 2
	2.	ù ����Ʈ�� ���� 2��Ʈ + �ι�° ����Ʈ�� ���� 4��Ʈ
		- & 0x03 -> & 0x00000011 : ���� 2 ��Ʈ ����
		: ((szCur[0] & 0x03) << 4) + (szCur[1] >> 4)
	3.	�ι�° ����Ʈ�� ���� 4��Ʈ + ����° ����Ʈ�� ���� 2��Ʈ
		- & 0x0f -> & 0x00001111 : ���� 4 ��Ʈ ����
		: ((szCur[1] & 0x0f) << 2) + (szCur[2] >> 6)
	4.	����° ����Ʈ�� ���� 6��Ʈ
		- & 0x3f -> & 0x00111111 : ���� 6 ��Ʈ ����
		: szCur[2] & 0x3f

	ex) input	: 0x14fb9c03d9
		8-bit	: 00010100 11111011 10011100  | 00000011 11011001 00(padding : 40 bit -> 42bit(multiples of 6))
		6-bit	: 000101 001111 101110 011100 | 000000 111101 100100
		Decimal	:      5     15     46     28 |      0     61     36
		Output	:	   F      P      u      c |      A      9      k      = (padding)

************************************************************************************************/

unsigned char* CBase64::base64encode(const unsigned char* sz, int nLength, int& nRet) 
{
	const unsigned char* szCur = sz ;
	int i = 0 ;
	int nSize = (nLength + 3 - nLength % 3) * 8 / 6 + 1/*\0*/ ;
	unsigned char* szRet = new unsigned char[nSize] ;

	while(2 < nLength)
	{
		szRet[i++] = szBase64[szCur[0] >> 2] ;
		szRet[i++] = szBase64[((szCur[0] & 0x03) << 4) + (szCur[1] >> 4)] ;
		szRet[i++] = szBase64[((szCur[1] & 0x0f) << 2) + (szCur[2] >> 6)] ;
		szRet[i++] = szBase64[szCur[2] & 0x3f] ;

	//	3����Ʈ �̵�
		szCur += 3 ;
	//	���� 3��ŭ ����
		nLength -= 3 ;
	}

//	�ܿ� ����Ʈ�� ���� ���
	if(0 != nLength)
	{
		szRet[i++] = szBase64[szCur[0] >> 2] ;
		if(1 < nLength)
		{
			szRet[i++] = szBase64[((szCur[0] & 0x03) << 4) + (szCur[1] >> 4)] ;
		//	���� 4 ��Ʈ ���� ��, 2 ��Ʈ�� 0���� padding
			szRet[i++] = szBase64[(szCur[1] & 0x0f) << 2] ;
		//	������ ���ڴ� padding
			szRet[i++] = szBase64Pad ;
		}

		else	//	(1 == nLength)
		{
		//	���� 2 ��Ʈ ���� ��, 4 ��Ʈ�� 0���� padding
			szRet[i++] = szBase64[(szCur[0] & 0x03) << 4] ;
		//	������ 2 ���ڴ� padding
			szRet[i++] = szBase64Pad ;
			szRet[i++] = szBase64Pad ;
		}
	}

	nRet = i ;
	szRet[i] = '\0' ;

	return szRet ;
}

unsigned char* CBase64::base64decode(const unsigned char* sz, int nLength, int& nRet)
{
	const unsigned char* szCur = sz ;
	int nSize = 128, i, j = 0 ;
	char ch ;
	unsigned char* szMap = new unsigned char[nSize] ;
	for(i=0 ; i<nSize ; i++)
		szMap[i] = -1 ;

	for(i=0 ; i<64 ; i++)
		szMap[szBase64[i]] = (unsigned char)i ;

	unsigned char* szRet = new unsigned char[nLength + 1] ;

	i = 0 ;
	while(1)
	{
		ch = *(szCur++) ;
		if('\0' == ch || szBase64Pad == ch)
			break ;

		if(' ' == ch)
			ch = '+' ;

		ch = szMap[ch] ;
		if(0 > ch)
			continue ;

		switch(i % 4)
		{
		case 0 :
			szRet[j] = ch << 2 ;
			break ;
		case 1 :
			szRet[j++] |= ch >> 4 ;
			szRet[j] = (ch & 0x0f) << 4 ;
			break ;
		case 2 :
			szRet[j++] |= ch >> 2 ;
			szRet[j] = (ch & 0x03) << 6 ;
			break ;
		case 3 :
			szRet[j++] |= ch ;
			break ;
		}
		i++ ;
	}
	delete[] szMap;
	int k = j ;
	if(szBase64Pad == ch)
	{
		switch(i % 4)
		{
		case 0 :
		case 1 :
			delete [] szRet ;
			return NULL ;
		case 2 :
			k++ ;
		case 3 :
			szRet[k++] = 0 ;
		}
	}

	nRet = j ;

	return szRet ;
}

CString CBase64::base64encode(const CString& msg)
{
	unsigned char* pMsg=(unsigned char*)(LPCTSTR)msg;
	int encodeLength=0;
	char* pEncodeMsg=NULL;
	pEncodeMsg=(char*)base64encode(pMsg, msg.GetLength(), encodeLength);
	CString encodeMsg;
	encodeMsg.Format(L"%s", pEncodeMsg);
	if(pEncodeMsg != NULL)
		delete[] pEncodeMsg;
	return encodeMsg;
}

CString CBase64::base64encode(char* msg, int len)
{
	unsigned char* pMsg = (unsigned char*)msg;
	int encodeLength = 0;
	char* pEncodeMsg = NULL;
	pEncodeMsg = (char*)base64encode(pMsg, len, encodeLength);
	CString encodeMsg = (CString)pEncodeMsg;
	if (pEncodeMsg != NULL)
		delete[] pEncodeMsg;
	return encodeMsg;
}

CString CBase64::base64decode(const CString& msg)
{
	unsigned char* pMsg=(unsigned char*)(LPCTSTR)msg;
	int encodeLength=0;
	char* pDecodeMsg=NULL;
	pDecodeMsg=(char*)base64decode(pMsg, msg.GetLength(), encodeLength);
	pDecodeMsg[encodeLength]='\0';
	CString decodeMsg;
	decodeMsg.Format(L"%s", pDecodeMsg);
	if(pDecodeMsg != NULL)
		delete[] pDecodeMsg;
	return decodeMsg;
}
