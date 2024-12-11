warnings=-Wall -Wextra -pedantic
sanitize=-fsanitize=address,undefined,leak -fno-omit-frame-pointer
CFLAGS=$(warnings) $(sanitize) -g3 -O3 -std=c89 -I./src/common
LDFLAGS=$(sanitize)
LDLIBS=-lm

common_sources=$(wildcard src/common/*.c)
common_objects=$(patsubst src/%,build/%,$(common_sources:.c=.o))

main_sources=$(wildcard src/*/main.c)
main_objects=$(patsubst src/%,build/%,$(main_sources:.c=.o))
dirs=$(sort $(dir $(common_objects)) $(dir $(main_objects)))

binaries=$(patsubst src/%/main.c,out/aoc-24-%,$(main_sources))

all: out/ $(dirs) $(binaries)

%/:
	mkdir -p $@

out/aoc-24-%: build/%/main.o $(common_objects)
	$(CC) $(LDFLAGS) -o $@ $(filter-out out/,$^) $(LDLIBS)

build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	-rm -r out/
