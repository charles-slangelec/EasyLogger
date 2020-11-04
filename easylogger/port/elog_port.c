/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */

#include <elog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#define ELOG_UDP_CLIENT //将log信息通过udp发送出去

#ifdef ELOG_UDP_CLIENT
#include <sys/types.h>      /* basic system data types */
#include <sys/socket.h>     /* basic socket definitions */
#include <arpa/inet.h>      /* inet(3) functions */
#include <fcntl.h>          /* nonblocking */
#define UDPSERVER_PORT (41234)
int l_logsocket = -1;
char l_serverip[24] = "";
#endif // ELOG_UDP_CLIENT

static pthread_mutex_t output_lock;
#ifdef ELOG_WRITE_LOCAL_FILE
#ifdef __WIN32__
#include <windows.h>
#include <io.h>
#include <direct.h>
#endif /* __WIN32__ */
#ifdef __LINUX__
#include <sys/stat.h>
#include <time.h>

#include <dirent.h>
int TraverseDir_Num(const char* strVideoDir) //返回该文件夹的文件个数
{
    //遍历目录
    static  int num = 0;
    DIR*        dp;
    struct dirent *entry;
    struct stat statbuf;
    dp = opendir(strVideoDir);
    if(!dp)
    {
        printf("无法打开目录:%s", strVideoDir);
        return -1;
    }
    chdir(strVideoDir);
    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode))
        {
            if(!strcmp(".",entry->d_name) || !strcmp("..",entry->d_name))
            {
                continue;
            }
            char        strNewDir[256];
            snprintf(strNewDir, sizeof(strNewDir), "%s/%s", strVideoDir, entry->d_name);
            TraverseDir_Num(strNewDir);
        }
        else
        {
            num += 1;
        }

    };

    chdir("..");
    closedir(dp);
    return num;
}

long long int GetDirectorySize(const char *dir) //计算某目录所占空间大小（包含本身的4096Byte）
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    long long int totalSize=0;

    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Cannot open dir: %s\n", dir);
        return -1; //可能是个文件，或者目录不存在
    }

    //先加上自身目录的大小
    lstat(dir, &statbuf);
    totalSize+=statbuf.st_size;

    while ((entry = readdir(dp)) != NULL)
    {
        char subdir[256];
        snprintf(subdir, sizeof(subdir), "%s/%s", dir, entry->d_name);
        lstat(subdir, &statbuf);

        if (S_ISDIR(statbuf.st_mode))
        {
            if (strcmp(".", entry->d_name) == 0 ||
                strcmp("..", entry->d_name) == 0)
            {
                continue;
            }

            long long int subDirSize = GetDirectorySize(subdir);
            totalSize+=subDirSize;
        }
        else
        {
            totalSize+=statbuf.st_size;
        }
    }

    closedir(dp);
    return totalSize;
}
#endif /* __LINUX__ */
FILE*  log_fp = NULL;
#endif // ELOG_WRITE_LOCAL_FILE

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    pthread_mutex_init(&output_lock, NULL);

#ifdef ELOG_WRITE_LOCAL_FILE
    const char* dir = "./log";
    if (access(dir, 0) == -1){ // 如果指定的文件夹不存在，则创建
#ifdef __WIN32__
        int flag = mkdir(dir);
#endif  /* __WIN32__ */
#ifdef __LINUX__
        int flag = mkdir(dir, 0777);
#endif  /* __LINUX__ */
        if (flag == 0) {
            printf("make \"%s\" successfully\n", dir);
        }
        else {
            printf("make \"%s\" errorly\n", dir);
        }
    }

#ifdef __LINUX__
    if(GetDirectorySize(dir) > 5*1024*1024){ // 如果大于5MB，则删除log文件夹中的所有日志文件
        system("rm ./log/*.log");
    }
#endif // __LINUX__

    static char log_file_name[34] = { 0 };
#ifdef __WIN32__
    static SYSTEMTIME currTime;
    GetLocalTime(&currTime);
    snprintf(log_file_name, 34, "%s/%02d-%02d-%02d.log", dir, currTime.wYear, currTime.wMonth, currTime.wDay);
#endif // __WIN32__
#ifdef __LINUX__
    char datebuf[24] = { 0 };
    time_t now = time(0);// 基于当前系统的当前日期/时间
    strftime(datebuf, sizeof(datebuf)-1, "%Y-%m-%d", localtime(&now));
    snprintf(log_file_name, 34, "./log/%s.log", datebuf);
#endif // __LINUX__
    if(log_fp == NULL)
        log_fp = fopen(log_file_name, "a+");
#endif //ELOG_WRITE_LOCAL_FILE

#if defined (__LINUX__) && defined (ELOG_UDP_CLIENT)
    l_logsocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (l_logsocket == -1) {
        perror("can't create socket file");
        return ELOG_SOCKET_ERR;
    }
    //设为非阻塞
    if (-1 == fcntl(l_logsocket, F_SETFL, fcntl(l_logsocket, F_GETFD, 0)|O_NONBLOCK)){
        perror("setnonblock error");
        return ELOG_SOCKET_ERR;
    }
