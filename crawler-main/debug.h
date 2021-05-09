#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "debugLevels.h"
#include "WString.h"

#ifndef  DEBUG_LEVEL
#define  DEBUG_LEVEL  DEBUG_LEVEL_NONE
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_ERR
#define INIT_DEBUG() Serial.begin(9600)
#else
#define INIT_DEBUG()
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_INFO
#define DEBUG_INFO(fmt, ...) \
    do{\
        debug_printf(F(fmt), ##__VA_ARGS__);\
    }while(0)
#else
#define DEBUG_INFO(fmt, ...)
#endif

#if DEBUG_LEVEL <= DEBUG_LEVEL_ERR
#define DEBUG_ERR(fmt, ...) \
    do{\
        debug_printf(F("[Error][%s:%s:%d]"),__FILE__,__FUNCTION__,__LINE__);\
        debug_printf(F(fmt), ##__VA_ARGS__);\
    } while(0)
#else
#define DEBUG_ERR(fmt, ...)
#endif

#ifndef DEBUG_BUFFER_SIZE
#define DEBUG_BUFFER_SIZE  128
#endif

int debug_freeMemory();
void debug_printf(const __FlashStringHelper* fmt, ...);

#endif  /*  _DEBUG_H  */
