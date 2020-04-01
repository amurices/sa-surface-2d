#include "IO.hpp"
#include <fstream>
#include <iostream>
#include <SurfaceProperties.hpp>

namespace IO {
    namespace {
        std::ofstream outputFile;
    }
    void sillyMapReader(const std::string fileName, std::unordered_map<std::string, std::string> &map)
    {
        std::ifstream input;
        input.open(fileName);
        if (input.fail()){
            printf("Input file %s not found.\n", fileName.c_str());
            exit(0);
        }
        std::string reader;
        std::unordered_map<std::string, std::string> inputMap;
        while (std::getline (input, reader)){
            int valPos = reader.find(" ");
            std::string key = reader.substr(0, valPos);
            std::string val = reader.substr(valPos, reader.size());
            map.insert(std::pair<std::string,std::string> (key, val));
        }
        input.close();
    }

    void sillyMapPrinter(const std::unordered_map<std::string, std::string> &map)
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

    void parseInputToParams(const std::unordered_map<std::string, std::string> &inputMap, InitSaParams* params)
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

            else if (it->first == "temperature")
                params->temperature = std::stod(it->second);

            else if (it->first == "bothCorrsDist")
                params->bothCorrsDist = std::stod(it->second);

            else if (it->first == "splitThreshold")
                params->splitThreshold = std::stod(it->second);

            else if (it->first == "craniumRadius")
                params->craniumRadius = std::stod(it->second);

            else{
                // explode
                std::cout << ":" << it->first << " " << it->second << std::endl;
                exit(-1);
            }
        }
    }

    void openOutputFileAndWriteHeader(const std::set<std::string> &recordedAttributes){
        outputFile.open(IO::OUTPUT_FILE_NAME);
        if (outputFile.fail()){
            printf("Error opening output file %s.\n", IO::OUTPUT_FILE_NAME.c_str());
            exit(0);
        }
        if (!recordedAttributes.empty()) {
            outputFile << *recordedAttributes.begin();
            for (auto it = ++recordedAttributes.begin(); it != recordedAttributes.end(); it++){
                outputFile << "," << *it;
            } outputFile << std::endl;
        }
    }

    void commitToOutputFile(const Graph::ThickSurface &thickSurface, const std::set<std::string> &recordedAttributes)
    {
        if (!recordedAttributes.empty()) {
            outputFile << SurfaceProperties::fns[*recordedAttributes.begin()](thickSurface);
            for (auto it = ++recordedAttributes.begin(); it != recordedAttributes.end(); it++){
                outputFile << "," << SurfaceProperties::fns[*it](thickSurface);
            } outputFile << std::endl;
        }
    }

    void closeOutputFile(){
        IO::outputFile.close();
    }
}