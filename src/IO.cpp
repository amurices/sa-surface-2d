#include "IO.hpp"
#include <fstream>

void IO::sillyMapReader(const std::string fileName, std::unordered_map<std::string, std::string> &map)
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

void IO::sillyMapPrinter(const std::unordered_map<std::string, std::string> &map)
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

void IO::parseInputToParams(const std::unordered_map<std::string, std::string> &inputMap, InitSaParams* params)
{
    for (auto it = inputMap.begin(); it != inputMap.end(); it++)
    {
        if (it->first == "radius")
            params->radius = std::stod(it->second);

        else if (it->first == "points")
            params->points = std::stoi(it->second);

        else if (it->first == "thickness")
            params->thickness = std::stod(it->second);

        else if (it->first == "scale")
            params->scale = std::stod(it->second);

        else if (it->first == "diffMul")
            params->diffMul = std::stod(it->second);

        else if (it->first == "diffPow")
            params->diffPow = std::stod(it->second);

        else if (it->first == "areaPow")
            params->areaPow = std::stod(it->second);

        else if (it->first == "areaMul")
            params->areaMul = std::stod(it->second);

        else if (it->first == "multiProb")
            params->multiProb = std::stod(it->second);

        else if (it->first == "tempProb")
            params->tempProb = std::stod(it->second);

        else if (it->first == "compression")
            params->compression = std::stod(it->second);

        else if (it->first == "smoothness")
            params->smooth = std::stoi(it->second);

        else if (it->first == "forceOffsetRange")
            params->forceOffsetRange = std::stod(it->second);

        else{
            // explode
            std::cout << ":" << it->first << " " << it->second << std::endl;
            exit(-1);
        }
    }
}