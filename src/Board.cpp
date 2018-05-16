#include "Board.h"

#include <algorithm>
#include <cmath>
#include <iostream>

std::ostream& operator<<(std::ostream& stream, sf::Vector2<int> v) {
    stream << "(" << v.x << "," << v.y << ")";
    return stream;
}

Board::Board(int width, int height) : m_width(width), m_height(height) {
    m_cells.resize(width * height);
}
void Board::shift_board(ShiftDirection dir) {
    auto shift = dir_offset(dir);
    bool changed = false;
    int idx = 1;
    do {
        changed = false;
        for(int y = 0; y < m_height; ++y) {
            for(int x = 0; x < m_width; ++x) {
                auto pos = sf::Vector2<int>(x, y);
                auto shift_pos = pos + shift;
                if(shift_pos.x < 0 || shift_pos.x >= m_width ||
                        shift_pos.y < 0 || shift_pos.y >= m_height) {
                    continue;
                }
                auto& shift_cell = get_cell(shift_pos.x, shift_pos.y);
                auto& cur_cell = get_cell(pos.x, pos.y);
                if(cur_cell.value != Cell::EMPTY) {
                    changed = changed || merge(shift_cell, cur_cell);
                }
            }
        }
        idx += 1;
    } while(changed == true);
}

const Cell& Board::get_cell(int x, int y) const {
    return m_cells[x + y * m_width];
}
Cell& Board::get_cell(int x, int y) { return m_cells[x + y * m_width]; }

sf::Vector2<int> Board::dir_offset(ShiftDirection dir) {
    switch(dir) {
    case ShiftDirection::Up:
        return sf::Vector2<int>(0, -1);
    case ShiftDirection::Left:
        return sf::Vector2<int>(-1, 0);
    case ShiftDirection::Down:
        return sf::Vector2<int>(0, 1);
    case ShiftDirection::Right:
        return sf::Vector2<int>(1, 0);
    default:
        return sf::Vector2<int>(0, 0);
    }
}

bool Board::merge(Cell& cell1, Cell& cell2) {
    if(cell1.value == Cell::EMPTY) {
        std::swap(cell1, cell2);
        return true;
    } else if(cell1.value == cell2.value) {
        cell1.value += cell2.value;
        cell2.value = Cell::EMPTY;
        return true;
    }
    return false;
}

void Board::add_new_block() {
    std::uniform_int_distribution<int> dist(0, m_width * m_height);
    sf::Vector2<int> pos;
    if(m_is_lost || is_filled()) {
        m_is_lost = true;
        return;
    }
    while(true) {
        auto pos = dist(m_rng);
        auto& cell = m_cells[pos];
        if(cell.value == Cell::EMPTY) {
            cell = 2;
            break;
        }
    }
}

bool Board::is_filled() const {
    return std::all_of(m_cells.begin(), m_cells.end(), [](auto& cell) {
        return cell.value != Cell::EMPTY;
    });
}
bool Board::is_lost() const { return m_is_lost; }

double Board::compute_score() const {
    double val = 0.0;
    for(const auto& cell : m_cells) {
        val += score_for_cell(cell.value);
    }
    return val;
}

double Board::score_for_cell(const Cell& cell) const {
    if(cell.value <= 2.0) {
        return 0.0;
    }
    auto x = cell.value >> 1;
    return 2.0 * (x + score_for_cell(x));
}

double Board::total_value() const {
    double val = 0.0;
    for(const auto& cell : m_cells) {
        val += cell.value;
    }
    return val;
}

double Board::max_value() const {
    double val = 0.0;
    for(const auto& cell : m_cells) {
        val = std::max(val, static_cast<double>(cell.value));
    }
    return val;
}

int Board::free_spaces() const {
    int val = 0;
    for(const auto& cell : m_cells) {
        if(cell.value == Cell::EMPTY) {
            val += 1;
        }
    }
    return val;
}

int Board::filled_spaces() const { return total_blocks() - free_spaces(); }

void Board::do_move(ShiftDirection dir) {
    shift_board(dir);
    add_new_block();
}

std::ostream& operator<<(std::ostream& stream, ShiftDirection dir) {
    switch(dir) {
    case ShiftDirection::Down:
        stream << "Down";
        break;
    case ShiftDirection::Left:
        stream << "Left";
        break;
    case ShiftDirection::Right:
        stream << "Right";
        break;
    case ShiftDirection::Up:
        stream << "Up";
        break;
    }
    return stream;
}
