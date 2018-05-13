#include "Board.h"

Board::Board(int width, int height) : m_width(width), m_height(height) {
    m_cells.resize(width * height);
}
