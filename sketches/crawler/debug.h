#ifndef _DEBUG_H_
#define _DEUBG_H_

#define  DEBUG_LEVEL_INFO  1
#define  DEBUG_LEVEL_ERR   2

#ifndef  DEBUG_LEVEL
#define  DEBUG_LEVEL  0
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
#define DEBUG_INFO(fmt, ...) \
    do{\
        arduino_printf(fmt, ##__VA_ARGS__);\
    }while(0)
#else
#define DEBUG_INFO(fmt, ...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_ERR
#define DEBUG_ERR(fmt, ...) \
    do{\
        arduino_printf("[Error][%s:%s:%d]",__FILE__,__FUNCTION__,__LINE__);\
        arduino_printf(fmt, ##__VA_ARGS__);\
    }while(0)
#else
#define DEBUG_ERR(fmt, ...)
#endif

void arduino_printf(char *fmt ,...);

#endif  /*  _DEBUG_  */
