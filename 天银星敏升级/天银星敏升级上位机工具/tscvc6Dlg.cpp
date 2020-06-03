// tscvc6Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "tscvc6.h"
#include "tscvc6Dlg.h"
#include "uart.h"
#include "crc32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define UPDATE_START 1
#define UPDATE_SEND  2
#define UPDATE_DONE  3
#define UPDATE_ERR   4
#define UPDATE_SENDCRC   6
#define UPDATE_OK    5

char UpdateFileName[256]={0};
UINT UpdateFileAddr=0;
UINT UpdateFileBlockSize=4096;

CWinThread*    Update_Thread=0;//声明线程
UINT StartUpdateThread(void *param);//声明线程函数
CWinThread*    UartRcv_Thread=0;//声明线程
UINT UartRcvThread(void *param);//声明线程函数

char SaveFileName[256]={0};
char CmdFileName[256]={0};
CFile SaveFile;      // CFile对象 
CStdioFile CmdFile;      // CFile对象 


#define CMD_MAXNUM 64
#define CMD_BUFFNUM 128
typedef enum
{
	CMD_LOOP,
	CMD_SEND,
	CMD_DELAY,
	CMD_RCV,
	CMD_END,
}CMD_e;

typedef struct
{
	UINT  loop;      /*循环次数*/
	UINT  cmdnum;    /*一条循环的个数*/
	UINT  cmdindex;  /*当前指令索引*/

	CMD_e cmd[CMD_MAXNUM];
	BYTE cmd_str[CMD_MAXNUM][CMD_BUFFNUM];  /*一个循环最多64条命令*/
	UINT cmd_val[CMD_MAXNUM];
}CMD_Ctrl_t;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTscvc6Dlg dialog

CTscvc6Dlg::CTscvc6Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTscvc6Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTscvc6Dlg)
	m_RcvStr = _T("");
	m_SendStr = _T("");
	m_SetTime = 0;
	m_UpdateFile = _T("");
	m_SrcAddr = 0;
	m_DesAddr = 0;
	m_SetAddr = 0;
	m_BootSendTimes = 0;
	m_BootSendPeriod = 0;
	m_UpdateFileAddr = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTscvc6Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTscvc6Dlg)
	DDX_Control(pDX, IDC_COMBO5, m_ComBoParity);
	DDX_Control(pDX, IDC_COMBO4, m_ComBoStopBit);
	DDX_Control(pDX, IDC_COMBO2, m_ComBoBaud);
	DDX_Control(pDX, IDC_COMBO3, m_ComBoData);
	DDX_Control(pDX, IDC_COMBO1, m_ComBoCom);
	DDX_Text(pDX, IDC_EDIT1, m_RcvStr);
	DDX_Text(pDX, IDC_EDIT4, m_SendStr);
	DDX_Text(pDX, IDC_EDIT14, m_SetTime);
	DDX_Text(pDX, IDC_EDIT7, m_UpdateFile);
	DDX_Text(pDX, IDC_EDIT11, m_SrcAddr);
	DDX_Text(pDX, IDC_EDIT12, m_DesAddr);
	DDX_Text(pDX, IDC_EDIT13, m_SetAddr);
	DDX_Text(pDX, IDC_EDIT15, m_BootSendTimes);
	DDX_Text(pDX, IDC_EDIT17, m_BootSendPeriod);
	DDX_Text(pDX, IDC_EDIT8, m_UpdateFileAddr);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTscvc6Dlg, CDialog)
	//{{AFX_MSG_MAP(CTscvc6Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON11, OnButton11)
	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTON17, OnButton17)
	ON_BN_CLICKED(IDC_BUTTON18, OnButton18)
	ON_BN_CLICKED(IDC_BUTTON21, OnButton21)
	ON_BN_CLICKED(IDC_BUTTON19, OnButton19)
	ON_BN_CLICKED(IDC_BUTTON20, OnButton20)
	ON_BN_CLICKED(IDC_BUTTON22, OnButton22)
	ON_BN_CLICKED(IDC_BUTTON10, OnButton10)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WN_SHOW_UPDATE,ShowUpdate)
	ON_MESSAGE(WN_SHOW_UARTRCV,ShowUartRcv)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTscvc6Dlg message handlers

