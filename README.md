# Sudoku Solver

A simple brute force sudoku solver. The input should be formatted in a .in file where all blank cells in the puzzle are replaced with a 0. The input file should have no whitespace, except for new lines separating rows of the puzzle. 

For example, the a simple 2x2 puzzle would be formatted as:

0000
0043
2400
0000

The included makefile has two targets: a single-threaded version, and a multi-threaded version. The multithreaded version is guaranteed to complete the computation without any race conditions, but may lead to undefined behavior if there a puzzle is underdefined (i.e. has more than one solution). The single-threaded version does not share this caveat. The targets can be built by running the following commands:

Single-Threaded:

`make sudokuSolver`

Multi-Threaded:

`make sudokuSolverThreaded`

Both take a single argument - the path to the input file, and will save the solved puzzle as a .solved file. 
