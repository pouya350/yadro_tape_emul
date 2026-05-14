#ifndef YADRO_TAPE_EMUL_TAPE_HPP
#define YADRO_TAPE_EMUL_TAPE_HPP
#include "vtype.hpp"
#include "config.hpp"
#include <string>
#include <fstream>
#include <thread>
#include <stdexcept>


class Type : public VTape
{
    std::fstream file_;
    std::string filename_;
    TapeConfig config_;
    size_t position_ = 0;
    size_t size_ = 0;
    void simulateDelay(const std::chrono::milliseconds& delay) const {
        if (delay.count() > 0) {
            std::this_thread::sleep_for(delay);
        }
    }
    void calculateInitialSize() {
        file_.seekg(0, std::ios::end);
        size_ = file_.tellg() / sizeof(int32_t);
        file_.seekg(0, std::ios::beg);
    }
public:
    Type(const std::string& filename, const TapeConfig& config, bool createIfNotExists = false)
        : filename_(filename), config_(config), position_(0), size_(0)
    {
        if (createIfNotExists) {
            std::ofstream out(filename_, std::ios::binary | std::ios::trunc);
        }
        file_.open(filename_, std::ios::in | std::ios::out | std::ios::binary);
        if (!file_.is_open()) {
            throw std::runtime_error("Non open" + filename_);
        }
        calculateInitialSize();
    }
    ~Type() override {
        if (file_.is_open()) file_.close();
    }
    int32_t read() override {
        simulateDelay(config_.readDelay);
        int32_t value = 0;
        file_.seekg(position_ * sizeof(int32_t), std::ios::beg);
        file_.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }
    void write(int32_t value) override {
        simulateDelay(config_.writeDelay);
        file_.seekp(position_ * sizeof(int32_t), std::ios::beg);
        file_.write(reinterpret_cast<const char*>(&value), sizeof(value));
        if (position_ >= size_) size_ = position_ + 1;
    }
    void moveForward() override {
        simulateDelay(config_.moveDelay);
        position_++;
    }
    void moveBackward() override {
        simulateDelay(config_.moveDelay);
        if (position_ > 0) position_--;
    }
    void rewindForward() override {
        simulateDelay(config_.rewindDelay);
        position_ = (size_ > 0) ? (size_ - 1) : 0;
    }

    void rewindBackward() override {
        simulateDelay(config_.rewindDelay);
        position_ = 0;
    }
    size_t getPosition() const override { return position_; }
    size_t getSize() const override { return size_; }
};


#endif
