#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "blockchain-api.c"
#define ____ 0x00
typedef unsigned char byte;

mol_seg_t build_Transaction();
mol_seg_t build_UncleBlock();

mol_seg_t build_RawHeader() {
  mol_builder_t b;
  mol_seg_res_t res;
  MolBuilder_RawHeader_init(&b);
  uint32_t version = 0xFF;
  uint32_t compact_target = 0x12;
  uint64_t timestamp = 0x23;
  uint64_t number = 0x34;
  uint64_t epoch = 0x45;
  byte parent_hash[32] = {0};
  byte transactions_root[32] = {0};
  byte propsals_hash[32] = {0};
  byte uncles_hash[32] = {0};
  byte dao[32] = {0};

  MolBuilder_RawHeader_set_version(&b, (const uint8_t *)&version);
  MolBuilder_RawHeader_set_compact_target(&b, (const uint8_t *)&compact_target);
  MolBuilder_RawHeader_set_timestamp(&b, (const uint8_t *)&timestamp);
  MolBuilder_RawHeader_set_number(&b, (const uint8_t *)&number);
  MolBuilder_RawHeader_set_epoch(&b, (const uint8_t *)&epoch);
  MolBuilder_RawHeader_set_parent_hash(&b, (const uint8_t *)&parent_hash);
  MolBuilder_RawHeader_set_transactions_root(&b, transactions_root);
  MolBuilder_RawHeader_set_proposals_hash(&b, propsals_hash);
  MolBuilder_RawHeader_set_uncles_hash(&b, uncles_hash);
  MolBuilder_RawHeader_set_dao(&b, dao);
  res = MolBuilder_RawHeader_build(b);
  assert(res.errno == 0);
  assert(MolReader_RawHeader_verify(&res.seg, false) == 0);
  return res.seg;
}

#define FROM_INT(i) ((const uint8_t *)(&i))

