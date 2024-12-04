# Advent of Code

For this years [AoC][] I will be writing in C89. This may be a msitake.

Each day will be in its own directory (zero-padded day number), and built with
`make`. This will produce an executable in the working directory with the name
`aoc-24-<day>` where `<day>` is the two digit zero-padded day.

## Things that I like about C89 (so far)

- It's C.

## Things that I don't like about C89 (so far)

- Even the loop counter variables need to be declared at the top of the scope.
- There is `size_t` type, but no `%zu` format specifier?
- No support for "C++" comments, (`//`), and `/**/` comments cannot be nested.
    This makes commenting out blocks of code (e.g. for testing) more of a
    hassle than it needs to be.

[AoC]: <https://adventofcode.com/2024>
