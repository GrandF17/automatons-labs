#include <algorithm>
#include <atomic>   // to use enable_if and is_arithmetic
#include <cstddef>  // to use enable_if and is_arithmetic
#include <iostream>
#include <string>
#include <type_traits>  // to use enable_if and is_arithmetic
#include <vector>

#ifndef BIGNUMBER_LIB
#define BIGNUMBER_LIB

using namespace std;

class BigNumber {
    // === =============== ===
    // === private section ===
    // === =============== ===

   private:
    // =========
    // variables
    bool isNegative;
    vector<unsigned long long int> data;

    // =========
    // functions
    /**
     * if returns -1 --> a < b
     * if returns 0 --> a = b
     * if returns 1 --> a > b
     */
    int compare(BigNumber a, BigNumber b) {
        // -a > b
        if (a.isNegative && !b.isNegative) return -1;
        // a > -b
        if (!a.isNegative && b.isNegative) return 1;

        // a < 0 && b < 0
        if (a.isNegative && b.isNegative) {
            if (a.data.size() < b.data.size())
                return -1;  // means a is closer to zero than b
            if (a.data.size() > b.data.size())
                return 1;  // means that a is further from zero than b

            // only option when there can be equality
            if (a.data.size() == b.data.size()) {
                for (size_t i = 0; i < a.data.size(); i++) {
                    if (a.data[i] < b.data[i])
                        return 1;  // means a is closer to zero than b
                    if (a.data[i] > b.data[i])
                        return -1;  // means that a is further from zero than b
                }
                return 0;  // numbers are equal
            }

        }
        // a < 0 && b < 0
        else {
            if (a.data.size() < b.data.size())
                return 1;  // means a is closer to zero than b
            if (a.data.size() > b.data.size())
                return -1;  // means that a is further from zero than b

            // only option when there can be equality
            if (a.data.size() == b.data.size()) {
                for (size_t i = 0; i < a.data.size(); i++) {
                    if (a.data[i] < b.data[i])
                        return -1;  // means a is closer to zero than b
                    if (a.data[i] > b.data[i])
                        return 1;  // means that a is further from zero than b
                }
                return 0;  // numbers are equal
            }
        }
    }

    int compareByMod(BigNumber a, BigNumber b) {
        a.isNegative = false;
        b.isNegative = false;
        return compare(a, b);
    }

    // ================
    // compare methods:
    /**
     * true if num = second
     */
    bool eq(BigNumber second) { return compare(*this, second) == 0; }

    /**
     * true if num <= second
     */
    bool leq(BigNumber second) {
        int res = compare(*this, second);
        return res == 0 || res == -1;
    }

    /**
     * true if num >= second
     */
    bool geq(BigNumber second) {
        int res = compare(*this, second);
        return res == 0 || res == 1;
    }

    /**
     * true if num < second
     */
    bool le(BigNumber second) { return compare(*this, second) == -1; }

    /**
     * true if num > second
     */
    bool ge(BigNumber second) { return compare(*this, second) == 1; }

    // ===== ========
    // basic methods:
    BigNumber add(BigNumber b) {
        // if [-a + -b] => [-(a + b)]

        // if [a + -b] => [a - b]

        // if [-a + b] => [b - a]

        // if numbers are positive:
    }

    BigNumber sub(BigNumber b) {
        // if [-a - -b] => [b - a]
        // if [a - -b] => [a + b]
        // if [-a - b] => [-(a + b)]
        // we got the same logic in "add"

        // if a < b

        // if numbers are positive and a >= b:
    }

    BigNumber mul(BigNumber b) {
        // if [-a * -b] => [a * b]

        // if [-a * b] => [-(a * b)]

        // if [a * -b] => [-(a * b)]
    }

    BigNumber div(BigNumber divisor) {
        if (divisor.eq(BigNumber(0))) throw "Division by zero!";
        if (*this == BigNumber(0)) return BigNumber(0);
        if (compareByMod(*this, divisor) == -1) return BigNumber(0);

        // division logic
    }

    // === ============== ===
    // === public section ===
    // === ============== ===

   public:
    /**
     * default constructor
     */
    BigNumber() : isNegative(false) { data.push_back(0); }

    /**
     * constructor for generic type T, enabled for arithmetic types
     */
    template <typename T>
    BigNumber(const T& digit,
              typename enable_if<is_integral<T>::value>::type* = 0) {
        if (digit < 0) {
            isNegative = true;
            data.push_back(static_cast<unsigned long long int>(-digit));
        } else {
            isNegative = false;
            data.push_back(static_cast<unsigned long long int>(digit));
        }
    }

