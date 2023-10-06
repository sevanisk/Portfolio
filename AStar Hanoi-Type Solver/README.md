The command arguments are used to choose the file and heuristic. The heuristic is optional and defaults to the best heuristic i.e. option 3

To compile: make

To run: make run file=[filename] heuristic=[number]

Filename value format: [path]/probA03.bwp

For example, all my problems are stored in the probs folder so I could run:
make run file=probs/probA03.bwp


Heuristic values:
0 - BFS
1 - heuristic 1 (worst)
2 - heuristic 2 (okay)
3 - BEST HEURISTIC

Example:
make run file=probs/probA03.bwp heuristic=3

Constraints:
As long as the file is set up in the same fashion as the example problem files, my program should be able to run on any number of stacks and blocks.

My program can solve almost all of the problems in the problem set (it fails on B19), but it struggles on complex problems where you must make a lot of moves that don't at first appear to improve the state. For example, in some more complex problems you must move one letter from one wrong stack to another wrong stack to get to the letter beneath it, a move that may on occaision not improve the heuristic value even though it is necesary to solving the problem.