


//#define FULLY_RANDOM // whether to completely randomise the population every single generation
//#define EXAMPLE_ODES // whether to run example ODE problems
//#define EXAMPLE_NLODES // whether to run example NLODE problems
//#define EXAMPLE_PDES // whether to run example PDE problems
#define HEAT // whether to run the heat equation problem
#define VERBOSE true


#ifdef FULLY_RANDOM
	#define POPULATION_SIZE 5000
	#define CHROMOSOME_SIZE 50
	#define REPLICATION_RATE 0.0002
	#define MUTATION_RATE 0.0002
	#define RANDOM_RATE 0.9996
	#define GENERATIONS 1000
#else
	#define POPULATION_SIZE 1000
	#define CHROMOSOME_SIZE 50
	#define REPLICATION_RATE 0.1
	#define MUTATION_RATE 0.05
	#define RANDOM_RATE 0.0
	#define GENERATIONS 2000
#endif


// ------------------------------------


#include <ctime>
#include <stdlib.h>
#include <cmath>
#include <thread>
#ifndef M_PI
#define M_PI 3.14159265359
#endif
#include "Vars.h"
#include "Addition.h"
#include "Multiplication.h"
#include "Division.h"
#include "Subtraction.h"
#include "Trig.h"
#include "Exponential.h"
#include "Logarithm.h"
#include "SquareRoot.h"
#include "Power.h"
#include "GrammarDecoder.h"
#include "Population.h"
#if defined(EXAMPLE_ODES) or defined(EXAMPLE_NLODES)
	#include "ExampleODEs.h"
#endif
#ifdef EXAMPLE_PDES
	#include "ExamplePDEs.h"
#endif



Fitness<double> heatPde(double tMax) {
	// exact solution:
	// u(x, t) = e^{- pi^2 t} sin(pi x)
	return Fitness<double>(
		[](FunctionParams<double> p) -> const double {
			return p.ddx2 - p.ddy; // d^2/dx^2 u = d/dt u
		},
		Domain<double>(0, 1, 50), Domain<double>(0, tMax, 50), 1,
		{
			Boundary<double>(0, 0, [](double t, double f, double dfdx, double ddfdx) -> double {
				return -f; // u(0, t) = 0
			}),
			Boundary<double>(1, 0, [](double t, double f, double dfdx, double ddfdx) -> double {
				return -f; // u(L, t) = 0
			}),
			Boundary<double>(0, 1, [](double x, double f, double dfdt, double ddfdt) -> double {
				return f - sin(M_PI * x); // u(x, 0) = sin(pi x)
			})
		}
	);
}




void solve(std::string name, Fitness<double> fitnessFunction, GrammarDecoder<double>* decoder, int seed) {

	// Init population
	Population<double> population(POPULATION_SIZE, CHROMOSOME_SIZE, REPLICATION_RATE, MUTATION_RATE, RANDOM_RATE, &fitnessFunction, decoder, seed);

	// Iterate over generations
	const Chromosome<double>* top = nullptr;
	int gen;
	double fitness = INFINITY;
	std::shared_ptr<Expression<double>> bestExpression = nullptr;
	for (gen = 1; gen <= GENERATIONS; ++gen) {
		top = population.nextGeneration();
		if (top && top->fitness < fitness) {
			fitness = top->fitness;
			bestExpression = top->expression;
			if (VERBOSE) {
				printf("%s \tGen. %d, \tfitness %f, \tf(x, y) = %s\n", name.c_str(), gen, fitness, top->expression->toString().c_str());
			}
		}
		if (top && top->fitness < 1e-7) {
			break;
		}
	}
	
	// Log result
	if (!top) {
		printf("Could not solve %s, null result.\n\n", name.c_str());
	} else {
		printf("\nFinished solving %s in %d generations: \tfitness %f, \tf(x, y) = %s\n\n", name.c_str(), gen, fitness, bestExpression->toString().c_str());
		printf("d/dx f(x, y) = %s\n", bestExpression->derivative(0)->simplify()->toString().c_str());
		printf("d/dy f(x, y) = %s\n", bestExpression->derivative(1)->simplify()->toString().c_str());
		printf("d^2/dx^2 f(x, y) = %s\n", bestExpression->derivative(0)->derivative(0)->simplify()->toString().c_str());
		printf("d^2/dy^2 f(x, y) = %s\n\n", bestExpression->derivative(1)->derivative(1)->simplify()->toString().c_str());
	}
}


int main() {

	// Set up grammar - two different variants for 1D problems (ODEs) and 2D problems (PDEs)
	std::vector<GrammaticalElement_base<double>*> variables1d = {
		Gd(VarX)
	};
	std::vector<GrammaticalElement_base<double>*> variables2d = {
		Gd(VarX),
		Gd(VarY)
	};
	std::vector<GrammaticalElement_base<double>*> operations = {
		G2d(Addition),
		G2d(Subtraction),
		G2d(Multiplication),
		G2d(Division),
		G2d(Power)
	};
	std::vector<GrammaticalElement_base<double>*> functions = {
		G1d(Sine),
		G1d(Cosine),
		G1d(Exponential),
		G1d(Logarithm),
		// G1d(SquareRoot) <- not including sqrt() in the grammar since it's not quite general enough, we already have powers so it's possible to obtain ^0.5 anyways
	};
	auto decoder1d = new GrammarDecoder<double>(0, variables1d, operations, functions);
	auto decoder2d = new GrammarDecoder<double>(0, variables2d, operations, functions);


	std::vector<std::thread*> threads;



	// solve example ODEs from the original paper
#ifdef EXAMPLE_ODES
	for (int i = 1; i <= 9; ++i) {
		threads.push_back(new std::thread(solve, "ODE" + std::to_string(i), getExampleODE(i), decoder1d, i));
	}
#endif

	// solve example NLODEs from the original paper
#ifdef EXAMPLE_NLODES
	for (int i = 1; i <= 4; ++i) {
		threads.push_back(new std::thread(solve, "NLODE" + std::to_string(i), getExampleNLODE(i), decoder1d, i));
	}
#endif

	// solve example PDEs from the original paper
#ifdef EXAMPLE_PDES
	for (int i = 1; i <= 6; ++i) {
		threads.push_back(new std::thread(solve, "PDE" + std::to_string(i), getExamplePDE(i), decoder2d, i));
	}
#endif

#ifdef HEAT
	threads.push_back(new std::thread(solve, "Heat", heatPde(1), decoder2d, 1));
#endif


	for (auto it = threads.begin(); it != threads.end(); it++) {
		(*it)->join();
		delete* it;
	}

	delete decoder1d;
	delete decoder2d;
	return 0;
}
