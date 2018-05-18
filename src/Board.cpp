#include "Board.h"

#include <cmath>
#include <iostream>

std::ostream& operator<<(std::ostream& stream, sf::Vector2<int> v) {
    stream << "(" << v.x << "," << v.y << ")";
    return stream;
}

Board::Board(int width, int height, uint64_t seed)
    : m_width(width), m_height(height) {
    // m_cells.resize(width * height);
    std::seed_seq s = {seed >> 32, seed & 0xFFFFFFFF};
    m_rng.seed(s);
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

int Board::get_new_cell_val() {
    int two_or_four = m_rng() % 10;
    if(two_or_four == 9) {
        return 4;
    } else {
        return 2;
    }
}


void Board::add_new_block() {
    int free = free_spaces();
    if(free == 0) {
        m_is_lost = true;
        return;
    }

    int selector = m_rng() % free;
    int val = get_new_cell_val();

    int free_cnt = 0;
    for(std::size_t i = 0; i < m_cells.size(); ++i) {
        if(m_cells[i].value == Cell::EMPTY) {
            if(free_cnt == selector) {
                m_cells[i].value = val;
                return;
            }
            free_cnt += 1;
        }
    }
}

bool Board::do_move(ShiftDirection dir) {
    if(m_is_lost) {
        return true;
    }
    auto ret = shift_board(dir);
    if(ret) {
        add_new_block();
    } else {
        m_is_lost = is_filled();
    }
    return ret;
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

bool Board::shift_board(ShiftDirection dir) {
    bool is_modified = false;
    switch(dir) {
    case ShiftDirection::Left:
        is_modified = shift_board_left();
        break;
    case ShiftDirection::Right:
        is_modified = shift_board_right();
        break;
    case ShiftDirection::Up:
        is_modified = shift_board_up();
        break;
    case ShiftDirection::Down:
        is_modified = shift_board_down();
        break;
    }
    if(is_modified) {
        m_turn += 1;
    }
    return is_modified;
}

// These functions are written to be fast, as opposed to clean.
// Using separate functions instead of ifs and generic logic improves
// execution time significantly, and for many search AIs, these
// are the most time consuming functions in the program.

bool Board::shift_board_left() {
    bool is_modified = false;
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
                        is_modified = true;
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
                        is_modified = true;
                        x -= 1;
                    }
                }
            }
        }
    }
    return is_modified;
}

bool Board::shift_board_right() {
    bool is_modified = false;
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
                        is_modified = true;
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
                        is_modified = true;
                        x += 1;
                    }
                }
            }
        }
    }
    return is_modified;
}

bool Board::shift_board_up() {
    bool is_modified = false;
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
                        is_modified = true;
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
                        is_modified = true;
                        y -= 1;
                    }
                }
            }
        }
    }
    return is_modified;
}

bool Board::shift_board_down() {
    bool is_modified = false;
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
                        is_modified = true;
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
                        is_modified = true;
                        y += 1;
                    }
                }
            }
        }
    }
    return is_modified;
}

bool Board::try_merge(Cell& lhs, Cell& rhs) {
    if(lhs.value == rhs.value) {
        lhs.value *= 2;
        rhs.value = Cell::EMPTY;
        return true;
    }
    return false;
}
