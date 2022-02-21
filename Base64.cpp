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
	 - 3byte를 6bit씩 4개로 나누어 놓기 때문에 자신보다 큰 3의 배수로 만든다.
		ex) 10byte -> 3byte * 3 + 1 byte -> 3byte * 3 + 3byte = 4 * 3 + 4 + 1
		10byte는 3byte 3개와 1byte이고, 마지막 1byte는 padding하여 3byte로 만든다.
		이는 12byte가 되면 encoding 후 16개의 ascii로 변환된다.
		마지막 null pointer를 위하여 1 추가
		따라서, 17
		nLength = 10 ;
		int nSize = (10 + 3 - 10 % 3) * 8 / 6 + 1 = 17 

	while()
	1.	첫 바이트의 상위 6비트
		: szCur[0] >> 2
	2.	첫 바이트의 하위 2비트 + 두번째 바이트의 상위 4비트
		- & 0x03 -> & 0x00000011 : 하위 2 비트 검출
		: ((szCur[0] & 0x03) << 4) + (szCur[1] >> 4)
	3.	두번째 바이트의 하위 4비트 + 세번째 바이트의 상위 2비트
		- & 0x0f -> & 0x00001111 : 하위 4 비트 검출
		: ((szCur[1] & 0x0f) << 2) + (szCur[2] >> 6)
	4.	세번째 바이트의 하위 6비트
		- & 0x3f -> & 0x00111111 : 하위 6 비트 검출
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

	//	3바이트 이동
		szCur += 3 ;
	//	길이 3만큼 감소
		nLength -= 3 ;
	}

//	잔여 바이트가 있을 경우
	if(0 != nLength)
	{
		szRet[i++] = szBase64[szCur[0] >> 2] ;
		if(1 < nLength)
		{
			szRet[i++] = szBase64[((szCur[0] & 0x03) << 4) + (szCur[1] >> 4)] ;
		//	하위 4 비트 검출 후, 2 비트는 0으로 padding
			szRet[i++] = szBase64[(szCur[1] & 0x0f) << 2] ;
		//	마지막 문자는 padding
			szRet[i++] = szBase64Pad ;
		}

		else	//	(1 == nLength)
		{
		//	하위 2 비트 검출 후, 4 비트는 0으로 padding
			szRet[i++] = szBase64[(szCur[0] & 0x03) << 4] ;
		//	마지막 2 문자는 padding
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
