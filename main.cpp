
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
	GrammarDecoder<double> decoder(2, operations, functions);


	// Set up an ODE problem
	std::function<const double(const double, const double, const double, const double)> ode;
	ode = [](const double x, const double y, const double dy, const double ddy) -> const double {
		return x * ddy + (1.0 - x) * dy + y; // xy'' + (1 - x)y' + y = 0
	};
	std::vector<Boundary<double>> boundaries = {
		{ 0, 1, 0 }, // y(0) = 1
		{ 1, 0, 0 }, // y(1) = 0
	};
	Fitness<double> fitness(ode, 0.1, 1, 10, 100, boundaries); // 10 points in [0,1], lambda penalty = 100


	// Initialize population
	Population<double> population(1000, 50, 0.1f, 0.05f, &fitness, &decoder, time(nullptr));
	double fit = INFINITY;
	for (int i = 0; i < 2000; ++i) {
		auto* top = population.nextGeneration();
		if (top->fitness < fit) {
			printf("Gen. %d \tfitness = %f: \ty = %s\n", i + 1, top->fitness, top->expression->toString().c_str());
			fit = top->fitness;
		}
		if (top->fitness < 1e-7) {
			break;
		}
	}

	return 0;
}
