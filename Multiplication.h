#pragma once

#include "Expression.h"

template<typename T>
class Multiplication : public Expression<T> {
private:
	ExpressionPtr<T> a;
	ExpressionPtr<T> b;
public:

	Multiplication(ExpressionPtr<T> a, ExpressionPtr<T> b) : a(a), b(b) {}

	T evaluate(T x, T y) const override;

	ExpressionPtr<T> derivative(int dimension) const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "(" + a->toString() + " * " + b->toString() + ")"; }

	bool isConstant() const override { return a->isConstant() && b->isConstant(); }
};
#define MultiplicationPtr(T, a, b) ExpressionPtr<T>(new Multiplication<T>(a, b))
#define MultiplicationPtrf(a, b) MultiplicationPtr(float, a, b)
#define MultiplicationPtrd(a, b) MultiplicationPtr(double, a, b)



template<typename T>
inline T Multiplication<T>::evaluate(T x, T y) const {
	return a->evaluate(x, y) * b->evaluate(x, y);
}

template<typename T>
inline ExpressionPtr<T> Multiplication<T>::derivative(int dimension) const {
	auto aPrime = a->derivative(dimension);
	auto bPrime = b->derivative(dimension);
	auto aTimesBPrime = MultiplicationPtr(T, a, bPrime);
	auto bTimesAPrime = MultiplicationPtr(T, b, aPrime);
	return AdditionPtr(T, aTimesBPrime, bTimesAPrime);
}

template<typename T>
inline ExpressionPtr<T> Multiplication<T>::simplify() const {
	ExpressionPtr<T> a_s = a->simplify();
	ExpressionPtr<T> b_s = b->simplify();
	bool constA = a_s->isConstant();
	bool constB = b_s->isConstant();
	if (constA && constB) {
		return ConstantPtr(T, a_s->evaluate(0, 0) * b_s->evaluate(0, 0));
	}
	if ((constA && a_s->evaluate(0, 0) == 0) || (constB && b_s->evaluate(0, 0) == 0)) {
		return ConstantPtr(T, 0);
	}
	if (constA && a_s->evaluate(0, 0) == 1) {
		return b_s;
	}
	if (constB && b_s->evaluate(0, 0) == 1) {
		return a_s;
	}
	return MultiplicationPtr(T, a_s, b_s);
}
