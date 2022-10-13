BlindForth
================================================================================

BlindForth is an attempted implementation of [Forth][forth], *done with little to
no knowledge of how Forth works, or what Forth is.*

BlindForth's sourcecode is intended to act as a literary document which details
the process of making a compiler, somewhat similar to [Literate Programming][litprog].

BlindForth is also being written with the intent of teaching myself how to write
compilers and interpreters, and create a substantial reference for designing
compilers, interpreters and related tools in the future.

This project is in a very rudimentary stage. Eventually a list of Markdown
documents will be added which will be converted from the source code files in
this repository. However if you would like to start reading anyway, I recommend
starting from the [tokenizer.cpp][tokenizer-cpp] file.

Here's an approximate order of the documents that will be written.

* [x] Tokenizer (Ongoing)
* [ ] Parser
* [ ] Semantic Analyzer
* [ ] Standard Library
* [ ] Interpreter


[forth]: https://en.wikipedia.org/wiki/Forth_(programming_language)
[litprog]: https://en.wikipedia.org/wiki/Literate_programming
[tokenizer-cpp]: ./tokenizer.cpp