
#include <ctime>
#include "VarX.h"
#include "Addition.h"
#include "Multiplication.h"
#include "Division.h"
#include "Subtraction.h"
#include "Trig.h"
#include "Exponential.h"
#include "Logarithm.h"
#include "GrammarDecoder.h"
#include "Fitness.h"
#include "Population.h"


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
	GrammarDecoder<double> decoder(0, operations, functions);


	// Set up an ODE problem
	std::function<const double(const double, const double, const double, const double)> ode;
	ode = [](const double x, const double y, const double dy, const double ddy) -> const double {
		return -dy - y/5.0 + exp(-x / 5.0) * cos(x);
	};
	std::vector<Boundary<double>> boundaries = {
		{ 0, 0, 0 }
	};
	Fitness<double> fitness(ode, 0, 1, 50, 80, boundaries); // lambda penalty = 80

	// Initialize population
	Population<double> population(5000, 50, 0.1f, 0.1f, 0.1f, &fitness, &decoder, time(nullptr));
	double fit = INFINITY;
	const Chromosome<double>* top = nullptr;
	for (int i = 0; i < 2000; ++i) {
		top = population.nextGeneration();
		if (top->fitness < fit) {
			printf("Gen. %d \tfitness = %f: \ty = %s\n", i + 1, top->fitness, top->expression->toString().c_str());
			fit = top->fitness;
		}
		if (top->fitness < 1e-7) {
			break;
		}
	}
	printf("\ny'(x) = %s\ny''(x) = %s\n", top->expression->derivative()->simplify()->toString().c_str(), top->expression->derivative()->derivative()->simplify()->toString().c_str());

	return 0;
}