#endif /* ELOG_UDP_CLIENT && __LINUX__ */

    return result;
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    //TODO：由指令控制是否需要写入文件
#ifdef ELOG_WRITE_LOCAL_FILE
#if defined (__LINUX__) && defined (ELOG_COLOR_ENABLE)
    if ( elog_get_text_color_enabled() ) {
        size_t csi_start_len = 0;
        size_t csi_end_len   = 0;
        int8_t level = elog_find_lvl(log);
        if(level >= 0){
            csi_start_len = strlen(CSI_START) + strlen(color_output_info[level]);
            csi_end_len   = strlen(CSI_END);
        }

        fwrite(log + csi_start_len, 1, size - csi_start_len - csi_end_len, log_fp);
        fflush(log_fp);
    }
    else{
        fwrite(log, 1, size, log_fp);
        fflush(log_fp);
    }
#else
    fwrite(log, 1, size, log_fp);
    fflush(log_fp);
#endif /* __LINUX__ && ELOG_COLOR_ENABLE*/
#ifdef __WIN32__
    fwrite(log, 1, size, log_fp);
    fflush(log_fp);
#endif /* __WIN32__ */
#endif // ELOG_WRITE_LOCAL_FILE

#if defined (__LINUX__) && defined (ELOG_UDP_CLIENT)
    if(l_serverip[0] != '\0'){
        struct sockaddr_in serveraddr;
        bzero(&serveraddr, sizeof(serveraddr));
        serveraddr.sin_family 		= AF_INET;
        serveraddr.sin_port   		= htons(UDPSERVER_PORT);
        serveraddr.sin_addr.s_addr 	= inet_addr(l_serverip);

#ifdef ELOG_COLOR_ENABLE//elog_async_enabled
        if ( elog_get_text_color_enabled() ) {
            int8_t level = elog_find_lvl(log);
            if(level >= 0){
                size_t csi_start_len = strlen(CSI_START) + strlen(color_output_info[level]);
                size_t csi_end_len   = strlen(CSI_END) + strlen(ELOG_NEWLINE_SIGN);
                sendto(l_logsocket, log + csi_start_len, size - csi_start_len - csi_end_len, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
                /* sendto newline sign */
                sendto(l_logsocket, ELOG_NEWLINE_SIGN, strlen(ELOG_NEWLINE_SIGN), 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
            }
            else
                sendto(l_logsocket, log, size, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
        }
        else
            sendto(l_logsocket, log, size, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
#else
        sendto(l_logsocket, log, size, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
#endif // ELOG_COLOR_ENABLE
    }
#endif /* __LINUX__ && ELOG_UDP_CLIENT*/

    if( elog_get_text_print_enabled() ){
#ifdef __WIN32__
        if ( elog_get_text_color_enabled() ) {
#ifndef ELOG_ASYNC_OUTPUT_ENABLE
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_output_info[level]);
#endif
            /* output to terminal */
            printf("%.*s", size, log);
        }
#endif /* __WIN32__ */
#ifdef __LINUX__
        printf("%.*s", size, log);
#endif /* __LINUX__ */
    }
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    pthread_mutex_lock(&output_lock);
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    pthread_mutex_unlock(&output_lock);
}


/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    static char cur_system_time[34] = { 0 };
#ifdef __WIN32__
    static SYSTEMTIME currTime;
    GetLocalTime(&currTime);
    snprintf(cur_system_time, 24, "%02d-%02d %02d:%02d:%02d.%03d", currTime.wMonth, currTime.wDay,
            currTime.wHour, currTime.wMinute, currTime.wSecond, currTime.wMilliseconds);
#endif // __WIN32__
#ifdef __LINUX__
//    time_t now = time(0);// 基于当前系统的当前日期/时间
    struct timeval now;
    gettimeofday(&now, 0);
    strftime(cur_system_time, sizeof(cur_system_time)-1, "%H:%M:%S", localtime(&now.tv_sec));
    snprintf(cur_system_time, sizeof (cur_system_time), "%s.%06ld", cur_system_time, now.tv_usec);
#endif // __LINUX__
    return cur_system_time;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    static char cur_process_info[10] = { 0 };
#ifdef __WIN32__
    snprintf(cur_process_info, 10, "pid:%04ld", GetCurrentProcessId());
#endif // __WIN32__
#ifdef __LINUX__
    snprintf(cur_process_info, 10, "pid:%04ld", (long int)getpid());
#endif // __LINUX__

    return cur_process_info;
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    static char cur_thread_info[10] = { 0 };

#ifdef __WIN32__
    snprintf(cur_thread_info, 10, "tid:%04ld", GetCurrentThreadId());
#endif // __WIN32__
#ifdef __LINUX__
    snprintf(cur_thread_info, 10, "tid:%04lu", pthread_self());
#endif // __LINUX__

    return cur_thread_info;
}
