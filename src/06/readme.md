# Day 6

[Day 6][]

```
Unique Tiles Visited -> 41
```

If the guard ends up on a previously visited space and is walking in the same
dirction, then the guard will loop. A barrel can only be placed in a tile that
has not been visited.

The input grid is 130x130, so I don't want to just brute force place a barrel
and run 16900 iterations to see if they loop.

- check if barrel can be placed
- if the current tile is rotated correctly, guard will loop
- check all tiles from the guard (after turning) to the first wall
- if any of the tiles match the new orientation, guard will loop
- if the tile before the wall is rotated correctly, guard will loop
- otherwise, copy grid and brute force

[Day 6]: <https://adventofcode.com/2024/day/6>
