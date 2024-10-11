#include <fraction.h>
#include <iostream>
#include <cmath>

using std::cout;
using std::cin;
using std::endl;

// fraction calculate_pi(fraction epsilon) {
//   fraction pi(0, 1);
//   fraction sign(1, 1);
//   int k = 1;

//   fraction sixteen_pow(16, 1);

//   fraction prev_pi(pi);

//   // Используем формулу Бэйли-Борвейна-Плаффа (BBP)
//   do {
//     fraction mult = fraction(4, 1) / fraction(big_integer(8 * k + 1), big_integer(1)) - fraction(2, 1) / fraction(big_integer(8 * k + 4), big_integer(1))
//      - fraction(1, 1) / fraction(big_integer(8 * k + 5), big_integer(1)) - fraction(1, 1) / fraction(big_integer(8 * k + 6), big_integer(1));
//     // ((4.0 / (8 * k + 1) - 2.0 / (8 * k + 4) - 1.0 / (8 * k + 5) - 1.0 / (8 * k + 6)));
//     prev_pi = pi;
//     pi += sign * mult;
//     sign *= fraction(-1, 1);
//     k++;
//     sixteen_pow *= fraction(16, 1);
//   } while ((pi - prev_pi).abs() > epsilon);

//   return pi;
// }

fraction pi(fraction eps) {
    fraction sum(0, 1);
    fraction sign(1, 1);

    int i = 1;
    fraction term(1, 1);

    while (term.abs() > eps) {
        term = sign / (fraction(2*i + 1, 1));
        sum += term;
        sign *= fraction(-1, 1);
        i++;
    }

    // for (int i = 0; i < n; ++i) {           
    //     sum += sign/(2.0*i+1.0);
    //     sign *= -1;
    // }
    return fraction(4, 1) * sum;
}

int main(int argc,char *argv[])
{
    // big_integer num(-5);
    // big_integer num2("-5");

    // num.dump_value(cout);
    // cout << endl;
    // num2.dump_value(cout);
    // cout << endl;

    // cout << (num) << endl;



    fraction a(big_integer(0), big_integer(32));
    // fraction b(big_integer(-0), big_integer(15));

    fraction eps(big_integer(1), big_integer("1000000"));


    // fraction c = a.cos(eps);
    cout << fraction(0, 1).lg(eps) / fraction(3, 1).lg(eps) << endl;

    // cout << (fraction(1, 1).arcsin(eps)) << endl;

    return 0;
}