CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -O2

TARGET := adventure
SRCS := main.c dungeon.c enemies.c
OBJS := $(SRCS:.c=.o)
HEADERS := dungeon.h enemies.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): $(HEADERS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean