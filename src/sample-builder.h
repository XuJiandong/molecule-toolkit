#ifndef MOLECULE_TOOLKIT_SRC_SAMPLE_BUILDER_H_
#define MOLECULE_TOOLKIT_SRC_SAMPLE_BUILDER_H_

#include "sample-api.h"

mol_seg_t build_SampleByte2() {
  mol_builder_t b;
  MolBuilder_SampleByte2_init(&b);
  MolBuilder_SampleByte2_set_nth0(&b, 1);
  MolBuilder_SampleByte2_set_nth1(&b, 2);
  mol_seg_res_t res = MolBuilder_SampleByte2_build(b);
  return res.seg;
}

mol_seg_t build_SampleFixedVector() {
  mol_builder_t b;
  MolBuilder_SampleFixedVector_init(&b);
  mol_seg_t s1 = build_SampleByte2();
  MolBuilder_SampleFixedVector_push(&b, 0xBE);
  MolBuilder_SampleFixedVector_push(&b, 0xEF);
  mol_seg_res_t res = MolBuilder_SampleFixedVector_build(b);

  assert(res.errno == 0);
  assert(MolReader_SampleFixedVector_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_SampleDynVector() {
  mol_builder_t b;
  MolBuilder_SampleDynVector_init(&b);
  mol_seg_t s1 = build_SampleFixedVector();
  MolBuilder_SampleDynVector_push(&b, s1.ptr, s1.size);
  MolBuilder_SampleDynVector_push(&b, s1.ptr, s1.size);
  mol_seg_res_t res = MolBuilder_SampleDynVector_build(b);

  assert(res.errno == 0);
  assert(MolReader_SampleDynVector_verify(&res.seg, false) == 0);
  return res.seg;
}

mol_seg_t build_SampleTable() {
  mol_builder_t b;
  MolBuilder_SampleTable_init(&b);
  mol_seg_t s1 = build_SampleDynVector();
  mol_seg_t s2 = build_SampleByte2();
  MolBuilder_SampleTable_set_byte_2d_vector(&b, s1.ptr, s1.size);
  MolBuilder_SampleTable_set_byte2(&b, s2.ptr, s2.size);
  mol_seg_res_t res = MolBuilder_SampleTable_build(b);

  assert(res.errno == 0);
  assert(MolReader_SampleTable_verify(&res.seg, false) == 0);
  return res.seg;
}


#endif //MOLECULE_TOOLKIT_SRC_SAMPLE_BUILDER_H_