BOOL CTscvc6Dlg::OnInitDialog()
{
		char strtmp[64];
		int i;
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	for(i=0;i<64;i++)
	{
	    _snprintf(strtmp,10,"COM%d",i);
		m_ComBoCom.AddString(strtmp);
	}
    m_ComBoCom.SetCurSel(0);
	m_ComBoBaud.AddString("250000");
	m_ComBoBaud.AddString("115200");
	m_ComBoBaud.AddString("57600");
    m_ComBoBaud.AddString("38400");
	m_ComBoBaud.AddString("19200");
	m_ComBoBaud.AddString("9600");
    m_ComBoBaud.AddString("4800");
    m_ComBoBaud.SetCurSel(0);

	m_ComBoData.AddString("8");
    m_ComBoData.AddString("7");
    m_ComBoData.AddString("6");
    m_ComBoData.SetCurSel(0);

    m_ComBoStopBit.AddString("1");
    m_ComBoStopBit.AddString("1.5");
    m_ComBoStopBit.AddString("2");
    m_ComBoStopBit.SetCurSel(0);

    m_ComBoParity.AddString("无");
    m_ComBoParity.AddString("奇校验");
    m_ComBoParity.AddString("偶校验");
    m_ComBoParity.SetCurSel(0);

	m_BootSendPeriod = 300;
	m_BootSendTimes = 20;
	m_UpdateFileAddr = 0;
	//((CButton *)GetDlgItem(IDC_CHECK3))->SetCheck(FALSE);

	UpdateData(FALSE);  // 变量->控件
    uart_init();
	/*创建接收线程*/
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTscvc6Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTscvc6Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTscvc6Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

static int ComOpenState = 0;
static int ComEnableState = 0;

static int FileSaveState = 0;
static int FileCmdState = 0;
static char ComStr[64]={0};
static int ComBaud=0;
static int ComData=0;
static int ComStop=0;
static int ComParity=0;
static CString EditStr = _T("");
static BYTE gReadBuff[64]={0};
static char gReadStr[64]={0};
static char gWriteBuff[32]={(char)0xFF,(char)0x3C,(char)0x01,(char)0x00,(char)0x3D,(char)0x0D};
static char xPointStr[64];
static char yPointStr[64];
static DWORD gReadLen;
static char gPosition = 0;
static unsigned int gPrintNum =0;
static gRaiodSel = 1;
static int xOffset;
char DigToChar(unsigned char val)
{
	if((val>=0) && (val<=9))
	{
	    return (val+'0');
	}
	else if((val>9) && (val<=15))
	{
	    return (val-10 +'A');
	}
	else
	{
	    return ' ';
	}
}

BYTE CharToHex(char val)
{
	if((val>='0') && (val<='9'))
	{
	    return (val-'0');
	}
	else if((val>='A') && (val<='F'))
	{
	    return (val-'A' + 10);
	}
	else if((val>='a') && (val<='f'))
	{
	    return (val-'a' + 10);
	}
	else
	{
	    return 0;
	}
}

void CTscvc6Dlg::OnButton1() 
{
	BYTE buff[128];
	BYTE hexbuff[128];
    UINT len;
	UINT i;
    UINT hexlen=0;
	UpdateData(TRUE); //   控件 -> 变量
    BOOL writestat;
	memset(buff,0,sizeof(buff));
	memset(hexbuff,0,sizeof(hexbuff));
	//m_SendStr;
	if(((CButton *)GetDlgItem(IDC_CHECK1))->GetCheck())
	{
	    /*16进制发送*/
		strncpy((char*)buff,m_SendStr.GetBuffer(m_SendStr.GetLength()),sizeof(buff)-1);
        len = strlen((char*)buff);

		for(i=0;i<len-1;i++)
		{
			if(isxdigit(buff[i]) && isxdigit(buff[i+1]))
			{
				hexbuff[hexlen++] = (CharToHex(buff[i])<<4) + CharToHex(buff[i+1]);
				i++;
			}
			else
			{

			}
		}
		uart_send(hexbuff, hexlen, &writestat);

	}
	else
	{
	    /*ASCII发送*/	
		strncpy((char*)buff,m_SendStr.GetBuffer(m_SendStr.GetLength()),sizeof(buff)-1);
        len = strlen((char*)buff);
		uart_send(buff, len, &writestat);
	}
}




void CTscvc6Dlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	int nIndex;
	if(0 == ComOpenState)
	{
		nIndex = m_ComBoCom.GetCurSel();
		if(nIndex>=10)
		{
			_snprintf(ComStr,20,"\\\\.\\COM%d",nIndex);
		}
		else
		{
			_snprintf(ComStr,20,"COM%d",nIndex);
		}
		if(uart_open(ComStr) != 0)
		{
			MessageBox("打开失败");
		}
		else
		{
            nIndex = m_ComBoBaud.GetCurSel();
            switch(nIndex)
			{
			case 0:
				ComBaud = 250000;
				break;
			case 1:
				ComBaud = 115200;
				break;
			case 2:
				ComBaud = 57600;
				break;
			case 3:
				ComBaud = 38400;
				break;
			case 4:
				ComBaud = 19200;
				break;
			case 5:
				ComBaud = 9600;
				break;
			case 6:
				ComBaud = 4800;
				break;
			default:
				ComBaud = 9600;
				break;
			}
            nIndex = m_ComBoData.GetCurSel();
            switch(nIndex)
			{
			case 0:
				ComData = 8;
				break;
			case 1:
				ComData = 7;
				break;
			case 2:
				ComData = 6;
				break;
			default:
				ComData = 8;
				break;
			}
            nIndex = m_ComBoParity.GetCurSel();
            switch(nIndex)
			{
			case 0:
				ComParity = NOPARITY;
				break;
			case 1:
				ComParity = ODDPARITY;
				break;
			case 2:
				ComParity = EVENPARITY;
				break;
			default:
				ComParity = NOPARITY;
				break;
			}
            nIndex = m_ComBoStopBit.GetCurSel();
            switch(nIndex)
			{
			case 0:
				ComStop = ONESTOPBIT;
				break;
			case 1:
				ComStop = ONE5STOPBITS;
				break;
			case 2:
				ComStop = TWOSTOPBITS;
				break;
			default:
				ComStop = ONESTOPBIT;
				break;
			}
			if(uart_config(ComBaud,ComData,ComParity,ComStop,300) != 0)
			{
				uart_close(ComStr);
				MessageBox("配置失败");
			}
			else
			{
				ComOpenState = 1;
				SetDlgItemText(IDC_BUTTON2,"关闭");
				((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			}
		}
	}
	else
	{
		if(uart_close(ComStr) != 0)
		{
			MessageBox("关闭失败");
		}
		else
		{
			ComOpenState = 0;
			SetDlgItemText(IDC_BUTTON2,"打开");
			((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
		}
	}
}

void CTscvc6Dlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	CString str = _T("");
    m_RcvStr = _T("");
   ((CEdit *)GetDlgItem(IDC_EDIT1))->SetWindowText(str);
}

BOOL CTscvc6Dlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	UINT nIndex;
	if(1 == ComOpenState)
	{
		nIndex = m_ComBoCom.GetCurSel();
		if(nIndex>=10)
		{
			_snprintf(ComStr,20,"\\\\.\\COM%d",nIndex);
		}
		else
		{
			_snprintf(ComStr,20,"COM%d",nIndex);
		}
		uart_close(ComStr);
	}
	return CDialog::DestroyWindow();
}

void CTscvc6Dlg::OnButton5() 
{
	// TODO: Add your control notification handler code here
	BYTE buff[2]={0};
	BOOL writestat;

	buff[0] = 0x00;
	buff[1] = 0xFF;
	//buff[1] - buff[7] 
	uart_send(buff, 2, &writestat);
}

void CTscvc6Dlg::OnButton6() 
{
	// TODO: Add your control notification handler code here
	//BYTE buff[8]={0};
	//BOOL writestat;

	//buff[0] = 0xD2;
	//buff[1] - buff[7] 
	//uart_send(buff, 8, &writestat);
}

void CTscvc6Dlg::OnButton7() 
{
	UINT retry;
	BYTE buff[8]={0};
	BOOL writestat;
    UpdateData(TRUE);   //控件->变量
	retry = m_BootSendTimes;
	while(retry--)
	{
		buff[0] = 0x05;
		buff[1] = 0x21;
		buff[2] = 0x01;
		uart_send(buff, 3, &writestat);

		///buff[0] = 0x00;
		///buff[1] = 0xFF;
		///uart_send(buff, 2, &writestat);
		Sleep(m_BootSendPeriod);
	}
	MessageBox("完成尝试进入BOOT模式,读程序确认是否进入");
}

void CTscvc6Dlg::OnButton8() 
{
	// TODO: Add your control notification handler code here
    ShellExecute(NULL,"open","help.docx",NULL,NULL, SW_SHOWNORMAL); 
}

void CTscvc6Dlg::OnButton10() 
{
    if(ComEnableState==0)
	{
		ComEnableState=1;
		SetDlgItemText(IDC_BUTTON10,"关闭串口接收");
		UartRcv_Thread =  AfxBeginThread(UartRcvThread, 
										 (LPVOID)this,
										 THREAD_PRIORITY_BELOW_NORMAL,
										 5*1024*1024,
										 0,
										 NULL);    
	}
	else
	{
		ComEnableState=0;
		SetDlgItemText(IDC_BUTTON10,"使能串口接收");
	}
}

CString CTscvc6Dlg::SelectFile()
{
    CString strFile = _T("");

    CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.bin)|*.bin|All Files (*.*)|*.*||"), NULL);

    if (dlgFile.DoModal())
    {
        strFile = dlgFile.GetPathName();
    }

    return strFile;
}

void CTscvc6Dlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	CString str = _T("");
	m_SendStr = str;
   ((CEdit *)GetDlgItem(IDC_EDIT4))->SetWindowText(str);
}

