#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "blockchain-api.c"
#define ____ 0x00
typedef unsigned char byte;


mol_seg_t build_RawHeader() {
  mol_builder_t b;
  mol_seg_res_t res;
  MolBuilder_RawHeader_init(&b);
  uint32_t version = 0;
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

#define FROM_INT(i) ((const uint8_t*)(&i))


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

mol_seg_t build_Script() {
  mol_builder_t b;
  mol_seg_res_t res;
  MolBuilder_Script_init(&b);
  byte code_hash[32] = {0};
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

  uint64_t capacity = 0;
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
  byte nonce[16] = {0};
  MolBuilder_Header_set_nonce(&b, nonce);
  res = MolBuilder_Header_build(b);
  assert(res.errno == 0);
  assert(MolReader_Header_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_ProposalShortIdVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  return res.seg;
}

mol_seg_t build_UncleBlockVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  return res.seg;
}

mol_seg_t build_TransactionVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  return res.seg;
}

mol_seg_t build_CellDepVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  return res.seg;

}

mol_seg_t build_CellInputVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  return res.seg;

}

mol_seg_t build_CellOutputVec() {
  mol_builder_t b;
  mol_seg_res_t res;

  return res.seg;
}

mol_seg_t build_UncleBlock() {
  mol_builder_t b;
  mol_seg_res_t res;

  MolBuilder_UncleBlock_init(&b);
  mol_seg_t header = build_Header();
  MolBuilder_UncleBlock_set_header(&b, header.ptr, header.size);
  //
  // MolBuilder_UncleBlock_set_proposals(&b, proposals);

  res = MolBuilder_UncleBlock_build(b);
  assert(res.errno == 0);
  assert(MolReader_UncleBlock_verify(&res.seg, false) == 0);
  return res.seg;
}



int main(int argc, const char* argv[]) {
  build_RawHeader();
}
