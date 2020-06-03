/**
 *****************************************************************************        
 * \brief       平台相关(PLATFORM)SHELL模块(SHELL)相关接口实现.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        shell.c  
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-08-26
 * \note        使用前请参考注释.\n
 * \since       李红洁      2017-8-26 11:11:46     1.0     新建 
 * \par 修订记录
 * - 2017-08-26 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "hal.h"
#include "osapi_freertos.h"
#include "driver.h"
#include "retarget.h"

/*****************************************************************************    
 *                                                                           
 *                             内部数据                                   
 *                                                                            
 ****************************************************************************/
#define SHELL_CMD_LEN 64                          /**< shell命令缓冲区长度  */
extern const shell_cmd_cfg shell_cmd_list[ ];     /**< shell_fun中定义  */
extern const shell_cmd_cfg shell_cmd_list_app[ ]; /**< app_shellfun中定义  */
static int shellpassed = 1;  /**< shell密码是否校验 0已经校验 1未校验  */
static unsigned char cmd_buf[SHELL_CMD_LEN]="\r";
static int cmd_buf_index=0;  /**< 当前接收到的命令字符数  */

/*****************************************************************************    
 *                                                                           
 *                             内部接口函数实现                                   
 *                                                                            
 ****************************************************************************/

/**
 *****************************************************************************
 * \fn          static int shell_getchar(unsigned char *data)
 * \brief       读取一个字符.
 * \note        . 
 * \param[out]  data 存储读到的数据.
 * retval       0  成功.
 * retval       -1 失败.
 *****************************************************************************
 */
static int shell_getchar(unsigned char *data)
{
    int erro=0;
    driver_uart_recv(getstdiouart(), data, 1,10,&erro);
    if(erro!=0)
    {
        return -1;
    }
    return 0;
}


/**
 *****************************************************************************
 * \fn          static unsigned int shell_cmd_len(char const *cmd)
 * \brief       获取命令字符长度.
 * \note        空格或者\0结束. 
 * \param[in]   cmd 字符串
 * return       unsigned int 命令字符的长度
 *****************************************************************************
 */
static unsigned int shell_cmd_len(unsigned char const *cmd)
{
    unsigned char const *p = cmd;
    unsigned int len = 0;
    while((*p != ' ') && (*p != 0)) 
    {
        p++;
        len++;
    }
    return len;
}

/**
 *****************************************************************************
 * \fn          static void shell_check_passwd(void)
 * \brief       shell密码确认.
 * \note        空格或者\0结束. 
 *****************************************************************************
 */
static void shell_check_passwd(void)
{
	unsigned int i;
    unsigned char ch[6] = {0};
    unsigned char pw[7] = SHELL_PASSWORD_STR;

    while(1)
    {
        printf("\r\nPassword:");
        memset(ch, 0, sizeof(ch));
        for (i = 0; i < sizeof(ch); i++)
        {
            ch[i] = getchar();
            putchar('\b');
            putchar('*');
        }
        if (memcmp(pw, ch, sizeof(ch)) == 0)
        {
            printf("\r\n");
            break;
        }
        printf("\r\nAccess denied\r\n");
    }
}

/**
 *****************************************************************************
 * \fn          static int shell_cmd_check(unsigned char *cmd, char const *str)
 * \brief       匹配命令字符.
 * \note        . 
 * \param[in]   cmd 字符串
 * \param[in]   str 匹配字符
 * retval       0 匹配成功
 * retval       1 匹配失败
 *****************************************************************************
 */
static int shell_cmd_check(unsigned char *cmd, unsigned char const *str)
{
    unsigned int len1 = shell_cmd_len((unsigned char const *)cmd);
    unsigned int len2 = shell_cmd_len(str);
    if(len1 != len2)
    {
        return 1;
    }
    return memcmp(cmd, str, len1);
}

/**
 *****************************************************************************
 * \fn          static unsigned int shell_read_line(int get(void), unsigned int len)
 * \brief       shell读命令行.
 * \note        回车结束,可以使用Backspace键清除输入. 
 * \param[in]   get 输入接口函数
 * \param[in]   p   接收缓冲区
 * \param[in]   len 需要接收的长度
 * return       unsigned int 实际接收的长度
 *****************************************************************************
 */
static unsigned int shell_read_line(int get(unsigned char* tmp))
{
    unsigned char ch = '\r';
    unsigned int count;
    unsigned char tmp;
    /*开始打印一次"sh>"*/
    if(cmd_buf[0]=='\r')
    {
        printf("sh>\r\n");
        OsMemset(cmd_buf,0x00,sizeof(cmd_buf));
    }
    /*如果接收到字符往下走,否则返回*/
    if(get(&tmp)==0)
    {
        ch = tmp; 
    }
    else
    {
        return 0;
    }
    /*如果接收到非打印字符且当前接收缓冲区不为0则认为接收到一帧否则打印"SH>"*/
    if((ch == '\r' || ch == '\n' || ch < ' ' || ch > '~') && (ch != '\b'))
    {
        if(cmd_buf_index==0)
        {
            printf("sh>\r\n");
        }
        else
        {
            count = cmd_buf_index;
            cmd_buf[cmd_buf_index]=0;
            cmd_buf_index =0;
            printf("\r\n");
            return count;
        }
    }
    else 
    {
        if(ch == '\b') 
        {
            if(cmd_buf_index != 0) 
            {
                cmd_buf_index--;
                putchar('\b');
                putchar(' ');
                putchar('\b');
                cmd_buf[cmd_buf_index]= '\0';
            }
        } 
        else 
        {
            /*如果接收到打印字符且当前接收缓冲区满则认为接收完一帧"*/
            putchar(ch);
            cmd_buf[cmd_buf_index++] = ch;
            if(cmd_buf_index>=(sizeof(cmd_buf)-1))
            {
                count = cmd_buf_index;
                cmd_buf[cmd_buf_index]=0;
                cmd_buf_index =0;
                printf("\r\n");
                return count;
            }
        } 
    } 
    return 0;
}

/**
 *****************************************************************************
 * \fn          int shell_exec_cmdlist(unsigned char* cmd)
 * \brief       搜索命令列表并执行.
 * retval       0 成功
 * retval       -1 失败
 * \note        . 
 *****************************************************************************
 */
int shell_exec_cmdlist(unsigned char* cmd)
{
    int i;
    /*平台相关命令*/
    for (i=0; shell_cmd_list[i].name != 0; i++)
    {
        if (shell_cmd_check(cmd, shell_cmd_list[i].name) == 0) 
        {
            shell_cmd_list[i].func(cmd);
            return 0;
        }            
    } 
//    /*应用相关命令*/
//    for (i=0; shell_cmd_list_app[i].name != 0; i++)
//    {
//        if (shell_cmd_check(cmd, shell_cmd_list_app[i].name) == 0) 
//        {
//            shell_cmd_list_app[i].func(cmd);
//            return 0;
//        }            
//    }  
    if(shell_cmd_list[i].name == NULL)
    {
        printf("unkown command\r\n");
        return -1;
    }
    return 0;
}

/*****************************************************************************    
 *                                                                           
 *                             对外接口函数实现                                   
 *                                                                            
 ****************************************************************************/
/**
 *****************************************************************************
 * \fn          void shell_exec_shellcmd(void)
 * \brief       执行shell命令.
 * \note        任务周期调用该函数. 
 *****************************************************************************
 */
void shell_exec_shellcmd(void)
{
    if (!shellpassed)
    {
        shell_check_passwd();
        shellpassed = 1;
    } 
    if(shell_read_line(shell_getchar))
    {
        shell_exec_cmdlist(cmd_buf);
    }
}
