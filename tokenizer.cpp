/**
 *
 * tokenizer.c - Breaks down input into processable symbols
 *
 * Copyright (c) 2022 Anamitra Ghorui
 *
 * LICENSE_HERE
 *
 */

#include <cstdint>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

/**md
 * -----------------------------------------------------------------------------
 *
 * Warning: The text below may change radically as the project progresses.
 *
 * -----------------------------------------------------------------------------
 *
 * Hi, welcome to BlindForth.
 *
 * This language is written with the intent of creating an extensively
 * documented language interpreter (and probably eventually a compiler), while
 * trying out a personal experiment by attempting to recreate a language from
 * a single vague idea of how it works.
 *
 * The Forth Language
 * ==================
 *
 * The language I am trying to imitate or recreate here is called "Forth".
 * What I know about Forth is that it's a stack based language, and commands
 * in it look somewhat like this:
 *
 * Adding 1 and 2 and printing it:
 *
 *     1 2 + print_stack_top
 *
 * Adding 3 and 4 then multiplying by 2:
 *
 *     2 4 3 + * print_stack_top
 *
 * (I am not sure whether `print_stack_top` exists or not)
 *
 * As you can see, statements here are written here in a postfix fashion,
 * opposite of what you might see in scheme or lisp (I have some experience with
 * them), which use a prefix notation of commands:
 *
 * Adding 1 and 2 and printing it:
 *
 *     (display (+ 1 2))
 *
 * Adding 3 and 4 then multiplying by 2:
 *
 *     (display (* (+ 4 3) 2))
 *
 * You might be able to form a mental image of what is happening in the Forth
 * example. Numerical values (1, 2, 3, ...) are being pushed into a stack as the
 * interpreter encounters them, then when it encounters an operation or function
 * like '+', it pops values from the top of the stack, processes the operation
 * or function and then puts the result on the top of the stack.
 *
 * The challenge now, is attempting to decide how to implement any of the higher
 * facilities of the language, for example:
 *
 * 1. How do you implement loops in something like this?
 * 2. How do you write if statements?
 * 3. What sort of type system does it use?
 * 4. What does it do to declare and use structured data?
 * 5. How does it declare types? Can it declare types?
 *
 * and other questions may arise in one's mind after looking at the above
 * statement in Forth.
 *
 * Having learnt scheme (a syntactically identical dialect of lisp), it might
 * give some pointers as to where to go. For example, Lisp-like languages do not
 * have any looping strucures unlike what procedural languages like C, C++ or
 * java do, but instead use recursion to create repeating behaviour.
 * Unfortunately, if all we can do in the language is push to the stack, pop
 * from it, and process them, I need to figure out some other way to do it.
 *
 * There will be more questions that will arise and I intend to answer these
 * questions as I write this program.
 *
 * The Interpreter/Compiler
 * ========================
 *
 * I have not written a full fledged language interpreter and compiler before,
 * and I intend to teach myself how to do that by doing this project.
 *
 * Something that has been a big help for me in actually getting started with
 * this project is getting the knowledge of Formal Grammars, Languages and
 * Automata. For most people who want to get started with writing a language,
 * I think this is the biggest logical obstacle. It would take a lot of time
 * to get how language grammar work and how to go about parsing them if you are
 * just trying to do it just by experimentation, as it did for me several years
 * ago, and I gave up halfway through. If you haven't already, I would suggest
 * you read up on formal languages, grammar and automata theory. If you are in
 * university, I'd also suggest taking up a course on Language Theory or
 * compiler design if you have the motivation. But I'll attempt to explain all
 * of this stuff as I progress through this project.
 *
 * Forth is, however, as you might be able to conclude, is a very simple
 * language in terms of syntax. This works in our favour by keeping mental
 * overhead low and our implementation simple to look at.
 *
 * The Process
 * ===========
 *
 * I intend to document all of the components of the project using comments
 * within the code, such that the files in the project can be read as a guide
 * for any future readers and myself. The parts of the compiler/interpreter
 * pipeline will be connected and numbered as per their actual order of
 * execution and will be the reading order of the files in this project. These
 * will be made available in the README file of this project.
 *
 * You are currently reading the first document in the series. There's also
 * a need to actually compile, run and test parts of the code, so there are
 * files that are required (and will be mentioned in the respective documents
 * as needed) that may need to be referred to multiple times and read out of
 * order.
 *
 * Why C++?
 * ========
 *
 * Originally I was going to write this comepletely in C, but later decided
 * against it because of some idioms that are simply cumbersome to write and
 * distract from the actual goal of the project (such as array allocation,
 * resizing, freeing, etc.) Having vector types will greatly simplify the code
 * that needs to be written and allow us to focus on the main objective.
 *
 * As for languages like Python, Java or Javascript, a lot of them don't have
 * strong typing, enum values or heavily depend on external tools, libraries
 * or elaborate standard libraries to accomplish things. To allow a rigid
 * definition of how we're doing things here, I think it is important that the
 * language allows us to do so. This again allows us to focus on the goal on
 * hand at the expense of safety, maybe.
 *
 *
 * Initial Targets for the Implementation
 * ======================================
 *
 * For now, I will attempt to replicate the example Forth statement that was
 * shown before in this project. Later on, as the project progresses,
 * incremental changes will be made to what I want to implement in the
 * language.
 *
 * For this, let me provide a rough definition of what the language will be:
 *
 * <TODO>
 *
 * This concludes the introduction to the project. The comment starting below
 * contains the content for this document.
 *
 * To
 *
 *
 * Part 1: The Tokenizer or Lexical Analyzer
 * ==========================================
 *
 * The first step to processing any source code or command for a given
 * programming language is to observe the text, and then attempt to extract
 * something that is meaningful. This can then be used for further processing,
 * such as actually verifying whether what the user has written is correct
 * or not. Take, for instance, the below expression:
 *
 *     "2+2"
 *
 * To a computer program, this is simply a sequence of numerical values. It
 * simply cannot tell a number from an operator, or a variable name by itself.
 *
 * In order to actually evaluate this expression, one will have to tell the
 * program to interpret a certain sequence of bytes as a number, another certain
 * sequence of bytes as a number, and so on.
 *
 * In addition to that, the programmer must also take into consideration how a
 * user may write the same expression, and make the program interpret all of
 * these expressions as the same.
 *
 *     "2 + 2"
 *
 *     "2   + 2"
 *
 *     "2 +           2"
 *
 * The programmer must also take into consideration that there may be errors
 * in the input as well, such as:
 *
 *     "2a3 - 4" <- An "a" has been randomly placed between 2 and 3, which makes
 *                  for a meaningless symbol.
 *
 *     "123.343 - 1.2.3.4.5" <- Multiple decimal points have been placed within
 *                              the same number without any space. Hence a
 *                              meaningless sequence.
 *
 * If you try to create a brute-force solution to the problem, you will
 * eventually end up with a function with a large number of if-statements that
 * will separate and categorise each and every sequence of meaningful bytes
 * within the program, and detect any errors in the expression.
 *
 * If you attempted to go a few steps forward then this, you might have used
 * regular expressions, and checked against each of them for each symbol, or
 * might have drawn a little diagram with arrows to help better visualise the
 * code.
 *
 * All of these may be good, and even elegant solutions, but a lot of them are
 * not aware the underlying problem that these programs solve, which is
 * detecting which segment of the string conforms to a certain regular grammar.
 *
 * Before we discuss what a "Regular Grammar" is, let's define some things
 * first.
 *
 * ## String
 *
 * A String is a sequential set of symbols. (I think you know this.)
 *
 * ## Language
 *
 * A "Language", in the language theory sense, is the set of all words
 * ("Strings") which conform to a certain set of rules. For example, the set of
 * all whole numbers is a language:
 *
 *     { 1, 2, 3, 4, ..., 22, ..., 2222, ... }
 *
 * You can see the specific pattern between all of these numbers, which is:
 *
 * 1. All numbers are formed with these 10 symbols: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9.
 *    This is also called the ''Alphabet'' of the language.
 * 2. A number is at least one symbol in length
 * 3. A number may have an indefinite number of symbols.
 *
 * If you know regular expressions, you might express these 3 rules with the
 * following statement:
 *
 *     [0123456789]+
 *
 * Or more succinctly as:
 *
 *     [0-9]+
 *
 * ## Grammar
 *
 * The set of rules we described above is called a "Grammar." A Grammar
 * describes how to determine whether a given String is a part of the
 * corresponding language or not.
 *
 * A Regular Grammar is a Grammar that is "simple" in nature. Grammars
 * such as the one above, or one that defines an alphanumeric variable name, or
 * one that defines a delimited string in a programming language are all
 * Regular Grammars. In fact, the term "Regular Expression" actually comes from
 * this, and Regular Expressions are used to define Regular Grammars.
 *
 * I am refraining from providing a concrete definition of a Regular Grammar
 * right now, but we will eventually get to it as we progress through the
 * project and learn about syntax analysis. However, as a rule of thumb,
 * anything that you cannot write using a Regular Expression is not a regular
 * grammar. Examples may include the entire grammar of the C programming
 * language, mathematical expressions like `2 + (3 * 5 - 4)` and other things
 * of higher complexity.
 *
 * You might also notice that we are defining a language for a component of a
 * much larger language. <REFRAME THIS SENTENCE>
 *
 * ## Finite Automata
 *
 * All regular languages can be expressed in terms of what is called a Finite
 * Automata. Any program that you write to match a regular grammar, such as
 * the one we discussed earlier about matching a number from a given string,
 * ultimately boils down to an implementation of a Finite Automaton, or a finite
 * state machine. It is a logical machine that has a finite number of states
 * that it can be in, and depending on the successive input given to the machine
 * (a symbol from an input string), it changes it state.
 *
 * A finite state machine has a certain number of states marked as "Accepting"
 * states. When all the sequential symbols of the input string ar exhausted,
 * and the final state the machine lands on is an "Accepting" state, we say that
 * the string has been accepted by the machine.
 *
 * To explain this better, let's go back to the number matching example, and
 * look at a bit of pseudocode for it:
 *
 *     state = NONE
 *
 *     while (input_not_exhausted) {
 *         if (input_character_is_a_digit) {
 *             state = NUMBER
 *         }
 *     }
 *
 *     if (state == NUMBER) {
 *         print("There was a number in the input")
 *     } else {
 *         print("There were no numbers in the input")
 *     }
 *
 * This is a finite state machine, albeit very simple and rudimentary in nature.
 *
 * This program will match any number that occurs within the input. We may also
 * make a state transition diagram of the program's behaviour
 *
 *                   <digit>
 *            (NONE) -------> ((NUMBER)) ---+
 *                                ^         |
 *                                +---------+
 *                                   <any>
 *
 * "NUMBER" is the accepting state here, and as you can see,
 *
 * (...)
 *
 * Go to wikipedia for more information.
 *
 *
 * ## Tokenizing
 *
 * Like I said in the beginning, we need to extract meaning from our text. To
 * first attempt to actually interpret and evaluate the syntax that the user has
 * written, we must first know *what* the user has written. To answer the "what",
 * we write a tokenizer.
 *
 * A tokenizer reads user input, processes it, and spits out a list of discrete
 * "chunks" with their type (such as integer, float, etc.) , and their actual,
 * logical values, such as an actual numerical value in memory instead of a
 * string for an integer. These "chunks" are called tokens. Tokens are the
 * unitary, atomic elements of a given language.
 *
 * These tokens allow us to now focus on higher level stuff, such as analyzing
 * what the user has written, checking for the correctness of the program and
 * other things. It allows us to dissassociate the actual text file which
 * constitutes the sourcecode, and give us a logical, processable representation
 * of what the user has written.
 *
 * ----
 * tokenizer - What has the user written? ("sadhajshdjasbfkjlhbfhaskbfkjlasdbf")
 * syntax analyzer - Does the user input even make sense? ("1 + + + + 2")
 * semantic analyzer - Is the user input logically sound? (1 + false * (2 * 3))
 * intermediate code generation - Generate platform independent intermediate code for your program
 * code optimization - Attempt to make your program more efficient
 * machine code generation - generate a ".exe"
 * ----
 *
 */

