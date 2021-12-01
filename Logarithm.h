#pragma once

#include "Expression.h"
#include "Multiplication.h"
#include "Division.h"

template<typename T>
class Logarithm : public Expression<T> {
private:
	ExpressionPtr<T> a;
public:

	Logarithm(ExpressionPtr<T> a) : a(a) {}

	T evaluate(T x, T y) const override;

	ExpressionPtr<T> derivative(int dimension) const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "log(" + a->toString() + ")"; }

	bool isConstant() const override { return a->isConstant(); }

};
#define LogarithmPtr(T, a) ExpressionPtr<T>(new Logarithm<T>(a))
#define LogarithmPtrf(a) LogarithmPtr(float, a)
#define LogarithmPtrd(a) LogarithmPtr(double, a)



template<typename T>
inline T Logarithm<T>::evaluate(T x, T y) const {
	T inner = a->evaluate(x, y);
	if (inner <= 0) {
		throw NAN;
	}
	return log(inner);
}

template<typename T>
inline ExpressionPtr<T> Logarithm<T>::derivative(int dimension) const {
	return DivisionPtr(T, a->derivative(dimension), a); // ln'(f) = f' / f
}

template<typename T>
inline ExpressionPtr<T> Logarithm<T>::simplify() const {
	if (a->isConstant()) {
		return ConstantPtr(T, log(a->evaluate(0, 0)));
	}
	return LogarithmPtr(T, a->simplify());
}
