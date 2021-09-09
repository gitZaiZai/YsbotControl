#ifndef _string_compare_h
#define _string_compare_h

namespace Ysbot
{
#ifdef _UNICODE  
#define LogicCompare LogicCompareW  
#define findDigitEnd findDigitEndW  
#else  
#define LogicCompare LogicCompareA  
#define findDigitEnd findDigitEndA  
#endif  

int  LogicCompareW(wchar_t *psza, wchar_t *pszb);  
int  LogicCompareA(char *psza, char *pszb);  
wchar_t * findDigitEnd(wchar_t **pszBuffer);  
char * findDigitEnd(char **pszBuffer);  

}

#endif