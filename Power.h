#pragma once

#include "Expression.h"
#include "Multiplication.h"
#include "Addition.h"
#include "Division.h"
#include "Logarithm.h"

template<typename T>
class Power : public Expression<T> {
private:
	ExpressionPtr<T> a;
	ExpressionPtr<T> b;
public:

	Power(ExpressionPtr<T> a, ExpressionPtr<T> b) : a(a), b(b) {}

	T evaluate(T x, T y) const override;

	ExpressionPtr<T> derivative(int dimension) const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "(" + a->toString() + " ^ " + b->toString() + ")"; }

	bool isConstant() const override { return a->isConstant() && b->isConstant(); }
};
#define PowerPtr(T, a, b) ExpressionPtr<T>(new Power<T>(a, b))
#define PowerPtrf(a, b) PowerPtr(float, a, b)
#define PowerPtrd(a, b) PowerPtr(double, a, b)



template<typename T>
inline T Power<T>::evaluate(T x, T y) const {
	return pow(a->evaluate(x, y), b->evaluate(x, y));
}

template<typename T>
inline ExpressionPtr<T> Power<T>::derivative(int dimension) const {
	auto aPrime = a->derivative(dimension);
	auto bPrime = b->derivative(dimension);
	// d/dx f(x)^g(x) = f(x)^g(x) * ( g'(x) ln(f(x)) + g(x)f'(x) / f(x) )
	return MultiplicationPtr(T,
		PowerPtr(T, a, b),
		AdditionPtr(T,
			MultiplicationPtr(T,
				bPrime,
				LogarithmPtr(T, a)
			),
			DivisionPtr(T,
				MultiplicationPtr(T, b, aPrime),
				a
			)
		)
	);
}

template<typename T>
inline ExpressionPtr<T> Power<T>::simplify() const {
	ExpressionPtr<T> a_s = a->simplify();
	ExpressionPtr<T> b_s = b->simplify();
	bool constA = a_s->isConstant();
	bool constB = b_s->isConstant();
	if (constA && constB) {
		return ConstantPtr(T, pow(a_s->evaluate(0, 0), b_s->evaluate(0, 0)));
	}
	if (constB && b_s->evaluate(0, 0) == 0) { // a^0 = 1 always
		return ConstantPtr(T, 1);
	}
	if (constA && a_s->evaluate(0, 0) == 0) { // 0^b = 0 always (except when b = 0)
		return ConstantPtr(T, 0);
	}
	if (constB && b_s->evaluate(0, 0) == 1) { // a^1 = 1
		return a_s;
	}
	return PowerPtr(T, a_s, b_s);
}
