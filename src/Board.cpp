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
// Mpstly works, but is over 1 OOM slower than the new methods.
void Board::shift_board_legacy(ShiftDirection dir) {
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
    sf::Vector2<int> pos;
    if(m_is_lost) {
        return;
    }
    int free = free_spaces();
    if(free == 0) {
        m_is_lost = true;
        return;
    }

    int selector = m_rng() % (free - 1);
    int free_cnt = 0;
    for(std::size_t i = 0; i < m_cells.size(); ++i) {
        if(m_cells[i].value == Cell::EMPTY) {
            if(free_cnt == selector) {
                m_cells[i].value = 2;
                return;
            }
            free_cnt += 1;
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

void Board::shift_board(ShiftDirection dir) {
    switch(dir) {
    case ShiftDirection::Left:
        return shift_board_left();
    case ShiftDirection::Right:
        return shift_board_right();
    case ShiftDirection::Up:
        return shift_board_up();
    case ShiftDirection::Down:
        return shift_board_down();
    }
}

// These functions are written to be fast, as opposed to clean.
// Using separate functions instead of ifs and generic logic improves
// execution time significantly, and for many search AIs, these
// are the most time consuming functions in the program.

void Board::shift_board_left() {
    // Iterate the board until finding an empty space
    for(int y = 0; y < m_height; ++y) {
        auto y_idx = y * m_width;
        for(int x = 0; x < m_width; ++x) {
            auto* cell = &m_cells[x + y_idx];
            if(cell->value == Cell::EMPTY) {
                // We found an empty space, now look forward for a non-empty
                // space.
                for(int x2 = x + 1; x2 < m_width; ++x2) {
                    auto& cell2 = m_cells[x2 + y_idx];
                    if(cell2.value != Cell::EMPTY) {
                        // We found a non-empty space.
                        // Swap the empty space and non-empty space.
                        std::swap(*cell, cell2);
                        // If we are not at the edge, try to merge
                        if(x != 0) {
                            Cell* back_cell = &m_cells[x - 1 + y_idx];
                            if(try_merge(*back_cell, *cell)) {
                                // We merged, so there is a new open space.
                                x -= 1;
                            }
                        }
                        // Update to look at the next cell.
                        x += 1;
                        cell = &m_cells[x + y_idx];
                    }
                }
                break;
            } else {
                // Try to merge blocks next to each other that didn't move.
                if(x != 0) {
                    auto& back_cell = m_cells[x - 1 + y_idx];
                    auto& front_cell = m_cells[x + y_idx];
                    if(try_merge(back_cell, front_cell)) {
                        x -= 1;
                    }
                }
            }
        }
    }
}

void Board::shift_board_right() {
    // Iterate the board until finding an empty space
    for(int y = 0; y < m_height; ++y) {
        auto y_idx = y * m_width;
        for(int x = m_width - 1; x >= 0; --x) {
            auto* cell = &m_cells[x + y_idx];
            if(cell->value == Cell::EMPTY) {
                // We found an empty space, now look forward for a non-empty
                // space.
                for(int x2 = x - 1; x2 >= 0; --x2) {
                    auto& cell2 = m_cells[x2 + y_idx];
                    if(cell2.value != Cell::EMPTY) {
                        // We found a non-empty space.
                        // Swap the empty space and non-empty space.
                        std::swap(*cell, cell2);
                        // If we are not at the edge, try to merge
                        if(x != m_width - 1) {
                            Cell* back_cell = &m_cells[x + 1 + y_idx];
                            if(try_merge(*back_cell, *cell)) {
                                // We merged, so there is a new open space.
                                x += 1;
                            }
                        }
                        // Update to look at the next cell.
                        x -= 1;
                        cell = &m_cells[x + y_idx];
                    }
                }
                break;
            } else {
                // Try to merge blocks next to each other that didn't move.
                if(x != m_width - 1) {
                    auto& back_cell = m_cells[x + 1 + y_idx];
                    auto& front_cell = m_cells[x + y_idx];
                    if(try_merge(back_cell, front_cell)) {
                        x += 1;
                    }
                }
            }
        }
    }
}

void Board::shift_board_up() {
    // Iterate the board until finding an empty space
    for(int x = 0; x < m_width; ++x) {
        for(int y = 0; y < m_height; ++y) {
            auto* cell = &m_cells[x + y * m_width];
            if(cell->value == Cell::EMPTY) {
                // We found an empty space, now look forward for a non-empty
                // space.
                for(int y2 = y + 1; y2 < m_height; ++y2) {
                    auto& cell2 = m_cells[x + y2 * m_width];
                    if(cell2.value != Cell::EMPTY) {
                        // We found a non-empty space.
                        // Swap the empty space and non-empty space.
                        std::swap(*cell, cell2);
                        // If we are not at the edge, try to merge
                        if(y != 0) {
                            Cell* back_cell = &m_cells[x + (y - 1) * m_width];
                            if(try_merge(*back_cell, *cell)) {
                                // We merged, so there is a new open space.
                                y -= 1;
                            }
                        }
                        // Update to look at the next cell.
                        y += 1;
                        cell = &m_cells[x + y * m_width];
                    }
                }
                break;
            } else {
                // Try to merge blocks next to each other that didn't move.
                if(y != 0) {
                    auto& back_cell = m_cells[x + (y - 1) * m_width];
                    auto& front_cell = m_cells[x + y * m_width];
                    if(try_merge(back_cell, front_cell)) {
                        y -= 1;
                    }
                }
            }
        }
    }
}

void Board::shift_board_down() {
    // Iterate the board until finding an empty space
    for(int x = 0; x < m_width; ++x) {
        for(int y = m_height - 1; y >= 0; --y) {
            auto* cell = &m_cells[x + y * m_width];
            if(cell->value == Cell::EMPTY) {
                // We found an empty space, now look forward for a non-empty
                // space.
                for(int y2 = y - 1; y2 >= 0; --y2) {
                    auto& cell2 = m_cells[x + y2 * m_width];
                    if(cell2.value != Cell::EMPTY) {
                        // We found a non-empty space.
                        // Swap the empty space and non-empty space.
                        std::swap(*cell, cell2);
                        // If we are not at the edge, try to merge
                        if(y != m_height - 1) {
                            Cell* back_cell = &m_cells[x + (y + 1) * m_width];
                            if(try_merge(*back_cell, *cell)) {
                                // We merged, so there is a new open space.
                                y += 1;
                            }
                        }
                        // Update to look at the next cell.
                        y -= 1;
                        cell = &m_cells[x + y * m_width];
                    }
                }
                break;
            } else {
                // Try to merge blocks next to each other that didn't move.
                if(y != m_height - 1) {
                    auto& back_cell = m_cells[x + (y + 1) * m_width];
                    auto& front_cell = m_cells[x + y * m_width];
                    if(try_merge(back_cell, front_cell)) {
                        y += 1;
                    }
                }
            }
        }
    }
}

bool Board::try_merge(Cell& lhs, Cell& rhs) {
    if(lhs.value == rhs.value) {
        lhs.value *= 2;
        rhs.value = Cell::EMPTY;
        return true;
    }
    return false;
}
