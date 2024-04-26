LALR Parsing
---

This repo represents a place for me to experiment with lalr parsing techniques that will eventually be used in a real language that I make.

LALR parsers are lookahead capable parsers with a lookahead of 1, sometimes notated as LALR(1). This lookahead is used as a little bit of extra context to decide which parse path we should choose when it encounters ambiguity.

Grammar
---
To keep this project limited in scope and act as a proof of concept, the only grammar it will support is:
```
<idlist> = <id> <idlist> | lambda
<param>  = <id> : <id>
```

Notice how both of these begin with an `<id>`? This is the primary motivation for the lookahead parser.

Note
---
I have reused some code I developed for [Spaz](https://github.com/rfmineguy/spaz)
* tokenizer.h/c

Build
---
```sh
make build
```

Usage
---
```sh
./out/main
```
- Note: Even though this usage exists, it is hard coded to pull from `code.rf`

Third Party Code
---
- [sv.h](https://github.com/tsoding/sv)
- [cvector](https://github.com/eteran/c-vector)
