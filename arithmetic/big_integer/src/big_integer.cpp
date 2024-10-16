#include <cstring>
#include <stdexcept>
#include <vector>
#include <limits>
#include <optional>
#include <not_implemented.h>

#include "big_integer.h"
#include <algorithm>
#pragma region multiplication
typedef unsigned int uint;
big_integer &big_integer::trivial_multiplication::multiply(
        big_integer &first_multiplier,
        big_integer const &second_multiplier) const
{
    if (second_multiplier.is_equal_to_zero())
    {
        return first_multiplier = second_multiplier;
    }

    if (first_multiplier.is_equal_to_zero())
    {
        return first_multiplier;
    }

    if (second_multiplier.is_equal_to_one())
    {
        return first_multiplier;
    }

    if (first_multiplier.is_equal_to_one())
    {
        return first_multiplier = second_multiplier;
    }

    if (first_multiplier.sign() == -1)
    {
        return multiply(first_multiplier.change_sign(), second_multiplier).change_sign();
    }

    if (second_multiplier.sign() == -1)
    {
        return multiply(first_multiplier, -second_multiplier).change_sign();
    }

    if (second_multiplier._oldest_digit == -1 && second_multiplier._other_digits == nullptr) {
        return (first_multiplier.change_sign());
    }

    auto first_value_digits_count = first_multiplier.get_digits_count();
    auto second_value_digits_count = second_multiplier.get_digits_count();
    auto max_digits_count = first_value_digits_count + second_value_digits_count;

    int shift = sizeof(unsigned int) << 2;
    int mask = (1 << shift) - 1;

    std::vector<unsigned int> half_digits_first(2 * first_value_digits_count, 0);
    std::vector<unsigned int> half_digits_second(2 * second_value_digits_count, 0);

    std::vector<unsigned int> half_digits_res(2 * max_digits_count + 1, 0);

    size_t pos_shift = 0;

    for (int i = 0; i < first_value_digits_count; i++) {
        half_digits_first[2 * i] = first_multiplier.get_digit(i) & mask;
        half_digits_first[2 * i+1] = first_multiplier.get_digit(i) >> shift;
    }

    for (int i = 0; i < second_value_digits_count; i++) {
        half_digits_second[2*i] = second_multiplier.get_digit(i) & mask;
        half_digits_second[2*i+1] = second_multiplier.get_digit(i) >> shift;
    }

    for (int i = 0; i < half_digits_first.size(); i++) {
        if (half_digits_first[i] == 0) {
            pos_shift++;
            continue;
        }

        size_t pos = 0;
        unsigned int operation_result = 0;

        for (int j = 0; j < half_digits_second.size(); j++) {
            operation_result += half_digits_first[i] * half_digits_second[j] + half_digits_res[pos_shift + pos];
            half_digits_res[pos_shift + pos++] = operation_result & mask;

            operation_result >>= shift;
        }

        for (; operation_result && (pos_shift + pos) < half_digits_res.size(); ++pos) {
            operation_result += half_digits_res[pos_shift + pos];
            half_digits_res[pos_shift + pos++] = operation_result & mask;

            operation_result >>= shift;
        }

        if (operation_result) {
            throw std::logic_error("too much digits probably");
        }

        ++pos_shift;
    }

    std::vector<int> result_digits(max_digits_count);

    for (size_t i = 0; i < max_digits_count; ++i)
    {
        result_digits[i] = (half_digits_res[2*i + 1] << shift) + half_digits_res[2*i];
    }

    remove_additional_zeroes(result_digits);

    first_multiplier.clear();
    first_multiplier.initialize_from(result_digits, result_digits.size());

    return first_multiplier;
}

big_integer &big_integer::Karatsuba_multiplication::multiply(
        big_integer &first_multiplier,
        big_integer const &second_multiplier) const
{
    throw not_implemented("big_integer &big_integer::Karatsuba_multiplication::multiply(big_integer &, big_integer const &)", "your code should be here...");
}

big_integer &big_integer::Schonhage_Strassen_multiplication::multiply(
        big_integer &first_multiplier,
        big_integer const &second_multiplier) const
{
    throw not_implemented("big_integer &big_integer::Schonhage_Strassen_multiplication::multiply(big_integer &, big_integer const &)", "your code should be here...");
}

#pragma endregion multiplication

#pragma region division

