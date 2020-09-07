cmake_minimum_required(VERSION 3.15)
project(molecule_toolkit)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(generated)

add_executable(builder-example
        include/molecule_builder.h
        include/molecule_reader.h
        src/builder-example.c)

add_executable(blockchain-builder
        include/molecule_reader.h
        include/molecule2_reader.h
        include/molecule_builder.h
        src/blockchain-builder.c)
