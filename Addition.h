#pragma once

#include "Expression.h"

template<typename T>
class Addition : public Expression<T> {
private:
	ExpressionPtr<T> a;
	ExpressionPtr<T> b;
public:

	Addition(ExpressionPtr<T> a, ExpressionPtr<T> b) : a(a), b(b) {}

	T evaluate(T x) const override;

	ExpressionPtr<T> derivative() const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "(" + a->toString() + " + " + b->toString() + ")"; }

	bool isConstant() const override { return a->isConstant() && b->isConstant(); }
};
#define AdditionPtr(T, a, b) ExpressionPtr<T>(new Addition<T>(a, b))
#define AdditionPtrf(a, b) AdditionPtr(float, a, b)
#define AdditionPtrd(a, b) AdditionPtr(double, a, b)



template<typename T>
inline T Addition<T>::evaluate(T x) const {
	return a->evaluate(x) + b->evaluate(x);
}

template<typename T>
inline ExpressionPtr<T> Addition<T>::derivative() const {
	auto aPrime = a->derivative();
	auto bPrime = b->derivative();
	return AdditionPtr(T, aPrime, bPrime);
}

template<typename T>
inline ExpressionPtr<T> Addition<T>::simplify() const {
	ExpressionPtr<T> a_s = a->simplify();
	ExpressionPtr<T> b_s = b->simplify();
	bool constA = a_s->isConstant();
	bool constB = b_s->isConstant();
	if (constA && constB) {
		return ConstantPtr(T, a_s->evaluate(0) + b_s->evaluate(0));
	}
	if (constA && a_s->evaluate(0) == 0) {
		return b_s;
	}
	if (constB && b_s->evaluate(0) == 0) {
		return a_s;
	}
	return AdditionPtr(T, a_s, b_s);
}
