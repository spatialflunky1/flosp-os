#ifndef DATATYPES_H
#define DATATYPES_H

typedef unsigned char ui8_t;
typedef unsigned short ui16_t;
typedef unsigned int ui32_t;
typedef unsigned int __attribute__((__mode__(__DI__))) ui64_t;

typedef signed char i8_t;
typedef signed short i16_t;
typedef signed int i32_t;
typedef signed int __attribute__((__mode__(__DI__))) i64_t;

typedef ui8_t bool;
#define true 1
#define false 0

typedef ui16_t size_t;

#define UI64_T_MAX_BIN_DIGITS 64
#define UI64_T_MAX_DEC_DIGITS 20
#define UI64_T_MAX_HEX_DIGITS 16

#define NULL (void*)0

#endif
