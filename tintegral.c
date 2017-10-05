

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "function.h"

int main(int argc, char *argv[])
{
	double y; 

	y = compute_f(100);
	printf ("%lf\n", y); 
}