/**md
 * For BlindForth, we will write a program that is very blatantly a finite state
 * automaton. Let's do so by first defining what sort of tokens BlindForth
 * will have:
 *
 *           Integers - All non-decimal Numbers (..., -1, 0, 1, 2, 3, 4, ...)
 *              Reals - Real Numbers, with decimals. (1, 1.3, 3.142, -2.718, etc.)
 *             String - Quote delimited strings ('apple', "ball", ....)
 *        Identifiers - Variable and function names and everything else (apple_1, *, /)
 *     Debug commands - Identifiers, but prefixed with a colon. I intend these to
 *                      be ignored by a compiler or allow macro statements.
 *                      (:break, :stack_trace, ...)
 *
 * Note that in BlindForth, we do not define the concept of operators and
 * their syntax, similar to lisp.
 *
 * Let's define a few terms first:
 * * ''Symbol'': By "Symbol," I mean a UTF-8 Character.
 * * ''Any Symbol'': By "Any Symbol", I mean any valid UTF-8 Character.
 * * ''Any Visible Symbol'': By "Any Visible Symbol", I mean any non control,
 *   non-whitespace UTF-8 Character. More rigidly, if the symbol's unicode value
 *   is `c`, then: `(c >= U+0021 && c <= U+007E) || (c >= 00A1)`
 *
 * Now, let's give a proper definition of the grammar of each one of the
 * tokens:
 *
 * ## Integers:
 *  * All integers consist of one or more symbols
 *  * An integer is composed only of the following symbols:
 *      1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -, +
 *  * Only the first symbol of an integer can be "-" or "+"
 *  * If the first symbol is a "-" or a "+", then the integer necessarily
 *    has a second symbol.
 *
 * The Finite state machine of this grammar looks like the following.
 *
 * <DIAGRAM>
 *
 * ## Reals:
 *  * All reals consist of one or more symbols
 *  * An real is composed only of the following symbols:
 *      1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -, +, .
 *  * Only the first symbol of a real can be "-" or "+"
 *  * If the first symbol is a "-" or a "+", then the integer necessarily
 *    has a second symbol.
 *  * There must be one and only one occurence of the "." symbol in a real.
 *  * If the first symbol is a ".", then the real necessarily has a second
 *    symbol.
 *  * If the first symbol is a "+" or a "-", and the second symbol is a "."
 *    then the real necessarily has a third symbol.
 *
 * The Finite state machine of this grammar looks like the following.
 *
 * <DIAGRAM>
 *
 * ## Strings:
 * * A string always starts and ends with the symbol `'` (single quote, or
 *   always starts and ends with the symbol ` " ` (double quote).
 * * A String may contain any symbol. However, no symbol except for the
 *   starting symbol may be ` ' ` (single quote), if the starting symbol is
 *   ` ' ` (single quote), or ` " ` (double quote), if the starting symbol is
 *   ` " ` (double quote). <Note: This will change later to replicate C-style
 *   string notation with escapes.>
 *
 * <DIAGRAM>
 *
 * ## Identifiers:
 * * An identifier is composed of any visible symbol except for its first
 *   symbol.
 * * The first symbol of an identifier is any visible symbol except for `:`
 *   (colon), or a digit (`0`...`9`).
 *
 * <DIAGRAM>
 *
 * ## Debug Commands:
 * * The first symbol of a debug command is always ":" (colon)
 * * An identifier is composed of any visible symbol except for its first
 *   symbol.
 *
 * <DIAGRAM>
 *
 *
 * Matching Tokens
 * ===============
 *
 * A question that might come to your mind is how do we signify an "end" to any
 * of these finite automata to store them for further processing, since they
 * look like they will seemingly run endlessly given that the correct input is
 * always supplied.
 *
 * Having a look at other languages gives us hints, for example, in C, this
 * is a valid expression, and the compiler is able to differentiate between
 * all the numbers and the operators:
 *
 *     2*2 + 4-apple / (5*6)
 *
 * It does so by looking out for a character that does not match the current
 * token's grammar.
 *
 * For example, we defined a number as a string of symbols that are any of
 * 0, 1, ..., 9. When we encounter, say, an operator, like "*" after "2" in the
 * above case, the compiler detects a mismatch, and ends the "number" token
 * right there.
 *
 * In BlindForth, all tokens are separated using whitespace (which is actually
 * unlike C, where whitespace is purely cosmetic excluding preprecessors.) There
 * is no concept of an operator or operation precedence in BlindForth. All
 * commands and expressions supplied to the compiler are seprarated by
 * whitespace. This is somewhat similar to lisp, wherein statements are made in
 * brackets, and parameters or arguments are separated by whitespace.
 *
 * Output
 * ======
 *
 * Our final output in this case is a list of tokens that contain the value read
 * from the raw input, and marked by what they are.
 *
 * In case of errors, the output should be the error.
 *
 * In the end, the output should contain enough information that our compiler
 * won't have to make another trip to the supplied raw user input.
 *
 * Implementation
 * ==============
 *
 * We will now combine each of the above definitions of finite automata
 * into a singular program: our tokenizer. This program will run on repeat till
 * the user input ends. Here's a rough algorithm of what we're trying to
 * implement:
 *
 *     While input is not exhausted:
 *         Read a symbol `S`
 *
 *         Switch (state)
 *             case Integer:
 *                 if `S` is not a digit or a decimal, throw error
 *                 else if `S` is a dot, switch state to Real
 *                 else if `S` is a digit, store and continue.
 *             (...)
 *
 *         (...)
 *
 *         if current token is completed:
 *             Store and append to token list
 *
 * The Transition Table
 * ====================
 *
 * We will represent the combined finite automata as a transition table.
 *
 * This table will list all the ways state will be changed in the program and
 * what 'actions' will the program perform on encountering a symbol.
 *
 * There are 4 actions that the tokenizer will perform:
 *
 *  * Do Nothing (''NOP'')
 *  * Iteratively store the current token as the symbols are being read. (''Build'')
 *  * Store the completed symbol in our list. (''Store'')
 *  * Throw an error. (''Error'')
 *
 * Let's also define the symbols and classes of symbols that the tokenizer will
 * accept:
 *
 *  * Whitespace: Spaces (` `), Tabs (`\t`), New Lines (`\n`), Carriage Returns (`\r`)
 *  * Anything: Any possible unicode character.
 *  * Digit: Symbols `0` through `9`
 *  * Plus: `+`
 *  * Minus: `-`
 *  * Dot: `.`
 *  * Colon: `:`
 *  * Single Quote: `'`
 *  * Double Quote: `"`
 *  * Idchar: Any Visible Symbol (`(c >= U+0021 && c <= U+007E) || (c >= 00A1)`) except W
 *  * Idalpha: Any Visible Symbol except Whitespace, Digit, `"`, `'`, `:`, and `.`
 *  * Dbgchar: Any of: `A`-`Z`, `a`-`z`, `_`, and `0`-`9`
 *  * EOF: Symbolic representation of End-Of-File
 *
 * Finally, let's give names to the states the tokenizer will transfer between:
 *
 *  * None - A Default State
 *  * Id   - Identifier
 *  * Int  - Integer
 *  * Dot  - A single encountered dot
 *  * Real - Real Numbers
 *  * Str  - String
 *  * Dbg  - Debug
 *  * End  - End of Tokenization
 *  * Err  - Error State
 *
 * Eventually, with some thinking, you will end up with a table that looks like
 * this. Any possible transition that is not in this table is a transition to
 * the error state.:
 *
 * |State     |Input                              |Action          |Next      |
 * |----------|-----------------------------------|----------------|----------|
 * |None      |Whitespace                         |NOP             |None      |
 * |None      |Idalpha                            |NOP             |Id        |
 * |None      |Digit                              |NOP             |Int       |
 * |None      |Single/Double Quote                |NOP             |Str       |
 * |None      |Colon                              |NOP             |Dbg       |
 * |None      |Dot                                |NOP             |Real      |
 * |None      |EOF                                |Store           |End       |
 * |Id        |Idchar                             |Build           |Id        |
 * |Id        |Whitespace                         |Store           |None      |
 * |Id        |EOF                                |Store           |End       |
 * |Int       |Digit                              |Build           |Int       |
 * |Int       |Dot                                |Build           |Real      |
 * |Int       |Whitespace                         |Store           |None      |
 * |Int       |EOF                                |Store           |End       |
 * |Real      |Whitespace                         |Store           |None      |
 * |Real      |EOF                                |Store           |End       |
 * |Str       |Anything except `"` or `'` (Quotes)|Build           |Str       |
 * |Str       |Single/Double Quote                |Store           |End       |
 * |Dbg       |Dbgchar                            |Build           |Dbg       |
 * |Dbg       |Whitespace                         |Store           |None      |
 * |Dbg       |EOF                                |Store           |End       |
 *
 * > Note:
 * > I haven't introduced string escape sequences here. I'll add them in this
 * > section later when I'm done with the actual program.
 *
 * We will encode this transition table as a matrix, where rows and columns
 * describe a state and an input character respectively, and a value at the
 * nth row and the mth column describe the state to transition to, at the
 * current state and input.
 *
 * Let's finally get to the code.
 *
 * ## Note
 *
 * All of these Markdown documents are written such that, if you were to join
 * all of the code blocks that start and end with ` ``` `, you will end up
 * with a valid C++ program. Therefore all of the code blocks here are written
 * in their logical, sequential order.
 *
 */

