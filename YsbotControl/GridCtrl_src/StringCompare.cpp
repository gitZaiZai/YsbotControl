#include "stdafx.h"
#include <stdio.h>  
#include <tchar.h>  
#include <stdlib.h>  
#include <locale.h>  

#include "StringCompare.h"

namespace Ysbot {
 
//unsigned....return the none-zero poniter or null-end and if find '0', it will move the input pointer.  
wchar_t * findDigitEndW(wchar_t **pszBuffer)  
{  
	wchar_t *pszEnd = *pszBuffer;  
	bool hasFirstZero = *pszEnd == _T('0');  
	while (*pszEnd)  
	{  
		//high case firstly!  
		if (*pszEnd < _T('0') || *pszEnd > _T('9'))  
		{  
			break;  
		}  
		else if (hasFirstZero && _T('0') == *pszEnd)  
		{  
			(*pszBuffer)++;  
		}  
		pszEnd++;  

	}  
	return pszEnd;  
}  

char * findDigitEndA(char **pszBuffer)  
{  
	char *pszEnd = *pszBuffer;  
	bool hasFirstZero = *pszEnd == _T('0');  
	while (*pszEnd)  
	{  
		//high case firstly!  
		if (*pszEnd < _T('0') || *pszEnd > _T('9'))  
		{  
			break;  
		}  
		else if (hasFirstZero && _T('0') == *pszEnd)  
		{  
			(*pszBuffer)++;  
		}  
		pszEnd++;  

	}  
	return pszEnd;  
}  

//fang0001san022san: a alpha(f,a,...) is a compared unit, but a number(0001,022) will be condsided as one too.  
int LogicCompareW(wchar_t *psza, wchar_t *pszb){  
	wchar_t *paCur = psza, *pbCur = pszb;  
	if (psza != NULL && NULL != pszb)  
	{  
		wchar_t *paDigitEnd, *pbDigitEnd;  
		wchar_t *paNonZero, *pbNonZero;  
		while (*paCur && *paCur){  
			paNonZero = paCur;  
			pbNonZero = pbCur;  
			//allow for:    0    ...    0      1      2...  
			//            paCur            paNonZero  
			paDigitEnd = findDigitEndW(&paNonZero);  
			pbDigitEnd = findDigitEndW(&pbNonZero);  

			//compare by number   
			if (paDigitEnd > paCur && pbDigitEnd > pbCur)  
			{                 
				int aDigitLength = paDigitEnd - paNonZero;  
				int bDigitLength = pbDigitEnd - pbNonZero;  
				//compare by digit   
				if (aDigitLength != bDigitLength)  
					return aDigitLength - bDigitLength;  
				//the number of their digit is same.  
				while (paNonZero < paDigitEnd){  
					if (*paNonZero != *pbNonZero)  
						return *paNonZero - *pbNonZero;  
					paNonZero++;  
					pbNonZero++;  
				}  

				//if they are equal compared by number, compare the number of '0' when start with "0"   
				aDigitLength = paNonZero - paCur;  
				bDigitLength = pbNonZero - pbCur;  
				if (aDigitLength != bDigitLength)  
					return bDigitLength - aDigitLength;  
				paCur = paDigitEnd;  
				pbCur = pbDigitEnd;  
			}  
			else{  
				if (*paCur != *pbCur)  
					return *paCur - *pbCur;  
				paCur++;  
				pbCur++;  
			}  
		}  
	}  
	//last condtion should be never reached.  
	return NULL == paCur ? -1 : NULL == pbCur ? -1 : *paCur - *pbCur;  
}  

//fang0001san022san: a alpha(f,a,...) is a compared unit, but a number(0001,022) will be condsided as one too.  
int  LogicCompareA(char *psza, char *pszb){  
	char *paCur = psza, *pbCur = pszb;  
	if (psza != NULL && NULL != pszb)  
	{  
		char *paDigitEnd, *pbDigitEnd;  
		char *paNonZero, *pbNonZero;  
		while (*paCur && *paCur){  
			paNonZero = paCur;  
			pbNonZero = pbCur;  
			//allow for:    0    ...    0      1      2...  
			//            paCur            paNonZero  
			paDigitEnd = findDigitEndA(&paNonZero);  
			pbDigitEnd = findDigitEndA(&pbNonZero);  

			//compare by number   
			if (paDigitEnd > paCur && pbDigitEnd > pbCur)  
			{  
				int aDigitLength = paDigitEnd - paNonZero;  
				int bDigitLength = pbDigitEnd - pbNonZero;  
				//compare by digit   
				if (aDigitLength != bDigitLength)  
					return aDigitLength - bDigitLength;  
				//the number of their digit is same.  
				while (paNonZero < paDigitEnd){  
					if (*paNonZero != *pbNonZero)  
						return *paNonZero - *pbNonZero;  
					paNonZero++;  
					pbNonZero++;  
				}  

				//if they are equal compared by number, compare the number of '0' when start with "0"   
				//ps note: paNonZero and pbNonZero can be added the above loop "while", but it is changed meanwhile.  
				//so, the following comparsion is ok.  
				aDigitLength = paNonZero - paCur;  
				bDigitLength = pbNonZero - pbCur;  
				if (aDigitLength != bDigitLength)  
					return bDigitLength - aDigitLength;  
				paCur = paDigitEnd;  
				pbCur = pbDigitEnd;  
			}  
			else{  
				if (*paCur != *pbCur)  
					return *paCur - *pbCur;  
				paCur++;  
				pbCur++;  
			}  
		}  
	}  
	//last condition should be never reached.  
	return NULL == paCur ? -1 : NULL == pbCur ? -1 : *paCur - *pbCur;  
}  

int LogicCompareWithPrint(TCHAR *psza, TCHAR *pszb){  
	_tprintf(_T("LogicCompare : %s%*s%s%*s"), psza, 12 - _tcslen(psza), ",", pszb, 10 - _tcslen(pszb), " ");  
	return LogicCompare(psza, pszb);  
}  

}
 