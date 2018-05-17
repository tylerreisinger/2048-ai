#include "MinimaxController.h"

#include <iostream>

void MinimaxController::do_turn(Board& board, const GameTime& time) {
    if(board.is_lost()) {
        return;
    }
    MinimaxStats stats;
    auto [maybe_move, score] = minimax(board, 4, true, stats);

    std::cout << "Nodes evaluated: " << stats.nodes_evaluated << std::endl;
    std::cout << "\tMax Score: " << stats.max_score << std::endl;
    std::cout << "\tDir: " << static_cast<ShiftDirection>(maybe_move)
              << std::endl;

    board.do_move(static_cast<ShiftDirection>(maybe_move));
}

std::tuple<MaybeMove, double> MinimaxController::minimax(
        Board& board, int depth, bool is_maximizing, MinimaxStats& stats) {
    return minimax_max(board, depth, stats);
}

double MinimaxController::score_board(const Board& board) {
    return board.compute_score() *
            (static_cast<double>(board.free_spaces()) / board.total_blocks());
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