/**md
 *
 * ### `enum TokenType`
 *
 * `TokenType` lists all of the types of tokens that the tokenizer output will
 * contain. This includes all the types I discussed earlier, and a default
 * "none" value that might come in handy.
 *
 */

typedef enum TokenType {
	TOKEN_TYPE_NONE          = 0,
	TOKEN_TYPE_INT           = 1,
	TOKEN_TYPE_REAL          = 2,
	TOKEN_TYPE_STRING        = 3,
	TOKEN_TYPE_ID            = 4,
	TOKEN_TYPE_DEBUG_COMMAND = 5
} TokenType;

/**md
 *
 * ### `enum TokenState`
 *
 * `TokenState` lists all the states that the tokenizer will be in. One slight
 * deviation I have made here is that there are separate states for maintaining
 * single-quoted strings and double-quoted strings. This simplifies our code
 * slightly by not having to maintain a variable to remember whether this is
 * a single or double quoted string.
 *
 */

typedef enum TokenState {
	TOKEN_STATE_ERROR         = 0,
	TOKEN_STATE_NONE          = 1,
	TOKEN_STATE_SIGN          = 2,
	TOKEN_STATE_INT           = 3,
	TOKEN_STATE_DOT           = 4,
	TOKEN_STATE_REAL          = 5,
	TOKEN_STATE_SQUOTE_STRING = 6,
	TOKEN_STATE_DQUOTE_STRING = 7,
	TOKEN_STATE_ID            = 8,
	TOKEN_STATE_DEBUG         = 9,
	TOKEN_STATE_END           = 10,
	TOKEN_STATE_SIZE // This simply marks the number of enum values
} TokenState;

