#ifndef YADRO_TAPE_EMUL_TAPESORTER_HPP
#define YADRO_TAPE_EMUL_TAPESORTER_HPP
#include "vtype.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>


class TapeSorter
{
    size_t memoryLimitBytes_;
    size_t splitToSortedTmpTapes(VTape& inputTape) {
        inputTape.rewindBackward();
        size_t maxElements = memoryLimitBytes_ / sizeof(int32_t);
        size_t total = inputTape.getSize();
        size_t processed = 0;
        size_t tmpTapeIndex = 0;
        while (processed < total) {
            std::vector<int32_t> buffer;
            size_t count = std::min(maxElements, total - processed);
            for (size_t i = 0; i < count; ++i) {
                buffer.push_back(inputTape.read());
                if (i + 1 < count) {
                    inputTape.moveForward();
                }
            }
            std::sort(buffer.begin(), buffer.end());
            std::string tmpFile = "tmp_tape_" + std::to_string(tmpTapeIndex) + ".bin";
            std::ofstream out(tmpFile, std::ios::binary);
            for (int32_t el : buffer) {
                out.write(reinterpret_cast<const char*>(&el), sizeof(el));
            }
            out.close();
            tmpTapeIndex++;
            processed += count;
            if (processed < total) {
                inputTape.moveForward();
            }
        }
        return tmpTapeIndex;
    }


    void mergeTmpTapes(size_t tmpFilesCount, VTape& outputTape) {
        std::vector<std::ifstream*> tmpFiles;
        std::vector<int32_t> currentValues(tmpFilesCount);
        std::vector<bool> hasValue(tmpFilesCount, false);
        for (size_t i = 0; i < tmpFilesCount; ++i) {
            std::string fname = "tmp_tape_" + std::to_string(i) + ".bin";
            auto* file = new std::ifstream(fname, std::ios::binary);
            tmpFiles.push_back(file);
            if (file->read(reinterpret_cast<char*>(&currentValues[i]), sizeof(int32_t))) {
                hasValue[i] = true;
            }
        }
        while (true) {
            int32_t minValue = std::numeric_limits<int32_t>::max();
            int minIndex = -1;
            for (size_t i = 0; i < tmpFilesCount; ++i) {
                if (hasValue[i] && currentValues[i] < minValue) {
                    minValue = currentValues[i];
                    minIndex = static_cast<int>(i);
                }
            }
            if (minIndex == -1) break;
            outputTape.write(minValue);
            outputTape.moveForward();
            if (!tmpFiles[minIndex]->read(reinterpret_cast<char*>(&currentValues[minIndex]), sizeof(int32_t))) {
                hasValue[minIndex] = false;
            }
        }
        for (size_t i = 0; i < tmpFilesCount; ++i) {
            tmpFiles[i]->close();
            delete tmpFiles[i];
            std::string fname = "tmp_tape_" + std::to_string(i) + ".bin";
            std::remove(fname.c_str());
        }
    }
public:
    TapeSorter(size_t memoryLimitBytes) : memoryLimitBytes_(memoryLimitBytes) {}
    void sort(VTape& inputTape, VTape& outputTape) {
        size_t tmpFilesCount = splitToSortedTmpTapes(inputTape);
        mergeTmpTapes(tmpFilesCount, outputTape);
        outputTape.rewindBackward();
    }
};


#endif
