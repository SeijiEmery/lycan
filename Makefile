#
# A more general purpose makefile.  Change the values the variables to
# whatever is needed for your project and let the rules do the rest!
#
# Written September 1998 by Mauricio A. Hernandez

# The name of the compiler
CC=gcc
# Flags used by the compiler when creating object code
CFLAGS=-g -std=c99
# Flags used by the compiler when linking
FLAGS=

# The name of your executable
TARGET=lycan

# The names of the .o files you need to build.
OBJS=   main.o  \
		LyCompiler.o \
		LyCore.o \
		LyExec.o \
		LyInstr.o \
		LyLog.o \
		LyNumber.o \
		LyQueue.o \
		LyString.o \
		LyTable.o \
		LyTypes.o \
		LyVM.o

#
# The Rules
#
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJS) $(TARGET)

# Done. 
