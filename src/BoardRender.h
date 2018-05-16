#ifndef BOARDRENDER_H_
#define BOARDRENDER_H_

#include "Board.h"

class ImDrawList;

class BoardRenderer {
public:
    BoardRenderer() = default;
    ~BoardRenderer() = default;

    BoardRenderer(const BoardRenderer& other) = delete;
    BoardRenderer(BoardRenderer&& other) noexcept = default;
    BoardRenderer& operator=(const BoardRenderer& other) = delete;
    BoardRenderer& operator=(BoardRenderer&& other) noexcept = default;

    void draw(const Board& board, ImDrawList* draw_list);

    uint32_t color_for_cell(const Cell& cell) const;

private:
};

#endif