void CTscvc6Dlg::OnButton11() 
{
	m_UpdateFile = this->SelectFile();
	strcpy(CmdFileName,m_UpdateFile.GetBuffer(m_UpdateFile.GetLength()));
	UpdateData(FALSE); // 变量 -> 控件
}

void CTscvc6Dlg::OnButton9() 
{
	// TODO: Add your control notification handler code here
	/*创建线程 传入文件名为参数*/
	CString filename;
    UpdateData(TRUE);   //控件->变量
    filename = m_UpdateFile;
	UpdateFileAddr = m_UpdateFileAddr;
	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS2);
    pProg->SetRange(0, 100);
   //创建线成,并传入主线成的事例
	if(0 == Update_Thread)
    {
		strcpy(UpdateFileName,m_UpdateFile.GetBuffer(m_UpdateFile.GetLength()));
		Update_Thread =  AfxBeginThread(StartUpdateThread, 
                                      (LPVOID)this,
                                      THREAD_PRIORITY_BELOW_NORMAL,
                                      5*1024*1024,
                                      0,
                                      NULL);    
		 ((CEdit *)GetDlgItem(IDC_BUTTON9))->SetWindowText("结束升级");
	}
	else
	{
		Update_Thread->PostThreadMessage(WN_EXIT_UPDATE,(WPARAM)0,0);
		//CloseHandle(Update_Thread);
		Update_Thread = 0;
		((CEdit *)GetDlgItem(IDC_BUTTON9))->SetWindowText("开始升级");
		((CEdit *)GetDlgItem(IDC_STATIC1))->SetWindowText("0/0");
	}
}

