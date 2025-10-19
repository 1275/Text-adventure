CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -O2
LDFLAGS := -lm

TARGET := adventure
SRCS := main.c dungeon.c enemies.c player.c
OBJS := $(SRCS:.c=.o)
HEADERS := dungeon.h enemies.h player.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): $(HEADERS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean