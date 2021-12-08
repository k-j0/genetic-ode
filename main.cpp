
#include <ctime>
#include <thread>
#include "Vars.h"
#include "Addition.h"
#include "Multiplication.h"
#include "Division.h"
#include "Subtraction.h"
#include "Trig.h"
#include "Exponential.h"
#include "Logarithm.h"
#include "GrammarDecoder.h"
#include "Population.h"
#include "ExampleODEs.h"


GrammarDecoder<double>* decoder;


//#define FULLY_RANDOM // whether to completely randomise the population every single generation
#define VERBOSE false

#ifdef FULLY_RANDOM
	#define POPULATION_SIZE 5000
	#define CHROMOSOME_SIZE 50
	#define REPLICATION_RATE 0.0002
	#define MUTATION_RATE 0.0002
	#define RANDOM_RATE 0.9996
	#define GENERATIONS 1000
#else
	#define POPULATION_SIZE 500
	#define CHROMOSOME_SIZE 50
	#define REPLICATION_RATE 0.1
	#define MUTATION_RATE 0.05
	#define RANDOM_RATE 0.15
	#define GENERATIONS 2000
#endif


void solve(std::string name, Fitness<double> fitnessFunction, int seed) {

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

	// Set up grammar
	std::vector<GrammaticalElement_base<double>*> variables = {
		Gd(VarX),
		//Gd(VarY) // <- included only for PDEs
	};
	std::vector<GrammaticalElement_base<double>*> operations = {
		G2d(Addition),
		G2d(Subtraction),
		G2d(Multiplication),
		G2d(Division)
	};
	std::vector<GrammaticalElement_base<double>*> functions = {
		G1d(Sine),
		G1d(Cosine),
		G1d(Exponential),
		G1d(Logarithm)
	};
	decoder = new GrammarDecoder<double>(0, variables, operations, functions);



	// solve example ODEs
	std::vector<std::thread*> threads;
	for (int i = 1; i <= 9; ++i) {
		threads.push_back(new std::thread(solve, "ODE" + std::to_string(i), getExampleODE(i), i));
	}
	for (auto it = threads.begin(); it != threads.end(); it++) {
		(*it)->join();
		delete *it;
	}

	/*solve("PDE3", Fitness<double>(
		[](FunctionParams<double> p) -> const double {
			return p.ddx2 + p.ddy2 - 4;
		},
		Domain<double>(), Domain<double>(), 1000,
		{
			Boundary<double>(0, 0, [](double y, double f, double dfdx, double ddfdx) -> double {
				return -f + y * y + y + 1; // psi(0, y) = y^2 + y + 1
			}),
			Boundary<double>(1, 0, [](double y, double f, double dfdx, double ddfdx) -> double {
				return -f + y * y + y + 3; // psi(1, y) = y^2 + y + 3
			}),
			Boundary<double>(0, 1, [](double x, double f, double dfdy, double ddfdy) -> double {
				return -f + x * x + x + 1; // psi(x, 0) = x^2 + x + 1
			}),
			Boundary<double>(1, 1, [](double x, double f, double dfdy, double ddfdy) -> double {
				return -f + x * x + x + 3; // psi(x, 1) = x^2 + x + 3
			})
		}
	), time(nullptr));*/
	


	delete decoder;

	return 0;
}
