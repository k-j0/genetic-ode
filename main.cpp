
#include "VarX.h"
#include "Addition.h"
#include "Multiplication.h"
#include "Division.h"
#include "Subtraction.h"
#include "Trig.h"
#include "Exponential.h"
#include "Logarithm.h"
#include "GrammarDecoder.h"

int main() {

	// Set up grammar
	std::vector<GrammaticalElement_base<float>*> operations = {
		new GrammaticalElement2Args<Addition<float>, float>(),
		new GrammaticalElement2Args<Subtraction<float>, float>(),
		new GrammaticalElement2Args<Multiplication<float>, float>(),
		new GrammaticalElement2Args<Division<float>, float>()
	};
	std::vector<GrammaticalElement_base<float>*> functions = {
		new GrammaticalElement1Arg<Sine<float>, float>(),
		new GrammaticalElement1Arg<Cosine<float>, float>(),
		new GrammaticalElement1Arg<Exponential<float>, float>(),
		new GrammaticalElement1Arg<Logarithm<float>, float>()
	};
	GrammarDecoder<float> decoder(2, operations, functions);

	// Decode test sequence
	std::vector<unsigned int> sequence = { 1, 3, 0, 2, 3, 0, 2, 2, 2, 1 }; // test sequence - expecting log(2 x cosx)
	ExpressionPtr<float> expression = decoder.decode(sequence);
	if (expression == nullptr) {
		printf("\n Invalid sequence decoded from test sequence...\n");
	} else {
		printf("\n Decoded sequence: %s\n", expression->toString().c_str());
	}



	// Test computing derivatives of a test function set up via expressions

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
