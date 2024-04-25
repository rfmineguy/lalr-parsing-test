SOURCES 	:= src/main.c src/tokenizer.c \
						 src/svimpl.c src/lalr.c

.PHONY: always build
always:
	mkdir -p out
build: always out/main
out/main:
	gcc $(SOURCES) -o $@
