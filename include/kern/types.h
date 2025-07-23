#ifndef ROOT_TYPES_H
#define ROOT_TYPES_H

#include <stddef.h>
#include <stdint.h>

#define ROOT_FALSE 0
#define ROOT_TRUE  1

#define ROOT_SUCCESS 0
#define ROOT_EINVAL  -2
#define ROOT_ENOENT  -3
#define ROOT_EDUPENT -4
#define ROOT_EALLOC  -5
#define ROOT_EDEV    -6

typedef uint8_t root_uint8_t;
typedef uint16_t root_uint16_t;
typedef uint32_t root_uint32_t;
typedef uint64_t root_uint64_t;

typedef int8_t root_int8_t;
typedef int16_t root_int16_t;
typedef int32_t root_int32_t;
typedef int64_t root_int64_t;

typedef root_int32_t root_ssize_t;
typedef size_t root_size_t;

typedef uintmax_t root_uintmax_t;
typedef intmax_t root_intmax_t;

typedef uintptr_t root_uintptr_t;
typedef intptr_t root_intptr_t;

typedef ptrdiff_t root_ptrdiff_t;

typedef root_int8_t root_err_t;

typedef root_int8_t root_boolean;

#endif
