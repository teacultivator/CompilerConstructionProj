CC      = gcc
CFLAGS  = -Wall -g
TARGET  = stage1exe

SRCS = driver.c lexer.c parser.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

driver.o: driver.c lexer.h parser.h
	$(CC) $(CFLAGS) -c driver.c

lexer.o: lexer.c lexerDef.h lexer.h
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c parserDef.h parser.h lexerDef.h lexer.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -f $(OBJS) $(TARGET) cleanFile.txt out.txt
