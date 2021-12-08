#pragma once

#include "Fitness.h"


/// Example ODEs from the original paper

#define ODE(deriv, function) \
	[](FunctionParams<double> p) -> const double { \
		double x = p.x; double y = p.f; double dy = p.ddx; double ddy = p.ddx2; \
		return -deriv + (function); \
	}

Fitness<double> ode1() {
	return Fitness<double>(
		ODE(dy, (2 * x - y) / x),
		Domain<double>(0.1), Domain<double>(EMPTY), 100,
		{
			Boundary<double>(0.1, 20.1, 0)
		});
}

Fitness<double> ode2() {
	return Fitness<double>(
		ODE(dy, (1-y*cos(x))/sin(x)),
		Domain<double>(0.1), Domain<double>(EMPTY), 100,
		{
			Boundary<double>(0.1, 2.1/sin(0.1), 0)
		});
}

Fitness<double> ode3() {
	return Fitness<double>(
		ODE(dy, -y/5 + exp(-x/5) * cos(x)),
		Domain<double>(), Domain<double>(EMPTY), 100,
		{
			Boundary<double>(0.0, 0.0, 0)
		});
}

Fitness<double> ode4() {
	return Fitness<double>(
		ODE(ddy, -100*y),
		Domain<double>(), Domain<double>(EMPTY), 100,
		{
			Boundary<double>(0.0, 0.0, 0),
			Boundary<double>(0.0, 10.0, 1)
		});
}

Fitness<double> ode5() {
	return Fitness<double>(
		ODE(ddy, 6 * dy - 9 * y),
		Domain<double>(), Domain<double>(EMPTY), 100,
		{
			Boundary<double>(0.0, 0.0, 0),
			Boundary<double>(0.0, 2.0, 1)
		});
}

Fitness<double> ode6() {
	return Fitness<double>(
		ODE(ddy, -dy / 5 - y - exp(-x / 5) * cos(x) / 5),
		Domain<double>(0, 2), Domain<double>(EMPTY), 100,
		{
			Boundary<double>(0.0, 0.0, 0),
			Boundary<double>(0.0, 1.0, 1)
		});
}

Fitness<double> ode7() {
	return Fitness<double>(
		ODE(ddy, -100 * y),
		Domain<double>(), Domain<double>(EMPTY), 100,
		{
			Boundary<double>(0.0, 0.0, 0),
			Boundary<double>(1.0, sin(10.0), 0)
		});
}

Fitness<double> ode8() {
	return Fitness<double>(
		ODE(0, x * ddy + (1 - x) * dy + y),
		Domain<double>(0), Domain<double>(EMPTY), 100,
		{
			Boundary<double>(0.0, 1.0, 0),
			Boundary<double>(1.0, 0.0, 0)
		});
}

Fitness<double> ode9() {
	return Fitness<double>(
		ODE(ddy, -dy / 5 - y - exp(-x / 5) * cos(x) / 5),
		Domain<double>(0), Domain<double>(EMPTY), 100,
		{
			Boundary<double>(0.0, 0.0, 0),
			Boundary<double>(1.0, sin(1.0) / exp(0.2), 0)
		});
}

Fitness<double> getExampleODE(int n) {
	switch (n) {
	case 2: return ode2();
	case 3: return ode3();
	case 4: return ode4();
	case 5: return ode5();
	case 6: return ode6();
	case 7: return ode7();
	case 8: return ode8();
	case 9: return ode9();
	default: return ode1();
	}
}

#undef ODE
