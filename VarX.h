#pragma once

#include "Expression.h"

template<typename T>
class VarX : public Expression<T> {
public:

	T evaluate(T x) const override;

	ExpressionPtr<T> derivative() const override;

	ExpressionPtr<T> simplify() const override;

	std::string toString() const override { return "x"; }

	bool isConstant() const override { return false; }

};
#define VarXPtr(T) ExpressionPtr<T>(new VarX<T>())
#define VarXPtrf VarXPtr(float)



template<typename T>
inline T VarX<T>::evaluate(T x) const {
	return x;
}

template<typename T>
inline ExpressionPtr<T> VarX<T>::derivative() const {
	return ExpressionPtr<T>(new Constant<T>(1));
}

template<typename T>
inline ExpressionPtr<T> VarX<T>::simplify() const {
	return ExpressionPtr<T>(new VarX<T>());
}
