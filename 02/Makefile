name=aoc-24-02

warnings=-Wall -Wextra -pedantic
sanitize=-fsanitize=address,undefined,leak -fno-omit-frame-pointer
CFLAGS=$(warnings) $(sanitize) -g3 -O3 -std=c89

$(name): src/main.c
	$(CC) $(CFLAGS) -o $@ $^
