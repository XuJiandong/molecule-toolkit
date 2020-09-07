#ifndef MOLECULE2_READER_H
#define MOLECULE2_READER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

#ifndef MOLECULE2_API_DECORATOR
#define __DEFINE_MOLECULE2_API_DECORATOR
#define MOLECULE2_API_DECORATOR
#endif /* MOLECULE2_API_DECORATOR */

#define MOLECULE2_API_VERSION 5000
#define MOLECULEC_VERSION_MIN 5000

#if MOLECULE_API_VERSION < MOLECULE_API_VERSION_MIN
#error This file was generated by a newer version of moleculec which is \
    incompatible with current headers in use. Please update the headers.
#endif

#if MOLECULEC_VERSION < MOLECULEC_VERSION_MIN
#error This file was generated by an older version of moleculec which is \
    incompatible with current headers in use. Please regenerate this file \
    with a newer version of moleculec.
#endif

/*
 * This part is not for normal users.
 */
//
// referenced API or macros
//
#define ASSERT assert

// predefined type
// If the types defined in schema is fundamental type:
// 1. dynvec of byte(<byte>) or
// 2. array([byte; N])
// They will be converted to the type mol2_cursor_t automatically
// 3. byte
// it will be converted to uint8_t.

// predefined type
// If the types defined in schema:
// 1. the name is same as below (case insensitive)
// 2. the type is matched (e.g. Uint32 should have type with [byte; 4])
// they will be converted to the corresponding types automatically instead of
// just returning raw byte array.
//
typedef uint64_t Uint64;  // [byte; 8]
typedef int64_t Int64;    // [byte; 8]
typedef uint32_t Uint32;  // [byte; 4]
typedef int32_t Int32;    // [byte; 4]
typedef uint16_t Uint16;  // [byte; 2]
typedef int16_t Int16;    // [byte; 2]
typedef uint8_t Uint8;    // [byte; 1]
typedef int8_t Int8;      // [byte; 1]

// converting function
// format: convert_to_${Type}
#define SWAP(a, b, t) \
  {                   \
    (t) = (a);        \
    (a) = (b);        \
    (b) = (t);        \
  }
#define is_le2()      \
  ((union {           \
     uint16_t i;      \
     unsigned char c; \
   }){.i = 1}         \
       .c)

#define MIN(a, b) ((a > b) ? (b) : (a))

void change_endian(uint8_t *ptr, int size) {
  if (is_le2()) return;
  ASSERT(size % 2 == 0);
  uint8_t t = 0;
  for (int i = 0; i < size / 2; i++) {
    SWAP(ptr[i], ptr[size - 1 - i], t);
  }
}

/**
 * read from a data source, with offset, up to "len" bytes into ptr.
 * the memory size of "ptr" is "len".
 * The "arg" will be passed into "read" function as the first argument.
 */
typedef uint32_t (*mol2_source_t)(void *arg, uint8_t *ptr, uint32_t len,
                                  uint32_t offset);

/**
 * --------------- MUST READ ----------------------
 * This is the most important data struct in this file, MUST READ!
 * The "read" together with "arg" are the data source.
 * The data source is a function with declaration of "mol2_source_t".
 * You can get an idea how to implement your own one from function
 * "mol2_source_memory".
 *
 * The offset and size, is an "view"/"slice" of the data source.
 *
 * When a new cursor is generated according to an old one,
 * the "read" and "arg" MUST be copied. The offset and size can be different.
 * Currently, there is no way to convert one data source to another.
 */
typedef struct mol2_cursor_t {
  uint32_t offset;     // offset of slice
  uint32_t size;       // size of slice
  mol2_source_t read;  // data source
  void *arg;           // data source
} mol2_cursor_t;

// a sample source over memory
uint32_t mol2_source_memory(void *arg, uint8_t *ptr, uint32_t len,
                            uint32_t offset) {
  uint8_t *start_mem = (uint8_t *)arg;
  memcpy(ptr, start_mem + offset, len);
  return len;
}

/**
 * mol2_read_at reads up to MIN(cur->size, buff_len) bytes from data source
 * "cur" into buff. It returns the number (n) of bytes read (0 <= n <=
 * MIN(cur->size, buff_len)).
 */
uint32_t mol2_read_at(const mol2_cursor_t *cur, uint8_t *buff,
                      uint32_t buff_len) {
  uint32_t read_len = MIN(cur->size, buff_len);
  return cur->read(cur->arg, buff, read_len, cur->offset);
}

/*
 * Definitions of types and simple utilities.
 */

/* Core types */

typedef uint32_t mol2_num_t;  // Item Id

typedef uint8_t mol2_errno;  // Error Number

#define Mol2Num UINT32_C

#define mol2_NUM_T_SIZE 4

// Bytes segment.
typedef struct {
  uint8_t *ptr;     // Pointer
  mol2_num_t size;  // Full size
} mol2_seg_t;

// Unpacked Union
typedef struct {
  mol2_num_t item_id;    // Item Id
  mol2_cursor_t cursor;  // Cursor
} mol2_union_t;

