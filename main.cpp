
#include <ctime>
#include "ExampleODEs.h"


GrammarDecoder<double>* decoder;


void solveEx(std::string name, Fitness<double> fitnessFunction, int seed, bool verbose) {

	// Init population
	Population<double> population(500, 50, 0.1f, 0.05f, 0.15f, &fitnessFunction, decoder, seed);

	// Iterate over generations
	const Chromosome<double>* top = nullptr;
	int gen;
	double fitness = INFINITY;
	for (gen = 1; gen <= 1000; ++gen) {
		top = population.nextGeneration();
		if (verbose && top && top->fitness < fitness) {
			fitness = top->fitness;
			printf("%s \tGen. %d, \tfitness %f, \ty(x) = %s\n", name.c_str(), gen, fitness, top->expression->toString().c_str());
		}
		if (top && top->fitness < 1e-7) {
			break;
		}
	}
	
	// Log result
	if (!top) {
		printf("Could not solve %s, null result.\n\n", name.c_str());
	} else {
		printf("Finished solving %s in %d generations: \tfitness %f, \ty(x) = %s\n\n", name.c_str(), gen, top->fitness, top->expression->toString().c_str());
	}
}


int main() {

	// Set up grammar
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
	decoder = new GrammarDecoder<double>(0, operations, functions);

	// solve examples
	solveEx("ODE1", ode1(), 1, true);
	solveEx("ODE2", ode2(), 2, true);
	solveEx("ODE8", ode8(), 8, true);

	
	delete decoder;

	return 0;
}
