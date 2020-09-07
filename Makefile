
CFLAGS ?=
LOCAL_CFLAGS = -I generated -I include

.PHONY: all clean

all: build/blockchain-builder

generated/blockchain-api.c: mol/blockchain.mol
	moleculec --language c --schema-file mol/blockchain.mol > generated/blockchain-api.c

generated/types-api.c: mol/types.mol
	moleculec --language c --schema-file mol/types.mol > generated/types-api.c

fmt: src/builder-example.c src/blockchain-builder.c
	clang-format -i -style=Google src/builder-example.c
	clang-format -i -style=Google src/blockchain-builder.c
	clang-format -i -style=Google include/molecule2_reader.h
	clang-format -i -style=Google generated/blockchain-api2.h

build/blockchain-builder: src/blockchain-builder.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) $< -o $@

clean:
	#rm -f generated/blockchain-api.c generated/types-api.c
	rm -f build/blockchain-builder

