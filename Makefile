CC = gcc
CFLAGS = -Wall -Wextra
TARGET = myshell
SRCS = main.c job.c shell.c 
OBJS = $(SRCS:.c=.o)
HEADERS = job.h shell.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

