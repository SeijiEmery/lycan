
To build:
> make lycan
> ./lycan

Lycan runs as an interactive interpreter. Available operations are limited (and intentionally so - this makes debugging and changes much easier). As such, only the string and number (floating point) data types are implemented, plus basic arithmetic operations (+, -, *, /); neither blocks nor flow control statements are implemented. (Note: I had blocks working in a previous version, but had to rewrite practically the entire thing to implement primitive error handling - thus, this version is slightly more primitive in some regards, but will actually abort when a syntax error is detected :P).

Input is quite simple: enter an operation (eg. 2 + 2) and the statement is evaluated with the last operation printed as output. Variables are defined and used as normal (eg. 'x = 10; x + 12'); statements are terminated either by an endline or semicolon. Numbers function normally;  adding two strings together is a concat operation, and multiplying a string by a number duplicates it many times (like python). Strings display some unusual behaviors however: prefixing a string by a negative sign inverts it (-"hello" -> "olleh"), and while adding a number to a string appends the stringified number to the string, adding a numeric string to a number will add the two (and likewise for subtraction).

Gotchas: There is no order of operations in this language (and parens haven't been implemented yet - well, the parser converts them into op codes, but the interpreter doesn't yet understand them, so you'll get a syntax error). Operations involving more than two entities also work their way backwards (since I accidentally implemented the instruction stack backwards (no big deal; just need to invert it between execution phases)). 
It's a quirky language - most of these 'features' (like number/string addition) will be removed from the 'practical' version - I just put them in there for fun :P

It also segfaults. I'm really not sure where the error is, and it happens inconsistently (though I have a sneaking suspicion that it may be linked to string operations). C is an effective language, but one slip up and your program will crash. This makes coding in it rather difficult.

As such, this particular project is now dead; I'm working on a successor (called Imp, for which I have a much more through language specification). The C code base is simply unmaintainable at this stage. I could continue, but coding in C is just too damn slow. There are problems with the current code base, and that means that I would need to rewrite half the code base... again. At this point it would quite honestly be easier to just switch to C++, and recode the thing from scratch, making use of templates and the STL.


