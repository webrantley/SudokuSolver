sudokuSolver:
	gcc -std=c99 -Wall -o sudokuSolver ssolver.c checker.c

sudokuSolverThreaded:
	gcc -std=c99 -Wall -lpthread -o sudokuSolverThreaded ssolver_pthreads.c checker.c