void CTscvc6Dlg::OnButton16() 
{

}


void CTscvc6Dlg::OnButton13() 
{

}

void CTscvc6Dlg::OnButton14() 
{

}

void CTscvc6Dlg::OnButton15() 
{

}

BYTE buff_sum(void* buff,BYTE len)
{
  BYTE sum=0;
  BYTE i;
  BYTE* p;
  p = (BYTE*)buff;
  for(i=0;i<len;i++)
  {
      sum += p[i];
  }
  return sum;
}

void CTscvc6Dlg::OnButton17() 
{
	// TODO: Add your control notification handler code here
	BYTE buff[8]={0xAA,0x55,0x00,0x00,0x00,0x00,0x00,0x00};
	BOOL writestat;
	UpdateData(TRUE); //控件 -> 变量

	buff[2] = m_SrcAddr;
	buff[3] = m_DesAddr;
    buff[7] = buff_sum(buff,7);
	//buff[2] - buff[7] 
	uart_send(buff, sizeof(buff), &writestat);	
}


UINT UartRcvThread(void *param)
{

    CTscvc6Dlg* dlg;
	dlg = (CTscvc6Dlg*)param;
	BYTE inbuff[128];
    
	void* p;

	while(1)
	{
		BOOL readstat=0;
		DWORD readbytes=0;
		if(ComEnableState==0)
		{
			AfxEndThread(0,1);
			return 0;
		}
		if(ComOpenState==0)
		{
			Sleep(1000);
		}
		else
		{
			//uart_flush();
	#if 1
			readbytes = uart_read(inbuff, sizeof(inbuff), &readstat);
			if((readbytes == 0) || (readstat != TRUE))
			{

			}

			else
			{
				p = malloc(readbytes);
				if(p)
				{
					memcpy(p,inbuff,readbytes);
					dlg->PostMessage(WN_SHOW_UARTRCV,readbytes,(long)p);
				}
			}
	#endif
		}
	}
	return 0;
}

