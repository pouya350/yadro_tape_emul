#include "type.hpp"
#include "typesorter.hpp"
int main(int argc, char* argv[])
{
    if (argc != 5) return 1;
    try {
        auto cfg = loadConfig(argv[3]);
        Type inTape(argv[1], cfg, false);
        Type outTape(argv[2], cfg, true);
        TapeSorter(std::stoull(argv[4])).sort(inTape, outTape);
    }
    catch (...) {
        return 1;
    }

    return 0;
}