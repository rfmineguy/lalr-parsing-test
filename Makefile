SOURCES 	:= src/main.c src/tokenizer.c \
						 src/svimpl.c src/lalr.c

.PHONY: always clean build
build: always out/main
always:
	mkdir -p out
clean:
	rm -r out
out/main:
	gcc $(SOURCES) -o $@