LRESULT CTscvc6Dlg::ShowUartRcv(WPARAM wParam, LPARAM lParam)
{
	static unsigned int g_rcvnum = 0;
	CString str;
	BYTE* p=0;
	WPARAM i;
	p = (BYTE*)lParam;
	if((wParam !=0) && (lParam !=0))
	{
		if(((CButton *)GetDlgItem(IDC_CHECK2))->GetCheck())
		{
			for(i=0;i<wParam;i++)
			{
				str.Format("%02X ",p[i]);
				m_RcvStr += str;
				g_rcvnum++;
				if((g_rcvnum%8)==0)
				{
					m_RcvStr += "\r\n";
				}	
			}
		}
		else
		{
			str.Format("%s",p);
			m_RcvStr += str;
		}
		UpdateData(FALSE); // 变量 -> 控件
        free(p); 
	}
	return 0;
}

void CTscvc6Dlg::OnButton12() 
{
	CString filename;
    char headstr[] = "A命令计数,B命令计数,A错误计数,B错误计数,状态,A发送错误,A接收错误,B发送错误,B接收错误,A复位,B复位,功率,速度,力矩,电流,电机温度,PCB温度,最大力矩,最大电流,系统时间,发送包数,接收正确包数,接收错误包数\r\n";
	// TODO: Add your control notification handler code here
    if(FileSaveState==0)
    {
		filename = this->SelectFile();
		strcpy(SaveFileName,filename.GetBuffer(filename.GetLength()));
		/*打开文件*/
		if(SaveFile.Open(filename, CFile::modeReadWrite|CFile::modeCreate))
		{
			/*成功*/
			SaveFile.Write(headstr,strlen(headstr));
			FileSaveState = 1;
			SetDlgItemText(IDC_BUTTON12,"停止");

		}
		else
		{
			/*失败*/
			AfxMessageBox("打开文件失败");
		}
	}
	else
	{
	    FileSaveState = 0;
		SetDlgItemText(IDC_BUTTON12,"记录");
		/*关闭文件*/
		SaveFile.Close();
	}
}


/*遥测测试帧
00 25 01 02 03 04 AA 55 06 07 08 09 0A 0B 01 00 C4 02 E6 52 3C F5 C2 8F FF 01 01 00 02 00 03 00 04 00 11 22 33 44 00 96

00 25 01 02 03 04 AA 55 06 07 08 09 0A 0B 01 00
C4 02 E6 52 速度 -523.59878
3C F5 C2 8F 力矩 0.030
FF 01 01 00 02 00 03 00
11 22 33 44
00
xx 求和

20 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 

D2 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 

20 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 

D2 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 

20 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
D2 00 00 00 00 00 00 00 

*/



void CTscvc6Dlg::OnButton18() 
{
	// TODO: Add your control notification handler code here
	BYTE buff[8]={0};
	UINT time;
	BOOL writestat;

	UpdateData(TRUE); //控件 -> 变量

	time = m_SetTime;
	buff[0] = 0x50;
	buff[1] = 0x05;
	/*大端*/
	buff[2] = (time >> 24) & 0x00FF;
	buff[3] = (time >> 16) & 0x00FF;
	buff[4] = (time >> 8) & 0x00FF;
	buff[5] = (time >> 0) & 0x00FF;
	uart_send(buff, 6, &writestat);
}

