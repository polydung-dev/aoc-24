warnings=-Wall -Wextra -pedantic
sanitize=-fsanitize=address,undefined,leak -fno-omit-frame-pointer
CFLAGS=$(warnings) $(sanitize) -g3 -O3 -std=c89 -I./src/common

aoc-24-01: src/01/main.c
	$(CC) $(CFLAGS) -o $@ $^
