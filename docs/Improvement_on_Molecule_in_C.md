# Improvement on Molecule in C

## Introduction
The [molecule](https://github.com/nervosnetwork/molecule), is yet another serialization system: minimalist and canonicalization. The official code generator only supports two languages: Rust and C. Here we discuss the generator for C. Our target is to improve the old generator to get better user experience and performance.

## What problem we have met
To improve anything, the first thing to do is to experience the old one and to find some problems. We will cover user experience and performance on the old molecule generate in C. We take [Block Definition](https://github.com/XuJiandong/molecule-toolkit/blob/master/mol/blockchain.mol) as an example. It's the most important part of the CKB data structure. The following files are involved:

- [Generated file](https://github.com/XuJiandong/molecule-toolkit/blob/master/generated/blockchain-api.c)
- [Predefined reader](https://github.com/XuJiandong/molecule-toolkit/blob/master/include/molecule_reader.h)
- [Predefined builder](https://github.com/XuJiandong/molecule-toolkit/blob/master/include/molecule_builder.h)
- [Example to read and build](https://github.com/XuJiandong/molecule-toolkit/blob/master/src/blockchain-builder.c)

The generated file is generated from blockchain.mol file by the molecule compiler. And predefined file (reader and builder), are not generated. They can be considered as toolkit files. The last file (Example to read and build) is to use the 3 files mentioned above.
 Here, the word "reader" is the same meaning of "deserializer/loader". The "builder" is the same meaning of "serializer/writer/dumper".

### Problem on builder
In order to generate an entire block, we need the following code:
[Build Block] (https://github.com/XuJiandong/molecule-toolkit/blob/ceac3b407ec13b689fc92bb212add65c8d432fd6/src/blockchain-builder.c#L12-L369). Note, it's not a formal block, just a demo: it is filled with a lot of random data. After this demo task, we have met 2 problems:

1. Too many codes to generate a block structure

There are more than 300 lines of code.  With some macro, we could remove some duplicated code: it's still too much and complex.

2. Can only be built from the bottom to top

Normally, when we use a serialization framework, it's very common to set what we need and then call "serialize/build/dump" functions at the end. We don't care about when and where to set the fields, variables. But in the molecule, it's not like this: 

Take (build_Block)[https://github.com/XuJiandong/molecule-toolkit/blob/ceac3b407ec13b689fc92bb212add65c8d432fd6/src/blockchain-builder.c#L349] as an example:
1. It requires "header", so "build_Header" is called
2. The "header" requires "raw", so "build_RawHeader" is called
3. continue...

The routine is like this:
Block->Header->RawHeader->version, compact_target, ... ... . From the execution point of view, it builds bottom fields first, then to build the upper data structure. 

So can we firstly build the top part of the data? No, because the "Header" of the top part is unknown when the bottom part is not built, especially the "items-count", "full-size", "offset-N".... See more in [Molecule header](https://github.com/nervosnetwork/rfcs/blob/master/rfcs/0008-serialization/0008-serialization.md#summary).

It's complicated and inconvenient.

### Good design on reader
We now look into reading part of molecule in C. The (read_Block)[https://github.com/XuJiandong/molecule-toolkit/blob/ceac3b407ec13b689fc92bb212add65c8d432fd6/src/blockchain-builder.c#L370-L386] demo function shows how to get capacity and lock type's code hash of one cell. Note, the following function calls only move the pointer to right position and don't fully unpack all the data:
```C
MolReader_Block_get_transactions(&data);
MolReader_TransactionVec_get(&txs, 0);
MolReader_Transaction_get_raw(&tx0);
MolReader_RawTransaction_get_outputs(&raw);
...
```
It can read data partially with great performance. If we need only several fields from a complicated data structure, it's a good choice to use old API. It's especially useful for scripting in [CKB-VM](https://github.com/nervosnetwork/ckb-vm) and [CKB Misc scripts](https://github.com/nervosnetwork/ckb-miscellaneous-scripts), which doesn't require many fields of the whole block.


### Problem on reader
What if we need almost every field of the data structure? The old molecule API has no good solution: we need to write a lot of code like above to fetch all the fields.

## Full reader and builder
Now we know that the problem of old molecule API lacks of full reader and builder. The ideal scenario for builder is like following:
The molecule generates a data structure for user. The data structure is is very similar to the molecule schema. The user can
set and modify the fields easily. When the modification is done, they can invoke "build" function and get the binary stream.

The ideal scenario for reader is the same: it invokes "read" function and get a full data structure. Then user can access every fields easily. 
The difficult part is how to design such a "data structure", according to the molecule schema. It's not so difficult for some advanced programming languages like 
C++/Java/Python/Ruby/Go. Because they have a lot of features like: reflection, operator overload, class and method, closure, GC and many more. The C programming language
doesn't have anything of them.


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


## Definition of the "Data Structure"

This is the most difficult part due to the limitation of the C programming languages. TODO


## Conclusion
From a demo usage of molecule, we know that it supports reading partially with great performance. But it doesn't support full reader and builder.
The difficult part is to how to design such a data structure by the molecule compile in C programming language.