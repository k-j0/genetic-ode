#pragma once

#include <array>
#include <string>
#include <memory>

template<typename T>
class Expression {
public:

	/**
	 * Evaluates the expression at point x
	 */
	virtual T evaluate(T x) const = 0;

	/**
	 * Returns the expression that corresponds to the first derivative of this expression
	 */
	virtual const std::shared_ptr<Expression<T>> derivative() const = 0;

	/**
	 * Simplifies the given expression to make it easier to write out
	 * This will most likely NOT produce the best possible simplification of the given expression, but will at least be an improvement
	 */
	virtual const std::shared_ptr<Expression<T>> simplify() const = 0;

	/**
	 * Returns a string representation of the expression
	 */
	virtual std::string toString() const = 0;

	/**
	 * Returns whether the given expression is constant (i.e. y(x) = y(0) for all x in R)
	 */
	virtual bool isConstant() const = 0;

};
template<typename T>
using ExpressionPtr = const std::shared_ptr<Expression<T>>;




// Define constants here as well since they'll be needed in most dependents

template<typename T>
class Constant : public Expression<T> {
private:
	int v;

public:

	Constant(int v) : v(v) {}

	T evaluate(T x) const override;

	ExpressionPtr<T> derivative() const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return std::to_string(v); }

	bool isConstant() const override { return true; }
};
#define ConstantPtr(T, v) ExpressionPtr<T>(new Constant<T>(v))
#define ConstantPtrf(v) ConstantPtr(float, v)
#define ConstantPtrd(v) ConstantPtr(double, v)




template<typename T>
inline T Constant<T>::evaluate(T x) const {
	return T(v);
}

template<typename T>
inline ExpressionPtr<T> Constant<T>::derivative() const {
	return ExpressionPtr<T>(new Constant<T>(0));
}

template<typename T>
inline ExpressionPtr<T> Constant<T>::simplify() const {
	return ExpressionPtr<T>(new Constant<T>(v));
}

