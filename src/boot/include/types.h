#ifndef ROOT_TYPE_H
#define ROOT_TYPE_H 1

#include <stddef.h>
#include <stdint.h>

typedef enum root_err_t
{
  ROOT_SUCCESS = 0,
  ROOT_EARG = -1,
  ROOT_EALLOC = -2,
  ROOT_EMEM = -3,
  ROOT_EDEV = -4,
  ROOT_EINVAL = -5,
} root_err_t;

typedef uint8_t root_uint8_t;
typedef uint16_t root_uint16_t;
typedef uint32_t root_uint32_t;
typedef uint64_t root_uint64_t;

typedef int8_t root_int8_t;
typedef int16_t root_int16_t;
typedef int32_t root_int32_t;
typedef int64_t root_int64_t;

typedef size_t root_size_t;
typedef root_int32_t root_ssize_t;
typedef uintptr_t root_uintptr_t;

#endif