void CTscvc6Dlg::OnButton21() 
{
	// TODO: Add your control notification handler code here
	BYTE buff[8]={0};
	BOOL writestat;

	buff[0] = 0x05;
	buff[1] = 0x21;
	buff[2] = 0x02;
	uart_send(buff, 3, &writestat);
}

void CTscvc6Dlg::OnButton19() 
{
	// TODO: Add your control notification handler code here
	BYTE buff[8]={0};
	UINT addr;
	BOOL writestat;

	UpdateData(TRUE); //控件 -> 变量

	addr = m_SetAddr;
	buff[0] = 0x05;
	buff[1] = 0x23;
	/*大端*/
	buff[2] = (addr >> 24) & 0x00FF;
	buff[3] = (addr >> 16) & 0x00FF;
	buff[4] = (addr >> 8) & 0x00FF;
	buff[5] = (addr >> 0) & 0x00FF;
	uart_send(buff, 6, &writestat);
}

void CTscvc6Dlg::OnButton20() 
{
	// TODO: Add your control notification handler code here
	BYTE buff[8]={0};
	BOOL writestat;

	buff[0] = 0x05;
	buff[1] = 0x24;
	buff[2] = 0x01;
	uart_send(buff, 3, &writestat);
}

void CTscvc6Dlg::OnButton22() 
{
	// TODO: Add your control notification handler code here
	BYTE buff[8]={0};
	BOOL writestat;

	buff[0] = 0x05;
	buff[1] = 0x24;
	buff[2] = 0x02;
	uart_send(buff, 3, &writestat);
}

LRESULT CTscvc6Dlg::ShowUpdate(WPARAM wParam, LPARAM lParam)
{
	CString str("");
	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS2);
    pProg->SetRange(0, 100);
	switch(wParam)
	{
	case UPDATE_SEND:
		pProg->SetPos(lParam>100?100:lParam);
		str.Format(_T("%d%%"),lParam>100?100:lParam);
		((CEdit *)GetDlgItem(IDC_STATIC1))->SetWindowText(str);
		break;
	case UPDATE_ERR:
		pProg->SetPos(100);
		((CEdit *)GetDlgItem(IDC_STATIC1))->SetWindowText("0%");
		((CEdit *)GetDlgItem(IDC_BUTTON9))->SetWindowText("开始升级");
		Update_Thread=0;
		break;
	case UPDATE_OK:
		pProg->SetPos(100);
		((CEdit *)GetDlgItem(IDC_STATIC1))->SetWindowText("100%");
		((CEdit *)GetDlgItem(IDC_BUTTON9))->SetWindowText("开始升级");
		Update_Thread=0;
		break;
	case UPDATE_SENDCRC:
		pProg->SetPos(0);
		((CEdit *)GetDlgItem(IDC_STATIC1))->SetWindowText("0%");
		((CEdit *)GetDlgItem(IDC_BUTTON9))->SetWindowText("开始升级");
		Update_Thread=0;
		break;
	}
	return 0;
}


UINT UpdateSendStart(UINT frames)
{
	return 0;
}

