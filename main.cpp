
#include "VarX.h"
#include "Addition.h"
#include "Multiplication.h"
#include "Division.h"
#include "Subtraction.h"
#include "Trig.h"
#include "Exponential.h"
#include "Logarithm.h"

int main() {

	// x^2
	ExpressionPtr<float> xx = MultiplicationPtrf(VarXPtrf, VarXPtrf); // x * x
	// x^3
	ExpressionPtr<float> xxx = MultiplicationPtrf(xx, VarXPtrf); // x^2 * x

	// 4x^3 + 3x^2 - 6x + 7
	ExpressionPtr<float> polynomial = AdditionPtrf(
		AdditionPtrf(
			MultiplicationPtrf(ConstantPtrf(4), xxx), // 4x^3
			// +
			MultiplicationPtrf(ConstantPtrf(3), xx) // 3x^2
		),
			// +
		AdditionPtrf(
			MultiplicationPtrf(ConstantPtrf(-6), VarXPtrf), // -6x
			// +
			ConstantPtrf(7) // 7
		)
	);

	// log( 4x^3 + 3x^2 - 6x + 7 )
	ExpressionPtr<float> logPolynomial = LogarithmPtrf(polynomial);

	// cos( log( 4x^3 + 3x^2 - 6x + 7 ) )
	ExpressionPtr<float> cosLogPolynomial = CosinePtrf(polynomial);

	// exp( cos( log( 4x^3 + 3x^2 - 6x + 7 ) ) )
	ExpressionPtr<float> expCosLogPolynomial = ExponentialPtrf(cosLogPolynomial);

	ExpressionPtr<float> expr = expCosLogPolynomial;
	printf("\n f(x) = %s\n", expr->toString().c_str());
	printf("\n f(0) = %f\n", expr->evaluate(0));
	printf("\n f(1) = %f\n", expr->evaluate(1));
	printf("\n f'(x) = %s\n", expr->derivative()->simplify()->toString().c_str());
	printf("\n f''(x) = %s\n", expr->derivative()->derivative()->simplify()->toString().c_str());
	printf("\n f'''(x) = %s\n", expr->derivative()->derivative()->derivative()->simplify()->toString().c_str());

	return 0;
}
