
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
#include "Power.h"
#include "GrammarDecoder.h"
#include "Population.h"
#include "ExampleODEs.h"


GrammarDecoder<double>* decoder;


void solve(std::string name, Fitness<double> fitnessFunction, int seed, bool verbose) {

	// Init population
	Population<double> population(500, 50, 0.1f, 0.05f, 0.15f, &fitnessFunction, decoder, seed);

	// Iterate over generations
	const Chromosome<double>* top = nullptr;
	int gen;
	double fitness = INFINITY;
	std::shared_ptr<Expression<double>> bestExpression = nullptr;
	for (gen = 1; gen <= 4000; ++gen) {
		top = population.nextGeneration();
		if (top && top->fitness < fitness) {
			fitness = top->fitness;
			bestExpression = top->expression;
			if (verbose) {
				printf("%s \tGen. %d, \tfitness %f, \ty(x) = %s\n", name.c_str(), gen, fitness, top->expression->toString().c_str());
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
		G1d(Logarithm)
	};
	decoder = new GrammarDecoder<double>(0, variables, operations, functions);

	// solve examples
	/*std::vector<std::thread*> threads;
	for (int i = 1; i <= 9; ++i) {
		threads.push_back(new std::thread(solve, "ODE" + std::to_string(i), getExampleODE(i), i, false));
	}
	for (auto it = threads.begin(); it != threads.end(); it++) {
		(*it)->join();
		delete *it;
	}*/

	solve("PDE3", Fitness<double>(
		[](FunctionParams<double> p) -> const double {
			return p.ddx2 + p.ddy2 - 4;
		},
		Domain<double>(), Domain<double>(), 1,
		{
			// PDE boundary conditions not yet defined!
		}
	), time(nullptr), true);
	
	delete decoder;

	return 0;
}
