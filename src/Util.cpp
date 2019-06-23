#include "Util.hpp"
#include "sys/time.h"
#include <fstream>

double Util::dist(point_t p1, point_t p2)
{
    std::pair<double, double> norm = std::make_pair(p2.x - p1.x, p2.y - p1.y);
    return sqrt(norm.first * norm.first + norm.second * norm.second);
}

bool Util::ltTolerance(double a, double b, double tol)
{
    return (a < (b - tol));
}
bool Util::gtTolerance(double a, double b, double tol)
{
    return (a > (b + tol));
}

bool Util::eqTolerance(double a, double b, double tol)
{
    return (a > b - tol) && (a < b + tol);
}

double Util::absol(double x)
{
    if (x < 0)
        return -x;
    return x;
}

double Util::getRandomRange(double lower, double upper)
{
    return lower + (upper - lower) * (double)rand() / static_cast<double>(RAND_MAX);
}

void Util::randColors(std::vector<triple_t> &colors, int numColors)
{
    colors.clear();
    for (int i = 0; i < numColors; i++)
    {
        colors.emplace_back(getRandomRange(0.0, 1.0), getRandomRange(0.0, 1.0), getRandomRange(0.0, 1.0));
    }
}


// Start timer
struct timeval tvalBefore;
void Util::time_before()
{
    gettimeofday (&tvalBefore, NULL);
}

// End timer, return seconds since last time_b call
double Util::time_after()
{
    struct timeval tvalAfter;
    gettimeofday (&tvalAfter, NULL);
    float timeSince = tvalAfter.tv_sec - tvalBefore.tv_sec;
    float diff = tvalBefore.tv_usec - tvalAfter.tv_usec;
    if (diff > 0)
        timeSince -= 1.0;
    timeSince += std::abs(diff) / 1000000;
    return timeSince;
}

void Util::sillyMapReader(const std::string fileName, std::map<std::string, std::string> &map)
{
    std::ifstream input;
    input.open(fileName);
    std::string reader;
    std::map<std::string, std::string> inputMap;
    while (std::getline (input, reader)){
        int valPos = reader.find(" ");
        std::string key = reader.substr(0, valPos);
        std::string val = reader.substr(valPos, reader.size()); 
        map.insert(std::pair<std::string,std::string> (key, val));
    }
}

void Util::sillyMapPrinter(const std::map<std::string, std::string> &map)
{
    std::cout << "{";
    for (auto it = map.begin(); it != map.end(); it++){
        if (it != map.begin()) std::cout <<  " ";
        std::cout << ":" << it->first << " " << it->second;
        auto it2 = it;
        it2++;
        if (it2 != map.end())
        std::cout << std::endl;
    } std::cout << "}\n";
}