std::pair<std::optional<big_integer>, big_integer> big_integer::trivial_division::divide_with_remainder(
        big_integer const &dividend,
        big_integer const &divisor,
        bool eval_quotient,
        big_integer::multiplication_rule multiplication_rule) const
{
    if (divisor.is_equal_to_zero())
    {
        throw std::logic_error("attempt to divide by zero");
    }

    if (dividend.is_equal_to_zero())
    {
        return std::make_pair(std::optional(big_integer(std::vector<int> {0})), big_integer(std::vector<int> {0}));
    }

    if (divisor._oldest_digit == 1 && divisor._other_digits == nullptr)
    {
        return std::make_pair(std::optional(dividend), big_integer(std::vector<int> {0}));
    }

    if (dividend.sign() == -1)
    {
        auto [quotient, remainder] = divide_with_remainder(-dividend, divisor, eval_quotient, multiplication_rule);

        if (quotient.has_value())
        {
            return std::make_pair(std::optional(-quotient.value()), -remainder);
        }
        else
        {
            return std::make_pair(std::optional<big_integer>(), -remainder);
        }
    }

    if (divisor.sign() == -1)
    {
        auto [quotient, remainder] = divide_with_remainder(dividend, -divisor, eval_quotient, multiplication_rule);

        if (quotient.has_value())
        {
            return std::make_pair(std::optional(-quotient.value()), -remainder);
        }
        else
        {
            return std::make_pair(std::optional<big_integer>(), -remainder);
        }
    }

    auto const dividend_digits_count = dividend.get_digits_count();

    std::vector<int> result_digits(1, 0);
    big_integer minuend(std::vector<int> {0});

    for (size_t i = 0; i < dividend_digits_count; ++i)
    {
        unsigned int cur_digit = dividend.get_digit(dividend_digits_count - i - 1);

        // minuend *= (1 << (8 * sizeof(unsigned int)));
        minuend <<= 8 * sizeof(unsigned int);
        minuend += big_integer(std::vector<int>( {*reinterpret_cast<int *>(&cur_digit), 0} ));

        if (minuend < divisor)
        {
            result_digits.push_back(0);
        }
        else
        {
            unsigned int digit = 0;
            big_integer subtrahend(std::vector<int> {0});

            for (unsigned int multiplier = 1 << (8*sizeof(unsigned int) - 1); multiplier > 0; multiplier >>= 1)
            {
                int multiplier_int = *reinterpret_cast<int *>(&multiplier);
                big_integer tmp = divisor * big_integer(std::vector<int>( {multiplier_int, 0} ));

                if (minuend >= subtrahend + tmp)
                {
                    subtrahend += tmp;
                    digit += multiplier;
                }
            }

            minuend -= subtrahend;

            if (eval_quotient)
            {
                result_digits.push_back(digit);
            }
        }
    }

    if (eval_quotient)
    {
        std::reverse(result_digits.begin(), result_digits.end());

        return std::make_pair(std::optional(big_integer(result_digits)), minuend);
    }

    return std::make_pair(std::optional<big_integer>(), minuend);
}


big_integer &big_integer::trivial_division::divide(
        big_integer &dividend,
        big_integer const &divisor,
        big_integer::multiplication_rule multiplication_rule) const
{
    return dividend = divide_with_remainder(dividend, divisor, true, multiplication_rule).first.value();
}

big_integer &big_integer::trivial_division::modulo(
        big_integer &dividend,
        big_integer const &divisor,
        big_integer::multiplication_rule multiplication_rule) const
{
    return dividend = divide_with_remainder(dividend, divisor, true, multiplication_rule).second;
}