/**md
 *
 * ### `struct TokenInput`
 *
 * `TokenInput` contains the possible types of input that the tokenizer will
 * receive.
 *
 */


typedef enum TokenInput {
	TOKEN_INPUT_EOF         = 0,
	TOKEN_INPUT_WHITESPACE  = 1,
	TOKEN_INPUT_ALPHABET    = 2,
	TOKEN_INPUT_NUMERIC     = 3,
	TOKEN_INPUT_DOT         = 4,
	TOKEN_INPUT_DOUBLEQUOTE = 5,
	TOKEN_INPUT_SINGLEQUOTE = 6,
	TOKEN_INPUT_SIGN        = 7,
	TOKEN_INPUT_COLON       = 8,
	TOKEN_INPUT_BACKSLASH   = 9, // unused for now.
	TOKEN_INPUT_IDCHAR      = 10,
	TOKEN_INPUT_OTHER       = 11,
	TOKEN_INPUT_SIZE // This simply marks the number of enum values
} TokenInput;


/**md
 *
 * ## `union TokenData`
 *
 * This is what we will store our token data. It's a union, which allows us to
 * reuse the same storage for different things.
 *
 * Identifiers and strings are stored here using allocated pointers. The reason
 * this works is because both identifiers and strings are a string of
 * characters.
 */

