#ifndef PROGRAMMING_LANGUAGES_AND_METHODS_BIG_INTEGER_H
#define PROGRAMMING_LANGUAGES_AND_METHODS_BIG_INTEGER_H

// #include <corecrt.h>
#include <string>
#include <vector>
#include <optional>
#include <iostream>

class big_integer final
{

public:
    enum class multiplication_rule
    {
        trivial,
        Karatsuba,
        SchonhageStrassen
    };

private:

    class multiplication

    {

    public:

        virtual ~multiplication() noexcept = default;

    public:

        virtual big_integer &multiply(
                big_integer &first_multiplier,
                big_integer const &second_multiplier) const = 0;

    };

    class trivial_multiplication final:
            public multiplication
    {

    public:

        big_integer &multiply(
                big_integer &first_multiplier,
                big_integer const &second_multiplier) const override;

    };

    class Karatsuba_multiplication final:
            public multiplication
    {

    public:

        big_integer &multiply(
                big_integer &first_multiplier,
                big_integer const &second_multiplier) const override;

    };

    class Schonhage_Strassen_multiplication final:
            public multiplication
    {

    public:

        big_integer &multiply(
                big_integer &first_multiplier,
                big_integer const &second_multiplier) const override;

    };

public:

    enum class division_rule
    {
        trivial,
        Newton,
        BurnikelZiegler
    };

private:

    class division
    {

    public:

        virtual ~division() noexcept = default;

    public:

        virtual big_integer &divide(
                big_integer &dividend,
                big_integer const &divisor,
                big_integer::multiplication_rule multiplication_rule) const = 0;

        virtual big_integer &modulo(
                big_integer &dividend,
                big_integer const &divisor,
                big_integer::multiplication_rule multiplication_rule) const = 0;

    };

    class trivial_division final:
            public division
    {

    public:

        big_integer &divide(
                big_integer &dividend,
                big_integer const &divisor,
                big_integer::multiplication_rule multiplication_rule) const override;

        big_integer &modulo(
                big_integer &dividend,
                big_integer const &divisor,
                big_integer::multiplication_rule multiplication_rule) const override;

    private:

        std::pair<std::optional<big_integer>, big_integer> divide_with_remainder(
                big_integer const &dividend,
                big_integer const &divisor,
                bool eval_quotient,
                big_integer::multiplication_rule multiplication_rule) const;


    };

    class Newton_division final:
            public division
    {

    public:

        big_integer &divide(
                big_integer &dividend,
                big_integer const &divisor,
                big_integer::multiplication_rule multiplication_rule) const override;

        big_integer &modulo(
                big_integer &dividend,
                big_integer const &divisor,
                big_integer::multiplication_rule multiplication_rule) const override;

    };

    class Burnikel_Ziegler_division final:
            public division
    {

    public:

        big_integer &divide(
                big_integer &dividend,
                big_integer const &divisor,
                big_integer::multiplication_rule multiplication_rule) const override;

        big_integer &modulo(
                big_integer &dividend,
                big_integer const &divisor,
                big_integer::multiplication_rule multiplication_rule) const override;

    };

private:

    void move_from(
            big_integer &&other);

    void clear();

    void copy_from(
            big_integer const &other);

    void initialize_from(
            int const *digits,
            size_t digits_count);

    void initialize_from(
            std::vector<int> const &digits,
            size_t digits_count);

    void initialize_from(
            std::string const &value,
            size_t base);

    void initialize_from(
            const int num);

private:

    static void print_byte(
            std::ostream &stream,
            unsigned char byte_value);

    static void dump_int_value(
            std::ostream &stream,
            int value);

public:

    void dump_value(
            std::ostream &stream) const
    {
        for (auto i = 0; i < get_digits_count(); ++i)
        {
            auto digit = get_digit(i);
            dump_int_value(stream, *reinterpret_cast<int *>(&digit));
            stream << ' ';
        }
    }

private:

    int _oldest_digit;
    unsigned int *_other_digits;

private:

    big_integer &change_sign();

public:

    inline int get_digits_count() const noexcept;

    inline int sign() const noexcept;

    inline bool is_equal_to_zero() const noexcept;

    inline bool is_equal_to_one() const noexcept;

    inline unsigned int get_digit(
            int position) const noexcept;

public:

    big_integer(
            int const *digits,
            size_t digits_count);

    big_integer(
            std::vector<int> const &digits);

    big_integer(
            std::string const &value,
            size_t base = 10);

    big_integer(
            const int num);

public:

    big_integer(
            big_integer const &other);

    big_integer &operator=(
            big_integer const &other);

    big_integer(
            big_integer &&other) noexcept;

    big_integer &operator=(
            big_integer &&other) noexcept;

    ~big_integer();

public:

    big_integer &operator+=(
            big_integer const &other);

    big_integer operator+(
            big_integer const &other) const;

    big_integer &operator-=(
            big_integer const &other);

    big_integer operator-(
            big_integer const &other) const;

    big_integer operator-() const;

    big_integer &operator*=(
            big_integer const &other);

    big_integer operator*(
            big_integer const &other) const;

    big_integer &operator/=(
            big_integer const &other);

    big_integer operator/(
            big_integer const &other) const;

    big_integer &operator%=(
            big_integer const &other);

    big_integer operator%(
            big_integer const &other) const;

    bool operator==(
            big_integer const &other) const;

    bool operator!=(
            big_integer const &other) const;

    bool operator<(
            big_integer const &other) const;

    bool operator<=(
            big_integer const &other) const;

    bool operator>(
            big_integer const &other) const;

    bool operator>=(
            big_integer const &other) const;

    big_integer operator~() const;

public:

    big_integer &operator&=(
            big_integer const &other);

    big_integer operator&(
            big_integer const &other) const;

    big_integer &operator|=(
            big_integer const &other);

    big_integer operator|(
            big_integer const &other) const;

    big_integer &operator^=(
            big_integer const &other);

    big_integer operator^(
            big_integer const &other) const;

    big_integer &operator<<=(
            size_t shift_value);

    big_integer operator<<(
            size_t shift_value) const;

    big_integer &operator>>=(
            size_t shift_value);

    big_integer operator>>(
            size_t shift_value) const;

    friend std::ostream &operator<<(
            std::ostream &stream,
            big_integer const &value);

    friend std::istream &operator>>(
            std::istream &stream,
            big_integer &value);

private:
    unsigned int char_to_int(char c);

private:
    static void remove_additional_zeroes(std::vector<int> &digits);

private:
    std::string to_string() const ;
    std::string bigint_to_string(big_integer const value) const;

private:
    std::pair<std::optional<big_integer>, big_integer> divide_with_remainder(
            big_integer const &dividend,
            big_integer const &divisor,
            bool eval_quotient) const ;

public:

    static big_integer &multiply(
            big_integer &first_multiplier,
            big_integer const &second_multiplier,
            big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer multiply(
            big_integer const &first_multiplier,
            big_integer const &second_multiplier,
            big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer &divide(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::division_rule division_rule = big_integer::division_rule::trivial,
            big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer divide(
            big_integer const &dividend,
            big_integer const &divisor,
            big_integer::division_rule division_rule = big_integer::division_rule::trivial,
            big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer &modulo(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::division_rule division_rule = big_integer::division_rule::trivial,
            big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer modulo(
            big_integer const &dividend,
            big_integer const &divisor,
            big_integer::division_rule division_rule = big_integer::division_rule::trivial,
            big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

#pragma endregion custom multiplication and division

};

#endif //PROGRAMMING_LANGUAGES_AND_METHODS_BIG_INTEGER_H