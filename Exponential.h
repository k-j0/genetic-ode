#pragma once

#include "Expression.h"
#include "Multiplication.h"

template<typename T>
class Exponential : public Expression<T> {
private:
	ExpressionPtr<T> a;
public:

	Exponential(ExpressionPtr<T> a) : a(a) {}

	T evaluate(T x) const override;

	ExpressionPtr<T> derivative() const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "exp(" + a->toString() + ")"; }

	bool isConstant() const override { return a->isConstant(); }

};
#define ExponentialPtr(T, a) ExpressionPtr<T>(new Exponential<T>(a))
#define ExponentialPtrf(a) ExponentialPtr(float, a)
#define ExponentialPtrd(a) ExponentialPtr(double, a)



template<typename T>
inline T Exponential<T>::evaluate(T x) const {
	return exp(x);
}

template<typename T>
inline ExpressionPtr<T> Exponential<T>::derivative() const {
	return MultiplicationPtr(T, a->derivative(), ExponentialPtr(T, a));
}

template<typename T>
inline ExpressionPtr<T> Exponential<T>::simplify() const {
	if (a->isConstant()) {
		return ConstantPtr(T, exp(a->evaluate(0)));
	}
	return ExponentialPtr(T, a->simplify());
}