typedef union TokenData {
	int64_t i;
	double r;
	void *s;
} TokenData;

/**md
 *
 * ## `struct Token`
 *
 * This is what we will store our token data. It's a union, which allows us to
 * reuse the same storage for different things.
 *
 * Identifiers and strings are stored here using allocated pointers. The reason
 * this works is because both identifiers and strings are a string of
 * characters.
 */


typedef struct Token {
	TokenType type;
	TokenData data;
} Token;

/**md
 *
 * ### `struct TokenError`
 *
 * `TokenError` is returned when the tokenizer encounters any erroneous input.
 * It returns the current position in the input (`current_offset`), the line
 * number (`line_pos`) and the column position (`col_pos`), and the current
 * type of the token that the
 *
 */

typedef struct TokenError {
	unsigned int curr_offset;
	unsigned int line_pos;
	unsigned int col_pos;
	TokenState curr_guess;
	TokenInput curr_input;
	char curr_input_val;
} Tokenerror;


/**md
 *
 * ### `struct TokenResult`
 *
 * `TokenResult` is what will be returned by the tokenizer.
 *
 * **TODO** contents of struct might change.
 *
 * I have used
 *
 * String/Symbol storage is handled by allocating a single array, and then copying
 * the strings to the array along with the null terminator. This prevents us
 * from doing a large number of calls to malloc, not make data fragment
 * in memory, forget about freeing data, and so on.
 *
 * To explicitly describe the intent, I've aliased the type of the buffer with a
 * name.
 */

typedef std::vector<char> CharBuffer;

typedef struct TokenResult {
	unsigned int characters_processed;
	unsigned int lines_processed;
	TokenError error;
	CharBuffer buffer;
	std::vector<Token> tokens;

	TokenResult() {
		characters_processed = 0;
		lines_processed = 0;
	}
} TokenResult;

/**md
 * The following functions perform the symbol/string buffer manipulation. This
 * simplifies later code for us and performs error checking for us as well.
 *
 *
 * ## Function `token_buffer_new`
 *
 * This function returns a valid pointer (but not allocated) block of
 * memory which will be used to store the string/symbol.
 *
 * Symbols/strings are allocated in a sequential manner and one after the
 * another.
 */

static inline void *token_buffer_new(std::vector<char>& buffer)
{
	if (buffer.empty()) {
		// Reserve a buffer size to reduce allocation frequency.
		// This is an arbitrary value.
		buffer.reserve(512);
	}

	// Insert a null character so we get a pointer to buffer[0]
	buffer.push_back('\0');
	return &buffer.back();
}

/**md
 *
 * ## Function `token_buffer_insert`
 *
 * This function inserts a char value to the current buffer pointed to. If
 * insertion fails it returns a value less than 0.
 */

static inline int token_buffer_insert(std::vector<char>& buffer, char c)
{
	buffer.push_back(c);
	return 0;
}

/**md
 *
 * ## Function `token_buffer_end`
 *
 * This ends the current buffer. It inserts a trailing null ('\0') character
 * into the buffer and returns a pointer to the end.
 */

static inline void *token_buffer_end(std::vector<char>& buffer)
{
	buffer.push_back('\0');
	return &buffer.back();
}

/**md
 *
 * ### The Transition Matrix
 *
 * Now that everything that's needed to make the matrix is defined, we can
 * finally make the transition matrix. I've defined the matrix in this static
 * array called `states`. The first index (row) of `states` holds the current
 * state, and the second index holds the current input. Therefore the state that
 * the tokenizer needs to transfer to when the current state is `i` and the
 * current input is `j`, is `states[i][j]`.
 *
 * To make the code more manageable and readable, each row of the array is
 * split into separate blocks, and the states/input have been marked with
 * comments for convenience.
 *
 * The reason I am able to do this in such a symbolic manner is because enum
 * values correspond to actual integer values in C++, which can be set
 * explicitly and known at compile time.
 *
 */

