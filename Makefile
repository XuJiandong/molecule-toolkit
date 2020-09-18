
CFLAGS ?=
LOCAL_CFLAGS = -I generated -I include

.PHONY: all clean

all: build/new-api-demo

generated/blockchain-api.h: mol/blockchain.mol
	moleculec --language c --schema-file mol/blockchain.mol > generated/blockchain-api.h

generated/sample-api.h: mol/sample.mol
	moleculec --language c --schema-file mol/sample.mol > generated/sample-api.h
	moleculec --language - --schema-file mol/sample.mol --format json > mol/sample.json

fmt: src/builder-example.c src/new-api-demo.c
	clang-format -i -style=Google src/builder-example.c
	clang-format -i -style=Google src/new-api-demo.c
	clang-format -i -style=Google src/sample.c
	clang-format -i -style=Google src/sample-builder.h
	clang-format -i -style=Google include/molecule2_reader.h
	clang-format -i -style=Google generated/sample-api2.h
	clang-format -i -style=Google generated/blockchain-api2.h

build/new-api-demo: src/new-api-demo.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) $< -o $@

clean:
	#rm -f generated/blockchain-api.c generated/types-api.c
	rm -f build/new-api-demo

