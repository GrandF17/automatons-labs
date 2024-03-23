#include "../libraries/BigNumber.h"

#include <cassert>

BigNumber zero("0");
BigNumber two("2");
BigNumber hundred("100");
BigNumber hundredNeg("-100");

BigNumber num1("333");
BigNumber num2("256");
BigNumber num3("-10000");

void test_comparison() {
    // equality tests
    assert(zero.eq(zero) == true);
    assert(zero.eq(two) == false);
    assert(hundred.eq(hundred) == true);
    assert(hundred.eq(hundredNeg) == false);
    assert(num2.eq(two) == false);
    assert(num3.eq(hundredNeg) == false);

    // a less or equal b tests
    assert(zero.leq(zero) == true);
    assert(zero.leq(hundred) == true);
    assert(zero.leq(hundredNeg) == false);
    assert(num3.leq(hundredNeg) == true);
    assert(num2.leq(num1) == true);
    assert(num3.leq(hundred) == true);

    // a less than b tests
    assert(zero.le(hundred) == true);
    assert(zero.le(hundredNeg) == false);
    assert(num3.le(hundredNeg) == true);
    assert(num2.le(num1) == true);
    assert(num3.le(hundred) == true);

    // a greater or equal b tests
    assert(zero.geq(zero) == true);
    assert(hundred.geq(hundredNeg) == true);
    assert(num2.geq(num2) == true);
    assert(num3.geq(two) == false);
    assert(num3.geq(hundredNeg) == false);

    // a greater than b tests
    assert(hundred.ge(hundredNeg) == true);
    assert(num3.ge(two) == false);
    assert(num3.ge(hundredNeg) == false);
}

void test_add() {
    assert(zero.add(zero).eq(zero));                    // 0 + 0
    assert(hundred.add(hundred).eq(BigNumber("200")));  // a + b
    assert(hundredNeg.add(hundred).eq(zero));           // -a + b
    assert(hundred.add(num3).eq(BigNumber("-9900")));   // a + -b
    assert(num3.add(num3).eq(BigNumber("-20000")));     // -a + -b
}

void test_subtract() {
    assert(zero.sub(zero).eq(zero));                        // 0 - 0
    assert(hundred.sub(hundred).eq(zero));                  // a - a == 0
    assert(hundred.sub(two).eq(BigNumber("98")));           // a - b
    assert(hundredNeg.sub(hundred).eq(BigNumber("-200")));  // -a - b
    assert(num3.sub(num2).eq(BigNumber("-10256")));         // -a - -b
    assert(hundred.sub(num3).eq(BigNumber("10100")));       // a - -b
}

void test_multiplication() {
    assert(zero.mul(two).eq(zero));      // 0 * 0
    assert(hundred.mul(zero).eq(zero));  // a * 0
    assert(zero.mul(hundred).eq(zero));  // 0 * a

    assert(hundred.mul(hundred).eq(BigNumber("10000")));        // a * b
    assert(num1.mul(num1).eq(BigNumber("110889")));             // a * b
    assert(hundredNeg.mul(hundred).eq(BigNumber("-10000")));    // -a * b
    assert(num2.mul(num3).eq(BigNumber("-2560000")));           // a * -b
    assert(hundredNeg.mul(hundredNeg).eq(BigNumber("10000")));  // -a * -b
}

void test_division() {
    try {  // division by zero
        two.div(zero);
        assert(1 == 0);  // menace that test failed
    } catch (const char* error_message) {
        assert(1 == 1);  // menace that test passed
    }

    assert(two.div(two).eq(BigNumber("1")));    // 2 / 2
    assert(num1.div(num2).eq(BigNumber("1")));  // a / b
    assert(num3.div(hundred).eq(hundredNeg));   // -a / b
    assert(num3.div(hundredNeg).eq(hundred));   // -a / -b

    assert(hundred.div(num3).eq(zero));                   // a < b => a / b == 0
    assert(hundred.div(hundredNeg).eq(BigNumber("-1")));  // a / -b

    assert(num2.div(two).eq(BigNumber("128")));  // 256 / 2 == 128
    assert(num1.div(two).eq(BigNumber("166")));  // 333 / 2 == 166
}

int main() {
    // simple tests: ADDITION & SUBTRACTION
    test_comparison();
    test_add();

    // complex tests: MULTIPLICATION & DIVISION
    test_multiplication();
    test_division();

    return 0;
}