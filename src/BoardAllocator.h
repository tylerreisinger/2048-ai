#ifndef BOARDALLOCATOR_H_
#define BOARDALLOCATOR_H_

#include <cstdint>
#include <vector>

class BoardVector {
public:
    BoardVector() = delete;
    ~BoardVector() = default;

    BoardVector(const BoardVector& other) = delete;
    BoardVector(BoardVector&& other) noexcept = delete;
    BoardVector& operator=(const BoardVector& other) = delete;
    BoardVector& operator=(BoardVector&& other) noexcept = delete;
};

class BoardAllocator {
public:
    BoardAllocator();
    ~BoardAllocator() = default;

    BoardAllocator(const BoardAllocator& other) = delete;
    BoardAllocator(BoardAllocator&& other) noexcept = default;
    BoardAllocator& operator=(const BoardAllocator& other) = delete;
    BoardAllocator& operator=(BoardAllocator&& other) noexcept = default;

private:
    int m_capacity;
    std::vector<uint8_t> elements;
};

#endif