// Result for returning segment.
typedef struct {
  mol2_errno errno;   // Error Number
  mol2_cursor_t cur;  // Cursor
} mol2_cursor_res_t;

/* Error Numbers */

#define mol2_OK 0x00
#define mol2_ERR 0xff

#define mol2_ERR_TOTAL_SIZE 0x01
#define mol2_ERR_HEADER 0x02
#define mol2_ERR_OFFSET 0x03
#define mol2_ERR_UNKNOWN_ITEM 0x04
#define mol2_ERR_INDEX_OUT_OF_BOUNDS 0x05
#define mol2_ERR_FIELD_COUNT 0x06
#define mol2_ERR_DATA 0x07

/* Utilities. */

MOLECULE2_API_DECORATOR mol2_num_t
mol2_unpack_number(const mol2_cursor_t *cursor) {
  uint8_t src[4];
  uint32_t len = mol2_read_at(cursor, src, 4);
  ASSERT(len == 4);
  if (is_le2()) {
    return *(const uint32_t *)src;
  } else {
    uint32_t output = 0;
    uint8_t *dst = (uint8_t *)&output;
    dst[3] = src[0];
    dst[2] = src[1];
    dst[1] = src[2];
    dst[0] = src[3];
    return output;
  }
}

/*
 * Core functions.
 */

/* Verify Functions. */

// Verify Array / Struct.
MOLECULE2_API_DECORATOR mol2_errno
mol2_verify_fixed_size(const mol2_cursor_t *input, mol2_num_t total_size) {
  return input->size == total_size ? mol2_OK : mol2_ERR_TOTAL_SIZE;
}

// Verify FixVec.
MOLECULE2_API_DECORATOR mol2_errno
mol2_fixvec_verify(const mol2_cursor_t *input, mol2_num_t item_size) {
  if (input->size < mol2_NUM_T_SIZE) {
    return mol2_ERR_HEADER;
  }
  mol2_num_t item_count = mol2_unpack_number(input);
  if (item_count == 0) {
    return input->size == mol2_NUM_T_SIZE ? mol2_OK : mol2_ERR_TOTAL_SIZE;
  }
  mol2_num_t total_size = mol2_NUM_T_SIZE + item_size * item_count;
  return input->size == total_size ? mol2_OK : mol2_ERR_TOTAL_SIZE;
}

/* Getters.
 *
 * ### Notice
 *
 * The input of getters should be checked.
 *
 * These getters will raise segmentation fault if the input is illegal or
 * return an incorrect result.
 */

// Check if an Option is None.
MOLECULE2_API_DECORATOR bool mol2_option_is_none(const mol2_cursor_t *input) {
  return input->size == 0;
}

// Get the inner of a Union.
MOLECULE2_API_DECORATOR mol2_union_t
mol2_union_unpack(const mol2_cursor_t *input) {
  mol2_union_t ret;
  ret.item_id = mol2_unpack_number(input);
  ret.cursor = *input;  // must copy
  ret.cursor.offset = input->offset + mol2_NUM_T_SIZE;
  ret.cursor.size = input->size - mol2_NUM_T_SIZE;
  return ret;
}

// Get the length of a FixVec.
MOLECULE2_API_DECORATOR mol2_num_t
mol2_fixvec_length(const mol2_cursor_t *input) {
  return mol2_unpack_number(input);
}

// Get the length of a DynVec.
MOLECULE2_API_DECORATOR mol2_num_t
mol2_dynvec_length(const mol2_cursor_t *input) {
  if (input->size == mol2_NUM_T_SIZE) {
    return 0;
  } else {
    mol2_cursor_t cur = *input;
    cur.offset = input->offset + mol2_NUM_T_SIZE;
    return (mol2_unpack_number(&cur) / 4) - 1;
  }
}

// Get the actual field count of a Table.
MOLECULE2_API_DECORATOR mol2_num_t
mol2_table_actual_field_count(const mol2_cursor_t *input) {
  return mol2_dynvec_length(input);
}

// If a Table has extra fields.
MOLECULE2_API_DECORATOR bool mol2_table_has_extra_fields(
    const mol2_cursor_t *input, mol2_num_t field_count) {
  return mol2_table_actual_field_count(input) > field_count;
}

// Slice a segment for Array / Struct by offset.
MOLECULE2_API_DECORATOR mol2_cursor_t mol2_slice_by_offset(
    const mol2_cursor_t *input, mol2_num_t offset, mol2_num_t size) {
  mol2_cursor_t cur = *input;

  cur.offset = input->offset + offset;
  cur.size = size;
  return cur;
}

// Slice a segment for FixVec by index.
MOLECULE2_API_DECORATOR mol2_cursor_res_t mol2_fixvec_slice_by_index(
    const mol2_cursor_t *input, mol2_num_t item_size, mol2_num_t item_index) {
  mol2_cursor_res_t res;
  res.cur = *input;
  mol2_num_t item_count = mol2_unpack_number(input);
  if (item_index >= item_count) {
    res.errno = mol2_ERR_INDEX_OUT_OF_BOUNDS;
  } else {
    res.errno = mol2_OK;
    res.cur.offset = input->offset + mol2_NUM_T_SIZE + item_size * item_index;
    res.cur.size = item_size;
  }
  return res;
}

