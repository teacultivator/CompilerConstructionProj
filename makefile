# Compiler settings
CC = gcc
CFLAGS = -Wall -g

# Target executable name
TARGET = stage1exe

# Source files and Object files
SRCS = driver.c lexer.c parser.c
OBJS = $(SRCS:.c=.o)

# Default rule to build the target
all: $(TARGET)

# Linking the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compiling driver.c
driver.o: driver.c lexer.h parser.h
	$(CC) $(CFLAGS) -c driver.c

# Compiling lexer.c
lexer.o: lexer.c lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c lexer.c

# Compiling parser.c
parser.o: parser.c parser.h parserDef.h lexerDef.h
	$(CC) $(CFLAGS) -c parser.c

# Clean rule to remove compiled files
clean:
	rm -f *.o $(TARGET) cleanFile.txt