

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "function.h"

#define BUF_SIZE  sizeof(double)
#define READ_END  0
#define WRITE_END 1

// integrate function in interval [L,L+width] by dividing into K subintervals
double integrate(double L, double width, int K);

int main(int argc, char *argv[])
{
	double L, U; // just so we don't need to cast to double to do division
	int K, N, i;
	pid_t pid;
	int **fd; // Nx2 array of read and write addresses of each pipe
	double width, res = 0, temp;
	int status;

	// parse arguments

	// CORRECTION: argument checks not necessary, assume arguments are correct
	// First two test cases failed because atoi returns same value for 0 as for non-integer values
	L = atoi(argv[1]);
	U = atoi(argv[2]);
	K = atoi(argv[3]);
	N = atoi(argv[4]);

	// width of each interval
	width = (U - L) / N;

	// create processes and pipes for each process
	fd = calloc(N, sizeof(int *));
	for (i = 0; i < N; ++i) {
		fd[i] = calloc(2, sizeof(int));
		if (pipe(fd[i]) == -1) {
			fprintf(stderr, "Error: could not create pipe for process %d.\n", i);
			return 1;
		}

		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "Error: could not fork child process %d.\n", i);
			return 1;
		} else if (pid == 0) {
			close(fd[i][READ_END]);
			// compute integral, send
			res = integrate(L, width / K, K);
			write(fd[i][WRITE_END], (char *) &res, BUF_SIZE);
			close(fd[i][WRITE_END]);
			// printf("Child %d terminated\n", i);
			exit(0); // child need not execute any further code
		}
		// printf("Child %d created\n", i); // delete this later
		close(fd[i][WRITE_END]);
		L += width;
	}

	// wait out each child process
	for (i = 0; i < N; ++i)
		wait(&status);

	// compute total and report
	res = 0;
	for (i = 0; i < N; ++i) {
		// receive value from pipe
		read(fd[i][READ_END], (char *) &temp, BUF_SIZE);
		close(fd[i][READ_END]);
		res += temp;
		// printf("Child %d returned %f\n", i, temp);
	}
	printf("%f\n", res);

	// frees
	for (i = 0; i < N; ++i)
		free(fd[i]);
	free(fd);
}

double integrate(double L, double width, int K)
{
	double res = 0, fa, fb; // fa, fb values of f at endpoints, kept so they're not calculated twice
	int i;
	for (i = 0, fa = compute_f(L); i < K; ++i, fa = fb) {
		fb = compute_f(L + width);
		res += (fa + fb) * width / 2; // area of trapezoid
		L += width;
	}
	return res;
}
