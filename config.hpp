#ifndef YADRO_TAPE_EMUL_CONFIG_HPP
#define YADRO_TAPE_EMUL_CONFIG_HPP
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>


struct TapeConfig
{
    std::chrono::milliseconds readDelay;
    std::chrono::milliseconds writeDelay;
    std::chrono::milliseconds moveDelay;
    std::chrono::milliseconds rewindDelay;
};


TapeConfig loadConfig(const std::string& filename)
{
    TapeConfig config{
        std::chrono::milliseconds(100),
        std::chrono::milliseconds(100),
        std::chrono::milliseconds(50),
        std::chrono::milliseconds(1000)
    };
    std::ifstream inputf(filename);
    if (!inputf) {
        return config;
    }
    std::string line;
    while (std::getline(inputf, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, ' ')) {
            int value = 0;
            iss >> value;
            if (key == "readDelay") {
                config.readDelay = std::chrono::milliseconds(value);
            }
            else if (key == "writeDelay") {
                config.writeDelay = std::chrono::milliseconds(value);
            }
            else if (key == "moveDelay") {
                config.moveDelay = std::chrono::milliseconds(value);
            }
            else if (key == "rewindDelay") {
                config.rewindDelay = std::chrono::milliseconds(value);
            }
        }
    }
    return config;
}

#endif
