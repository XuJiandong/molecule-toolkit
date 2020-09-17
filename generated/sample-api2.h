// Generated by Molecule 0.6.1

#define MOLECULEC_VERSION 6001
#define MOLECULE_API_VERSION_MIN 5000

#include "molecule2_reader.h"
#include "molecule_builder.h"

#ifndef _SAMPLE_API2_H_
#define _SAMPLE_API2_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef MOLECULE_API_DECORATOR
#define __DEFINE_MOLECULE_API_DECORATOR_SAMPLE
#define MOLECULE_API_DECORATOR
#endif /* MOLECULE_API_DECORATOR */

/*
 * Reader APIs
 */

// class
// format: {Name}Type
struct SampleStructType;
struct SampleTableType;
struct SampleDynVectorType;

// class's virtual tables
// format: {Name}VTable
struct SampleStructVTable;
struct SampleTableVTable;
struct SampleDynVectorVTable;

// functions to get virtual tables
// format: {Name}VTable* Get{Name}VTable
struct SampleStructVTable* GetSampleStructVTable(void);
struct SampleTableVTable* GetSampleTableVTable(void);
struct SampleDynVectorVTable* GetSampleDynVectorVTable(void);


// entries of virtual tables
// format: {ReturnType} {Name}_get_<Field>_impl({Name}Type* this)
mol2_cursor_t SampleStruct_get_byte2_impl(struct SampleStructType*);
uint32_t SampleStruct_get_u32_impl(struct SampleStructType*);
mol2_cursor_t SampleTable_get_byte2_impl(struct SampleTableType*);
mol2_cursor_t SampleDynVector_get_impl(struct SampleDynVectorType*, uint32_t, int*);
uint32_t SampleDynVector_len_impl(struct SampleDynVectorType*);
struct SampleDynVectorType SampleTable_get_byte_2d_vector_impl(struct SampleTableType*);

// --------- declaration above ------------

// ----------definition below -------------

// definition of virtual table
typedef struct SampleStructVTable {
  mol2_cursor_t (*byte2)(struct SampleStructType *);
  uint32_t (*u32)(struct SampleStructType *);
} SampleStructVTable;

typedef struct SampleTableVTable {
  mol2_cursor_t (*byte2)(struct SampleTableType*);
  struct SampleDynVectorType (*byte_2d_vector)(struct SampleTableType*);
} SampleTableVTable;

typedef struct SampleDynVectorVTable {
  uint32_t (*len)(struct SampleDynVectorType*);
  mol2_cursor_t (*get)(struct SampleDynVectorType*, uint32_t, int*);
} SampleDynVectorVTable;

// definition of class
typedef struct SampleStructType {
  mol2_cursor_t cur;
  SampleStructVTable *tbl;
} SampleStructType;

typedef struct SampleTableType {
  mol2_cursor_t cur;
  SampleTableVTable *tbl;
} SampleTableType;

typedef struct SampleDynVectorType {
  mol2_cursor_t cur;
  SampleDynVectorVTable *tbl;
} SampleDynVectorType;

// definition of "make" class instance
struct SampleStructType make_SampleStruct(mol2_cursor_t *cur) {
  SampleStructType ret;
  ret.cur = *cur;
  ret.tbl = GetSampleStructVTable();
  return ret;
}

struct SampleTableType make_SampleTable(mol2_cursor_t *cur) {
  SampleTableType ret;
  ret.cur = *cur;
  ret.tbl = GetSampleTableVTable();
  return ret;
}

struct SampleDynVectorType make_SampleDynVector(mol2_cursor_t *cur) {
  SampleDynVectorType ret;
  ret.cur = *cur;
  ret.tbl = GetSampleDynVectorVTable();
  return ret;
}

// definition of "get" class vtable
struct SampleStructVTable *GetSampleStructVTable(void) {
  static SampleStructVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.u32 = SampleStruct_get_u32_impl;
  s_vtable.byte2 = SampleStruct_get_byte2_impl;
  return &s_vtable;
}

struct SampleTableVTable *GetSampleTableVTable(void) {
  static SampleTableVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.byte2 = SampleTable_get_byte2_impl;
  s_vtable.byte_2d_vector = SampleTable_get_byte_2d_vector_impl;
  return &s_vtable;
}

struct SampleDynVectorVTable *GetSampleDynVectorVTable(void) {
  static SampleDynVectorVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.len = SampleDynVector_len_impl;
  s_vtable.get = SampleDynVector_get_impl;

  return &s_vtable;
}

// entries of virtual tables
mol2_cursor_t SampleStruct_get_byte2_impl(SampleStructType* this) {
  mol2_cursor_t ret;
  ret = mol2_slice_by_offset(&this->cur, 0, 4);
  return ret;
}

uint32_t SampleStruct_get_u32_impl(SampleStructType* this) {
  uint32_t ret;
  mol2_cursor_t temp = mol2_slice_by_offset(&this->cur, 4, 2);
  ret = convert_to_Uint32(&temp);
  return ret;
}

mol2_cursor_t SampleTable_get_byte2_impl(SampleTableType* this) {
  mol2_cursor_t ret;
  ret = mol2_table_slice_by_index(&this->cur, 1);
  return ret;
}

mol2_cursor_t SampleDynVector_get_impl(SampleDynVectorType* this, uint32_t index, int* existing) {
  mol2_cursor_t ret;
  mol2_cursor_res_t res = mol2_dynvec_slice_by_index(&this->cur, index);
  *existing = ((res.errno == 0) ? 1 : 0);
  ret = res.cur;
  return ret;
}

uint32_t SampleDynVector_len_impl(SampleDynVectorType* this) {
  return mol2_dynvec_length(&this->cur);
}

SampleDynVectorType SampleTable_get_byte_2d_vector_impl(SampleTableType* this) {
  SampleDynVectorType ret;
  mol2_cursor_t cur = mol2_table_slice_by_index(&this->cur, 1);
  ret.cur = cur;
  ret.tbl = GetSampleDynVectorVTable();
  return ret;
}

#ifdef __DEFINE_MOLECULE_API_DECORATOR_SAMPLE
#undef MOLECULE_API_DECORATOR
#undef __DEFINE_MOLECULE_API_DECORATOR_SAMPLE
#endif /* __DEFINE_MOLECULE_API_DECORATOR_SAMPLE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SAMPLE_API2_H_ */
