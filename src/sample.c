#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "sample-api2.h"
#include "sample-builder.h"

int main(int argc, const char* argv[]) {
  mol_seg_t sample_table = build_SampleTable();

  mol2_cursor_t sample_table2;
  sample_table2.offset = 0;
  sample_table2.size = sample_table.size;
  sample_table2.read = mol2_source_memory;
  sample_table2.arg = sample_table.ptr;

  SampleTableType t = make_SampleTable(&sample_table2);
  mol2_cursor_t b2 = t.tbl->byte2(&t);
  uint8_t buff[2];
  mol2_read_at(&b2, buff, 2);
  assert(buff[0] == 1);
  assert(buff[1] == 2);

  SampleDynVectorType byte_2d = t.tbl->byte_2d_vector(&t);
  int existing = 0;
  uint32_t len = byte_2d.tbl->len(&byte_2d);
  mol2_cursor_t cur = byte_2d.tbl->get(&byte_2d, 0, &existing);
  mol2_read_at(&cur, buff, 2);
  assert(buff[0] == 0xBE);
  assert(buff[1] == 0xEF);
  cur = byte_2d.tbl->get(&byte_2d, 1, &existing);
  mol2_read_at(&cur, buff, 2);
  assert(buff[0] == 0xBE);
  assert(buff[1] == 0xEF);

  return 0;
}