uint8_t states[TOKEN_STATE_SIZE][TOKEN_INPUT_SIZE] = {
	/* TOKEN_STATE_ERROR */
	// TODO Remove this from the list by setting it to -1.
	{
		TOKEN_STATE_ERROR, // EOF
		TOKEN_STATE_ERROR, // WHITESPACE
		TOKEN_STATE_ERROR, // ALPHABET
		TOKEN_STATE_ERROR, // NUMERIC
		TOKEN_STATE_ERROR, // DOT
		TOKEN_STATE_ERROR, // DOUBLEQUOTE
		TOKEN_STATE_ERROR, // SINGLEQUOTE
		TOKEN_STATE_ERROR, // SIGN
		TOKEN_STATE_ERROR, // COLON
		TOKEN_STATE_ERROR, // BACKSLASH
		TOKEN_STATE_ERROR, // IDCHAR
		TOKEN_STATE_ERROR  // OTHER
	},

	/* TOKEN_STATE_NONE */
	{
		TOKEN_STATE_END,           // EOF
		TOKEN_STATE_NONE,          // WHITESPACE
		TOKEN_STATE_ID,            // ALPHABET
		TOKEN_STATE_INT,           // NUMERIC
		TOKEN_STATE_DOT,           // DOT
		TOKEN_STATE_DQUOTE_STRING, // DOUBLEQUOTE
		TOKEN_STATE_SQUOTE_STRING, // SINGLEQUOTE
		TOKEN_STATE_SIGN,          // SIGN
		TOKEN_STATE_DEBUG,         // COLON
		TOKEN_STATE_ERROR,         // BACKSLASH
		TOKEN_STATE_ID,            // IDCHAR
		TOKEN_STATE_ERROR          // OTHER
	},

	/* TOKEN_STATE_SIGN */
	{
		TOKEN_STATE_END,    // EOF
		TOKEN_STATE_NONE,   // WHITESPACE
		TOKEN_STATE_ERROR,  // ALPHABET
		TOKEN_STATE_INT,    // NUMERIC
		TOKEN_STATE_DOT,    // DOT
		TOKEN_STATE_ERROR,  // DOUBLEQUOTE
		TOKEN_STATE_ERROR,  // SINGLEQUOTE
		TOKEN_STATE_ERROR,  // SIGN
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
		TOKEN_STATE_ERROR,  // IDCHAR
		TOKEN_STATE_ERROR   // OTHER
	},

	/* TOKEN_STATE_INT */
	{
		TOKEN_STATE_END,    // EOF
		TOKEN_STATE_NONE,   // WHITESPACE
		TOKEN_STATE_ERROR,  // ALPHABET
		TOKEN_STATE_INT,    // NUMERIC
		TOKEN_STATE_DOT,    // DOT
		TOKEN_STATE_ERROR,  // DOUBLEQUOTE
		TOKEN_STATE_ERROR,  // SINGLEQUOTE
		TOKEN_STATE_ERROR,  // SIGN
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
		TOKEN_STATE_ERROR,  // IDCHAR
		TOKEN_STATE_ERROR   // OTHER
	},

	/* TOKEN_STATE_DOT */
	{
		TOKEN_STATE_ERROR,  // EOF
		TOKEN_STATE_ERROR,  // WHITESPACE
		TOKEN_STATE_ERROR,  // ALPHABET
		TOKEN_STATE_REAL,   // NUMERIC
		TOKEN_STATE_ERROR,  // DOT
		TOKEN_STATE_ERROR,  // DOUBLEQUOTE
		TOKEN_STATE_ERROR,  // SINGLEQUOTE
		TOKEN_STATE_ERROR,  // SIGN
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
		TOKEN_STATE_ERROR,  // IDCHAR
		TOKEN_STATE_ERROR   // OTHER
	},

	/* TOKEN_STATE_REAL */
	{
		TOKEN_STATE_END,    // EOF
		TOKEN_STATE_NONE,   // WHITESPACE
		TOKEN_STATE_ERROR,  // ALPHABET
		TOKEN_STATE_REAL,   // NUMERIC
		TOKEN_STATE_ERROR,  // DOT
		TOKEN_STATE_ERROR,  // DOUBLEQUOTE
		TOKEN_STATE_ERROR,  // SINGLEQUOTE
		TOKEN_STATE_ERROR,  // SIGN
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
		TOKEN_STATE_ERROR,  // IDCHAR
		TOKEN_STATE_ERROR   // OTHER
	},

	/* TOKEN_STATE_DQUOTE_STRING */
	{
		TOKEN_STATE_ERROR,         // EOF
		TOKEN_STATE_DQUOTE_STRING, // WHITESPACE
		TOKEN_STATE_DQUOTE_STRING, // ALPHABET
		TOKEN_STATE_DQUOTE_STRING, // NUMERIC
		TOKEN_STATE_DQUOTE_STRING, // DOT
		TOKEN_STATE_NONE,          // DOUBLEQUOTE
		TOKEN_STATE_DQUOTE_STRING, // SINGLEQUOTE
		TOKEN_STATE_DQUOTE_STRING, // SIGN
		TOKEN_STATE_DQUOTE_STRING, // COLON
		TOKEN_STATE_DQUOTE_STRING, // BACKSLASH
		TOKEN_STATE_DQUOTE_STRING, // IDCHAR
		TOKEN_STATE_DQUOTE_STRING, // OTHER
	},

	/* TOKEN_STATE_SQUOTE_STRING */
	{
		TOKEN_STATE_ERROR,         // EOF
		TOKEN_STATE_SQUOTE_STRING, // WHITESPACE
		TOKEN_STATE_SQUOTE_STRING, // ALPHABET
		TOKEN_STATE_SQUOTE_STRING, // NUMERIC
		TOKEN_STATE_SQUOTE_STRING, // DOT
		TOKEN_STATE_SQUOTE_STRING, // DOUBLEQUOTE
		TOKEN_STATE_NONE,          // SINGLEQUOTE
		TOKEN_STATE_SQUOTE_STRING, // SIGN
		TOKEN_STATE_SQUOTE_STRING, // COLON
		TOKEN_STATE_SQUOTE_STRING, // BACKSLASH
		TOKEN_STATE_SQUOTE_STRING, // IDCHAR
		TOKEN_STATE_SQUOTE_STRING, // OTHER
	},

	/* TOKEN_STATE_ID */
	{
		TOKEN_STATE_ERROR,  // EOF
		TOKEN_STATE_NONE,   // WHITESPACE
		TOKEN_STATE_ID,     // ALPHABET
		TOKEN_STATE_ID,     // NUMERIC
		TOKEN_STATE_ERROR,  // DOT
		TOKEN_STATE_ERROR,  // DOUBLEQUOTE
		TOKEN_STATE_ERROR,  // SINGLEQUOTE
		TOKEN_STATE_ERROR,  // SIGN
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
		TOKEN_STATE_ID,     // IDCHAR
		TOKEN_STATE_ERROR,  // OTHER
	},

	/* TOKEN_STATE_DEBUG */
	{
		TOKEN_STATE_ERROR,  // EOF
		TOKEN_STATE_NONE,   // WHITESPACE
		TOKEN_STATE_DEBUG,  // ALPHABET
		TOKEN_STATE_DEBUG,  // NUMERIC
		TOKEN_STATE_ERROR,  // DOT
		TOKEN_STATE_ERROR,  // DOUBLEQUOTE
		TOKEN_STATE_ERROR,  // SINGLEQUOTE
		TOKEN_STATE_ERROR,  // SIGN
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
		TOKEN_STATE_ERROR,  // IDCHAR
		TOKEN_STATE_ERROR   // OTHER
	},
};

