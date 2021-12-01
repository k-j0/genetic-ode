#pragma once

#include "VarX.h"
#include "Addition.h"
#include "Multiplication.h"
#include "Division.h"
#include "Subtraction.h"
#include "Trig.h"
#include "Exponential.h"
#include "Logarithm.h"
#include "GrammarDecoder.h"
#include "Fitness.h"
#include "Population.h"


/// Example ODEs from the original paper

#define ODE(deriv, function) \
	[](const double x, const double y, const double dy, const double ddy) -> const double { \
		return -deriv + (function); \
	}

Fitness<double> ode1() {
	return Fitness<double>(
		ODE(dy, (2 * x - y) / x),
		0.1, 1, 10, 100,
		{
			{ 0.1, 20.1, 0 }
		});
}

Fitness<double> ode2() {
	return Fitness<double>(
		ODE(dy, (1-y*cos(x))/sin(x)),
		0.1, 1, 10, 100,
		{
			{ 0.1, 2.1/sin(0.1), 0 }
		});
}

Fitness<double> ode3() {
	return Fitness<double>(
		ODE(dy, -y/5 + exp(-x/5) * cos(x)),
		0, 1, 10, 100,
		{
			{ 0, 0, 0 }
		});
}

Fitness<double> ode4() {
	return Fitness<double>(
		ODE(ddy, -100*y),
		0, 1, 10, 100,
		{
			{ 0, 0, 0 },
			{ 0, 0, 1 }
		});
}

Fitness<double> ode5() {
	return Fitness<double>(
		ODE(ddy, 6 * dy - 9 * y),
		0, 1, 10, 100,
		{
			{ 0, 0, 0 },
			{ 0, 2, 1 }
		});
}

Fitness<double> ode6() {
	return Fitness<double>(
		ODE(ddy, -dy / 5 - y - exp(-x / 5) * cos(x) / 5),
		0, 2, 10, 100,
		{
			{ 0, 0, 0 },
			{ 0, 1, 1 }
		});
}

Fitness<double> ode7() {
	return Fitness<double>(
		ODE(ddy, -100 * y),
		0, 1, 10, 100,
		{
			{ 0, 0, 0 },
			{ 1, sin(10), 0 }
		});
}

Fitness<double> ode8() {
	return Fitness<double>(
		ODE(0, x * ddy + (1 - x) * dy + y),
		0, 1, 10, 100,
		{
			{ 0, 1, 0 },
			{ 1, 0, 0 }
		});
}

Fitness<double> ode9() {
	return Fitness<double>(
		ODE(ddy, -dy / 5 - y - exp(-x / 5) * cos(x) / 5),
		0, 1, 10, 100,
		{
			{ 0, 0, 0 },
			{ 1, sin(1) / exp(0.2), 0 }
		});
}

#undef ODE
