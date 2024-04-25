SOURCES 	:= src/main.c src/tokenizer.c \
						 src/svimpl.c src/lalr.c

.PHONY: always clean build
always:
	mkdir -p out
clean:
	rm -r out
build: always out/main
out/main:
	gcc $(SOURCES) -o $@
