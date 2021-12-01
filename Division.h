#pragma once

#include "Expression.h"
#include "Multiplication.h"
#include "Subtraction.h"

template<typename T>
class Division : public Expression<T> {
private:
	ExpressionPtr<T> a;
	ExpressionPtr<T> b;
public:

	Division(ExpressionPtr<T> a, ExpressionPtr<T> b) : a(a), b(b) {}

	T evaluate(T x) const override;

	ExpressionPtr<T> derivative() const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "(" + a->toString() + " / " + b->toString() + ")"; }

	bool isConstant() const override { return a->isConstant() && b->isConstant(); }
};
#define DivisionPtr(T, a, b) ExpressionPtr<T>(new Division<T>(a, b))
#define DivisionPtrf(a, b) DivisionPtr(float, a, b)
#define DivisionPtrd(a, b) DivisionPtr(double, a, b)



template<typename T>
inline T Division<T>::evaluate(T x) const {
	T denominator = b->evaluate(x);
	if (denominator == 0) {
		throw NAN;
	}
	return a->evaluate(x) / denominator;
}

template<typename T>
inline ExpressionPtr<T> Division<T>::derivative() const {
	// f(x) = a(x) / b(x)
	// f' = (a'b - ab') / b^2
	auto aPrime = a->derivative();
	auto bPrime = b->derivative();
	auto aPrimeB = MultiplicationPtr(T, aPrime, b);
	auto bPrimeA = MultiplicationPtr(T, bPrime, a);
	return DivisionPtr(T, SubtractionPtr(T, aPrimeB, bPrimeA), MultiplicationPtr(T, b, b));
}

template<typename T>
inline ExpressionPtr<T> Division<T>::simplify() const {
	ExpressionPtr<T> a_s = a->simplify();
	ExpressionPtr<T> b_s = b->simplify();
	bool constA = a_s->isConstant();
	bool constB = b_s->isConstant();
	if (constA && constB) {
		return ConstantPtr(T, a_s->evaluate(0) / b_s->evaluate(0));
	}
	if (constA && a_s->evaluate(0) == 0) {
		return ConstantPtr(T, 0);
	}
	if (constB && b_s->evaluate(0) == 1) {
		return a_s;
	}
	return DivisionPtr(T, a_s, b_s);
}
