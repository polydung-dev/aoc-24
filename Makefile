warnings=-Wall -Wextra -pedantic
sanitize=-fsanitize=address,undefined,leak -fno-omit-frame-pointer
CFLAGS=$(warnings) $(sanitize) -g3 -O3 -std=c89 -I./src/common

all: out/ out/aoc-24-01 out/aoc-24-02

%/:
	mkdir -p $@

out/aoc-24-01: src/01/main.c
	$(CC) $(CFLAGS) -o $@ $(filter-out out/,$^)

out/aoc-24-02: src/02/main.c
	$(CC) $(CFLAGS) -o $@ $(filter-out out/,$^)

.PHONY: clean
clean:
	-rm -r out/
