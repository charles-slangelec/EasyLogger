/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015-2016, Armink, <armink.ztl@gmail.com>
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
 * Function: It is the configure head file for this library.
 * Created on: 2015-07-30
 */

#ifndef _ELOG_CFG_H_
#define _ELOG_CFG_H_

/* 启动将log信息写入本地文件中的功能 */
//#define ELOG_WRITE_LOCAL_FILE
/* enable log output. */
#define ELOG_OUTPUT_ENABLE
/* setting static output log level. range: from ELOG_LVL_ASSERT to ELOG_LVL_VERBOSE */
#define ELOG_OUTPUT_LVL                          ELOG_LVL_VERBOSE
/* enable assert check */
#define ELOG_ASSERT_ENABLE      //开后启,EasyLogger自带的断言，可以直接用户软件，在断言表达式不成立后会输出断言信息并保持`while(1)`，或者执行断言钩子方法 ( 建议在调试(DEBUG)的时候才开启，不然会影响程序性能 )
/* buffer size for every line's log */
#define ELOG_LINE_BUF_SIZE                       1024
/* output line number max length */
#define ELOG_LINE_NUM_MAX_LEN                    6
/* output filter's tag max length */
#define ELOG_FILTER_TAG_MAX_LEN                  30
/* output filter's keyword max length */
#define ELOG_FILTER_KW_MAX_LEN                   16
/* output newline sign */
#define ELOG_NEWLINE_SIGN                        "\n"

/* enable log color */
#define ELOG_COLOR_ENABLE
/* change the some level logs to not default color if you want */
//#define ELOG_COLOR_ASSERT                        (F_MAGENTA B_NULL S_NORMAL)
//#define ELOG_COLOR_ERROR                         (F_RED B_NULL S_NORMAL)
//#define ELOG_COLOR_WARN                          (F_YELLOW B_NULL S_NORMAL)
//#define ELOG_COLOR_INFO                          (F_CYAN B_NULL S_NORMAL)
//#define ELOG_COLOR_DEBUG                         (F_GREEN B_NULL S_NORMAL)
//#define ELOG_COLOR_VERBOSE                       (F_BLUE B_NULL S_NORMAL)

#ifndef __DEBUG__//asynchronous output mode
/* enable asynchronous output mode */
#define ELOG_ASYNC_OUTPUT_ENABLE
/* buffer size for asynchronous output mode */
#define ELOG_ASYNC_OUTPUT_BUF_SIZE               (ELOG_LINE_BUF_SIZE * 10)
/* each asynchronous output's log which must end with newline sign */
#define ELOG_ASYNC_LINE_OUTPUT
/* asynchronous output mode using POSIX pthread implementation */
#define ELOG_ASYNC_OUTPUT_USING_PTHREAD
#endif
#if 0 //buffered output mode
/* enable buffered output mode */
#define ELOG_BUF_OUTPUT_ENABLE
/* buffer size for buffered output mode */
#define ELOG_BUF_OUTPUT_BUF_SIZE                 (ELOG_LINE_BUF_SIZE * 10)
#endif

#ifdef ELOG_COLOR_ENABLE
#ifdef __LINUX__
/**
* CSI(Control Sequence Introducer/Initiator) sign
* more information on https://en.wikipedia.org/wiki/ANSI_escape_code
*/
#define CSI_START                      "\033["
#define CSI_END                        "\033[0m"
/* output log front color */
#define F_BLACK                        "30;"
#define F_RED                          "31;"
#define F_GREEN                        "32;"
#define F_YELLOW                       "33;"
#define F_BLUE                         "34;"
#define F_MAGENTA                      "35;"
#define F_CYAN                         "36;"
#define F_WHITE                        "37;"
/* output log background color */
#define B_NULL
#define B_BLACK                        "40;"
#define B_RED                          "41;"
#define B_GREEN                        "42;"
#define B_YELLOW                       "43;"
#define B_BLUE                         "44;"
#define B_MAGENTA                      "45;"
#define B_CYAN                         "46;"
#define B_WHITE                        "47;"
/* output log fonts style */
#define S_BOLD                         "1m"
#define S_UNDERLINE                    "4m"
#define S_BLINK                        "5m"
#define S_NORMAL                       "22m"
/* output log default color definition: [front color] + [background color] + [show style] */
#ifndef ELOG_COLOR_ASSERT
#define ELOG_COLOR_ASSERT              (F_MAGENTA B_NULL S_NORMAL)
#endif
#ifndef ELOG_COLOR_ERROR
#define ELOG_COLOR_ERROR               (F_RED B_NULL S_NORMAL)
#endif
#ifndef ELOG_COLOR_WARN
#define ELOG_COLOR_WARN                (F_YELLOW B_NULL S_NORMAL)
#endif
#ifndef ELOG_COLOR_INFO
#define ELOG_COLOR_INFO                (F_CYAN B_NULL S_NORMAL)
#endif
#ifndef ELOG_COLOR_DEBUG
#define ELOG_COLOR_DEBUG               (F_GREEN B_NULL S_NORMAL)
#endif
#ifndef ELOG_COLOR_VERBOSE
#define ELOG_COLOR_VERBOSE             (F_BLUE B_NULL S_NORMAL)
#endif
#endif /* __LINUX__ */

#ifdef __WIN32__
#define CSI_START                      ""
#define CSI_END                        ""
/* output log front color */
#include <windows.h>
/* SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.
*/
#define F_RED                          FOREGROUND_RED
#define F_GREEN                        FOREGROUND_GREEN
#define F_BLUE                         FOREGROUND_BLUE
#define F_YELLOW                       FOREGROUND_GREEN | FOREGROUND_RED
#define F_MAGENTA                      FOREGROUND_RED | FOREGROUND_BLUE
#define F_CYAN                         FOREGROUND_GREEN | FOREGROUND_BLUE
#define F_WHITE                        FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED
#define F_BLACK                        F_YELLOW | F_MAGENTA | F_CYAN
/* output log background color */
#define B_NULL
#define B_RED                          BACKGROUND_RED
#define B_GREEN                        BACKGROUND_GREEN
#define B_BLUE                         BACKGROUND_BLUE
#define B_YELLOW                       BACKGROUND_GREEN | BACKGROUND_RED
#define B_MAGENTA                      BACKGROUND_RED | BACKGROUND_BLUE
#define B_CYAN                         BACKGROUND_BLUE | BACKGROUND_GREEN
#define B_WHITE						   BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED
#define B_BLACK                        B_MAGENTA | B_CYAN | B_YELLOW
/* output log fonts style */
#define S_BOLD                         FOREGROUND_INTENSITY
#define S_UNDERLINE
#define S_BLINK
#define S_NORMAL
/* output log default color definition: [front color] + [background color] + [show style] */
#ifndef ELOG_COLOR_ASSERT
#define ELOG_COLOR_ASSERT              F_MAGENTA
#endif
#ifndef ELOG_COLOR_ERROR
#define ELOG_COLOR_ERROR               F_RED
#endif
#ifndef ELOG_COLOR_WARN
#define ELOG_COLOR_WARN                F_YELLOW
#endif
#ifndef ELOG_COLOR_INFO
#define ELOG_COLOR_INFO                F_CYAN
#endif
#ifndef ELOG_COLOR_DEBUG
#define ELOG_COLOR_DEBUG               F_GREEN
#endif
#ifndef ELOG_COLOR_VERBOSE
#define ELOG_COLOR_VERBOSE             F_BLUE
#endif
#endif /* __WIN32__ */
#endif /* ELOG_COLOR_ENABLE */

#endif /* _ELOG_CFG_H_ */
