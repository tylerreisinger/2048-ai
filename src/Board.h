#ifndef BOARD_H_
#define BOARD_H_

#include <cstdint>
#include <ostream>
#include <random>
#include <vector>

#include <SFML/System/Vector2.hpp>

enum class ShiftDirection {
    Down = 0,
    Left = 1,
    Right = 2,
    Up = 3,
};

class Cell {
public:
    static constexpr uint64_t EMPTY = 0;
    Cell() = default;
    Cell(uint64_t value) : value(value) {}
    ~Cell() = default;

    Cell(const Cell& other) = default;
    Cell(Cell&& other) noexcept = default;
    Cell& operator=(const Cell& other) = default;
    Cell& operator=(Cell&& other) noexcept = default;

    uint64_t value = EMPTY;
};

class Board {
public:
    Board(int width, int height);
    ~Board() = default;

    Board(const Board& other) = default;
    Board(Board&& other) noexcept = default;
    Board& operator=(const Board& other) = default;
    Board& operator=(Board&& other) noexcept = default;

    void shift_board(ShiftDirection dir);
    void do_move(ShiftDirection dir);

    double compute_score() const;
    double total_value() const;
    double max_value() const;

    int free_spaces() const;
    int filled_spaces() const;

    const Cell& get_cell(int x, int y) const;
    Cell& get_cell(int x, int y);

    int width() const { return m_width; }
    int height() const { return m_height; }
    int total_blocks() const { return width() * height(); }
    const std::vector<Cell>& cells() const { return m_cells; }
    void add_new_block();

    bool is_filled() const;
    bool is_lost() const;

private:
    static sf::Vector2<int> dir_offset(ShiftDirection dir);
    bool merge(Cell& cell1, Cell& cell2);
    double score_for_cell(const Cell& cell) const;


    int m_width;
    int m_height;
    std::vector<Cell> m_cells;
    std::mt19937 m_rng;
    bool m_is_lost = false;
};

std::ostream& operator<<(std::ostream& stream, ShiftDirection dir);

#endif
