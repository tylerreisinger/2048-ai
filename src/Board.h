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

    bool is_empty() const { return value == EMPTY; }

    uint64_t value = EMPTY;
};

class Board {
public:
    Board(int width, int height, uint64_t seed = 0);
    ~Board() = default;

    Board(const Board& other) = default;
    Board(Board&& other) noexcept = default;
    Board& operator=(const Board& other) = default;
    Board& operator=(Board&& other) noexcept = default;

    bool shift_board(ShiftDirection dir);
    void shift_board_legacy(ShiftDirection dir);
    bool do_move(ShiftDirection dir);

    double compute_score() const;
    double total_value() const;
    double max_value() const;

    int free_spaces() const;
    int filled_spaces() const;
    int turn() const { return m_turn; }

    const Cell& get_cell(int x, int y) const;
    Cell& get_cell(int x, int y);
    const Cell& get_cell(int idx) const { return m_cells[idx]; }
    Cell& get_cell(int idx) { return m_cells[idx]; }

    int width() const { return m_width; }
    int height() const { return m_height; }
    int total_blocks() const { return width() * height(); }
    const std::vector<Cell>& cells() const { return m_cells; }
    void add_new_block();
    int get_new_cell_val();

    bool is_filled() const;
    bool is_lost() const;

private:
    static sf::Vector2<int> dir_offset(ShiftDirection dir);
    bool merge(Cell& cell1, Cell& cell2);
    double score_for_cell(const Cell& cell) const;

    bool try_merge(Cell& lhs, Cell& rhs);

    bool shift_board_left();
    bool shift_board_right();
    bool shift_board_up();
    bool shift_board_down();


    int m_width;
    int m_height;
    int m_turn = 0;
    std::vector<Cell> m_cells;
    std::minstd_rand m_rng;
    bool m_is_lost = false;
};

inline uint64_t fast_pow2_log2(uint64_t v) {
    /*
    switch(v) {
        case 2: return 1;
        case 4: return 2;
        case 8: return 3;
        case 16: return 4;
        case 32: return 5;
        case 64: return 6;
        case 128: return 7;
        case 256: return 8;
        case 512: return 9;
        case 1024: return 10;
        case 2048: return 11;
        case 4096: return 12;
        case 8192: return 13;
        case 8192: return 13;
    }
    */
    return 64 - __builtin_clzl(v) - 1;
}

inline double Board::score_for_cell(const Cell& cell) const {
    auto x = cell.value >> 1;
    auto n = fast_pow2_log2(x);
    return 2.0 * x * n;
}


std::ostream& operator<<(std::ostream& stream, ShiftDirection dir);

#endif
