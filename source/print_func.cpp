/*类似printf的可变参数的打印函数实现*/
#include <stdio.h>

void Printf_Func(char* szFormat, ...)
{
	char aszMsg[512];
	int nMsgLen = 0;

	va_list pvList;
	va_start(pvList, szFormat);
	nMsgLen = vsprintf(aszMsg, szFormat, pvList);
	va_end(pvList);

	aszMsg[nMsgLen] = '\0';
	
	printf(aszMsg);
}