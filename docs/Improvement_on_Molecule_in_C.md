# Improvement on Molecule in C

## Introduction
The [molecule](https://github.com/nervosnetwork/molecule), is yet another serialization system: minimalist and canonicalization. The official code generator only supports two languages: Rust and C. Here we discuss the generator for C. Our target is to improve the old generator to get better user experience and performance.

## What problem we have met
To improve anything, the first thing to do is to experience the old one and to find some problems. We will cover user experience and performance on the old molecule generator in C. We take [Block Definition](https://github.com/XuJiandong/molecule-toolkit/blob/master/mol/blockchain.mol) as an example. It's the most important part of the CKB data structure. The following files are involved:

- [Generated file](https://github.com/XuJiandong/molecule-toolkit/blob/master/generated/blockchain-api.c)
- [Predefined reader](https://github.com/XuJiandong/molecule-toolkit/blob/master/include/molecule_reader.h)
- [Predefined builder](https://github.com/XuJiandong/molecule-toolkit/blob/master/include/molecule_builder.h)
- [Example to read and build](https://github.com/XuJiandong/molecule-toolkit/blob/master/src/blockchain-builder.c)

The generated file is generated from blockchain.mol file by the molecule compiler. And predefined file (reader and builder), are not generated. They can be considered as toolkit files. The last file (Example to read and build) is to use the 3 files mentioned above.
 Here, the word "reader" is the same meaning of "deserializer/loader". The "builder" is the same meaning of "serializer/writer/dumper".

### Problem on builder
In order to generate an entire block, we need the following code:
[Build Block](https://github.com/XuJiandong/molecule-toolkit/blob/ceac3b407ec13b689fc92bb212add65c8d432fd6/src/blockchain-builder.c#L12-L369). Note, it's not a formal block, just a demo: it is filled with a lot of random data. After this demo task, we have met 2 problems:

1. Too many codes to generate a block structure

There are more than 300 lines of code.  With some macro, we could remove some duplicated code: it's still too much and complex.

2. Can only be built from the bottom to the top

Normally, when we use a serialization framework, it's very common to set what we need and then call "serialize/build/dump" functions at the end. We don't care about when and where to set the fields, variables. But in the molecule, it's not like this: 

Take [build_Block](https://github.com/XuJiandong/molecule-toolkit/blob/ceac3b407ec13b689fc92bb212add65c8d432fd6/src/blockchain-builder.c#L349) as an example:
1. It requires "header", so "build_Header" is called
2. The "header" requires "raw", so "build_RawHeader" is called
3. continue...

The routine is like this:
Block->Header->RawHeader->version, compact_target, ... ... . From the execution point of view, it builds bottom fields first, then to build the upper data structure. 

So can we firstly build the top part of the data? No, because the "Header" of the top part is unknown when the bottom part is not built, especially the "items-count", "full-size", "offset-N".... See more in [Molecule header](https://github.com/nervosnetwork/rfcs/blob/master/rfcs/0008-serialization/0008-serialization.md#summary).

It's complicated and inconvenient.

### Good design on reader
We now look into reading part of molecule in C. The [read_Block](https://github.com/XuJiandong/molecule-toolkit/blob/ceac3b407ec13b689fc92bb212add65c8d432fd6/src/blockchain-builder.c#L370-L386) demo function shows how to get capacity and lock type's code hash of one cell. Note, the following function calls only move the pointer to right position and don't fully unpack all the data:
```C
MolReader_Block_get_transactions(&data);
MolReader_TransactionVec_get(&txs, 0);
MolReader_Transaction_get_raw(&tx0);
MolReader_RawTransaction_get_outputs(&raw);
...
```
It can read data partially with great performance. If we need only several fields from a complicated data structure, it's a good choice to use the old API. It's especially useful for scripting in [CKB-VM](https://github.com/nervosnetwork/ckb-vm) and [CKB Misc scripts](https://github.com/nervosnetwork/ckb-miscellaneous-scripts), which doesn't require many fields of the whole block.

### Problem on reader (1)
What if we need almost every field of the data structure? The old molecule API has no good solution: we need to write a lot of code like above to fetch all the fields.

### Problem on reader (2)
If we look into the code of [read_Block](https://github.com/XuJiandong/molecule-toolkit/blob/ceac3b407ec13b689fc92bb212add65c8d432fd6/src/blockchain-builder.c#L370-L386), we find that mol_seg_t is everywhere: that's the problem. We can't use type system of C compilers to check whether we use the API correctly. The type system can help programmer to reduce possibilities for bugs, checking that the code is written in a consistent way, giving hint while coding, etc.

In order to solve this problem, we will try to introduce known types in [Extra support for known types](#extra-support-for-known-types). Also, we will try to add more type systems to replace "mol_seg_t", see [Add type system to API](#add-type-system-to-API).

## Full reader and builder
Now we know that the problem of old molecule API lacks of full reader and builder. The ideal scenario for builder is like the following:
The molecule generates a data structure for users. The data structure is very similar to the molecule schema. The user can
set and modify the fields easily. When the modification is done, they can invoke "build" function and get the binary data instantly.

The ideal scenario for reader is the same: the users invokes "read" function on binary data and get a full data structure. Then user can access every fields easily. 

The difficult part is how to design such a "data structure", according to the molecule schema. It's not so difficult for some advanced programming languages like 
C++/Java/Python/Ruby/Go, because they have a lot of features like: reflection, operator overload, class and method, closure, GC and many more. The C programming language
doesn't have any of them.


## Extra support for known types
From the [Encoding Spec](https://github.com/nervosnetwork/molecule/blob/master/docs/encoding_spec.md), we know that there is no types system in molecule.
For example, we can find the following definitions in [molecule](https://github.com/XuJiandong/molecule-toolkit/blob/master/mol/blockchain.mol):
```text
array Uint32 [byte; 4];
array Uint64 [byte; 8];
```
We now have "version" with type "Uint32". But with old molecule API, the API still returns "uint_8*" instead of "uint32_t".
The "mol_unpack_number" should be called to get the "uint32_t" type. 

We suggest that the following type names can be reserved for types:
- Uint8, Int8
- Uint16, Int16
- Uint32, Int32
- Uint64, Int64

When they appear in schema file, it is automatically converted to the corresponding types in the generated files.

## Add type system to API
To solve part of "problem on reader (2)", we can try to refactor the API: add type system. Let's look at the code again: [read_Block](https://github.com/XuJiandong/molecule-toolkit/blob/ceac3b407ec13b689fc92bb212add65c8d432fd6/src/blockchain-builder.c#L370-L386)
```C
void read_Block(mol_seg_t data) {
  mol_seg_t txs = MolReader_Block_get_transactions(&data);
  mol_seg_res_t res = MolReader_TransactionVec_get(&txs, 0);
  assert(res.errno == 0);
  mol_seg_t tx0 = res.cur;
  mol_seg_t raw = MolReader_Transaction_get_raw(&tx0);
  mol_seg_t outputs = MolReader_RawTransaction_get_outputs(&raw);
  res = MolReader_CellOutputVec_get(&outputs, 0);
  assert(res.errno == 0);
  mol_seg_t output = res.cur;
  mol_seg_t capacity = MolReader_CellOutput_get_capacity(&output);
  assert(mol_unpack_number(capacity.ptr) == 1000);

  mol_seg_t lock = MolReader_CellOutput_get_lock(&output);
  mol_seg_t code_hash = MolReader_Script_get_code_hash(&lock);
  assert(code_hash.ptr[0] == 0x12 && code_hash.ptr[1] == 0x34);
}
```
Here we can see, almost every function returns type with "mol_seg_t". The compiler can pass the following code:
```C
code_hash = MolReader_Script_get_code_hash(&data)
```
Which is definitely wrong. The root cause is that the variables "data" and "lock" share same type. We need to add different types for "txs", "raw", "outputs", "lock", etc. Then they can only be used properly in right functions. After adding type system, we hope to get code like below:
```C
void read_Block(BlockType data) {
  TransactionVecType txs = Block_get_transactions(&data);
  TransactionType tx0 = TransactionVec_get(&txs, 0);
  RawTransactionType raw = Transaction_get_raw(&tx0);
  CellOutputVecType outputs = RawTransaction_get_outputs(&raw);
  CellOutputType output = CellOutputVec_get(&outputs, 0);
  uint64_t capacity = CellOutput_get_capacity(&output);
  assert(capacity == 1000);

  ScriptType lock = CellOutput_get_lock(&output);
  Byte32Type code_hash = Script_get_code_hash(&lock);
}
```
with type system, it's impossible to write code like this:
```C
code_hash = MolReader_Script_get_code_hash(data);
```
The compiler will complain about mismatched type. 

Also we can provide an OOP version, like below:

```C
void read_Block(BlockType data) {
    //TransactionVecType txs = Block_get_transactions(&data); 
    TransactionVecType txs = data.tbl->trasactions(&data);
    //TransactionType tx0 = TransactionVec_get(&txs, 0);
    TransactionType tx0 = txs.tbl->get(&txs, 0);
    //RawTransactionType raw = Transaction_get_raw(&tx0);
    RawTransactionType raw = tx0.tbl->raw(&tx0);
    //CellOutputVecType outputs = RawTransaction_get_outputs(&raw);
    CellOutputVecType outputs = raw.tbl->outputs(&raw);
    //CellOutputType output = CellOutputVec_get(&outputs, 0);
    CellOutputType output = outputs.tbl->get(&outputs, 0);
    //uint64_t capacity = CellOutput_get_capacity(&output);
    uint64_t capacity = output.tbl->capacity(&output);
    assert(capacity == 1000);

    //ScriptType lock = CellOutput_get_lock(&output);
    ScriptType lock = output.tbl->lock(&output);
    //Byte32Type code_hash = Script_get_code_hash(&lock);
    Byte32Type code_hash = lock.tbl->code_hash(&lock);
```

Note, every function returns a "struct" instead of a "pointer to struct". It can avoid allocate memory on heap, using stack allocating instead. So we need to make sure the "struct" can't be very large. This "struct", in OOP, is well known as "instance".

We invoke methods directly using:
```C
lock.tbl->code_hash
```
instead of
```C
lock.get_code_hash
```
The reason is we need to put methods together in a table (an array of methods), and only put pointer to table (tbl) in struct. This is like [virtual methods table](https://en.wikipedia.org/wiki/Virtual_method_table) in C++. It's a trick to reduce struct size and save some copy operations while creating a new struct. We name the "tbl": vtable.


Note: This is a draft which is subject to change.

### The vtable "tbl"  and special methods
In [Molecule Spec](https://github.com/nervosnetwork/rfcs/blob/master/rfcs/0008-serialization/0008-serialization.md), there are actually 2 categories types:
1. Fundamental type
2. Compound type

There are only 3 definitions in schema which are "Fundamental type" category:
1. array of byte
2. fixvec of byte
3. byte

All the others are "Compound type", including:
1. struct
2. dynvec
3. table
4. option
5. union

Note: The names "Fundamental type" and "Compound type" are not defined in official RFC, they're just concept used here. The standard to classify these two is whether the type
can be broken down into small ones except for byte.

The fundamental type has no vtable field. They are finally converted to a special predefined type. The field "tbl" doesn't exist in predefined type.

The compound type have different vtable according to its concrete type. For example, for every fields in struct/table, it occupies one method in vtable entry with same name. The dynvec has special method like "len" and "at". Sample usage below:

```C
  TransactionVecType txs = block.tbl->transactions(&block);
  size_t length = txs.tbl->len(&txs);
  TransactionType tx0 = txs.tbl->at(&txs, 0);
```
The dynvec type (TransactionVecType) has 2 methods: "len" and "at" in "tbl".
The "option" and "union" also have their special methods.

For one of the fundamental type, array, if the type name is matched with special name, it is converted to special type, 
mentioned in [Extra support for known types](#extra-support-for-known-types). They are:
```text
array Uint8 [byte; 1]; // converted to uint8_t
array Int8 [byte; 1];  // converted to int8_t
array Uint16 [byte; 2]; // converted to uint16_t
array Int16 [byte; 2]; // converted to int16_t
array Uint32 [byte; 4]; // converted to uint32_t
array Int32 [byte; 4]; // converted to int32_t
array Uint64 [byte; 8]; // converted to uint64_t
array Int64 [byte; 8]; // converted to int64_t
```


## Improvement on mol_seg_t
mol_seg_t, is the most important data structure in old molecule API:
```C
typedef struct {
    uint8_t                     *ptr;               // Pointer
    mol_num_t                   size;               // Full size
} mol_seg_t;
```
It comes with an assumption: the data has been loaded into memory already. It's not a good design to system like [CKB-VM](https://github.com/nervosnetwork/ckb-vm) which only has very limited memory. 

As we look into the [Molecule Spec](https://github.com/nervosnetwork/rfcs/blob/master/rfcs/0008-serialization/0008-serialization.md), if we only need some part of data, we can get the data through some "hops". We can read the header only, estimating where to hop and don't need to read the remaining data. For a lot of scenarios which only need read part of data, we can have a load-on-demand mechanic.

This load-on-demand mechanic is introduced by the following [data structure](https://github.com/XuJiandong/molecule-toolkit/blob/2a843898f8bd228e93399c7d450948759c1edc35/include/molecule2_reader.h#L110-L115
):
```C
typedef struct mol2_cursor_t {
  uint32_t offset;     // offset of slice
  uint32_t size;       // size of slice
  mol2_source_t read;  // data source
  void *arg;           // data source
} mol2_cursor_t;
```
The "read" together with "arg" are the data source. The "offset" together with "size", is an "view"/"slice" of the data source. Here the relationship between "read" and the "arg" is the same as "fopen" and "FILE*" in standard C library.

We have a very simple implementation of "read" field over memory: [mol2_source_memory](https://github.com/XuJiandong/molecule-toolkit/blob/2a843898f8bd228e93399c7d450948759c1edc35/include/molecule2_reader.h#L118-L123). We can also make another one based on syscall.


When "mol2_cursor_t" is returned from one function, it doesn't access memory. As the name "cursor" suggests, it's only an cursor. We can access memory on demand by "mol2_read_at", for example:
```C
    mol2_cursor_t witness_cur = witnesses.tbl->at(&witnesses, 0);
    uint8_t witness[witness_cur.size];
    mol2_read_at(&witness_cur, witness, witness_cur.size);
    assert(witness_cur.size == 3 && witness[0] == 0x12 && witness[1] == 0x34);
```
The "mol2_read_at" is not called if we don't have interests on content of memory. Here is an [example](https://github.com/XuJiandong/molecule-toolkit/blob/2a843898f8bd228e93399c7d450948759c1edc35/src/new-api-demo.c#L12-L42): it only accesses memory by "mol2_read_at" twice: one for nonce and the other for witness.

All changes are located in [this file](https://github.com/XuJiandong/molecule-toolkit/blob/master/include/molecule2_reader.h). It's compatible with old molecule API: you can use it solely or use it with old API.

## Definition of the "Data Structure"

This is the most difficult part due to the limitation of the C programming languages. TODO


## Conclusion
From a demo usage of molecule, we know that it supports reading partially with great performance. But it doesn't support full reader and builder.
The difficult part is how to design such a data structure by the molecule compile in C programming language. Adding type system to existing API, we can make it easy to write code correctly and avoid a lot of manual converting.
