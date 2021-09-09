#include "stdafx.h"
#include "stringconvert.h"
#include <cctype>
#include <cstdio>
#include <sstream>
#include <cmath>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

using namespace std;

void split_string (deque<string>& dest, const string& src) throw (std::bad_alloc) {
  dest.clear();
  if (src.length()==0)
    return;
  int i=-1;
  bool was_white = (isspace (src[0]) !=0 ) ? true : false ;
  for (unsigned int j=0; j<src.length(); j++) {
    if (isspace (src[j])) {
      if (was_white) {
	i=j;
      } else {
	was_white=true;
	dest.push_back (src.substr (i+1, j-i-1));
	i=j;
      }
    } else {
      was_white=false;
    }
  }
  if (!was_white) {
    dest.push_back (src.substr (i+1, src.length()));
  }
}

void split_string(std::deque<std::string>& dest,const char* src, const char* strSeparator) throw (std::bad_alloc) 
{
	dest.clear();
	if ( src == NULL || strlen(src)==0)
		return;

	std::string::size_type pos;  
	std::string str = std::string(src);
	str+=string(strSeparator);  
	unsigned int size=str.size();  

	for( unsigned int i=0; i < size; i++ )  
	{  
		pos = str.find(strSeparator,i);  
		if( pos < size )  
		{  
			std::string s=str.substr(i,pos-i);  
			dest.push_back(s);  
			i=pos+string(strSeparator).size()-1;  
		}  
	}  
}

bool prefix (const std::string& p, const std::string w) throw () {
  const unsigned int n=p.length();
  if (n>w.length())
    return false;
  for (unsigned int i=0; i<n; i++)
    if (p[i]!=w[i])
      return false;
  return true;
}

bool string2double (double& dest, const string& src) throw () {
  char* end_char;
  dest = strtod (src.c_str(), &end_char);
  return ((*end_char)=='\0');
}

bool string2float (float& dest, const string& src) throw () {
  double d;
  bool success = string2double (d, src);
  dest=(float)d;
  return success;
}

bool string2lint (long int& d, const string& s) throw () {
  char* end_char;
  d = strtol (s.c_str(), &end_char, 0);
  if ((*end_char)!='\0')
    return false;
  return true;
}

bool string2ulint (unsigned long int& d, const string& s) throw () {
  char* end_char;
  d = strtoul (s.c_str(), &end_char, 0);
  if ((*end_char)!='\0')
    return false;
  if ((s[0]=='-') && (d!=0))
      return false;
  return true;
}

bool string2int (int& d, const string& s) throw () {
  long int l;
  bool success = string2lint (l, s);
  d=l;
  return success;
}

bool string2uint (unsigned int& d, const string& s) throw () {
  unsigned long int l;
  bool success = string2ulint (l, s);
  d=l;
  return success;
}

bool string2bool (bool& d, const string& s) throw () {
  if (s==string("0")) {
    d=false;
    return true;
  }
  if (s==string("1")) {
    d=true;
    return true;
  }
  if (s==string("true")) {
    d=true;
    return true;
  }
  if (s==string("false")) {
    d=false;
    return true;
  }
  return false;
}

std::string __fastcall Unicode2Ansi(std::wstring szUnicode)
{
	int len = WideCharToMultiByte(CP_ACP,NULL, szUnicode.c_str(), szUnicode.size(), NULL,0,NULL,NULL);
	char* szAnsi = new char[len+1];
	WideCharToMultiByte(CP_ACP, NULL, szUnicode.c_str(), szUnicode.size(), szAnsi, len, NULL, NULL );
	szAnsi[len] = 0;
	std::string rs = szAnsi;
	delete[] szAnsi;
	return rs;
}

std::wstring __fastcall Ansi2Unicode( std::string szAnsi )
{
	int len = MultiByteToWideChar(CP_ACP, NULL, szAnsi.c_str(),szAnsi.size(),NULL,0);
	wchar_t* szUnicode = new wchar_t[len+1];
	MultiByteToWideChar(CP_ACP, NULL, szAnsi.c_str(), szAnsi.size(),szUnicode, len );
	szUnicode[len] = 0;
	std::wstring rs = szUnicode;
	delete[] szUnicode;
	return rs;
}

//string类型的utf-8字符串转为CString类型的unicode字符串
CString Utf8ToUnicode( std::string utf8str )
{
    /* 预转换，得到所需空间的大小 */
    int nLen = ::MultiByteToWideChar( CP_UTF8, NULL,
                      utf8str.data(), utf8str.size(), NULL, 0 );
    /* 转换为Unicode */
    std::wstring wbuffer;
    wbuffer.resize( nLen );
    ::MultiByteToWideChar( CP_UTF8, NULL, utf8str.data(), utf8str.size(),
                   (LPWSTR) (wbuffer.data() ), wbuffer.length() );
 
#ifdef UNICODE
    return(CString( wbuffer.data(), wbuffer.length() ) );
#else
    /*
     * 转换为ANSI
     * 得到转换后长度
     */
    nLen = WideCharToMultiByte( CP_ACP, 0,
                    wbuffer.data(), wbuffer.length(), NULL, 0, NULL, NULL );
 
    std::string ansistr;
    ansistr.resize( nLen );
 
    /* 把unicode转成ansi */
    WideCharToMultiByte( CP_ACP, 0, (LPWSTR) (wbuffer.data() ), wbuffer.length(),
                 (LPSTR) (ansistr.data() ), ansistr.size(), NULL, NULL );
    return(CString( ansistr.data(), ansistr.length() ) );
#endif
}
 
