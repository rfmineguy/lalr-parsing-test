LALR Parsing
---

This repo represents a place for me to experiment with lalr parsing techniques that will eventually be used in a real language that I make.

LALR parsers are lookahead capable parsers with a lookahead of 1, sometimes notated as LALR(1). This lookahead is used as a little bit of extra context to decide which parse path we should choose when it encounters ambiguity.

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
