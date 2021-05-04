#ifndef _DEBUG_H_
#define _DEUBG_H_

#define  DEBUG_LEVEL_INFO  1
#define  DEBUG_LEVEL_ERR   2
#define  DEBUG_LEVEL_NONE  0xFF

#ifndef  DEBUG_LEVEL
#define  DEBUG_LEVEL  DEBUG_LEVEL_INFO
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_ERR
#define INIT_DEBUG() Serial.begin(9600)
#else
#define INIT_DEBUG()
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
#define DEBUG_INFO(fmt, ...) \
    do{\
        debug_printf(fmt, ##__VA_ARGS__);\
    }while(0)
#else
#define DEBUG_INFO(fmt, ...)
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_ERR
#define DEBUG_ERR(fmt, ...) \
    do{\
        debug_printf("[Error][%s:%s:%d]",__FILE__,__FUNCTION__,__LINE__);\
        debug_printf(fmt, ##__VA_ARGS__);\
    } while(0)
#else
#define DEBUG_ERR(fmt, ...)
#endif

void debug_printf(char* fmt, ...);

#endif  /*  _DEBUG_H  */