big_integer &big_integer::Newton_division::divide(
        big_integer &dividend,
        big_integer const &divisor,
        big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Newton_division::divide(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

big_integer &big_integer::Newton_division::modulo(
        big_integer &dividend,
        big_integer const &divisor,
        big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Newton_division::modulo(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

big_integer &big_integer::Burnikel_Ziegler_division::divide(
        big_integer &dividend,
        big_integer const &divisor,
        big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Burnikel_Ziegler_division::divide(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

big_integer &big_integer::Burnikel_Ziegler_division::modulo(
        big_integer &dividend,
        big_integer const &divisor,
        big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Burnikel_Ziegler_division::modulo(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

#pragma endregion division

void big_integer::move_from(
        big_integer &&other)
{
    _oldest_digit = other._oldest_digit;
    _other_digits = other._other_digits;
    // _allocator = other._allocator;

    other._oldest_digit = 0;
    other._other_digits = nullptr;
    // other._allocator = nullptr;
}

void big_integer::clear()
{
    _oldest_digit = 0;
    delete[] _other_digits;
    _other_digits = nullptr;
}

void big_integer::copy_from(
        big_integer const &other)
{
    _oldest_digit = other._oldest_digit;
    _other_digits = nullptr;
    if (other._other_digits == nullptr)
    {
        return;
    }

    _other_digits = new unsigned int[*other._other_digits];
    std::memcpy(_other_digits, other._other_digits, sizeof(unsigned int) * (*other._other_digits));
}

void big_integer::initialize_from(
        int const *digits,
        size_t digits_count)
{
    if (digits == nullptr)
    {
        throw std::logic_error("pointer to digits array must not be nullptr");
    }

    if (digits_count == 0)
    {
        throw std::logic_error("digits array length must  be GT 0");
    }

    _oldest_digit = digits[digits_count - 1];
    _other_digits = (digits_count == 1
                     ? nullptr
                     : new unsigned int[digits_count]);

    if (_other_digits == nullptr)
    {
        return;
    }

    *_other_digits = (unsigned int)digits_count;

    std::memcpy(_other_digits + 1, digits, sizeof(unsigned int) * (digits_count - 1));
}

void big_integer::initialize_from(
        std::vector<int> const &digits,
        size_t digits_count)
{
    _other_digits = nullptr;

    if (digits.empty() || digits_count == 0)
    {
        throw std::logic_error("std::vector<int> of digits should not be empty");
    }

    _oldest_digit = digits[digits_count - 1];

    if (digits_count == 1)
    {
        return;
    }

    _other_digits = new unsigned int[digits_count];
    *_other_digits = digits_count;

    for (auto i = 0; i < digits_count - 1; ++i)
    {
        _other_digits[1 + i] = *reinterpret_cast<unsigned int const *>(&digits[i]);
    }
}

void big_integer::initialize_from(
        std::string const &value,
        size_t base)
{
    _oldest_digit = 0;
    _other_digits = nullptr;

    bool is_negative = false;

    if (value[0] == '-') {
        is_negative = true;
    }

    for (int i = (is_negative); i < value.size(); i++) {

        unsigned int digit = char_to_int(value[i]);

        if (digit >= base) {

        }

        *this *= big_integer(std::vector<int> {(int)base});
        *this += big_integer(std::vector<int> {(int)digit});
    }

    if  (is_negative) {
        this->change_sign();
    }
}

void big_integer::initialize_from(
        const int num)
{
    std::string st = std::to_string(num);
    initialize_from(st, 10);
}

void big_integer::print_byte(
        std::ostream &stream,
        unsigned char byte_value)
{
    for (int i = 0; i < 8; i++)
    {
        stream << ((byte_value >> (7 - i)) & 1);
    }
}

void big_integer::dump_int_value(
        std::ostream &stream,
        int value)
{
    auto *p = (unsigned char *)&value;
    for (int i = 0; i < sizeof(int); i++)
    {
        print_byte(stream, *p++);
        stream << ' ';
    }
}

big_integer &big_integer::change_sign()
{
    _oldest_digit ^= (1 << ((sizeof(int) << 3) - 1));

    return *this;
}

inline int big_integer::get_digits_count() const noexcept
{
    return static_cast<int>(_other_digits == nullptr
                            ? 1
                            : *_other_digits);
}

inline int big_integer::sign() const noexcept
{
    if (is_equal_to_zero())
    {
        return 0;
    }

    return 1 - (static_cast<int>((*reinterpret_cast<unsigned int const *>(&_oldest_digit) >> ((sizeof(int) << 3) - 1))) << 1);
}

inline bool big_integer::is_equal_to_zero() const noexcept
{
    return ((_oldest_digit << 1) == 0) && _other_digits == nullptr;
}

inline bool big_integer::is_equal_to_one() const noexcept
{
    return _oldest_digit == 1 && _other_digits == nullptr;
}

inline unsigned int big_integer::get_digit(
        int position) const noexcept
{
    if (_other_digits == nullptr)
    {
        return position == 0
               ? _oldest_digit
               : 0;
    }

    int const digits_count = get_digits_count();
    if (position < digits_count - 1)
    {
        return _other_digits[position + 1];
    }

    if (position == digits_count - 1)
    {
        return _oldest_digit;
    }

    return 0;
}

big_integer::big_integer(
        int const *digits,
        size_t digits_count)
{
    initialize_from(digits, digits_count);
}

big_integer::big_integer(
        std::vector<int> const &digits)
{
    std::vector<int> new_vec = digits;
    remove_additional_zeroes(new_vec);
    initialize_from(new_vec, new_vec.size());
}

big_integer::big_integer(
        std::string const &value,
        size_t base)
{
    initialize_from(value, base);
}

big_integer::big_integer(
        const int num)
{
    initialize_from(num);
}

big_integer::big_integer(
        big_integer const &other)
{
    copy_from(other);
}

big_integer &big_integer::operator=(
        big_integer const &other)
{
    if (this != &other)
    {
        clear();
        copy_from(other);
    }

    return *this;
}

big_integer::big_integer(
        big_integer &&other) noexcept
{
    move_from(std::move(other));
}

big_integer &big_integer::operator=(
        big_integer &&other) noexcept
{
    if (this != &other)
    {
        clear();
        move_from(std::move(other));
    }

    return *this;
}

big_integer::~big_integer()
{
    clear();
}

big_integer &big_integer::operator+=(
        big_integer const &other)
{
    if (other.is_equal_to_zero())
    {
        return *this;
    }

    if (is_equal_to_zero())
    {
        return *this = other;
    }

    if (sign() == -1)
    {
        change_sign();
        *this += -other;
        return change_sign();
    }

    if (other.sign() == -1)
    {
        return *this -= -other;
    }

    auto const first_value_digits_count = get_digits_count();
    auto const second_value_digits_count = other.get_digits_count();
    auto const digits_count = std::max(first_value_digits_count, second_value_digits_count);

    unsigned int operation_result = 0;

    constexpr int shift = sizeof(unsigned int) << 2;
    constexpr int mask = (1 << shift) - 1;

    std::vector<int> result_digits(digits_count + 1, 0);

    for (int i = 0; i < digits_count; ++i)
    {
        unsigned int first_value_digit = get_digit(i);
        unsigned int second_value_digit = other.get_digit(i);
        result_digits[i] = 0;

        for (int j = 0; j < 2; ++j)
        {
            operation_result += (first_value_digit & mask) + (second_value_digit & mask);
            first_value_digit >>= shift;
            second_value_digit >>= shift;
            *reinterpret_cast<unsigned int *>(&result_digits[i]) |= ((operation_result & mask) << shift * j);
            operation_result >>= shift;
        }
    }

    auto result_digits_count = result_digits.size();

    if (operation_result == 1)
    {
        if ((*reinterpret_cast<uint *>(&result_digits[digits_count - 1]) >> ((sizeof(uint) << 3) - 1)) == 0)
        {
            *reinterpret_cast<uint *>(&result_digits[digits_count - 1]) |= (1u << ((sizeof(uint) << 3) - 1));
        }
        else
        {
            result_digits.back() = 1;
        }
    }
    else if ((*reinterpret_cast<uint *>(&result_digits[digits_count - 1]) >> ((sizeof(uint) << 3) - 1)) == 0)
    {
        --result_digits_count;
    }

    remove_additional_zeroes(result_digits);

    clear();
    initialize_from(result_digits, result_digits.size());

    return *this;
}

big_integer big_integer::operator+(
        big_integer const &other) const
{
    return big_integer(*this) += other;
}

big_integer &big_integer::operator-=(
        big_integer const &other)
{
    if (other.is_equal_to_zero())
    {
        return *this;
    }

    if (is_equal_to_zero())
    {
        return *this = -other;
    }

    if (sign() == -1)
    {
        return change_sign()
                .operator-=(-other)
                .change_sign();
    }

    if (other.sign() == -1)
    {
        return *this += -other;
    }

    if (*this < other)
    {
        return *this = -(other - *this);
    }

    auto const first_value_digits_count = get_digits_count();
    auto const second_value_digits_count = other.get_digits_count();
    auto const digits_count = std::max(first_value_digits_count, second_value_digits_count);

    unsigned int borrow_cnt = 0;
    constexpr unsigned int decremented_borrow_value = std::numeric_limits<unsigned int>::max();

    std::vector<int> result_digits(digits_count + 1, 0);

    unsigned int shift_h = sizeof(unsigned int) << 2;
    unsigned int mask_h = (1 << shift_h) - 1;

    int ind = 0;

    for (; ind < digits_count; ++ind)
    {
        unsigned int first_value = this->get_digit(ind);
        unsigned int second_value = other.get_digit(ind);

        if ((first_value != 0 && first_value - borrow_cnt >= second_value) ||
            (second_value == 0 && first_value >= borrow_cnt))
        {
            result_digits[ind] = first_value - second_value - borrow_cnt;
            borrow_cnt = 0;
        }
        else
        {
            result_digits[ind] = (decremented_borrow_value - second_value - borrow_cnt) + 1 + first_value;
            borrow_cnt = 1;
        }
    }

    if (borrow_cnt)
    {
        throw std::logic_error("Borrowing from zero\n");
    }

    remove_additional_zeroes(result_digits);

    clear();
    initialize_from(result_digits, result_digits.size());

    return *this;
}

big_integer big_integer::operator-(
        big_integer const &other) const
{
    return big_integer(*this) -= other;
}

big_integer big_integer::operator-() const
{
    return big_integer(*this).change_sign();
}

big_integer &big_integer::operator*=(
        big_integer const &other)
{
    return trivial_multiplication().multiply(*this, other);
}

big_integer big_integer::operator*(
        big_integer const &other) const
{
    return big_integer(*this) *= other;
}

big_integer &big_integer::operator/=(
        big_integer const &other)
{
    return trivial_division().divide(*this, other, multiplication_rule::trivial);
}

big_integer big_integer::operator/(
        big_integer const &other) const
{
    return big_integer(*this) /= other;
}

big_integer &big_integer::operator%=(
        big_integer const &other)
{
    return trivial_division().modulo(*this, other, multiplication_rule::trivial);
}

big_integer big_integer::operator%(
        big_integer const &other) const
{
    return big_integer(*this) %= other;
}

bool big_integer::operator==(
        big_integer const &other) const
{
    if (get_digits_count() != other.get_digits_count()) {
        return false;
    }

    if (_oldest_digit != other._oldest_digit) {
        return false;
    }

    for (int i = 0; i < get_digits_count(); i++) {
        if (get_digit(i) != other.get_digit(i)) {
            return false;
        }
    }

    return true;
}

bool big_integer::operator!=(
        big_integer const &other) const
{
    return !(*this == other);
}

bool big_integer::operator<(
        big_integer const &other) const
{
    if (sign() == -1 ^ other.sign() == -1)
    {
        return sign() == -1;
    }

    if (get_digits_count() != other.get_digits_count())
    {
        return get_digits_count() < other.get_digits_count() ^ sign() == -1;
    }

    int ind_this = get_digits_count() - 1;

    while (ind_this >= 0)
    {
        if (this->get_digit(ind_this) != other.get_digit(ind_this))
        {
            return this->get_digit(ind_this) < other.get_digit(ind_this) ^ sign() == -1;
        }

        ind_this--;
    }

    return false;
}

// bool big_integer::operator<(
//     big_integer const &other) const
// {
//     if (this->sign() == -1 && other.sign() == -1) {
//         big_integer tmp1 = *this;
//         big_integer tmp2 = other;
//         tmp1.change_sign();
//         tmp2.change_sign();
//         return (tmp1 > tmp2);
//     } // mb more effective?

//     if (this->sign() == -1 || other.sign() == -1) {
//         return (sign() == -1);
//     }

//     if (this->get_digits_count() != other.get_digits_count()) {
//         return (this->get_digits_count() < other.get_digits_count());
//     }

//     for (int i = get_digits_count() - 1; i >= 0; i--) {
//         int dig_from_this = this->get_digit(i);
//         int dig_from_other = other.get_digit(i);

//         if (dig_from_this != dig_from_other) {
//             return (dig_from_this < dig_from_other);
//         }
//     }

//     return false;
// }

bool big_integer::operator<=(
        big_integer const &other) const
{
    return !(*this > other);
}

bool big_integer::operator>(
        big_integer const &other) const
{
    if (sign() == -1 ^ other.sign() == -1)
    {
        return sign() == -1;
    }

    if (get_digits_count() != other.get_digits_count())
    {
        return get_digits_count() > other.get_digits_count() ^ sign() == -1;
    }

    int ind = get_digits_count() - 1;

    while (ind >= 0)
    {
        if (this->get_digit(ind) != other.get_digit(ind))
        {
            return this->get_digit(ind) > other.get_digit(ind) ^ sign() == -1;
        }

        ind--;
    }

    return false;
}

bool big_integer::operator>=(
        big_integer const &other) const
{
    return !(*this < other);
}

big_integer big_integer::operator~() const
{
    std::vector<int> res_vec;

    for (int i = 0; i < this->get_digits_count(); i++) {
        res_vec.push_back(~(this->get_digit(i)));
    }

    return big_integer(res_vec);
}

big_integer &big_integer::operator&=(
        big_integer const &other)
{
    int this_digits_amount = this->get_digits_count();
    int other_digits_amount = other.get_digits_count();

    std::vector<int> res(std::max(this_digits_amount, other_digits_amount), 0);
    // res.reserve(std::max(this_digits_amount, other_digits_amount));

    int i = 0;
    while (i < std::min(this_digits_amount, other_digits_amount)) {
        res[i] = this->get_digit(i) & other.get_digit(i);
        i++;
    }

    remove_additional_zeroes(res);

    clear();
    initialize_from(res, res.size());

    return *this;
}

big_integer big_integer::operator&(
        big_integer const &other) const
{
    return (big_integer(*this) &= other);
}

big_integer &big_integer::operator|=(
        big_integer const &other)
{
    int this_digits_amount = this->get_digits_count();
    int other_digits_amount = other.get_digits_count();

    std::vector<int> res(std::max(this_digits_amount, other_digits_amount), 0);
    // res.reserve(std::max(this_digits_amount, other_digits_amount));

    int i = 0;
    while (i < std::min(this_digits_amount, other_digits_amount)) {
        res[i] = this->get_digit(i) & other.get_digit(i);
        i++;
    }

    if (this_digits_amount > other_digits_amount) {
        while (i < this_digits_amount) {
            res[i] = this->get_digit(i);
            i++;
        }
    }
    else if (this_digits_amount < other_digits_amount) {
        while (i < other_digits_amount) {
            res[i] = other.get_digit(i);
            i++;
        }
    }

    remove_additional_zeroes(res);

    clear();
    initialize_from(res, res.size());

    return *this;
}

big_integer big_integer::operator|(
        big_integer const &other) const
{
    return (big_integer(*this) |= other);
}

big_integer &big_integer::operator^=(
        big_integer const &other)
{
    int this_digits_amount = this->get_digits_count();
    int other_digits_amount = other.get_digits_count();

    std::vector<int> res(std::max(this_digits_amount, other_digits_amount), 0);
    // res.reserve(std::max(this_digits_amount, other_digits_amount));

    int i = 0;
    while (i < std::min(this_digits_amount, other_digits_amount)) {
        res[i] = this->get_digit(i) ^ other.get_digit(i);
        i++;
    }

    if (this_digits_amount > other_digits_amount) {
        while (i < this_digits_amount) {
            res[i] = 0 ^ this->get_digit(i);
            i++;
        }
    }
    else if (this_digits_amount < other_digits_amount) {
        while (i < other_digits_amount) {
            res[i] = 0 ^ other.get_digit(i);
            i++;
        }
    }

    remove_additional_zeroes(res);

    clear();
    initialize_from(res, res.size());

    return *this;
}

big_integer big_integer::operator^(
        big_integer const &other) const
{
    return (big_integer(*this) ^= other);
}

big_integer &big_integer::operator<<=(
        size_t shift)
{
    if (is_equal_to_zero() || shift == 0)
    {
        return *this;
    }

    auto value_sign = sign();
    if (value_sign == -1)
    {
        change_sign();
    }

    auto const added_young_zeros = shift / (sizeof(unsigned int) << 3);
    shift %= (sizeof(unsigned int) << 3);

    constexpr int half_shift = sizeof(unsigned int) << 2;
    constexpr int half_mask = (1 << half_shift) - 1;
    bool is_shift_bigger_than_half = shift >= half_shift;
    shift %= half_shift;

    size_t pos = added_young_zeros;
    std::vector<int> result_digits(get_digits_count() + added_young_zeros + 1, 0);

    std::vector<int> half_digits(this->get_digits_count() * 2, 0);

    unsigned int shift_help = sizeof(unsigned int) << 2;
    unsigned int mask_help = (1 << shift_help) - 1;

    for (int i = 0; i < this->get_digits_count(); i++) {
        half_digits[2*i] = this->get_digit(i) & mask_help;
        half_digits[2*i+1] = this->get_digit(i) >> shift_help;
    }

    int ind = 0;

    for (; ind != 2 * this->get_digits_count(); ++ind)
    {
        bool is_writing_oldest_firstly = (1 & ind) ^ is_shift_bigger_than_half;

        unsigned int operation_result = half_digits[ind] << shift;
        result_digits[pos] |= (operation_result & half_mask) << (is_writing_oldest_firstly ? half_shift : 0);

        if (is_writing_oldest_firstly)
        {
            ++pos;
        }

        result_digits[pos] |= (operation_result >> half_shift) << (is_writing_oldest_firstly ? 0 : half_shift);
    }

    remove_additional_zeroes(result_digits);

    clear();
    initialize_from(result_digits, result_digits.size());

    if (value_sign == -1)
    {
        change_sign();
    }

    return *this;
}

big_integer big_integer::operator<<(
        size_t shift_value) const
{
    return big_integer(*this) <<= shift_value;
}

big_integer &big_integer::operator>>=(
        size_t shift)
{
    if (is_equal_to_zero() || shift == 0)
    {
        return *this;
    }

    auto value_sign = sign();
    if (value_sign == -1)
    {
        change_sign();
    }

    auto const erased_digits = shift / (sizeof(unsigned int) << 3);
    shift %= (sizeof(unsigned int) << 3);

    if (erased_digits >= get_digits_count())
    {
        *this = big_integer(0);
        if (value_sign == -1)
        {
            change_sign();
        }
        return *this;
    }

    constexpr int half_shift = sizeof(unsigned int) << 2;
    constexpr int half_mask = (1 << half_shift) - 1;
    bool is_shift_bigger_than_half = shift >= half_shift;
    shift %= half_shift;

    size_t pos = 0;
    std::vector<int> result_digits(get_digits_count() - erased_digits, 0);

    std::vector<int> half_digits(this->get_digits_count() * 2, 0);

    unsigned int shift_help = sizeof(unsigned int) << 2;
    unsigned int mask_help = (1 << shift) - 1;

    for (int i = 0; i < this->get_digits_count(); i++) {
        half_digits[2*i] = this->get_digit(i) & mask_help;
        half_digits[2*i+1] = this->get_digit(i) >> shift_help;
    }

    int ind = 0;

    for (size_t i = 0; i < 2*erased_digits; ++i)
    {
        ++ind;
    }

    for (; ind != half_digits.size(); ++ind)
    {
        bool is_writing_oldest_firstly = (half_digits[ind] & 1) == is_shift_bigger_than_half;
        unsigned int operation_result = half_digits[ind] << half_shift >> shift;

        if (pos > 0)
        {
            result_digits[pos - 1] |= (operation_result & half_mask) << (is_writing_oldest_firstly ? half_shift : 0);
        }

        if (is_writing_oldest_firstly)
        {
            ++pos;
        }

        if (pos > 0)
        {
            result_digits[pos - 1] |= (operation_result >> half_shift) << (is_writing_oldest_firstly ? 0 : half_shift);
        }
    }

    remove_additional_zeroes(result_digits);

    clear();
    initialize_from(result_digits, result_digits.size());

    if (value_sign == -1)
    {
        change_sign();
    }

    return *this;
}

big_integer big_integer::operator>>(
        size_t shift_value) const
{
    return (big_integer(*this) >>= shift_value);
}

std::ostream &operator<<(
        std::ostream &stream,
        big_integer const &value)
{
    std::string ans = value.to_string();

    return stream << ans;
}

std::istream &operator>>(
        std::istream &stream,
        big_integer &value)
{
    std::string value_as_string;

    stream >> value_as_string;

    value.clear();
    value.initialize_from(value_as_string, 10);

    return stream;
}

unsigned int big_integer::char_to_int(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 36;
    }

    throw std::logic_error("invalid char");
}

void big_integer::remove_additional_zeroes(std::vector<int> &digits) {
    if (digits.size() <= 1) {
        return;
    }

    bool has_minus = false;
    int meaningful_digits = digits.size();

    if ((digits[digits.size() - 1] >> ((sizeof(unsigned int) << 3) - 1))) {
        has_minus = true;
    }

    if ((digits[digits.size() - 1] << 1) != 0) {
        return;
    }
    else {
        if ((digits[digits.size() - 2] >> ((sizeof(unsigned int) << 3) - 1))) {
            return;
        }
    }

    meaningful_digits--;
    for (int i = digits.size() - 2; i >= 1; i--) {
        if (digits[i] == 0 && !(digits[i-1] >> ((sizeof(unsigned int) << 3) - 1))) {
            meaningful_digits--;
        }
        else {
            break;
        }
    }

    if (meaningful_digits == 0) {
        digits.resize(1);
        digits[0] = 0;
        return;
    }

    digits.resize(meaningful_digits);
    if (has_minus) {
        digits.push_back(1 << (sizeof(unsigned int) << 3 - 1));
    }

}

std::string big_integer::to_string() const
{

    if (this->is_equal_to_zero()) {
        return std::string("0");
    }
    std::string res;
    res.reserve(11 * get_digits_count());

    big_integer big_number(*this);
    int sign = big_number.sign();

    if (sign == -1) {
        big_number.change_sign();
    }

    int const big_modulus_zeros_cnt = 9;
    int const big_modulus = 1e9;

    big_integer big_modulus_int_big_int(std::vector<int> {big_modulus});

    while (!big_number.is_equal_to_zero())
    {
        auto [quotient, remainder] =  big_integer::divide_with_remainder(big_number, big_modulus_int_big_int, true);

        unsigned int tmp = remainder.get_digit(0);
        for (size_t i = 0; i < big_modulus_zeros_cnt && (tmp || !quotient.value().is_equal_to_zero()); ++i)
        {
            res.push_back('0' + tmp % 10);
            tmp /= 10;
        }

        big_number = quotient.value();
    }

    if (is_equal_to_zero())
    {
        res.push_back('0');
    }

    if (sign == -1)
    {
        res.push_back('-');
    }

    std::reverse(res.begin(), res.end());

    return res;

}

std::pair<std::optional<big_integer>, big_integer> big_integer::divide_with_remainder(
        big_integer const &dividend,
        big_integer const &divisor,
        bool eval_quotient) const
{
    if (divisor.is_equal_to_zero())
    {
        throw std::logic_error("attempt to divide by zero");
    }

    if (dividend.is_equal_to_zero())
    {
        return std::make_pair(std::optional(big_integer(0)), big_integer(0));
    }

    if (divisor._oldest_digit == 1 && divisor._other_digits == nullptr)
    {
        return std::make_pair(std::optional(dividend), big_integer(0));
    }

    if (dividend.sign() == -1)
    {
        auto [quotient, remainder] = divide_with_remainder(-dividend, divisor, eval_quotient);

        if (quotient.has_value())
        {
            return std::make_pair(std::optional(-quotient.value()), -remainder);
        }
        else
        {
            return std::make_pair(std::optional<big_integer>(), -remainder);
        }
    }

    if (divisor.sign() == -1)
    {
        auto [quotient, remainder] = divide_with_remainder(dividend, -divisor, eval_quotient);

        if (quotient.has_value())
        {
            return std::make_pair(std::optional(-quotient.value()), -remainder);
        }
        else
        {
            return std::make_pair(std::optional<big_integer>(), -remainder);
        }
    }

    auto const dividend_digits_count = dividend.get_digits_count();

    std::vector<int> result_digits(1, 0);
    big_integer minuend(std::vector<int> {0});

    for (size_t i = 0; i < dividend_digits_count; ++i)
    {
        unsigned int cur_digit = dividend.get_digit(dividend_digits_count - i - 1);

        // minuend *= (1 << (8 * sizeof(unsigned int)));
        minuend <<= 8 * sizeof(unsigned int);
        minuend += big_integer(std::vector<int>( {*reinterpret_cast<int *>(&cur_digit), 0} ));

        if (minuend < divisor)
        {
            result_digits.push_back(0);
        }
        else
        {
            unsigned int digit = 0;
            big_integer subtrahend(std::vector<int> {0});

            for (unsigned int multiplier = 1 << (8*sizeof(unsigned int) - 1); multiplier > 0; multiplier >>= 1)
            {
                int multiplier_int = *reinterpret_cast<int *>(&multiplier);
                big_integer tmp = divisor * big_integer(std::vector<int>( {multiplier_int, 0} ));

                if (minuend >= subtrahend + tmp)
                {
                    subtrahend += tmp;
                    digit += multiplier;
                }
            }

            minuend -= subtrahend;

            if (eval_quotient)
            {
                result_digits.push_back(digit);
            }
        }
    }

    if (eval_quotient)
    {
        std::reverse(result_digits.begin(), result_digits.end());

        return std::make_pair(std::optional(big_integer(result_digits)), minuend);
    }

    return std::make_pair(std::optional<big_integer>(), minuend);
}