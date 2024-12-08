# Advent of Code

For this years [AoC][] I will be writing in C89. This may be a msitake.

## Building

```
$ make
```

This command will produce an executable in `out/` named `aoc-24-<day>` for each
day that I have completed so far.

Each day has its own directory under `src/` which contains the day-specific code
along with a `readme.md` which links to the AoC page for that day along with
the results for the test input (located under `data/`).

Common code used by multiple days can be found under `src/common/`.

## Things that I like about C89 (so far)

- It's C.

## Things that I don't like about C89 (so far)

- Even the loop counter variables need to be declared at the top of the scope.
- There is `size_t` type, but no `%zu` format specifier?
- No support for "C++" comments, (`//`), and `/**/` comments cannot be nested.
    This makes commenting out blocks of code (e.g. for testing) more of a
    hassle than it needs to be.
- No `restrict` keyword.

[AoC]: <https://adventofcode.com/2024>