//CString类型的unicode字符串转为string类型的utf-8字符串
string UnicodeToUtf8(CString Unicodestr)
{
	wchar_t* unicode = Unicodestr.AllocSysString();
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
	string result = szUtf8;
	free(szUtf8);
	return result;
}
 
bool is_str_utf8(string src)
{
	const char* str= src.c_str();
	unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr = *str;
	bool bAllAscii = true;
	for (unsigned int i = 0; str[i] != '\0'; ++i){
		chr = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
		if (nBytes == 0 && (chr & 0x80) != 0){
			bAllAscii = false;
		}
		if (nBytes == 0) {
			//如果不是ASCII码,应该是多字节符,计算字节数
			if (chr >= 0x80) {
				if (chr >= 0xFC && chr <= 0xFD){
					nBytes = 6;
				}
				else if (chr >= 0xF8){
					nBytes = 5;
				}
				else if (chr >= 0xF0){
					nBytes = 4;
				}
				else if (chr >= 0xE0){
					nBytes = 3;
				}
				else if (chr >= 0xC0){
					nBytes = 2;
				}
				else{
					return false;
				}
				nBytes--;
			}
		}
		else{
			//多字节符的非首字节,应为 10xxxxxx
			if ((chr & 0xC0) != 0x80){
				return false;
			}
			//减到为零为止
			nBytes--;
		}
	}
	//违反UTF8编码规则
	if (nBytes != 0) {
		return false;
	}
	if (bAllAscii){ //如果全部都是ASCII, 也是UTF8
		return true;
	}
	return true;
}
 
double potenzen10 [] = { 1,10,100,1000,10000,100000 };

std::string ToString (double value, unsigned int precision) 
{   
	std::stringstream inout;
	std::string sss;
	double faktor = ( precision<=5 ? potenzen10[precision] : 100000 );
	inout << floor(value*faktor+0.5)/faktor << '\n';
	inout >> sss;
	return sss;
}

/* Whether string s is a number. 
   Returns 0 for non-number, 1 for integer, 2 for hex-integer, 3 for float */
int is_number(const char * strdata)
{
	int base = 10;
	char *ptr;
	int type = 0;

	if (strdata==NULL) return 0;

	ptr = (char*)strdata;

	/* skip blank */
	while (IS_BLANK(*ptr)) {
		ptr++;
	}

	/* skip sign */
	if (*ptr == '-' || *ptr == '+') {
		ptr++;
	}

	/* first char should be digit or dot*/
	if (IS_DIGIT(*ptr) || ptr[0]=='.') {

		if (ptr[0]!='.') {
			/* handle hex numbers */
			if (ptr[0] == '0' && ptr[1] && (ptr[1] == 'x' || ptr[1] == 'X')) {
				type = 2;
				base = 16;
				ptr += 2;
			}

			/* Skip any leading 0s */
			while (*ptr == '0') {
				ptr++;
			}

			/* Skip digit */
			while (IS_DIGIT(*ptr) || (base == 16 && IS_HEX_DIGIT(*ptr))) {
					ptr++;
			}
		}

		/* Handle dot */
		if (base == 10 && *ptr && ptr[0]=='.') {
			type = 3;
			ptr++;
		}

		/* Skip digit */
		while (type==3 && base == 10 && IS_DIGIT(*ptr)) {
			ptr++;
		}

		/* if end with 0, it is number */
		if (*ptr==0) 
			return (type>0) ? type : 1;
		else
			type = 0;
	}
	return type;
}

void trim_str(char *str)
{
	if ( str == NULL || str[0] == '\0' )
		return;

	int s_len = strlen(str);
	if(s_len>0)
	{
		char *s_b = str,
			*s_e = str+s_len,
			*s_copy = str;
		for( ; s_b!=s_e && *s_b==' '; ++s_b);
		while(1){
			for( ; s_b!=s_e && *s_b!=' '; ++s_b)
				*s_copy++ = *s_b;
			if(s_b==s_e)
				break;
			*s_copy++ = *s_b++;
			for( ; s_b!=s_e && *s_b==' '; ++s_b);
			if(s_b==s_e){
				--s_copy;
				break;
			}
		}
		*s_copy = '\0';
	}
}

// trim from start

std::string &ltrim(std::string &s) {

	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));

	return s;

}


// trim from end

std::string &rtrim(std::string &s) {

	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());

	return s;

}


// trim from both ends

std::string &trim(std::string &s) {

	return ltrim(rtrim(s));

}