# Day 5

[Day 5][]

```
$ make
$ ./aoc-24-05
Output -> 143
```

Having the loop counter variable exist outside of the loops scope causes a
problem when using macros; The loop variables have to be unique, `i` cannot be
used for each loop. Rewriting the dynamic array to move away from macros and
use a void* implementation solves this along with adding potentially improved
error handling, at the cost of the small degree of type safety that C offers.

[Day 5]: <https://adventofcode.com/2024/day/5>
