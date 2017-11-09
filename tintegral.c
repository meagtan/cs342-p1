

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "function.h"

// integrate function in interval [L,U] by dividing into K subintervals
struct arglist {
	double L;
	double width;
	double K;
};
void *integrate(void *args);

double sum = 0;

int main(int argc, char *argv[])
{
	double L, U; // just so we don't need to cast to double to do division
	int K, N, i, rc;
	pthread_t *pt; // array of thread ids
	double width;
	struct arglist *arg;

	// parse arguments

        // CORRECTION: argument checks not necessary, assume arguments are correct
        // First two test cases failed because atoi returns same value for 0 as for non-integer values
	L = atoi(argv[1]);
	U = atoi(argv[2]);
	K = atoi(argv[3]);
	N = atoi(argv[4]);

	// width of each interval
	width = (U - L) / N;

	// create child threads
	pt = calloc(N, sizeof(pthread_t));
	for (i = 0; i < N; ++i) {
		arg = malloc(sizeof(struct arglist));
		arg->L = L;
		arg->width = width / K;
		arg->K = K;

		rc = pthread_create(pt+i, NULL, integrate, (void *) arg); // cannot cast struct to void * apparently

		if (rc) {
			fprintf(stderr, "Error: pthread_create failed for thread %d with error code %d.\n", i, rc);
			return 1;
		}
		L += width;
	}

	// wait out each thread
	for (i = 0; i < N; ++i)
		pthread_join(pt[i], NULL);

	// report total
	printf("%f\n", sum);

	// frees
	free(pt);

	pthread_exit(NULL);
}

void *integrate(void *args)
{
	struct arglist *a = (struct arglist *) args;
	double fa, fb; // fa, fb values of f at endpoints, kept so they're not calculated twice
	int i;

	for (i = 0, fa = compute_f(a->L); i < a->K; ++i, fa = fb) {
		fb = compute_f(a->L + a->width);
		sum += (fa + fb) * a->width / 2; // area of trapezoid
		a->L += a->width;
	}

	free(a);
	pthread_exit(NULL);
}
