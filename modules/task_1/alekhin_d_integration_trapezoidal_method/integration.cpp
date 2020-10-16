#include<iostream>
#include <mpi.h>
#include <vector>
#include "integration.h"

double myFunc(double x) { return x / 2; }

std::function<double(double)> f = myFunc;

double localIntegration(double func_x[], double func_y[], int size)
{
	double local_integral = 0;
	double height = func_x[1] - func_x[0];

	for (int i = 0; i < size - 1; i++)
		local_integral += height * (func_y[i] + func_y[i + 1]) / 2;

	return local_integral;
}

double integration(std::function<double(double)> func, int n, int a, int b)
{
	if (n < 1) throw "Error: n < 1";
	if (a > b) throw "Error: a > b";

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (n < size) throw "Error: a > b";

	double** func_x = new double*[size];
	double** func_y = new double* [size];

	double step_x = (b - a) / (double)n;
	double local_integ_width = (b - a) / (double)size;

	if(rank == 0)
		for (int i = 0; i < size; i++)
		{
			double* local_integ_x = new double[n / size + 1];
			double* local_integ_y = new double[n / size + 1];

			for (int j = 0; j <= n / size; j++)
			{
				local_integ_x[j] = a + i * local_integ_width + j * step_x;
				local_integ_y[j] = func(local_integ_x[j]);	
			}
			func_x[i] = local_integ_x;
			func_y[i] = local_integ_y;
			
		}


	double* local_func_x = new double[n / size + 1];
	double* local_func_y = new double[n / size + 1];

	if (rank == 0)
	{
		for (int i = 1; i < size; i++)
		{
			MPI_Send(func_x[i], n / size + 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
			MPI_Send(func_y[i], n / size + 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
		}

		local_func_x = func_x[0];
		local_func_y = func_y[0];
	}
	else
	{
		MPI_Recv(local_func_x, n / size + 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(local_func_y, n / size + 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	double local_integral = localIntegration(local_func_x, local_func_y, n / size + 1);
	double global_integral = 0;
	MPI_Reduce(&local_integral, &global_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Bcast(&global_integral, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	return global_integral;
}





















































/*
// passing function, number of segments(n) and integration limits(a <= b)
double integrationSequentialMethod(std::function<double(double)> func, int n, int a, int b) 
{
	if (n < 1) throw "Error: n < 1";
	if (a > b) throw "Error: a > b";

	double width = (b - a) / (double)n;
	double integral = 0;

	for (int i = 0; i < n; i++)
	{
		int leftPoint = width * i + a;
		int rightPoint = width * (i + 1) + a;

		// area of integral = ((a + b) * h) / 2
		integral += (f(leftPoint) + f(rightPoint)) * (rightPoint - leftPoint) / 2;
	}

	return integral;
}

double trapezoinArea(double x1, double x2, double x1_val, double x2_val)
{ return (x2_val * x1_val) * (x2 - x1) / 2; }

// passing function, number of segments(n) and integration limits(a <= b)
double integrationParallelMethod(std::function<double(double)> func, int n, int a, int b)
{
	if (n < 1) throw "Error: n < 1";
	if (a > b) throw "Error: a > b";

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/*if (size == 1)
	{
		MPI_Finalize();
		return integrationSequentialMethod(func, n, a, b);
	}

	std::vector<double> func_x(n);
	std::vector<double> func_y(n);

	int step_x = (b - a) / n;

	for (int i = 0; i < n; i++)
	{
		func_x[i] = a + step_x;
		func_y[i] = func(func_x[i]);
	}

	if (rank == 0)
	{
		for (int i = 1; i < size; i++)
		{
			MPI_Send(&func_x, n, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
			MPI_Send(&func_y, n, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
		}
	}

	std::vector<double> local_func_x;
	std::vector<double> local_func_y;

	if (rank == 0)
	{
		local_func_x = func_x;
		local_func_y = func_y;
	}
	else
	{
		MPI_Recv(&local_func_x, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&local_func_y, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	double global_integral = 0;
	double local_integral = 0;
	for (int i = 0; i < size; i++)
	{
		if (rank == i)
			local_integral = trapezoinArea(local_func_x[i], local_func_x[i + 1], local_func_y[i], local_func_y[i + 1]);
	}
	MPI_Reduce(&local_integral, &global_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	return global_integral;
}*/