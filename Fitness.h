#pragma once

#include <functional>
#include "Expression.h"


/**
 * Represents an ODE boundary, of the form y^(d) (x = x_0) = y_0, where d, x_0 and y_0 are parameters
 */
template<typename T>
struct Boundary {

	/**
	 * The point in x at which to evaluate the boundary condition
	 */
	T x = 0;

	/**
	 * The point in y at which to evaluate the boundary condition
	 */
	T y = 0;

	/**
	 * The expected value for the boundary condition
	 */
	T result = 0;

	/**
	 * The derivative to evaluate; 0 for f(x, y), 1 for f'(x, y), 2 for f''(x, y)
	 */
	int derivative = 0;

	/**
	 * The variable with respect to which to take the derivative (0 -> x, 1 -> y, etc.)
	 */
	int dimension = 0;

	/// Constructors
	inline Boundary(T x, T f, int derivative) : x(x), result(f), derivative(derivative) {}
	inline Boundary(T x, T y, T f, int derivative, int dimension) : x(x), y(y), result(f), derivative(derivative), dimension(dimension) {}

	/**
	 * Evaluate the boundary condition as f^(d) (x_0, y_0) - f_0
	 */
	inline const T evaluate(const ExpressionPtr<T>& f, const ExpressionPtr<T>& dFdx, const ExpressionPtr<T>& dFdy, const ExpressionPtr<T>& ddFdx2, const ExpressionPtr<T>& ddFdy2) const {
		T d;
		if (dimension == 0) {
			switch (derivative) {
			case 2: d = dFdx->evaluate(x, y); break;
			case 1: d = ddFdx2->evaluate(x, y); break;
			default: d = f->evaluate(x, y); break;
			}
		} else if (dimension == 1) {
			switch (derivative) {
			case 2: d = dFdy->evaluate(x, y); break;
			case 1: d = ddFdy2->evaluate(x, y); break;
			default: d = f->evaluate(x, y); break;
			}
		} else { assert(false); }
		return d - result;
	}
};


/**
 * Represents the function parameters that are sent to the ODE/PDE function
 */
template<typename T>
struct FunctionParams {
	T x;
	T y;
	T f; // f(x, y)
	T ddx; // ∂/∂x f(x, y)
	T ddy; // ∂/∂y f(x, y)
	T ddx2; // ∂²/∂x² f(x, y)
	T ddy2; // ∂²/∂y² f(x, y)
};


/**
 * Represents a linearly spaced range on one of the axes
 */
template<typename T>
struct Domain {
	const T rangeStart, rangeEnd;
	const int numPoints;

	inline Domain(T rangeStart = 0, T rangeEnd = 1, int numPoints = 10) : rangeStart(rangeStart), rangeEnd(rangeEnd), numPoints(numPoints) {}

	/**
	 * Returns the nth point in the range defined
	 */
	inline T point(int n) const {
		if (rangeStart == rangeEnd) return rangeStart;
		T t = T(n) / T(numPoints - 1); // 0..1 (inclusive)
		return rangeStart + t * (rangeEnd - rangeStart);
	}
};
#define EMPTY 0, 0, 1


/**
 * Utility evaluator of the fitness function to solve an ODE/PDE
 */
template<typename T>
class Fitness {

	/**
	 * Ordinary differential equation for which the fitness will be evaluated
	 */
	const std::function<const T(const FunctionParams<T> params)> function;

	/**
	 * Range of points for which the functions will be evaluated
	 */
	const Domain<T> domainX;
	const Domain<T> domainY;

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
	 * @param std::function<const T(const FunctionParams<T>) fn		The differential equation to compute
	 * @param Domain<T> domainX										Range of points on X for which to evaluate the function
	 * @param Domain<T> domainY										Range of points on Y for which to evaluate the function
	 * @param T lambda												Extra parameter to control how much weight to place on boundary conditions in fitness evaluation
	 * @param std::vector<Boundary<T>> boundaries					Boundary conditions
	 */
	Fitness(std::function<const T(const FunctionParams<T>)> fn, Domain<T> domainX, Domain<T> domainY, T lambda, std::vector<Boundary<T>> boundaries) :
		function(fn), domainX(domainX), domainY(domainY), lambda(lambda), boundaries(boundaries) {}

	/**
	 * Computes the fitness of a expression taken with respect to the given ODE
	 */
	const T fitness(const ExpressionPtr<T>& f) const;

}; // class Fitness



template<typename T>
inline const T Fitness<T>::fitness(const ExpressionPtr<T>& f) const {

	// Compute the first and second derivatives of the expression with respect to x and y
	ExpressionPtr<T> dFdx = f->derivative(0)->simplify();
	ExpressionPtr<T> ddFdx2 = dFdx->derivative(0)->simplify();
	ExpressionPtr<T> dFdy = f->derivative(1)->simplify();
	ExpressionPtr<T> ddFdy2 = dFdy->derivative(1)->simplify();
	
	// Compute E(M_g), the sum of the squared evaluation of the expression with respect to the given ODE
	T e = 0;
	for (int ix = 0; ix < domainX.numPoints; ++ix) {
		for (int iy = 0; iy < domainY.numPoints; ++iy) {
			FunctionParams<T> p;
			p.x = domainX.point(ix);
			p.y = domainY.point(iy);
			p.f = f->evaluate(p.x, p.y);
			p.ddx = dFdx->evaluate(p.x, p.y);
			p.ddy = dFdy->evaluate(p.x, p.y);
			p.ddx2 = ddFdx2->evaluate(p.x, p.y);
			p.ddy2 = ddFdy2->evaluate(p.x, p.y);
			T result = function(p);
			e += result * result;
		}
	}

	// Compute the boundary conditions into the penalty
	T p = 0;
	for (auto& b : boundaries) {
		if (b.x < domainX.rangeStart || b.x > domainX.rangeEnd) {
			printf("Warning: boundary condition is outside domain; x = %f, domain is [%f, %f]!\n", b.x, domainX.rangeStart, domainX.rangeEnd);
			exit(-1);
		}
		if (b.y < domainY.rangeStart || b.y > domainY.rangeEnd) {
			printf("Warning: boundary condition is outside domain; y = %f, domain is [%f, %f]!\n", b.y, domainY.rangeStart, domainY.rangeEnd);
			exit(-1);
		}
		T result = b.evaluate(f, dFdx, dFdy, ddFdx2, ddFdy2);
		p += result * result;
	}

	return e + lambda * p;
}
