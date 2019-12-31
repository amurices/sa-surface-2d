#pragma once

#include <unordered_map>
#include <string>
#include <iostream>

namespace IO {
    struct InitSaParams
    {
        // Surface params
        double radius;
        double thickness;
        int points;
        // Optimizer params
        double scale;
        double a0;
        int smooth;
        double diffMul;
        double diffPow;
        double areaPow;
        double areaMul;
        double multiProb;
        double tempProb;
        double forceOffsetRange;
        double compression;
    };

void sillyMapReader(const std::string fileName, std::unordered_map<std::string, std::string> &map);
void sillyMapPrinter(const std::unordered_map<std::string, std::string> &map);
void parseInputToParams(const std::unordered_map<std::string, std::string> &inputMap, InitSaParams* params);
};

inline std::ostream &operator<<(std::ostream &os, IO::InitSaParams const &params)
{
    return os << "Params:\n"
              << "thickness: " << params.thickness << std::endl
              << "radius: " << params.radius << std::endl
              << "points: " << params.points << std::endl
              << "a0: " << params.a0 << std::endl
              << "smooth: " << params.smooth << std::endl
              << "diffMul: " << params.diffMul << std::endl
              << "diffPow: " << params.diffPow << std::endl
              << "areaPow: " << params.areaPow << std::endl
              << "areaMul: " << params.areaMul << std::endl
              << "multiProb: " << params.multiProb << std::endl
              << "tempProb: " << params.tempProb << std::endl
              << "forceOffsetRange: " << params.forceOffsetRange << std::endl
              << "compression: " << params.compression << std::endl;
}