/**md
 *
 * ### Function `get_input` (unexported)
 *
 * `get_input` takes actual user input characters and returns the correct
 * `TokenInput` value for it. That value is then used by the tokenizer to take
 * decisions.
 *
 */

TokenInput get_input(int input)
{
	TokenInput ret;

	// We first try matching any of the single-character input types, then
	// move on to the range-based ones.

	if (input == EOF) {
		return TOKEN_INPUT_EOF;
	}

	switch (input) {
	case ' ':
	case '\r':
	case '\n':
	case '\t':
		return TOKEN_INPUT_WHITESPACE;
		break;

	case '.':
		return TOKEN_INPUT_DOT;
		break;

	case '\0':
		return TOKEN_INPUT_EOF;
		break;

	case ':':
		return TOKEN_INPUT_COLON;
		break;

	case '\\':
		return TOKEN_INPUT_BACKSLASH;
		break;

	case '_':
		return TOKEN_INPUT_ALPHABET;
		break;

	case '+':
	case '-':
		return TOKEN_INPUT_SIGN;
		break;

	case '\"':
		return TOKEN_INPUT_DOUBLEQUOTE;
		break;

	case '\'':
		return TOKEN_INPUT_SINGLEQUOTE;
		break;
	}

	// Match number

	if (input >= '0' && input <= '9') {
		return TOKEN_INPUT_NUMERIC;
	}

	// Match Alphabet

	if ((input >= 'a' && input <= 'z') ||
		(input >= 'A' && input <= 'Z')) {
		return TOKEN_INPUT_ALPHABET;
	}

	// Match a visible character

	// TODO add UTF-8 Support here. A lookahead of at most 2 bytes is needed
	// here.

	if ((input >= 0x21 && input <= 0x7E) || (input >= 0xA1)) {
		return TOKEN_INPUT_IDCHAR;
	}

	return TOKEN_INPUT_OTHER;
}


/**md
 *
 * ## Tokenizer Build functions
 *
 * These functions help us in building the currently concerned token. These
 * help us in doing tasks like converting a string segment to an integer,
 * to a float, storing an identifier and so on.
 *
 * We have defined a part of these set of required functions before, which
 * are the `token_buffer_*` functions.
 *
 */

/**md
 *
 * ### Function `init_token` (unexported)
 *
 * This function set the token type to the supplied parameter and the token
 * data to zero. Note that setting token data to zero is unnecessary for a
 * few of the token types (strings, other token types that need a pointer), so
 * this function may be broken down into a few other specialised functions for
 * other token types for the sake of efficiency.
 *
 */

void init_token(Token &token, TokenType type)
{
	token.type = type;
	memset(&token.data, 0, sizeof(token.data));
}

/**md
 *
 * ### Function `build_int` (unexported)
 *
 * This builds a standard base 10 integer from a string of characters in the way
 * you would expect. `c` is the current character to append to the token. The
 * return value returns a value less than 0 if the current token integer is
 * overflowing.
 *
 */

int build_int(Token &token, int c)
{
	// A custom assert may be used later to allow for prettier printing and
	// omission in release builds.
	assert(c >= '0');
	if (token.data.i >= (INT64_MAX - 9)) {
		return -1;
	}
	token.data.i *= 10;
	token.data.i += c - '0';
	return 0;
}

/**md
 *
 * ### Function `build_real` (unexported)
 *
 * This *attempts* to build a standard IEEE 64 bit floating point. This function
 * is only used for adding the base 10 mantissa to the number. The integral part
 * is built by `build_int` until the tokenizer finds the dot.
 *
 * Building a function that encompasses all the conformant representations of an
 * IEEE floating point number is pretty hard actually. They can have a
 * ridiculous amount of digits. Looking at the implementation of `atof` is a
 * good place to start.
 *
 * What this does is the following:
 *
 * 1. At the start, the current `int64` is cast to a `double`.
 * 2. We then keep appending digits to the number as we did with `int`, while
 *    keeping track of the number of mantissa places.
 * 3. At the end of build, we divide the number by 10 to the power of the
 *    number of mantissa places
 *
 * There are a few obvious problems with this impelementation, for example
 * this does not allow the integram part of the number to be greater than
 * `INT64_MAX`.
 *
 * ** *Correction Due:* **
 *
 * Instead of doing the entire "build" phase thing, it may be just a better
 * idea to keep a string buffer instead, and once the token is complete,
 * perform the actual conversion.
 *
 */

int build_real(Token &token, int c)
{
	// A custom assert may be used later to allow for prettier printing and
	// omission in release builds.
	assert(c >= '0');
	if (token.data.r >= ( - 9)) {
		return -1;
	}
	token.data.i *= 10;
	token.data.i += c - '0';
	return 0;
}


