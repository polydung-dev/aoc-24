# Day 5

[Day 5][]

```
Output 1 -> 143
Output 2 -> 123

```

Having the loop counter variable exist outside of the loops scope causes a
problem when using macros; The loop variables have to be unique, `i` cannot be
used for each loop. Rewriting the dynamic array to move away from macros and
use a void* implementation solves this along with adding potentially improved
error handling, at the cost of the small degree of type safety that C offers.

[Day 5]: <https://adventofcode.com/2024/day/5>
