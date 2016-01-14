#ifndef HUTILS_STDINT_H
#define HUTILS_STDINT_H

#include <hconf/include/hutils/stdint.h>

#if defined(HCONF_STDINT_STDINT_H)
/* LIBS=dont */
# include <stdint.h>
#elif defined(HCONF_STDINT_SYS_TYPES_H)
/* LIBS=dont */
# include <sys/types.h>
typedef u_int8_t uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int64_t uint64_t;
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
#else
# error Not hconf:ed.
#endif

#define HCONF_TEST \
	char pi8[1 == sizeof(int8_t) ? 1 : -1];\
	char pu8[1 == sizeof(uint8_t) ? 1 : -1];\
	char pi16[2 == sizeof(int16_t) ? 1 : -1];\
	char pu16[2 == sizeof(uint16_t) ? 1 : -1];\
	char pi32[4 == sizeof(int32_t) ? 1 : -1];\
	char pu32[4 == sizeof(uint32_t) ? 1 : -1];\
	char pi64[8 == sizeof(int64_t) ? 1 : -1];\
	char pu64[8 == sizeof(uint64_t) ? 1 : -1];\
	char pip[sizeof(void *) == sizeof(intptr_t) ? 1 : -1];\
	char pup[sizeof(void *) == sizeof(uintptr_t) ? 1 : -1];\
	(void)pi8;\
	(void)pu8;\
	(void)pi16;\
	(void)pu16;\
	(void)pi32;\
	(void)pu32;\
	(void)pi64;\
	(void)pu64;\
	(void)pip;\
	(void)pup

#endif

