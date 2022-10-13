/**
 *
 * tokenizer.c - Breaks down input into processable symbols
 *
 * Copyright (c) 2022 Anamitra Ghorui
 *
 * LICENSE_HERE
 *
 */

#include <vector>
#include <stdint.h>

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
 */

/**md
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
 * * A string always starts and ends with the symbol "'" (single quote, or
 *   always starts and ends with the symbol " " " (double quote).
 * * A String may contain any symbol. However, no symbol except for the
 *   starting symbol may be "'" (single quote), if the starting symbol is
 *   "'" (single quote), or " " " (double quote), if the starting symbol is
 *   " " " (double quote). <Note: This will change later to replicate C-style
 *   string notation with escapes.>
 *
 * <DIAGRAM>
 *
 * ## Identifiers:
 * * An identifier is composed of any visible symbol except for its first
 *   symbol.
 * * The first symbol of an identifier is any symbol except for ":" (colon)
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
 * A question might come to your mind is how do we signify an "end" to any of
 * these finite automata to store them for further processing, since they look
 * like they will seemingly run endlessly given that the correct input is always
 * supplied.
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
 * 
 *
 */

typedef enum SymbolType {
	SYMBOLTYPE_NONE          = 0,
	SYMBOLTYPE_INT           = 1,
	SYMBOLTYPE_REAL          = 2,
	SYMBOLTYPE_STRING        = 3,
	SYMBOLTYPE_ID            = 4,
	SYMBOLTYPE_DEBUG_COMMAND = 5
} SymbolType;

typedef enum TokenState {
	TOKEN_STATE_END           = 0,
	TOKEN_STATE_NONE          = 1,
	TOKEN_STATE_INT           = 2,
	TOKEN_STATE_REAL          = 3,
	TOKEN_STATE_SQUOTE_STRING = 4,
	TOKEN_STATE_DQUOTE_STRING = 5,
	TOKEN_STATE_ID            = 6,
	TOKEN_STATE_DEBUG         = 7,
	TOKEN_STATE_ERROR         = 8,
	TOKEN_STATE_SIZE
} TokenState;

typedef struct TokenError {
	int current_offset;
	TokenState current_guess;
} Tokenerror;

typedef struct TokenResult {
	TokenError error;
	std::vector<int> start_offsets;
	std::vector<int> end_offsets;
	std::vector<SymbolType> types;
} TokenResult;

typedef enum TokenInput {
	TOKEN_INPUT_EOF         = 0,
	TOKEN_INPUT_WHITESPACE  = 1,
	TOKEN_INPUT_ALPHABET    = 2,
	TOKEN_INPUT_NUMERIC     = 3,
	TOKEN_INPUT_DOT         = 4,
	TOKEN_INPUT_DOUBLEQUOTE = 5,
	TOKEN_INPUT_SINGLEQUOTE = 6,
	TOKEN_INPUT_COLON       = 7,
	TOKEN_INPUT_BACKSLASH   = 8,
	TOKEN_INPUT_OTHER       = 9,
	TOKEN_INPUT_SIZE
} TokenInput;

/**
 * This is the state table.
 *
 * We encode the previously shown State Table for use in our tokenizer.
 */
uint8_t states[TOKEN_STATE_SIZE][TOKEN_INPUT_SIZE] = {
	/* TOKEN_STATE_NONE */
	{
		TOKEN_STATE_END,           // EOF
		TOKEN_STATE_NONE,          // WHITESPACE
		TOKEN_STATE_ID,            // ALPHABET
		TOKEN_STATE_INT,           // NUMERIC
		TOKEN_STATE_REAL,          // DOT
		TOKEN_STATE_DQUOTE_STRING, // DOUBLEQUOTE
		TOKEN_STATE_SQUOTE_STRING, // SINGLEQUOTE
		TOKEN_STATE_DEBUG,         // COLON
		TOKEN_STATE_ERROR,         // BACKSLASH
		TOKEN_STATE_ID             // OTHER
	},

	/* TOKEN_STATE_INT */
	{
		TOKEN_STATE_END,    // EOF
		TOKEN_STATE_NONE,   // WHITESPACE
		TOKEN_STATE_ID,     // ALPHABET
		TOKEN_STATE_INT,    // NUMERIC
		TOKEN_STATE_REAL,   // DOT
		TOKEN_STATE_ERROR,  // DOUBLEQUOTE
		TOKEN_STATE_ERROR,  // SINGLEQUOTE
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
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
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
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
		TOKEN_STATE_DQUOTE_STRING, // COLON
		TOKEN_STATE_DQUOTE_STRING, // BACKSLASH
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
		TOKEN_STATE_SQUOTE_STRING, // COLON
		TOKEN_STATE_SQUOTE_STRING, // BACKSLASH
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
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
		TOKEN_STATE_ID,     // OTHER
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
		TOKEN_STATE_ERROR,  // COLON
		TOKEN_STATE_ERROR,  // BACKSLASH
		TOKEN_STATE_DEBUG   // OTHER
	},
};

// TokenInput get_input(char input) {
// 	return TOKEN_INPUT_EOF;
// }

// int tokenize(char *input, int size, TokenResult *list)
// {
// 	for (int i = 0; i < size; i++) {
// 		switch (input[i]) {
// 			case ' ':
// 			case '\t':
// 			case '\n':

// 		}
// 	}
// }