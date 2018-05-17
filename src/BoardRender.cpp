#include "BoardRender.h"

#include <imgui/imgui.h>

#include <cmath>
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


    for(int y = 0; y < board.height(); ++y) {
        for(int x = 0; x < board.width(); ++x) {
            const auto& cell = board.get_cell(x, y);
            auto cell_pos = outer_padding + pos;
            cell_pos.x += x * (cell_size.x + inner_padding.x);
            cell_pos.y += y * (cell_size.y + inner_padding.y);
            uint32_t cell_color;
            if(board.is_lost()) {
                cell_color = 0xFF2222AA;
            } else {
                cell_color = color_for_cell(cell.value);
            }

            if(cell.value == Cell::EMPTY) {
                draw_list->AddRectFilled(
                        cell_pos, cell_pos + cell_size, 0xAAAAAAAA, 0.0);
            } else {
                draw_list->AddRectFilled(
                        cell_pos, cell_pos + cell_size, cell_color, 3.0);
                auto text = std::to_string(cell.value);
                draw_list->AddText(cell_pos + ImVec2(26.0, 26.0),
                        0xFF000000,
                        text.c_str());
            }
        }
    }
}

uint32_t BoardRenderer::color_for_cell(const Cell& cell) const {
    switch(static_cast<int>(std::log2(cell.value))) {
    case 1:
        return 0xFFE00073;
    case 2:
        return 0xFFE40042;
    case 3:
        return 0xFFED2300;
    case 4:
        return 0xFFF15900;
    case 5:
        return 0xFFF9CC00;
    case 6:
        return 0xFFE9FA07;
    case 7:
        return 0xFFAFFB0F;
    case 8:
        return 0xFF44FC1E;
    case 9:
        return 0xFF57FF7A;
    case 10:
        return 0xFF46FF95;
    case 11:
        return 0xFF36FFB7;
    case 12:
        return 0xFF25FFE1;
    case 13:
        return 0xFF15EBFF;
    case 14:
        return 0xFF05B3FF;
    case 15:
        return 0xFFBCC2FF;
    case 16:
        return 0xFFD4C2FE;
    default:
        return 0xFFECC6FE;
    }
}
