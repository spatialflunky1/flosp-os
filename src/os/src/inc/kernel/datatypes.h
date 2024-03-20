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

#define size_t ui16_t

#define UI64_T_MAX_DEC_DIGITS 20
#define UI64_T_MAX_HEX_DIGITS 16

#endif
