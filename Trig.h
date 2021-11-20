#pragma once

#include "Expression.h"
#include "Multiplication.h"

template<typename T>
class Sine : public Expression<T> {
private:
	ExpressionPtr<T> a;
public:

	Sine(ExpressionPtr<T> a) : a(a) {}

	T evaluate(T x) const override;

	ExpressionPtr<T> derivative() const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "sin(" + a->toString() + ")"; }

	bool isConstant() const override { return a->isConstant(); }

};
#define SinePtr(T, a) ExpressionPtr<T>(new Sine<T>(a))
#define SinePtrf(a) SinePtr(float, a)

template<typename T>
class Cosine : public Expression<T> {
private:
	ExpressionPtr<T> a;
public:

	Cosine(ExpressionPtr<T> a) : a(a) {}

	T evaluate(T x) const override;

	ExpressionPtr<T> derivative() const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "cos(" + a->toString() + ")"; }

	bool isConstant() const override { return a->isConstant(); }

};
#define CosinePtr(T, a) ExpressionPtr<T>(new Cosine<T>(a))
#define CosinePtrf(a) CosinePtr(float, a)




template<typename T>
inline T Sine<T>::evaluate(T x) const {
	return sin(x);
}

template<typename T>
inline ExpressionPtr<T> Sine<T>::derivative() const {
	// sin'(a) = a' cos(a)
	return MultiplicationPtr(T, a->derivative(), CosinePtr(T, a));
}

template<typename T>
inline ExpressionPtr<T> Sine<T>::simplify() const {
	if (a->isConstant()) {
		return ConstantPtr(T, sin(a->evaluate(0)));
	}
	return SinePtr(T, a->simplify());
}

template<typename T>
inline T Cosine<T>::evaluate(T x) const {
	return cos(x);
}

template<typename T>
inline ExpressionPtr<T> Cosine<T>::derivative() const {
	// cos'(a) = -a' sin(a)
	auto minusAPrime = MultiplicationPtr(T, ConstantPtr(T, -1), a->derivative());
	return MultiplicationPtr(T, minusAPrime, SinePtr(T, a));
}

template<typename T>
inline ExpressionPtr<T> Cosine<T>::simplify() const {
	if (a->isConstant()) {
		return ConstantPtr(T, cos(a->evaluate(0)));
	}
	return CosinePtr(T, a->simplify());
}