UINT UpdateSendData(BYTE* buff,UINT len,UINT startaddr,CTscvc6Dlg* dlg)
{
	int res=0;
	int retry;
	void* p;
	UINT crc=0;
	UINT addr = 0;
	BYTE sendbuff[4096+16] = {0};
	BYTE readbuff[8] = {0};
	memset(readbuff,0xFF,sizeof(readbuff));
	memset(sendbuff,0xFF,sizeof(sendbuff));
    sendbuff[0] = 0x10;  /*长度*/
    sendbuff[1] = 0x0E;
    sendbuff[2] = 0x0F;
    sendbuff[3] = 0x0A;
	
	memcpy(&sendbuff[8],buff,UpdateFileBlockSize); /*4096字节数据*/

    //sendbuff[4104] = 0xFF;
    //sendbuff[4105] = 0xFF;

    sendbuff[4104] =  sendbuff[4102];
    sendbuff[4105] =  sendbuff[4103];

    sendbuff[4106] = 0x10;
    sendbuff[4107] = 0x00;

	crc = crc32(&sendbuff[8], 4096);
#if 0

    sendbuff[7] = (startaddr>>24)&0xFF;  
    sendbuff[6] = (startaddr>>16)&0xFF;
    sendbuff[5] = (startaddr>>8)&0xFF;
    sendbuff[4] = (startaddr>>0)&0xFF;

    sendbuff[4111] = (crc>>24)&0xFF;  
    sendbuff[4110] = (crc>>16)&0xFF;
    sendbuff[4109] = (crc>>8)&0xFF;
    sendbuff[4108] = (crc>>0)&0xFF;
#else
    sendbuff[4108] = (crc>>24)&0xFF;  
    sendbuff[4109] = (crc>>16)&0xFF;
    sendbuff[4110] = (crc>>8)&0xFF;
    sendbuff[4111] = (crc>>0)&0xFF;

    sendbuff[4] = (startaddr>>24)&0xFF;  
    sendbuff[5] = (startaddr>>16)&0xFF;
    sendbuff[6] = (startaddr>>8)&0xFF;
    sendbuff[7] = (startaddr>>0)&0xFF;
#endif
	retry = 0;
	do
	{
		BOOL writestat;
#if 0
		uart_send(sendbuff,1024,&writestat);
		//Sleep(10);
		uart_send(&sendbuff[1024],1024,&writestat);
		//Sleep(10);
		uart_send(&sendbuff[2048],1024,&writestat);
		//Sleep(10);
		uart_send(&sendbuff[3072],1024,&writestat);
		//Sleep(10);
		res = uart_trans(&sendbuff[4096],16,readbuff,8,2000);
#else 
		res = uart_trans(&sendbuff[0],4112,readbuff,8,2000);
#endif
		if(res)
		{
				p = malloc(8);
				if(p)
				{
					memcpy(p,readbuff,8);
					dlg->PostMessage(WN_SHOW_UARTRCV,8,(long)p);
				}

			addr = (readbuff[4]<<24) | (readbuff[5]<<16) | (readbuff[6]<<8) | (readbuff[47]<<0);
			if((readbuff[0]==0xF5) && (readbuff[1]==0x0A) && (readbuff[2]==0x11) && (readbuff[3]==0x55))
			{
				res = 1;
			}
			else
			{
				res = 0;
			}
		}else{}
		Sleep(200);
	}while((res==0) && ((retry--)>0));
    if(res)
    {
		return 0;
	}
	else
	{
		return 1;
	}
}

