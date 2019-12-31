#pragma once

#include <vector>
#include <string>

namespace Util
{
    const std::string OUTPUT_FILE_NAME = "../output.txt";

    struct triple_t
    {
        double _1;
        double _2;
        double _3;

        triple_t(double r, double g, double b)
        {
            _1 = r;
            _2 = g;
            _3 = b;
        }

        triple_t operator*(const double &a) const
        {
            return triple_t(_1 * a, _2 * a, _3 * a);
        }

        void operator*=(const double &a)
        {
            _1 *= a;
            _2 *= a;
            _3 *= a;
        }

        triple_t operator+(const double &a) const
        {
            return triple_t(_1 + a, _2 + a, _3 + a);
        }

        void operator+=(const double &a)
        {
            _1 += a;
            _2 += a;
            _3 += a;
        }

        void operator+=(const triple_t &a)
        {
            _1 += a._1;
            _2 += a._2;
            _3 += a._3;
        }
    };

double absol(double x);

double getRandomRange(double lower, double upper);

void randColors(std::vector<triple_t> &colors, int numColors);

void time_before();
double time_after();
}; // namespace Util
