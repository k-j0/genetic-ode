#pragma once

#include "Expression.h"
#include "Multiplication.h"

template<typename T>
class Subtraction : public Expression<T> {
private:
	ExpressionPtr<T> a;
	ExpressionPtr<T> b;
public:

	Subtraction(ExpressionPtr<T> a, ExpressionPtr<T> b) : a(a), b(b) {}

	T evaluate(T x) const override;

	ExpressionPtr<T> derivative() const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "(" + a->toString() + " - " + b->toString() + ")"; }

	bool isConstant() const override { return a->isConstant() && b->isConstant(); }
};
#define SubtractionPtr(T, a, b) ExpressionPtr<T>(new Subtraction<T>(a, b))
#define SubtractionPtrf(a, b) SubtractionPtr(float, a, b)



template<typename T>
inline T Subtraction<T>::evaluate(T x) const {
	return a->evaluate(x) - b->evaluate(x);
}

template<typename T>
inline ExpressionPtr<T> Subtraction<T>::derivative() const {
	auto aPrime = a->derivative();
	auto bPrime = b->derivative();
	return SubtractionPtr(T, aPrime, bPrime);
}

template<typename T>
inline ExpressionPtr<T> Subtraction<T>::simplify() const {
	ExpressionPtr<T> a_s = a->simplify();
	ExpressionPtr<T> b_s = b->simplify();
	bool constA = a_s->isConstant();
	bool constB = b_s->isConstant();
	if (constA && constB) {
		return ConstantPtr(T, a_s->evaluate(0) - b_s->evaluate(0));
	}
	if (constA && a_s->evaluate(0) == 0) {
		return MultiplicationPtr(T, ConstantPtr(T, -1), b_s); // if 0 - b, simplify to -1 * b
	}
	if (constB && b_s->evaluate(0) == 0) {
		return a_s;
	}
	return SubtractionPtr(T, a_s, b_s);
}
