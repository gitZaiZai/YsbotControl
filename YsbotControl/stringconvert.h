#ifndef _stringconvert_h
#define _stringconvert_h
#include <string>
#include <deque>
#include <stdexcept>

#define IS_BLANK(c) ((c) == ' ' || (c) == '\t')
#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_ALPHA(c) ( ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') )
#define IS_HEX_DIGIT(c) (((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))

void split_string (std::deque<std::string>&, const std::string&) throw (std::bad_alloc);
void split_string (std::deque<std::string>&, const char* src, const char* strSeparator) throw (std::bad_alloc);

bool prefix (const std::string&, const std::string) throw ();

bool string2double (double&, const std::string&) throw ();

bool string2float (float&, const std::string&) throw ();

bool string2int (int&, const std::string&) throw ();

bool string2uint (unsigned int&, const std::string&) throw ();

bool string2lint (long int&, const std::string&) throw ();

bool string2ulint (unsigned long int&, const std::string&) throw ();

bool string2bool (bool&, const std::string&) throw ();

std::wstring  __fastcall Ansi2Unicode(std::string szAnsi);

/*
 * UnicodeToAnsi converts the Unicode string pszW to an ANSI string
 * and returns the ANSI string through ppszA. Space for the
 * the converted string is allocated by UnicodeToAnsi.
 */ 

std::string __fastcall Unicode2Ansi(std::wstring szUnicode);

CString Utf8ToUnicode( std::string utf8str );
string UnicodeToUtf8(CString Unicodestr);
bool is_str_utf8(string src);

std::string ToString (double value, unsigned int precision =0); 

/* Whether string s is a number. 
   Returns 0 for non-number, 1 for integer, 2 for hex-integer, 3 for float */
int  is_number(const char * strdata);
void trim_str(char *str);
std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);
std::string &trim(std::string &s);

#endif
