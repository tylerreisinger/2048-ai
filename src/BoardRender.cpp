#include "BoardRender.h"

#include <imgui/imgui.h>

#include <iostream>

ImVec2 operator+(ImVec2 lhs, ImVec2 rhs) {
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}
ImVec2& operator+=(ImVec2& lhs, ImVec2 rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

void BoardRenderer::draw(const Board& board, ImDrawList* draw_list) {
    auto pos = ImGui::GetWindowPos();
    auto size = ImGui::GetWindowSize();
    pos += ImGui::GetWindowContentRegionMin();

    auto outer_padding = ImVec2(10.0, 10.0);
    auto inner_padding = ImVec2(2.0, 2.0);
    auto cell_size = ImVec2(64.0, 64.0);

    auto cell_color = 0xFF886644;
    if(board.is_lost()) {
        cell_color = 0xFF2222AA;
    }

    for(int y = 0; y < board.height(); ++y) {
        for(int x = 0; x < board.width(); ++x) {
            const auto& cell = board.get_cell(x, y);
            auto cell_pos = outer_padding + pos;
            cell_pos.x += x * (cell_size.x + inner_padding.x);
            cell_pos.y += y * (cell_size.y + inner_padding.y);

            if(cell.value == Cell::EMPTY) {
                draw_list->AddRectFilled(
                        cell_pos, cell_pos + cell_size, 0xAAAAAAAA, 0.0);
            } else {
                draw_list->AddRectFilled(
                        cell_pos, cell_pos + cell_size, cell_color, 3.0);
                auto text = std::to_string(cell.value);
                draw_list->AddText(cell_pos + ImVec2(26.0, 26.0),
                        0xFFFFFFFF,
                        text.c_str());
            }
        }
    }
}
