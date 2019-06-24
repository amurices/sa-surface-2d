#include <unordered_map>
#include <string>

#include "defs.hpp"

namespace IO {

void sillyMapReader(const std::string fileName, std::unordered_map<std::string, std::string> &map);
void sillyMapPrinter(const std::unordered_map<std::string, std::string> &map);
void parseInputToParams(const std::unordered_map<std::string, std::string> &inputMap, InitSaParams* params);
};