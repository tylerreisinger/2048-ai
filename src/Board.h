#ifndef BOARD_H_
#define BOARD_H_

#include <cstdint>
#include <vector>

class Cell {
public:
    Cell() = default;
    Cell(uint64_t value) : value(value) {}
    ~Cell() = default;

    Cell(const Cell& other) = default;
    Cell(Cell&& other) noexcept = default;
    Cell& operator=(const Cell& other) = default;
    Cell& operator=(Cell&& other) noexcept = default;

    uint64_t value;
};

class Board {
public:
    Board(int width, int height);
    ~Board() = default;

    Board(const Board& other) = default;
    Board(Board&& other) noexcept = default;
    Board& operator=(const Board& other) = default;
    Board& operator=(Board&& other) noexcept = default;

private:
    int m_width;
    int m_height;
    std::vector<Cell> m_cells;
};

#endif
