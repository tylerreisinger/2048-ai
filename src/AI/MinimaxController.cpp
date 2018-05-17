#include "MinimaxController.h"

#include <iostream>

#include <imgui/imgui.h>

double powi(double base, int pow) {
    double out = base;
    for(int i = 1; i < pow; ++i) {
        out *= base;
    }
    return out;
}

void MinimaxController::do_turn(Board& board, const GameTime& time) {
    if(board.is_lost()) {
        return;
    }
    MinimaxStats stats;
    auto [maybe_move, score] = minimax(board, 6, stats);
    if(score < 0.00000001) {
        std::tie(maybe_move, score) = minimax(board, 4, stats);
    }
    if(score < 0.00000001) {
        std::tie(maybe_move, score) = minimax(board, 2, stats);
    }
    if(score < 0.00000001) {
        std::tie(maybe_move, score) = minimax(board, 0, stats);
    }

    board.do_move(static_cast<ShiftDirection>(maybe_move));
    m_stats = stats;
}

std::tuple<MaybeMove, double> MinimaxController::minimax(
        Board& board, int depth, MinimaxStats& stats) {
    return minimax_max(board, depth, stats);
}

double MinimaxController::score_board(const Board& board) {
    auto score = board.compute_score() * powi(0.95, board.filled_spaces());
    if(board.free_spaces() == 1) {
        score *= 0.25;
    } /* else if(board.free_spaces() == 2) {
         score *= 0.33;
     }
     if(board.get_cell(0, board.height()-1).value == board.max_value()) {
         score *= 2.0;
     }
     if(board.get_cell(0, board.height()-2).value == board.max_value()) {
         score *= 1.5;
     }
     if(board.get_cell(1, board.height()-1).value == board.max_value()) {
         score *= 1.5;
     }
     */

    return score;
}

std::tuple<MaybeMove, double> MinimaxController::minimax_max(
        Board& board, int depth, MinimaxStats& stats) {
    double max_score = std::numeric_limits<double>::min();
    ShiftDirection max_dir = ShiftDirection::Left;
    for(int i = 0; i < 4; ++i) {
        auto dir = static_cast<ShiftDirection>(i);
        Board board_copy = board;
        auto works = board_copy.shift_board(dir);
        stats.nodes_evaluated += 1;
        if(!works) {
            continue;
        }

        double score;
        if(depth > 0) {
            score = minimax_min(board_copy, depth - 1, stats);
        } else {
            score = score_board(board_copy);
        }
        score *= score_move(dir);

        if(score > max_score) {
            max_score = score;
            max_dir = dir;
        }
    }
    stats.max_score = std::max(stats.max_score, max_score);
    return std::tuple(static_cast<MaybeMove>(max_dir), max_score);
}

double MinimaxController::minimax_min(
        Board& board, int depth, MinimaxStats& stats) {
    double max_score = std::numeric_limits<double>::min();
    int max_idx = 0;
    for(int i = 0; i < board.total_blocks(); ++i) {
        auto cell = board.get_cell(i);
        if(!cell.is_empty()) {
            continue;
        }
        Board board_copy = board;
        board_copy.get_cell(i) = Cell(2);
        stats.nodes_evaluated += 1;

        auto score = 0.0;
        if(board_copy.is_filled()) {
            score = std::numeric_limits<double>::min();
            continue;
        }
        if(depth > 0) {
            auto [move, out_score] = minimax_max(board_copy, depth - 1, stats);
            score = out_score;
        } else {
            score = score_board(board_copy);
        }

        if(score > max_score) {
            max_score = score;
            max_idx = i;
        }
    }
    return max_score;
}

void MinimaxController::draw_state(const Board& board, const GameTime& time) {
    ImGui::Begin("Controller State");
    ImGui::BulletText("Nodes Evaluated: %d", m_stats.nodes_evaluated);
    ImGui::BulletText("Max Score: %f", m_stats.max_score);
    ImGui::End();
}

double MinimaxController::score_move(ShiftDirection dir) {
    /*if(dir == ShiftDirection::Up) {
        return 0.25;
    } else if(dir == ShiftDirection::Right) {
        return 0.60;
    }*/
    return 1.00;
}
