#pragma once

#pragma once

#include <cassert>
#include <vector>
#include <algorithm>
#include <random>
#include "GrammarDecoder.h"
#include "Fitness.h"
#include "Expression.h"

#define RAND abs(int(rng()))

/**
 * Represents a single chromosome (i.e. individual) in the population
 */
template<typename T>
struct TreeChromosome {
	std::shared_ptr<Expression<T>> expression = nullptr; // the expression represented by the chromosome
	T fitness = INFINITY; // last fitness value computed for the chromosome
};

/**
 * Represents a population of genes that is being used to determine a differential equation's analytical form through GA
 */
template<typename T>
class TreePopulation {

private:

	/**
	 * Random number generator to use for the population
	 */
	std::mt19937 rng;

	/**
	 * Gives the number of generations that the population has gone through
	 */
	int generation = 0;

	/**
	 * Percentage of individuals that will create children
	 */
	float replicationRate;

	/**
	 * Probability of mutation of any single gene
	 */
	float mutationRate;

	/**
	 * Fitness function, which encodes the problem at hand
	 */
	const Fitness<T>* fitnessFunction;

	/**
	 * Decoder to use to read mathematical expressions from chromosomes
	 */
	const GrammarDecoder<T>* decoder;

	/**
	 * The actual population, made up of a collection of chromosomes
	 */
	std::vector<TreeChromosome<T>> chromosomes;

public:

	/**
	 * Default constructor; initializes the population with random values for all of the genes
	 */
	TreePopulation(unsigned int n, float replicationRate, float mutationRate, const Fitness<T>* fitnessFunction, const GrammarDecoder<T>* decoder, unsigned int seed = 0);

	/**
	 * Run through a single generation of the population
	 */
	const TreeChromosome<T>* nextGeneration();

};




template<typename T>
inline TreePopulation<T>::TreePopulation(unsigned int n, float replicationRate, float mutationRate, const Fitness<T>* fitnessFunction, const GrammarDecoder<T>* decoder, unsigned int seed) :
			replicationRate(replicationRate), mutationRate(mutationRate), fitnessFunction(fitnessFunction), decoder(decoder) {

	rng = std::mt19937(seed);

	assert(n >= 2);
	assert(fitnessFunction);
	assert(decoder);

	// Initialize population with random genes using the grammar
	chromosomes = std::vector<TreeChromosome<T>>(n);
	for (int i = 0; i < n; ++i) {
		auto sequence = std::vector<unsigned int>(20);
		for (int j = 0; j < sequence.size(); ++j) {
			sequence[j] = RAND % 1000;
		}
		chromosomes[i].expression = decoder->decode(sequence);
	}

}

template<typename T>
inline const TreeChromosome<T>* TreePopulation<T>::nextGeneration() {

	++generation;

	// Compute each chromosome's fitness
	for (auto& ch : chromosomes) {
		if (ch.expression == nullptr || ch.expression->isConstant()) {
			ch.fitness = INFINITY; // invalid expression, definitely don't want to keep this one
		} else {
			try {
				ch.expression = ch.expression->simplify();
				ch.fitness = fitnessFunction->fitness(ch.expression);
			} catch (...) { // handle invalid expressions with /0, log(-1), etc.
				ch.fitness = INFINITY;
			}
		}
	}

	// Sort by fitness - best chromosomes at the top, worst at the end
	std::sort(chromosomes.begin(), chromosomes.end(), [&](const TreeChromosome<T>& a, const TreeChromosome<T>& b) -> bool {
		return a.fitness < b.fitness;
	});

	// Genetic operations

	// Replication
	unsigned int parentCount = int(replicationRate * chromosomes.size());
	for (int i = parentCount; i < chromosomes.size(); ++i) {
		// replace chromosome with a parent selected at random
		for (int j = 0; j < parentCount; ++j) {
			if (RAND % 2 == 0 || j == parentCount-1) {

				// replicate chromosome [j]
				chromosomes[i].expression = chromosomes[j].expression;

				// mutations

				// modify random nodes in expression
				chromosomes[i].expression = chromosomes[i].expression->mutate(rng, mutationRate, decoder);

				break;
			}
		}
	}

	// Return top performer
	return &chromosomes[0];
}


#undef RAND
