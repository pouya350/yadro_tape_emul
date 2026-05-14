#ifndef YADRO_TAPE_EMUL_VTYPE_HPP
#define YADRO_TAPE_EMUL_VTYPE_HPP
#include <cstdint>
#include <cstddef>


class VTape
{
public:
    virtual ~VTape() = default;
    virtual int32_t read() = 0;
    virtual void write(int32_t value) = 0;
    virtual void moveForward() = 0;
    virtual void moveBackward() = 0;
    virtual void rewindForward() = 0;
    virtual void rewindBackward() = 0;
    virtual size_t getPosition() const = 0;
    virtual size_t getSize() const = 0;
};


#endif
