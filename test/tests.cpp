#include "../src/poly_parser.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <cmath>

TEST(PolyBasic, SingleMonom) {
    Polynomus p;
    p.addMonom(Monomus(2, 1, 0, 3.0));

    std::stringstream ss;
    ss << p;

    EXPECT_NE(ss.str().find('3'), std::string::npos);
}

TEST(PolyBasic, MergeSameDegrees) {
    Polynomus p;
    p.addMonom(Monomus(1, 0, 0, 2.0));
    p.addMonom(Monomus(1, 0, 0, -2.0));

    EXPECT_TRUE(p.isZero());
}

TEST(PolyOps, AdditionDifferentTerms) {
    Polynomus a;
    a.addMonom(Monomus(1,0,0,1.0));

    Polynomus b;
    b.addMonom(Monomus(0,1,0,2.0));

    Polynomus c;
    c.addMonom(Monomus(0,0,1,3.0));

    auto res = a + b + c;

    Polynomus expected;
    expected.addMonom(Monomus(0,0,1,3.0));
    expected.addMonom(Monomus(0,1,0,2.0));
    expected.addMonom(Monomus(1,0,0,1.0));


    EXPECT_EQ(res, expected);
}

TEST(PolyOps, SubtractionToZero) {
    Polynomus a;
    a.addMonom(Monomus(2,0,0,5.0));

    auto res = a - a;

    EXPECT_TRUE(res.isZero());
}

TEST(PolyOps, SimpleMultiplication) {
    Polynomus a;
    a.addMonom(Monomus(1,0,0,2.0));

    Polynomus b;
    b.addMonom(Monomus(0,1,0,3.0));

    auto res = a * b;

    Polynomus expected;
    expected.addMonom(Monomus(1,1,0,6.0));

    EXPECT_EQ(res, expected);
}

TEST(PolyEdge, MultiplyByZero) {
    Polynomus a;
    a.addMonom(Monomus(1,1,1,7.0));

    auto res = a * 0.0;

    EXPECT_TRUE(res.isZero());
}

TEST(PolyMath, DistributiveLaw) {
    Polynomus x;
    x.addMonom(Monomus(1,0,0,1.0));

    Polynomus y;
    y.addMonom(Monomus(0,1,0,1.0));

    Polynomus z;
    z.addMonom(Monomus(0,0,1,1.0));

    auto left = x * (y + z);
    auto right = (x * y) + (x * z);

    EXPECT_EQ(left, right);
}

TEST(PolyMath, SquareSum) {
    Polynomus x;
    x.addMonom(Monomus(1,0,0,1.0));

    Polynomus y;
    y.addMonom(Monomus(0,1,0,1.0));

    auto res = (x + y) * (x + y);

    Polynomus expected;
    expected.addMonom(Monomus(0,2,0,1.0));
    expected.addMonom(Monomus(1,1,0,2.0));
    expected.addMonom(Monomus(2,0,0,1.0));

    EXPECT_EQ(res, expected);
}

TEST(PolyStructure, OrderMaintained) {
    Polynomus p;

    p.addMonom(Monomus(0,0,0,1.0));
    p.addMonom(Monomus(3,0,0,1.0));
    p.addMonom(Monomus(1,0,0,1.0));

    std::stringstream ss;
    ss << p;

    std::string out = ss.str();

    EXPECT_TRUE(out.find('x') < out.find("x^3"));
}

TEST(PolyMath, Commutativity) {
    Polynomus a;
    a.addMonom(Monomus(2,0,1,3.0));

    Polynomus b;
    b.addMonom(Monomus(1,1,0,4.0));

    EXPECT_EQ(a + b, b + a);
    EXPECT_EQ(a * b, b * a);
}

TEST(PolyEval, ValueCheck) {
    Polynomus p;
    p.addMonom(Monomus(1,0,0,2.0));
    p.addMonom(Monomus(0,0,0,3.0));

    double val = p.evaluate(2.0, 0, 0);

    EXPECT_NEAR(val, 7.0, 1e-9);
}

TEST(PolyEdge, OverflowCheck) {
    Polynomus a;
    a.addMonom(Monomus(1000,0,0,1.0));

    Polynomus b;
    b.addMonom(Monomus(30,0,0,1.0));

    EXPECT_THROW(a * b, std::overflow_error);
}

// parser test

TEST(TranslatorBasic, SingleTerm) {
    auto p = poly_parser::parse("x");

    Polynomus expected;
    expected.addMonom(Monomus(1,0,0,1.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorBasic, ConstantOnly) {
    auto p = poly_parser::parse("5");

    Polynomus expected;
    expected.addMonom(Monomus(0,0,0,5.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorBasic, SimpleSum) {
    auto p = poly_parser::parse("x + y");

    Polynomus expected;
    expected.addMonom(Monomus(0,1,0,1.0));
    expected.addMonom(Monomus(1,0,0,1.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorLogic, PriorityCheck) {
    auto p = poly_parser::parse("x+y*z");

    Polynomus expected;
    expected.addMonom(Monomus(1,0,0,1.0));
    expected.addMonom(Monomus(0,1,1,1.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorLogic, Parentheses) {
    auto p = poly_parser::parse("(x+y) * z");

    Polynomus expected;
    expected.addMonom(Monomus(0,1,1,1.0));
    expected.addMonom(Monomus(1,0,1,1.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorLogic, ChainOperations) {
    auto p = poly_parser::parse("x*y+ z * x");

    Polynomus expected;
    expected.addMonom(Monomus(1,0,1,1.0));
    expected.addMonom(Monomus(1,1,0,1.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorAdvanced, Powers) {
    auto p = poly_parser::parse("x^2* y");

    Polynomus expected;
    expected.addMonom(Monomus(2,1,0,1.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorAdvanced, Coefficients) {
    auto p = poly_parser::parse("3 * x");

    Polynomus expected;
    expected.addMonom(Monomus(1,0,0,3.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorAdvanced, ComplexExpression) {
    auto p = poly_parser::parse("(x+2)*(y+3)");

    Polynomus expected;
    expected.addMonom(Monomus(0,0,0,6.0));
    expected.addMonom(Monomus(0,1,0,2.0));
    expected.addMonom(Monomus(1,0,0,3.0));
    expected.addMonom(Monomus(1,1,0,1.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorRobustness, Spaces) {
    auto p = poly_parser::parse("  x   +   y  ");

    Polynomus expected;
    expected.addMonom(Monomus(0,1,0,1.0));
    expected.addMonom(Monomus(1,0,0,1.0));

    EXPECT_EQ(p, expected);
}

TEST(TranslatorErrors, BadParentheses) {
    EXPECT_THROW(
        poly_parser::parse("(x+y"),
        std::runtime_error
    );
}

TEST(TranslatorErrors, InvalidChar) {
    EXPECT_THROW(
        poly_parser::parse("x + @"),
        std::runtime_error
    );
}

TEST(TranslatorErrors, BadPower) {
    EXPECT_THROW(
        poly_parser::parse("x^"),
        std::runtime_error
    );
}

TEST(TranslatorErrors, EmptyInput) {
    EXPECT_THROW(
        poly_parser::parse(""),
        std::runtime_error
    );
}

TEST(TranslatorMath, DistributivityViaParser) {
    auto left = poly_parser::parse("x*(y+z)");
    auto right = poly_parser::parse("x*y + x*z");

    EXPECT_EQ(left, right);
}