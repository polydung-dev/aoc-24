warnings=-Wall -Wextra -pedantic
sanitize=-fsanitize=address,undefined,leak -fno-omit-frame-pointer
CFLAGS=$(warnings) $(sanitize) -g3 -O3 -std=c89 -I./src/common

SHELL=bash
filenames=$(addprefix out/aoc-24-,$(shell echo {01..05}))
all: out/ $(filenames)

%/:
	mkdir -p $@

out/aoc-24-05: src/05/main.c src/common/da2.c src/common/utils.c
	$(CC) $(CFLAGS) -o $@ $(filter-out out/,$^)

out/aoc-24-%: src/%/main.c
	$(CC) $(CFLAGS) -o $@ $(filter-out out/,$^)

.PHONY: clean
clean:
	-rm -r out/