UINT UpdateSendCRC(BYTE* buff,UINT len,UINT startaddr,CTscvc6Dlg* dlg)
{
	// TODO: Add your control notification handler code here
	int res=0;
	int retry;
	int i=0;
	void* p;
	BYTE sendbuff[7] = {0};
	BYTE readbuff[4] = {0};
	memset(readbuff,0xFF,sizeof(readbuff));
	memset(sendbuff,0xFF,sizeof(sendbuff));
    sendbuff[0] = 0x05;  
    sendbuff[1] = 0x25;

	retry = 5;
	for(i=0;i<256;i++)
	{
		do
		{
			BOOL writestat;
			sendbuff[2] = i;  /*地址*/

			sendbuff[3] = (crc_tab[i]>>24)&0xFF;  
			sendbuff[4] = (crc_tab[i]>>16)&0xFF;
			sendbuff[5] = (crc_tab[i]>>8)&0xFF;
			sendbuff[6] = (crc_tab[i]>>0)&0xFF;

			res = uart_trans(&sendbuff[0],sizeof(sendbuff),readbuff,4,1);
			if(res)
			{
				p = malloc(4);
				if(p)
				{
					memcpy(p,readbuff,4);
					dlg->PostMessage(WN_SHOW_UARTRCV,4,(long)p);
				}
				if((readbuff[0]==0x05) && (readbuff[1]==0x25) && (readbuff[3]==0x11) && ((readbuff[2]==i+1) || (readbuff[2]==0)))
				{
					res = 1;
				}
				else
				{
					res = 0;
				}
			}else{}
			Sleep(30);
		}while((res==0) && ((retry--)>0));
		if(res)
		{
		}
		else
		{
			break;
		}
	}
	if(res)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


UINT UpdateSendDone(void)
{
	return 0;
}

UINT StartUpdateThread(void *param)
{
    CTscvc6Dlg* dlg;
	MSG msg;
	UINT filelen;
	dlg = (CTscvc6Dlg*)param;
	WPARAM frameindex = 0;
	LPARAM framenum = 100;
	CFile file;      // CFile对象 
    BYTE FileBuffer[1024*1024];      // 文件内容
	UINT udatestate=UPDATE_START;
	UINT sendfrmes = 0;
	UINT lastfrmenum = 0;
	memset(FileBuffer,0xFF,sizeof(FileBuffer));
	/*文件处理*/
	if(file.Open(UpdateFileName, CFile::modeRead))
	{
		filelen=file.GetLength();
		if(filelen>1024*1024)
		{
			AfxMessageBox("文件不能超过1M");
			file.Close();
			dlg->PostMessage(WN_SHOW_UPDATE,UPDATE_ERR,0);
			Sleep(10);
			return 1;
		}
		file.SeekToBegin(); 
		UINT nRet = file.Read(FileBuffer, file.GetLength()); 
		if(nRet < filelen)
		{
			AfxMessageBox("read err",0);
			file.Close();
			dlg->PostMessage(WN_SHOW_UPDATE,UPDATE_ERR,0);
			Sleep(10);
			return 1;
		}
		else
		{
			file.Close();
			while(1)
			{
				Sleep(10);
				/*结束处理*/
				if(PeekMessage(&msg,NULL, 0, 0, PM_REMOVE))
				{
					switch (msg.message)
					{
					case WN_EXIT_UPDATE:
						return 0;
						break;
					}
				}
		    	/*升级处理*/
				switch(udatestate)
				{
				case UPDATE_START:
					    udatestate = UPDATE_SENDCRC;
				break;
				case UPDATE_SENDCRC:
#if 1
					if(UpdateSendCRC(&FileBuffer[UpdateFileBlockSize*frameindex],UpdateFileBlockSize,UpdateFileAddr+UpdateFileBlockSize*frameindex,dlg)==0)
					{
						dlg->PostMessage(WN_SHOW_UPDATE,UPDATE_SEND,(frameindex*100)/16);
					    udatestate = UPDATE_SEND;
					}
					else
					{
						dlg->PostMessage(WN_SHOW_UPDATE,UPDATE_SENDCRC,0);
                        AfxMessageBox("上注CRC_TAB失败");
						Sleep(10);
						return 1;
					}
#else
					    udatestate = UPDATE_SEND;
#endif
				break;
				case UPDATE_SEND:
					if(frameindex < 16)
					{

						if(UpdateSendData(&FileBuffer[UpdateFileBlockSize*frameindex],UpdateFileBlockSize,UpdateFileAddr+UpdateFileBlockSize*frameindex,dlg)==0)
						{
							frameindex++;
							dlg->PostMessage(WN_SHOW_UPDATE,UPDATE_SEND,(frameindex*100)/16);
						}
						else
						{
							AfxMessageBox("发送失败");
							dlg->PostMessage(WN_SHOW_UPDATE,UPDATE_ERR,0);
							Sleep(10);
							return 1;
						}

					}
					else
					{
						udatestate = UPDATE_DONE;
					}
				break;
				case UPDATE_DONE:
					if(UpdateSendDone() == 0)
					{
                        AfxMessageBox("成功");
						dlg->PostMessage(WN_SHOW_UPDATE,UPDATE_OK,0);
						Sleep(10);
						return 0;
					}
					else
					{
                        AfxMessageBox("失败");
						dlg->PostMessage(WN_SHOW_UPDATE,UPDATE_ERR,0);
						Sleep(10);
						return 1;
					}
				break;
				}
			}
		}
	}
	else
	{
		AfxMessageBox("打开文件失败",0);
		dlg->PostMessage(WN_SHOW_UPDATE,UPDATE_ERR,0);
		Sleep(10);
		AfxEndThread(0,1);
		return 1;
	}
	return 0;
}

