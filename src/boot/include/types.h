#ifndef ROOT_TYPE_H
#define ROOT_TYPE_H 1

#include <stddef.h>
#include <stdint.h>

typedef enum root_err_t
{
  ROOT_SUCCESS = 0,
  ROOT_ERR_ARG = -1,
  ROOT_ERR_ALLOC = -2,
  ROOT_ERR_MEM = -3,
  ROOT_EDEV = -4,
} root_err_t;

typedef uint8_t root_u8;
typedef uint16_t root_u16;
typedef uint32_t root_u32;
typedef uint64_t root_u64;

typedef int8_t root_i8;
typedef int16_t root_i16;
typedef int32_t root_i32;
typedef int64_t root_i64;

typedef size_t root_size_t;
typedef root_i32 root_ssize_t;
typedef uintptr_t root_uintptr_t;

#endif
