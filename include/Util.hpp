#pragma once

#include <vector>
#include <string>

namespace Util {
    const std::string OUTPUT_FILE_NAME = "../output.txt";

    double getRandomRange(double lower, double upper);

    void time_before();

    double time_after();
}; // namespace Util
