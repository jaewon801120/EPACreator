// Base64.h: interface for the CBase64 class.
//
//////////////////////////////////////////////////////////////////////

//	refer to RFC3548 3. Base 64 encoding, Table 1. Base 64 alphabet
static char szBase64[] =
	{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 
	  'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 
	  'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 
	  'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
	} ;

static char szBase64Pad = '=' ;

class CBase64  
{
public:
	CBase64() ;
	virtual ~CBase64() ;

	unsigned char* base64encode(const unsigned char* sz, int nLength, int& nRet) ;
	unsigned char* base64decode(const unsigned char* sz, int nLength, int& nRet) ;
	CString base64encode(const CString& msg);
	CString base64encode(char* msg, int len);
	CString base64decode(const CString& msg);
};
