#include <mpi.h>
#include "integration.h"

double myFunc(double x) { return x / 2; }

std::function<double(double)> f = myFunc;

// passing function, number of segments(n) and integration limits(a <= b)
double integrationSequentialMethod(std::function<double(double)> func, int n, int a, int b) 
{
	if (n < 1)
		throw "Error: n < 1";

	if(a > b)
		throw "Error: a > b";

	double width = (b - a) / n;
	
	int integral = 0;
	for (int i = 0; i < n; i++)
	{
		int leftSide = func(width * i);
		int rightSide = func(width * (i + 1));
		int h = width;

		// area of integral = ((a + b) * h) / 2
		integral += ((leftSide + rightSide) * h) / 2;
	}

	return integral;
}

// passing function, number of segments(n) and integration limits(a <= b)
double integrationParallelMethod(std::function<double(double)> func, int n, int a, int b)
{

}
