#include <stdio.h>
#include <Windows.h>
#include <process.h>

static HANDLE portHandle = INVALID_HANDLE_VALUE;
static HANDLE hMutex = 0;

int uart_init(void) 
{
	if(hMutex ==0)
	{
		hMutex = CreateMutex(NULL, FALSE, NULL);
	}
	if(hMutex == 0)	
	{
		return 1;
	}
	else
	{
	    return 0;
	}

}


int uart_open(const char *name) 
{
	if(WaitForSingleObject(hMutex, 1000) == WAIT_FAILED)
	{
		return 1;
	}
	if(portHandle != INVALID_HANDLE_VALUE)
	{
        CloseHandle(portHandle);
		portHandle = INVALID_HANDLE_VALUE;
	}
    portHandle = CreateFileA(name, 
        GENERIC_READ|GENERIC_WRITE,  // 注意串口号如果大于COM9应该在前面加上\\.\，比如COM10表示为"\\\\.\\COM10"
        0,                           //允许读和写  
        NULL,                        //独占方式  
        OPEN_EXISTING,               //打开而不是创建  
        0,                           //0同步方式 ; FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED异步方式
        NULL);

    if( portHandle == INVALID_HANDLE_VALUE ) 
	{
        DWORD errorCode = GetLastError();
        if( errorCode == ERROR_FILE_NOT_FOUND ) 
		{
            //串口不存在
        }
        else
		{
            //串口存在打开失败
		}
		ReleaseMutex(hMutex);
        return errorCode;
    }
	ReleaseMutex(hMutex);
    return 0;
}

int uart_config(DWORD baudrate,BYTE bytesize, BYTE parity, BYTE stopbits,UINT timeout)
{
	/*1.获取串口的初始配置,并修改指定参数*/
	DCB dcb; 
	COMMTIMEOUTS TimeOuts;


	if(WaitForSingleObject(hMutex, 1000) == WAIT_FAILED)
	{
		return 1;
	}

	if(portHandle == INVALID_HANDLE_VALUE)
	{
		ReleaseMutex(hMutex);
        return 1;
	}
	GetCommState(portHandle,(LPDCB)&dcb); 
	dcb.BaudRate=baudrate;     
	dcb.ByteSize=bytesize; 
	dcb.Parity=parity; 
	dcb.StopBits=stopbits; 
	if(0 == SetCommState(portHandle,&dcb))
	{
		ReleaseMutex(hMutex);
        return 1;
	}
	/*2.设置缓冲区大小*/
	if(0 == SetupComm(portHandle,4096*2,4096*2))//输入缓冲区和输出缓冲区的大小都是1024
	{
		ReleaseMutex(hMutex);
        return 1;
	}
	/*3.设置超时*/
	//GetCommTimeouts(portHandle,&TimeOuts);
	TimeOuts.ReadIntervalTimeout=0x7FFFFFFF;         //读字符间隔超时时间  如果ReadIntervalTimeout=MAXDWORD并且ReadTotalTimeoutMultiplier=0，ReadTotalTimeoutConstant=0 则不等待立即返回。 
	TimeOuts.ReadTotalTimeoutMultiplier=0;   //读总超时＝ReadTotalTimeoutMultiplier×字节数＋ReadTotalTimeoutConstant
	TimeOuts.ReadTotalTimeoutConstant=timeout; 
	TimeOuts.WriteTotalTimeoutMultiplier=0;  //写总超时 
	TimeOuts.WriteTotalTimeoutConstant=timeout; 
	SetCommTimeouts(portHandle,&TimeOuts);  
    /*6.清空缓冲区*/
	PurgeComm(portHandle,PURGE_TXCLEAR | PURGE_RXCLEAR); 
	ReleaseMutex(hMutex);
	return 0;
}

int uart_flush(void)
{
	if(WaitForSingleObject(hMutex, 1000) == WAIT_FAILED)
	{
		return 1;
	}
	PurgeComm(portHandle,PURGE_TXCLEAR | PURGE_RXCLEAR);
	ReleaseMutex(hMutex);
	return 0;
}

int uart_close(const char *name) 
{
	if(WaitForSingleObject(hMutex, 1000) == WAIT_FAILED)
	{
		return 1;
	}
	if(INVALID_HANDLE_VALUE != portHandle)
	{
		CloseHandle(portHandle);
		portHandle = INVALID_HANDLE_VALUE;
	}
	ReleaseMutex(hMutex);
	return 0;
}


DWORD uart_read(char* buff, DWORD len, BOOL* readstat)
{
	/*同步读*/
	DWORD wCount;     //读取的字节数 
	BOOL bReadStat; 

	DWORD dwErrorFlags; 
	COMSTAT ComStat; 

	if(WaitForSingleObject(hMutex, 1000) == WAIT_FAILED)
	{
		return 1;
	}

	if(portHandle == INVALID_HANDLE_VALUE)
	{
		ReleaseMutex(hMutex);
	    return 0;
	}
	ClearCommError(portHandle,&dwErrorFlags,&ComStat); 

	bReadStat=ReadFile(portHandle,buff,len,&wCount,NULL); 
	*readstat = bReadStat;
	ReleaseMutex(hMutex);
	return wCount;
}

int uart_send(char* buff, DWORD len, BOOL* writestat)
{
	BOOL bWriteStat; 
	DWORD dwBytesWrite; 

	if(WaitForSingleObject(hMutex, 1000) == WAIT_FAILED)
	{
		return 1;
	}

	if(portHandle == INVALID_HANDLE_VALUE)
	{
		ReleaseMutex(hMutex);
	    return 0;
	}
	//PurgeComm(portHandle, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); 
	bWriteStat=WriteFile(portHandle,buff,len,&dwBytesWrite,NULL); 
	*writestat = bWriteStat;
	ReleaseMutex(hMutex);
	return dwBytesWrite;
}

int uart_trans(BYTE* outbuff, UINT outlen, BYTE* inbuff, UINT inlen,UINT delay)
{
	BOOL writestat;
	DWORD writebytes;
	BOOL readstat;
	DWORD readbytes;
	uart_flush();

	if(WaitForSingleObject(hMutex, 1000) == WAIT_FAILED)
	{
		return 0;
	}
	PurgeComm(portHandle,PURGE_TXCLEAR | PURGE_RXCLEAR); 
	writebytes = uart_send(outbuff, outlen, &writestat);
	if(portHandle == INVALID_HANDLE_VALUE)
	{
		ReleaseMutex(hMutex);
	    return 0;
	}
	if((writebytes != outlen) || (writestat != TRUE))
	{
		ReleaseMutex(hMutex);
		return 0;
	}
	else
	{
		Sleep(delay);
		readbytes = uart_read(inbuff, inlen, &readstat);
		if((readbytes != inlen) || (readstat != TRUE))
		{
			ReleaseMutex(hMutex);
			return 0;
		}
		else
		{
			ReleaseMutex(hMutex);
			return 1;
		}
	}
}