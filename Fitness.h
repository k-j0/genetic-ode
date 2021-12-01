#pragma once

#include <functional>
#include "Expression.h"


/**
 * Represents an ODE boundary, of the form y^(d) (x = x_0) = y_0, where d, x_0 and y_0 are parameters
 */
template<typename T>
struct Boundary {

	/**
	 * The points in x at which to evaluate the boundary condition
	 */
	T x = 0;

	/**
	 * The expected value for the boundary condition
	 */
	T result = 0;

	/**
	 * The derivative to evaluate; 0 for y(x), 1 for y'(x), 2 for y''(x)
	 */
	int derivative = 0;

	/**
	 * Evaluate the boundary condition as y^(d) (x_0) - y_0
	 */
	inline const T evaluate(const ExpressionPtr<T>& y, const ExpressionPtr<T>& dy, const ExpressionPtr<T>& ddy) const {
		T d;
		switch (derivative) {
		case 2: d = ddy->evaluate(x); break;
		case 1: d = dy->evaluate(x); break;
		default: d = y->evaluate(x); break;
		}
		return d - result;
	}
};


/**
 * Utility evaluator of the fitness function to solve an ODE
 */
template<typename T>
class Fitness {

	/**
	 * Ordinary differential equation for which the fitness will be evaluated
	 * First parameter is x
	 * Second parameter is f(x)
	 * Third parameter is f'(x)
	 * Fourth parameter is f''(x)
	 */
	const std::function<const T(const T, const T, const T, const T)> function;

	/**
	 * Range of points for which the functions will be evaluated
	 */
	const T rangeStart, rangeEnd;
	const int numPoints;

	/**
	 * Extra parameter to control how much weight to place on boundary conditions when computing fitness function
	 */
	const T lambda;

	/**
	 * Boundary conditions
	 */
	std::vector<Boundary<T>> boundaries;

public:

	/**
	 * Default constructor
	 * @param std::function<const T(const T, const T, const T, const T) ode		The ordinary differential equation to compute; the first parameter is x, the second is f(x), the third is f'(x), the fourth is f''(x).
	 * @param T rangeStart														Start of the range of points for which the functions will be evaluated
	 * @param T rangeEnd														End of the range of points for which the functions will be evaluated
	 * @param int numPoints														Number of points to distribute in the range at which the functions will be evaluated
	 * @param T lambda															Extra parameter to control how much weight to place on boundary conditions in fitness evaluation
	 * @param std::vector<Boundary<T>> boundaries								Boundary conditions
	 */
	Fitness(std::function<const T(const T, const T, const T, const T)> ode, T rangeStart, T rangeEnd, int numPoints, T lambda, std::vector<Boundary<T>> boundaries) :
		function(ode), rangeStart(rangeStart), rangeEnd(rangeEnd), numPoints(numPoints), lambda(lambda), boundaries(boundaries) {}

	/**
	 * Computes the fitness of a expression taken with respect to the given ODE
	 */
	const T fitness(const ExpressionPtr<T>& y) const;

}; // class Fitness



template<typename T>
inline const T Fitness<T>::fitness(const ExpressionPtr<T>& y) const {

	// Compute the first and second derivatives of the expression
	ExpressionPtr<T> dy = y->derivative()->simplify();
	ExpressionPtr<T> ddy = dy->derivative()->simplify();
	
	// Compute E(M_g), the sum of the squared evaluation of the expression with respect to the given ODE
	T e = 0;
	for (int i = 0; i < numPoints; ++i) {
		T t = T(i) / T(numPoints-1); // 0..1 (inclusive)
		T x = rangeStart + t * (rangeEnd - rangeStart); // rangeStart..rangeEnd
		T result = function(x, y->evaluate(x), dy->evaluate(x), ddy->evaluate(x));
		e += result * result;
	}

	// Compute the boundary conditions into the penalty
	T p = 0;
	for (auto& b : boundaries) {
		if (b.x < rangeStart || b.x > rangeEnd) {
			printf("Warning: boundary condition is outside domain; x = %f, domain is [%f, %f]!\n", b.x, rangeStart, rangeEnd);
			exit(-1);
		}
		T result = b.evaluate(y, dy, ddy);
		p += result * result;
	}

	return e + lambda * p;
}