    /**
     * basic operands overload for code simplicity
     */
    template <typename T>
    typename enable_if<is_arithmetic<T>::value, BigNumber&>::type operator=(
        const T& digit) {
        num = to_string(digit);
        return *this;
    }

    BigNumber operator+(const BigNumber& other) const {
        return BigNumber(*this).add(other);
    }

    BigNumber& operator+=(const BigNumber& other) {
        *this = *this + other;
        return *this;
    }

    /**
     * prefix operator overload
     */
    BigNumber& operator++() {
        *this += BigNumber(1);
        // return current value (watch order of operations for prefix)
        return *this;
    }

    /**
     * postfix operator overload
     */
    BigNumber operator++(int) {
        // making a copy of current number
        BigNumber temp(*this);
        ++*this;
        // return current copy (watch order of operations for postfix)
        return temp;
    }

    BigNumber operator-(const BigNumber& other) const {
        return BigNumber(*this).sub(other);
    }

    BigNumber& operator-=(const BigNumber& other) {
        *this = *this - other;
        return *this;
    }

    /**
     * prefix operator overload
     */
    BigNumber& operator--() {
        *this -= BigNumber(1);
        // return current value (watch order of operations for prefix)
        return *this;
    }

    /**
     * postfix operator overload
     */
    BigNumber operator--(int) {
        // making a copy of current number
        BigNumber temp(*this);
        --*this;
        // return current copy (watch order of operations for postfix)
        return temp;
    }

    BigNumber operator*(const BigNumber& other) const {
        return BigNumber(*this).mul(other);
    }

    BigNumber& operator*=(const BigNumber& other) {
        *this = *this * other;
        return *this;
    }

    BigNumber operator/(const BigNumber& other) const {
        return BigNumber(*this).div(other);
    }

    BigNumber& operator/=(const BigNumber& other) {
        *this = *this / other;
        return *this;
    }

    BigNumber operator%(const BigNumber& other) const {
        return BigNumber(*this).mod(other);
    }

    BigNumber& operator%=(const BigNumber& other) {
        *this = *this % other;
        return *this;
    }

    // ========== ========= ========
    // comparison operators overload
    bool operator==(const BigNumber& other) const {
        return BigNumber(*this).eq(other);
    }

    bool operator!=(const BigNumber& other) const {
        return !BigNumber(*this).eq(other);
    }

    bool operator<=(const BigNumber& other) const {
        return BigNumber(*this).leq(other);
    }

    bool operator<(const BigNumber& other) const {
        return BigNumber(*this).le(other);
    }

    bool operator>=(const BigNumber& other) const {
        return BigNumber(*this).geq(other);
    }

    bool operator>(const BigNumber& other) const {
        return BigNumber(*this).ge(other);
    }

    /**
     * other public functions
     */
    BigNumber pow(BigNumber degree) {
        if (BigNumber(*this).eq(BigNumber(0))) return BigNumber(0);
        if (degree.eq(BigNumber(0))) return BigNumber(1);
        return BigNumber(*this).mul(
            BigNumber(*this).pow(degree.sub(BigNumber(1))));
    }

    /**
     * a narrowly focused function that accepts only a positive modulus and if
     * num is negative, then returns a positive number NOT GREATER than modulus
     * Examples:
     * 123 mod -2 --> ERROR
     * -5 mod 2 --> 1
     * 20 mod 3 --> 2
     */
    BigNumber mod(BigNumber modulus) {
        if (modulus.isNegative)
            throw invalid_argument("Invalid argument: modulus is negative");
        /**
         * -n mod m = (-n * -1 + (n - m)) mod m
         */
        if (BigNumber(*this).isNegative) {
            BigNumber revretedNum(BigNumber(*this).mul(BigNumber(-1)));
            BigNumber subResult(BigNumber(*this).sub(modulus));

            return revretedNum.add(subResult.mod(modulus));
        }

        BigNumber factorOne = BigNumber(*this).div(modulus);
        return BigNumber(*this).sub(factorOne.mul(modulus));
    }

    // =======
    // friends

    /**
     * work book electronic extract
     */
    friend ostream& operator<<(ostream& os, const BigNumber& num) {
        for (auto it = num.data.rbegin(); it != num.data.rend(); ++it)
            os << *it;
        return os;
    }
};

#endif