mol_seg_t build_Bytes() {
  mol_builder_t b;
  mol_seg_res_t res;
  MolBuilder_Bytes_init(&b);
  MolBuilder_Bytes_push(&b, 0x12);
  MolBuilder_Bytes_push(&b, 0x34);
  MolBuilder_Bytes_push(&b, 0x56);
  res = MolBuilder_Bytes_build(b);
  assert(res.errno == 0);
  assert(MolReader_Bytes_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_BytesVec() {
  mol_builder_t b;
  mol_seg_res_t res;
  MolBuilder_BytesVec_init(&b);
  mol_seg_t bytes = build_Bytes();
  MolBuilder_BytesVec_push(&b, bytes.ptr, bytes.size);
  MolBuilder_BytesVec_push(&b, bytes.ptr, bytes.size);
  res = MolBuilder_BytesVec_build(b);
  assert(res.errno == 0);
  assert(MolReader_BytesVec_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_Byte32Vec() {
  mol_builder_t b;
  mol_seg_res_t res;
  MolBuilder_Byte32Vec_init(&b);

  byte content[32] = {0};
  MolBuilder_Byte32Vec_push(&b, content);
  MolBuilder_Byte32Vec_push(&b, content);

  res = MolBuilder_Byte32Vec_build(b);
  assert(res.errno == 0);
  assert(MolReader_Byte32Vec_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_Script() {
  mol_builder_t b;
  mol_seg_res_t res;
  MolBuilder_Script_init(&b);
  byte code_hash[32] = {0x12, 0x34, 0x56, 0x78};
  byte hash_type = 0x12;

  MolBuilder_Script_set_code_hash(&b, code_hash, 32);
  MolBuilder_Script_set_hash_type(&b, hash_type);
  mol_seg_t bytes = build_Bytes();
  MolBuilder_Script_set_args(&b, bytes.ptr, bytes.size);

  res = MolBuilder_Script_build(b);
  assert(res.errno == 0);
  assert(MolReader_Script_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_OutPoint() {
  mol_builder_t b;
  mol_seg_res_t res;
  MolBuilder_OutPoint_init(&b);

  byte tx_hash[32] = {0};
  MolBuilder_OutPoint_set_tx_hash(&b, tx_hash);
  uint32_t index = 0x12;
  MolBuilder_OutPoint_set_index(&b, FROM_INT(index));

  res = MolBuilder_OutPoint_build(b);
  assert(res.errno == 0);
  assert(MolReader_OutPoint_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_CellInput() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_CellInput_init(&b);

  uint64_t since = 0x12;
  MolBuilder_CellInput_set_since(&b, FROM_INT(since));

  mol_seg_t previous_output = build_OutPoint();
  MolBuilder_CellInput_set_previous_output(&b, previous_output.ptr);
  res = MolBuilder_CellInput_build(b);
  assert(res.errno == 0);
  assert(MolReader_CellInput_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_CellOutput() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_CellOutput_init(&b);

  uint64_t capacity = 1000;
  MolBuilder_CellOutput_set_capacity(&b, FROM_INT(capacity), 8);
  mol_seg_t lock = build_Script();
  MolBuilder_CellOutput_set_lock(&b, lock.ptr, lock.size);
  MolBuilder_CellOutput_set_type_(&b, lock.ptr, lock.size);

  res = MolBuilder_CellOutput_build(b);
  assert(res.errno == 0);
  assert(MolReader_CellOutput_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_CellDep() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_CellDep_init(&b);
  mol_seg_t out_point = build_OutPoint();
  MolBuilder_CellDep_set_out_point(&b, out_point.ptr);
  MolBuilder_CellDep_set_dep_type(&b, 0);
  res = MolBuilder_CellDep_build(b);
  assert(res.errno == 0);
  assert(MolReader_CellDep_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_Header() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_Header_init(&b);
  mol_seg_t raw = build_RawHeader();
  MolBuilder_Header_set_raw(&b, raw.ptr);
  byte nonce[16] = {0x12, 0x34};
  MolBuilder_Header_set_nonce(&b, nonce);
  res = MolBuilder_Header_build(b);
  assert(res.errno == 0);
  assert(MolReader_Header_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_ProposalShortId() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_ProposalShortId_init(&b);
  MolBuilder_ProposalShortId_set_nth0(&b, 0x12);
  MolBuilder_ProposalShortId_set_nth1(&b, 0x34);
  res = MolBuilder_ProposalShortId_build(b);
  assert(res.errno == 0);
  return res.seg;
}

mol_seg_t build_ProposalShortIdVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_ProposalShortIdVec_init(&b);

  MolBuilder_ProposalShortIdVec_push(&b, build_ProposalShortId().ptr);
  MolBuilder_ProposalShortIdVec_push(&b, build_ProposalShortId().ptr);
  res = MolBuilder_ProposalShortIdVec_build(b);
  assert(res.errno == 0);
  assert(MolReader_ProposalShortIdVec_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_UncleBlockVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_UncleBlockVec_init(&b);

  mol_seg_t uncle_block = build_UncleBlock();

  MolBuilder_UncleBlockVec_push(&b, uncle_block.ptr, uncle_block.size);
  MolBuilder_UncleBlockVec_push(&b, uncle_block.ptr, uncle_block.size);

  res = MolBuilder_UncleBlockVec_build(b);
  assert(res.errno == 0);
  assert(MolReader_UncleBlockVec_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_TransactionVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_TransactionVec_init(&b);

  mol_seg_t tx = build_Transaction();

  MolBuilder_TransactionVec_push(&b, tx.ptr, tx.size);
  MolBuilder_TransactionVec_push(&b, tx.ptr, tx.size);

  res = MolBuilder_TransactionVec_build(b);
  assert(res.errno == 0);
  assert(MolReader_TransactionVec_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_CellDepVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_CellDepVec_init(&b);

  MolBuilder_CellDepVec_push(&b, build_CellDep().ptr);
  MolBuilder_CellDepVec_push(&b, build_CellDep().ptr);

  res = MolBuilder_CellDepVec_build(b);
  assert(res.errno == 0);
  assert(MolReader_CellDepVec_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_CellInputVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_CellInputVec_init(&b);

  MolBuilder_CellInputVec_push(&b, build_CellInput().ptr);
  MolBuilder_CellInputVec_push(&b, build_CellInput().ptr);

  res = MolBuilder_CellInputVec_build(b);
  assert(res.errno == 0);
  assert(MolReader_CellInputVec_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_CellOutputVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_CellOutputVec_init(&b);

  mol_seg_t cell_output = build_CellOutput();
  MolBuilder_CellOutputVec_push(&b, cell_output.ptr, cell_output.size);
  MolBuilder_CellOutputVec_push(&b, cell_output.ptr, cell_output.size);

  res = MolBuilder_CellOutputVec_build(b);
  assert(res.errno == 0);
  assert(MolReader_CellOutputVec_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_UncleBlock() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_UncleBlock_init(&b);
  mol_seg_t header = build_Header();
  MolBuilder_UncleBlock_set_header(&b, header.ptr, header.size);
  mol_seg_t proposal = build_ProposalShortIdVec();
  MolBuilder_UncleBlock_set_proposals(&b, proposal.ptr, proposal.size);

  res = MolBuilder_UncleBlock_build(b);
  assert(res.errno == 0);
  assert(MolReader_UncleBlock_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_RawTransaction() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_RawTransaction_init(&b);
  uint32_t version = 0x12;
  MolBuilder_RawTransaction_set_version(&b, FROM_INT(version), 4);
  mol_seg_t cell_dep_vec = build_CellDepVec();
  MolBuilder_RawTransaction_set_cell_deps(&b, cell_dep_vec.ptr,
                                          cell_dep_vec.size);
  mol_seg_t header_dep = build_Byte32Vec();
  MolBuilder_RawTransaction_set_header_deps(&b, header_dep.ptr,
                                            header_dep.size);
  mol_seg_t cell_input = build_CellInputVec();
  MolBuilder_RawTransaction_set_inputs(&b, cell_input.ptr, cell_input.size);
  mol_seg_t cell_output = build_CellOutputVec();
  MolBuilder_RawTransaction_set_outputs(&b, cell_output.ptr, cell_output.size);
  mol_seg_t output_data = build_BytesVec();
  MolBuilder_RawTransaction_set_outputs_data(&b, output_data.ptr,
                                             output_data.size);

  res = MolBuilder_RawTransaction_build(b);
  assert(res.errno == 0);
  assert(MolReader_RawTransaction_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_Transaction() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_Transaction_init(&b);
  mol_seg_t raw_transaction = build_RawTransaction();
  MolBuilder_Transaction_set_raw(&b, raw_transaction.ptr, raw_transaction.size);
  mol_seg_t witness = build_BytesVec();
  MolBuilder_Transaction_set_witnesses(&b, witness.ptr, witness.size);

  res = MolBuilder_Transaction_build(b);
  assert(res.errno == 0);
  assert(MolReader_Transaction_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_Block() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_Block_init(&b);
  mol_seg_t header = build_Header();
  MolBuilder_Block_set_header(&b, header.ptr, header.size);
  mol_seg_t uncles = build_UncleBlockVec();
  MolBuilder_Block_set_uncles(&b, uncles.ptr, uncles.size);
  mol_seg_t transaction_vec = build_TransactionVec();
  MolBuilder_Block_set_transactions(&b, transaction_vec.ptr,
                                    transaction_vec.size);
  mol_seg_t proposals = build_ProposalShortIdVec();
  MolBuilder_Block_set_proposals(&b, proposals.ptr, proposals.size);

  res = MolBuilder_Block_build(b);
  assert(res.errno == 0);
  assert(MolReader_Block_verify(&res.seg, false) == 0);
  return res.seg;
}

void read_Block(mol_seg_t data) {
  mol_seg_t txs = MolReader_Block_get_transactions(&data);
  mol_seg_res_t res = MolReader_TransactionVec_get(&txs, 0);
  assert(res.errno == 0);
  mol_seg_t tx0 = res.seg;
  mol_seg_t raw = MolReader_Transaction_get_raw(&tx0);
  mol_seg_t outputs = MolReader_RawTransaction_get_outputs(&raw);
  res = MolReader_CellOutputVec_get(&outputs, 0);
  assert(res.errno == 0);
  mol_seg_t output = res.seg;
  mol_seg_t capacity = MolReader_CellOutput_get_capacity(&output);
  assert(mol_unpack_number(capacity.ptr) == 1000);

  mol_seg_t lock = MolReader_CellOutput_get_lock(&output);
  mol_seg_t code_hash = MolReader_Script_get_code_hash(&lock);
  assert(code_hash.ptr[0] == 0x12 && code_hash.ptr[1] == 0x34);
}

// -----------------------------------------------------
// start of sample API
// -----------------------------------------------------

//
// referenced API or macros
//
#define ASSERT assert

// predefined type
// If the types defined in schema is fundamental type:
// 1. dynvec of byte(<byte>) or
// 2. array([byte; N])
// They will be converted to the type mol_seg_t automatically
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
#define IS_LE()       \
  ((union {           \
     uint16_t i;      \
     unsigned char c; \
   }){.i = 1}         \
       .c)

void change_endian(uint8_t *ptr, int size) {
  if (IS_LE()) return;
  ASSERT(size % 2 == 0);
  uint8_t t = 0;
  for (int i = 0; i < size / 2; i++) {
    SWAP(ptr[i], ptr[size - 1 - i], t);
  }
}

Uint64 convert_to_Uint64(mol_seg_t *seg) {
  uint64_t ret = *((uint64_t *)seg->ptr);
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Int64 convert_to_Int64(mol_seg_t *seg) {
  int64_t ret = *((int64_t *)seg->ptr);
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Uint32 convert_to_Uint32(mol_seg_t *seg) {
  uint32_t ret = *((uint32_t *)seg->ptr);
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Int32 convert_to_Int32(mol_seg_t *seg) {
  int32_t ret = *((int32_t *)seg->ptr);
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Uint16 convert_to_Uint16(mol_seg_t *seg) {
  uint16_t ret = *((uint16_t *)seg->ptr);
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Int16 convert_to_Int16(mol_seg_t *seg) {
  int16_t ret = *((int16_t *)seg->ptr);
  change_endian((uint8_t *)&ret, sizeof(ret));
  return ret;
}

Uint8 convert_to_Uint8(mol_seg_t *seg) {
  uint8_t ret = *((uint8_t *)seg->ptr);
  return ret;
}

Int8 convert_to_Int8(mol_seg_t *seg) {
  int8_t ret = *((int8_t *)seg->ptr);
  return ret;
}

//
// auto generated content
//

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
  header.seg = MolReader_Block_get_header(&block->seg);
  header.tbl = GetHeaderVTable();
  return header;
}

struct TransactionVecType Block_get_transactions_impl(struct BlockType *block) {
  struct TransactionVecType header;
  header.seg = MolReader_Block_get_transactions(&block->seg);
  header.tbl = GetTransactionVecVTable();
  return header;
}

mol_seg_t Header_get_nonce_impl(struct HeaderType *this) {
  // nonce is an array, can return directly
  return MolReader_Header_get_nonce(&this->seg);
}

Uint32 RawHeader_get_version_impl(struct RawHeaderType *this) {
  mol_seg_t s = MolReader_RawHeader_get_version(&this->seg);
  return convert_to_Uint32(&s);
}

RawHeaderType Header_get_raw_impl(struct HeaderType *this) {
  struct RawHeaderType raw;
  raw.seg = MolReader_Header_get_raw(&this->seg);
  raw.tbl = GetRawHeaderVTable();
  return raw;
}

TransactionType TransactionVec_at_impl(struct TransactionVecType *this,
                                       size_t index) {
  struct TransactionType ret;
  mol_seg_res_t res = MolReader_TransactionVec_get(&this->seg, index);
  ASSERT(res.errno == 0);
  ret.seg = res.seg;
  ret.tbl = GetTransactionVTable();
  return ret;
}

size_t TransactionVec_len_impl(struct TransactionVecType *this) {
  return (size_t)MolReader_TransactionVec_length(&this->seg);
}

BytesVecType Transaction_get_witnesses(struct TransactionType *this) {
  struct BytesVecType ret;
  ret.seg = MolReader_Transaction_get_witnesses(&this->seg);
  ret.tbl = GetBytesVecVTable();
  return ret;
}

mol_seg_t BytesVec_at_impl(struct BytesVecType *this, size_t index) {
  mol_seg_t ret;
  mol_seg_res_t res = MolReader_BytesVec_get(&this->seg, index);
  ASSERT(res.errno == 0);
  // Bytes is a dynvec of byte, need to be converted.
  return MolReader_Bytes_raw_bytes(&res.seg);
}

size_t BytesVec_len_impl(struct BytesVecType *this) {
  return (size_t)MolReader_BytesVec_length(&this->seg);
}

uint32_t RawTransaction_get_version_impl(struct RawTransactionType *this) {
  mol_seg_t s = MolReader_RawTransaction_get_version(&this->seg);
  return convert_to_Uint32(&s);
}

RawTransactionType Transaction_get_raw(struct TransactionType *this) {
  struct RawTransactionType ret;
  ret.seg = MolReader_Transaction_get_raw(&this->seg);
  ret.tbl = GetRawTransactionVTable();
  return ret;
}

//
// end of Sample API
//
void read_with_new_api(mol_seg_t data) {
  // note, the life time of the memory which data.ptr points to,
  // should be longer than any other objects(header, raw, egc)
  // which are derived from "block"
  BlockType block = make_Block(&data);
  HeaderType header = block.tbl->header(&block);
  mol_seg_t nonce = header.tbl->nonce(&header);
  assert(nonce.size == 16 && nonce.ptr[0] == 0x12 && nonce.ptr[1] == 0x34);
  RawHeaderType raw = header.tbl->raw(&header);
  uint32_t version = raw.tbl->version(&raw);
  assert(version == 0xFF);
  TransactionVecType txs = block.tbl->transactions(&block);
  size_t length = txs.tbl->len(&txs);
  assert(length == 2);
  TransactionType tx0 = txs.tbl->at(&txs, 0);
  BytesVecType witnesses = tx0.tbl->witnesses(&tx0);
  size_t witnesses_length = witnesses.tbl->len(&witnesses);
  assert(witnesses_length == 2);

  mol_seg_t witness = witnesses.tbl->at(&witnesses, 0);
  assert(witness.size == 3 && witness.ptr[0] == 0x12);

  RawTransactionType raw_tx = tx0.tbl->raw(&tx0);
  uint32_t tx_version = raw_tx.tbl->version(&raw_tx);
  assert(tx_version == 0x12);

  printf("done");
}

int main(int argc, const char *argv[]) {
  mol_seg_t block = build_Block();
  read_Block(block);

  read_with_new_api(block);
  return 0;
}