// Slice a segment for DynVec by index.
MOLECULE2_API_DECORATOR mol2_cursor_res_t
mol2_dynvec_slice_by_index(const mol2_cursor_t *input, mol2_num_t item_index) {
  mol2_cursor_res_t res;
  res.cur = *input;
  struct mol2_cursor_t temp = *input;

  mol2_num_t total_size = mol2_unpack_number(input);
  if (total_size == mol2_NUM_T_SIZE) {
    res.errno = mol2_ERR_INDEX_OUT_OF_BOUNDS;
  } else {
    temp.offset = input->offset + mol2_NUM_T_SIZE;
    mol2_num_t item_count = (mol2_unpack_number(&temp) / 4) - 1;
    if (item_index >= item_count) {
      res.errno = mol2_ERR_INDEX_OUT_OF_BOUNDS;
    } else {
      temp.offset = input->offset + mol2_NUM_T_SIZE * (item_index + 1);
      mol2_num_t item_start = mol2_unpack_number(&temp);
      if (item_index + 1 == item_count) {
        res.errno = mol2_OK;
        res.cur.offset = input->offset + item_start;
        res.cur.size = total_size - item_start;
      } else {
        temp.offset = input->offset + mol2_NUM_T_SIZE * (item_index + 2);
        mol2_num_t item_end = mol2_unpack_number(&temp);
        res.errno = mol2_OK;
        res.cur.offset = input->offset + item_start;
        res.cur.size = item_end - item_start;
      }
    }
  }
  return res;
}

// Slice a segment for Table by index.
MOLECULE2_API_DECORATOR mol2_cursor_t
mol2_table_slice_by_index(const mol2_cursor_t *input, mol2_num_t field_index) {
  mol2_cursor_res_t res = mol2_dynvec_slice_by_index(input, field_index);
  return res.cur;
}

// Slice the raw bytes from a `vector <byte>` (FixVec, with a header).
MOLECULE2_API_DECORATOR mol2_cursor_t
mol2_fixvec_slice_raw_bytes(const mol2_cursor_t *input) {
  mol2_cursor_t cur = *input;
  cur.offset = input->offset + mol2_NUM_T_SIZE;
  cur.size = mol2_unpack_number(input);
  return cur;
}

// predefined type implementation
Uint64 convert_to_Uint64(mol2_cursor_t *cur) {
  uint64_t ret;
  uint32_t len = mol2_read_at(cur, (uint8_t *)&ret, sizeof(ret));
  ASSERT(len == sizeof(ret));
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Int64 convert_to_Int64(mol2_cursor_t *cur) {
  int64_t ret;
  uint32_t len = mol2_read_at(cur, (uint8_t *)&ret, sizeof(ret));
  ASSERT(len == sizeof(ret));
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Uint32 convert_to_Uint32(mol2_cursor_t *cur) {
  uint32_t ret;
  uint32_t len = mol2_read_at(cur, (uint8_t *)&ret, sizeof(ret));
  ASSERT(len == sizeof(ret));
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Int32 convert_to_Int32(mol2_cursor_t *cur) {
  int32_t ret;
  uint32_t len = mol2_read_at(cur, (uint8_t *)&ret, sizeof(ret));
  ASSERT(len == sizeof(ret));
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Uint16 convert_to_Uint16(mol2_cursor_t *cur) {
  uint16_t ret;
  uint32_t len = mol2_read_at(cur, (uint8_t *)&ret, sizeof(ret));
  ASSERT(len == sizeof(ret));
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Int16 convert_to_Int16(mol2_cursor_t *cur) {
  int16_t ret;
  uint32_t len = mol2_read_at(cur, (uint8_t *)&ret, sizeof(ret));
  ASSERT(len == sizeof(ret));
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Uint8 convert_to_Uint8(mol2_cursor_t *cur) {
  uint8_t ret;
  uint32_t len = mol2_read_at(cur, (uint8_t *)&ret, sizeof(ret));
  ASSERT(len == sizeof(ret));
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Int8 convert_to_Int8(mol2_cursor_t *cur) {
  int8_t ret;
  uint32_t len = mol2_read_at(cur, (uint8_t *)&ret, sizeof(ret));
  ASSERT(len == sizeof(ret));
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

/*
 * Undef macros which are internal use only.
 */

#undef is_le2

#ifdef __DEFINE_MOLECULE2_API_DECORATOR
#undef MOLECULE2_API_DECORATOR
#undef __DEFINE_MOLECULE2_API_DECORATOR
#endif /* __DEFINE_MOLECULE2_API_DECORATOR */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MOLECULE2_READER_H */
// -----------------------------------------------------
// start of sample API
// -----------------------------------------------------