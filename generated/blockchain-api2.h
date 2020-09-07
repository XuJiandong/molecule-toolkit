
//
// auto generated content
//
#include "molecule2_reader.h"

// main class
// format: ${Name}Type
struct BlockType;
struct HeaderType;
struct RawHeaderType;
struct TransactionVecType;
struct TransactionType;
struct BytesVecType;
struct RawTransactionType;

// main classes' virtual tables
// format: ${Name}VTable
struct BlockVTable;
struct HeaderVTable;
struct RawHeaderVTable;
struct TransactionVecVTable;
struct TransactionVTable;
struct BytesVecVTable;
struct RawTransactionVTable;

// functions to get virtual tables
// format: ${Name}VTable* Get${Name}VTable
struct BlockVTable *GetBlockVTable(void);
struct HeaderVTable *GetHeaderVTable(void);
struct RawHeaderVTable *GetRawHeaderVTable(void);
struct TransactionVecVTable *GetTransactionVecVTable(void);
struct TransactionVTable *GetTransactionVTable(void);
struct BytesVecVTable *GetBytesVecVTable(void);
struct RawTransactionVTable *GetRawTransactionVTable(void);

// entries of virtual tables
// format: ${ReturnType} ${Name}_get_<Field>_impl(${Name}Type* this)
// The ReturnType normally is the type of field. But if it's same as "predefined
// type" it's converted to "predefined type" and marked as leaf function. Leaf
// functions doesn't have vtable and returns "predefined type".
mol_seg_t Header_get_nonce_impl(struct HeaderType *this);
struct HeaderType Block_get_header_impl(struct BlockType *block);
struct TransactionVecType Block_get_transactions_impl(struct BlockType *block);
Uint32 RawHeader_get_version_impl(struct RawHeaderType *raw_header);
struct RawHeaderType Header_get_raw_impl(struct HeaderType *this);
struct TransactionType TransactionVec_at_impl(struct TransactionVecType *this,
                                              size_t index);
size_t TransactionVec_len_impl(struct TransactionVecType *this);
struct BytesVecType Transaction_get_witnesses(struct TransactionType *this);
mol_seg_t BytesVec_at_impl(struct BytesVecType *this, size_t index);
size_t BytesVec_len_impl(struct BytesVecType *this);
uint32_t RawTransaction_get_version_impl(struct RawTransactionType *this);
struct RawTransactionType Transaction_get_raw(struct TransactionType *this);

typedef struct RawTransactionVTable {
  uint32_t (*version)(struct RawTransactionType *);
} RawTransactionVTable;

typedef struct RawTransactionType {
  mol_seg_t seg;
  RawTransactionVTable *tbl;
} RawTransactionType;

RawTransactionType make_RawTransaction(mol_seg_t *seg) {
  RawTransactionType ret;
  ret.seg = *seg;
  ret.tbl = GetRawTransactionVTable();
  return ret;
}

typedef struct BytesVecVTable {
  mol_seg_t (*at)(struct BytesVecType *, size_t index);
  size_t (*len)(struct BytesVecType *);
} BytesVecVTable;

typedef struct BytesVecType {
  mol_seg_t seg;
  BytesVecVTable *tbl;
} BytesVecType;

BytesVecType make_BytesVec(mol_seg_t *seg) {
  BytesVecType ret;
  ret.seg = *seg;
  ret.tbl = GetBytesVecVTable();
  return ret;
}

typedef struct TransactionVecVTable {
  struct TransactionType (*at)(struct TransactionVecType *, size_t index);
  size_t (*len)(struct TransactionVecType *);
} TransactionVecVTable;

typedef struct TransactionVecType {
  mol_seg_t seg;
  TransactionVecVTable *tbl;
} TransactionVecType;

TransactionVecType make_TransactionVec(mol_seg_t *seg) {
  TransactionVecType ret;
  ret.seg = *seg;
  ret.tbl = GetTransactionVecVTable();
  return ret;
}

typedef struct TransactionVTable {
  BytesVecType (*witnesses)(struct TransactionType *);
  RawTransactionType (*raw)(struct TransactionType *);
} TransactionVTable;

typedef struct TransactionType {
  mol_seg_t seg;
  TransactionVTable *tbl;
} TransactionType;

TransactionType make_Transaction(mol_seg_t *seg) {
  TransactionType ret;
  ret.seg = *seg;
  ret.tbl = GetTransactionVTable();
  return ret;
}

typedef struct HeaderVTable {
  mol_seg_t (*nonce)(struct HeaderType *);
  struct RawHeaderType (*raw)(struct HeaderType *);
} HeaderVTable;

typedef struct HeaderType {
  mol_seg_t seg;
  HeaderVTable *tbl;
} HeaderType;

HeaderType make_Header(mol_seg_t *seg) {
  HeaderType ret;
  ret.seg = *seg;
  ret.tbl = GetHeaderVTable();
  return ret;
}

typedef struct BlockVTable {
  struct HeaderType (*header)(struct BlockType *);
  struct TransactionVecType (*transactions)(struct BlockType *);
  // TODO
} BlockVTable;

typedef struct BlockType {
  mol_seg_t seg;
  BlockVTable *tbl;
} BlockType;