/**md
 *
 * ### Function `tokenize`
 *
 * This is the actual tokenizer function.
 *
 *
 * Note how we try to check for all of the actions. From the state diagram
 * you will notice that there are various actions that are self loops, and
 * almost all states go to NONE on completion. (etc. etc.)
 *
 * The flag `end` specifies whether or not this is the final segment that needs
 * to be processed.
 *
 *
 * Another interesting thing you might notice is the file line counter I have
 * implemented here. You might be aware of the differences of the [types of
 * line ending markers that are used in different operating systems][line-endings],
 * which text editors (and programs like this one) need to account for.
 *
 * I attempt to implement that here as well, and if you look closely, that is
 * a finite state machine as well. Try drawing the state diagram of the line
 * counter to expose this fact more clearly.
 *
 * [line-endings]: https://en.wikipedia.org/wiki/Newline#Representation
 *
 *
 * TODO add tokenizer state instead?
 *
 */

int tokenize(char *input, int size, bool end, TokenResult &result)
{
	TokenState curr_state = TOKEN_STATE_NONE;
	Token token;

	bool line_ending_check = false;
	unsigned int col_pos = 0;

	CharBuffer &buffer = result.buffer;

	result.characters_processed = 0;
	result.lines_processed = 0;


	// state-specific variables
	bool sign = false;

	for (int i = 0; i < size; i++) {
		TokenInput curr_input = get_input(input[i]);
		TokenState next_state = (TokenState) states[curr_state][curr_input];
		result.characters_processed++;

		/**
		 *
		 * We have to count a line ending if:
		 * - There's a sequence that looks like '\r' (Old MacOS)
		 * - There's a sequence that looks like '\n' (New MacOS, Linux)
		 * - There's a sequence that looks like '\r\n' (Windows)
		 * This will cover virtually all of the platforms it will run on.
		 *
		 * Currently this doesn't run further analysis to determine the line
		 * ending convention the file is using, so it will interpret all
		 * such sequences as valid line endings.
		 */
		if (input[i] == '\n') {
			line_ending_check = false;
			result.lines_processed += 1;
			col_pos = 0;
		} else if (input[i] == '\r') {
			// line_ending_check only gets activated on input being '\r'

			// if previous was '\r' and current is '\r', then we incremeent the
			// line counter and continue checking if there is next in the list.
			if (line_ending_check) {
				result.lines_processed += 1;
				col_pos = 0;

			// else if this is the first one, we turn on the line checker.
			} else {
				line_ending_check = true;
			}
		} else if (line_ending_check) {
			// if its any other character, we stop the line checker if active
			result.lines_processed += 1;
			line_ending_check = false;
			col_pos = 0;
		} else {
			// otherwise we increment the col pos
			col_pos++;
		}

		switch (next_state) {
		case TOKEN_STATE_ERROR:
			// If we encounter an error, the program collects what we know about
			// the problem, where the problem is happening and sends it to the
			// user.
			result.error.col_pos = col_pos;
			result.error.line_pos = result.lines_processed;
			result.error.curr_guess = curr_state;
			result.error.curr_input = curr_input;
			result.error.curr_input_val = input[i];
			return 1;
			break;

		case TOKEN_STATE_NONE:
			// If there's nothing in the input, continue.
			if (curr_state == next_state) {
				continue;
			}

			// Encountering a STATE_NONE means that the current token's content
			// is over. We now need to end the token building.
			switch (curr_state) {
			case TOKEN_STATE_INT:
				break;
			case TOKEN_STATE_REAL:
				break;
			case TOKEN_STATE_SQUOTE_STRING:
				break;
			case TOKEN_STATE_DQUOTE_STRING:
				break;
			case TOKEN_STATE_ID:
				break;
			case TOKEN_STATE_DEBUG:
				break;

			default:
			//error here.
				break;
			}
			break;

		case TOKEN_STATE_SIGN:
			// Ideally this should be two states, not one. Instead of adding
			// states called SIGN_PLUS and SIGN_MINUS, in this state we store
			// the sign value in a separate boolean variable. if there is a
			// minus sign, we turn on the sign value.
			sign = true;
			break;

		case TOKEN_STATE_INT:
			// Check if currstate == nextstate. If so, build. If not, start.

		case TOKEN_STATE_DOT:
			// We don't have to do anything here. Just wait for the state
			// machine to pick the correct thing wrt input

		case TOKEN_STATE_REAL:
			// Check if currstate == nextstate. If so, build. If not, start.
			// In all cases, currstate should be STATE_DOT here. Add a debug
			// check for that.

		case TOKEN_STATE_SQUOTE_STRING:
			// Check if currstate == nextstate. If so, build. If not, start.
			// Unlike the other cases here, the start phase only results in the
			// creation of an empty string with no addition of data.

		case TOKEN_STATE_DQUOTE_STRING:
			// Check if currstate == nextstate. If so, build. If not, start.
			// Unlike the other cases here, the start phase only results in the
			// creation of an empty string with no addition of data.

		case TOKEN_STATE_ID:
			// Check if currstate == nextstate. If so, build. If not, start.

		case TOKEN_STATE_DEBUG:
			// Check if currstate == nextstate. If so, build. If not, start.
			// Unlike the other cases here, the start phase only results in the
			// creation of the token entry but the identifier remains empty at
			// the start
			if (curr_state == next_state) { // build

			}
			// start building
			break;


		case TOKEN_STATE_END:
			// By returning 1, we signify that we are done with the tokenization
			return 1;
			break;

		default:
			printf("Invalid state encountered.\n");
			return -1; // We signify invalid states using -1.
			break;
		}

	}

	// By returning 0, we signify that we need more data to complete the
	// tokenization of the current input.

	// Will control ever jump here?
	return 0;
}