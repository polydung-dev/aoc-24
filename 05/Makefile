name=aoc-24-05

warnings=-Wall -Wextra -pedantic
sanitize=-fsanitize=address,undefined,leak -fno-omit-frame-pointer
CFLAGS=$(warnings) $(sanitize) -g3 -O3 -std=c89

$(name): src/main.c src/utils.c src/da.c
	$(CC) $(CFLAGS) -o $@ $^