BlockType make_Block(mol_seg_t *seg) {
  BlockType ret;
  ret.seg = *seg;
  ret.tbl = GetBlockVTable();
  return ret;
}

typedef struct RawHeaderVTable {
  uint32_t (*version)(struct RawHeaderType *);
  // TODO:
} RawHeaderVTable;

typedef struct RawHeaderType {
  mol_seg_t seg;
  RawHeaderVTable *tbl;
} RawHeaderType;

struct RawHeaderType make_RawHeader(mol_seg_t *seg) {
  RawHeaderType ret;
  ret.seg = *seg;
  ret.tbl = GetRawHeaderVTable();
  return ret;
}

//
// functions to get virtual tables
//
struct BlockVTable *GetBlockVTable(void) {
  static BlockVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.header = Block_get_header_impl;
  s_vtable.transactions = Block_get_transactions_impl;
  // TODO, add more fields
  return &s_vtable;
}

struct HeaderVTable *GetHeaderVTable(void) {
  static HeaderVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.nonce = Header_get_nonce_impl;
  s_vtable.raw = Header_get_raw_impl;
  return &s_vtable;
}

struct RawHeaderVTable *GetRawHeaderVTable(void) {
  static RawHeaderVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.version = RawHeader_get_version_impl;
  // TODO, add more fields
  return &s_vtable;
}

struct TransactionVecVTable *GetTransactionVecVTable(void) {
  static TransactionVecVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.at = TransactionVec_at_impl;
  s_vtable.len = TransactionVec_len_impl;
  return &s_vtable;
}

struct TransactionVTable *GetTransactionVTable(void) {
  static TransactionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.witnesses = Transaction_get_witnesses;
  s_vtable.raw = Transaction_get_raw;
  return &s_vtable;
}

struct BytesVecVTable *GetBytesVecVTable(void) {
  static BytesVecVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.len = BytesVec_len_impl;
  s_vtable.at = BytesVec_at_impl;

  return &s_vtable;
}

struct RawTransactionVTable *GetRawTransactionVTable(void) {
  static RawTransactionVTable s_vtable;
  static int inited = 0;
  if (inited) return &s_vtable;
  s_vtable.version = RawTransaction_get_version_impl;
  return &s_vtable;
}

//
// entries of virtual tables
//
struct HeaderType Block_get_header_impl(struct BlockType *block) {
  struct HeaderType header;
  header.seg = mol_table_slice_by_index(&block->seg, 0);
  header.tbl = GetHeaderVTable();
  return header;
}

struct TransactionVecType Block_get_transactions_impl(struct BlockType *block) {
  struct TransactionVecType header;
  header.seg = mol_table_slice_by_index(&block->seg, 2);
  header.tbl = GetTransactionVecVTable();
  return header;
}

mol_seg_t Header_get_nonce_impl(struct HeaderType *this) {
  // nonce is an array, can return directly
  return mol_slice_by_offset(&this->seg, 192, 16);
}

Uint32 RawHeader_get_version_impl(struct RawHeaderType *this) {
  mol_seg_t s = mol_slice_by_offset(&this->seg, 0, 4);
  return convert_to_Uint32(&s);
}

RawHeaderType Header_get_raw_impl(struct HeaderType *this) {
  struct RawHeaderType raw;
  raw.seg = mol_slice_by_offset(&this->seg, 0, 192);
  raw.tbl = GetRawHeaderVTable();
  return raw;
}

TransactionType TransactionVec_at_impl(struct TransactionVecType *this,
                                       size_t index) {
  struct TransactionType ret;
  mol_seg_res_t res = mol_dynvec_slice_by_index(&this->seg, index);
  ASSERT(res.errno == 0);
  ret.seg = res.seg;
  ret.tbl = GetTransactionVTable();
  return ret;
}

size_t TransactionVec_len_impl(struct TransactionVecType *this) {
  return (size_t)mol_dynvec_length(&this->seg);
}

BytesVecType Transaction_get_witnesses(struct TransactionType *this) {
  struct BytesVecType ret;
  ret.seg = mol_table_slice_by_index(&this->seg, 1);
  ret.tbl = GetBytesVecVTable();
  return ret;
}

mol_seg_t BytesVec_at_impl(struct BytesVecType *this, size_t index) {
  mol_seg_t ret;
  mol_seg_res_t res = mol_dynvec_slice_by_index(&this->seg, index);
  ASSERT(res.errno == 0);
  // Bytes is a dynvec of byte, need to be converted.
  return mol_fixvec_slice_raw_bytes(&res.seg);
}

size_t BytesVec_len_impl(struct BytesVecType *this) {
  return (size_t)mol_dynvec_length(&this->seg);
}

uint32_t RawTransaction_get_version_impl(struct RawTransactionType *this) {
  mol_seg_t s = mol_table_slice_by_index(&this->seg, 0);
  return convert_to_Uint32(&s);
}

RawTransactionType Transaction_get_raw(struct TransactionType *this) {
  struct RawTransactionType ret;
  ret.seg = mol_table_slice_by_index(&this->seg, 0);
  ret.tbl = GetRawTransactionVTable();
  return ret;
}

//
// end of Sample API
//
