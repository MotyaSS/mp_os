#ifndef MP_OS_FRACTION_H
#define MP_OS_FRACTION_H

#include <big_integer.h>

class fraction final
{

private:
    static big_integer gcd (big_integer a, big_integer b);
    static big_integer lcm (big_integer a, big_integer b);
    fraction& simplify();
    static void make_same_denominator(fraction &a, fraction &b);
    static int is_valid_eps(fraction const &eps);

private:
    static fraction get_pi();

public:
    int sign() const;
    fraction& change_sign();
    bool is_equal_to_zero() const;

private:

    big_integer _numerator;
    big_integer _denominator;

public:

    fraction(
            big_integer &&numerator,
            big_integer &&denominator);

    fraction(
            big_integer &numerator,
            big_integer &denominator);

    fraction(
            big_integer &numerator);

    fraction();

    fraction(
            int numerator,
            int denominator);

public:

    ~fraction() noexcept;

    fraction(
            fraction const &other);

    fraction &operator=(
            fraction const &other);

    fraction(
            fraction &&other) noexcept;

    fraction &operator=(
            fraction &&other) noexcept;

public:

    fraction &operator+=(
            fraction const &other);

    fraction operator+(
            fraction const &other) const;

    fraction &operator-=(
            fraction const &other);

    fraction operator-(
            fraction const &other) const;

    fraction operator-() const;

    fraction &operator*=(
            fraction const &other);

    fraction operator*(
            fraction const &other) const;

    fraction &operator/=(
            fraction const &other);

    fraction operator/(
            fraction const &other) const;

public:

    bool operator==(
            fraction const &other) const;

    bool operator!=(
            fraction const &other) const;

public:

    bool operator>=(
            fraction const &other) const;

    bool operator>(
            fraction const &other) const;

    bool operator<=(
            fraction const &other) const;

    bool operator<(
            fraction const &other) const;

public:

    friend std::ostream &operator<<(
            std::ostream &stream,
            fraction const &obj);

    friend std::istream &operator>>(
            std::istream &stream,
            fraction &obj);

public:

    fraction abs() const;

    fraction sin(
            fraction const &epsilon) const;

    fraction cos(
            fraction const &epsilon) const;

    fraction tg(
            fraction const &epsilon) const;

    fraction ctg(
            fraction const &epsilon) const;

    fraction sec(
            fraction const &epsilon) const;

    fraction cosec(
            fraction const &epsilon) const;

    fraction arcsin(
            fraction const &epsilon) const;

    fraction arccos(
            fraction const &epsilon) const;

    fraction arctg(
            fraction const &epsilon) const;

    fraction arcctg(
            fraction const &epsilon) const;

    fraction arcsec(
            fraction const &epsilon) const;

    fraction arccosec(
            fraction const &epsilon) const;

public:

    fraction pow(
            size_t degree) const;

public:

    fraction root(
            size_t degree,
            fraction const &epsilon) const;

public:

    fraction log2(
            fraction const &epsilon) const;

    fraction ln(
            fraction const &epsilon) const;

    fraction lg(
            fraction const &epsilon) const;

};

#endif //MP_OS_FRACTION_H