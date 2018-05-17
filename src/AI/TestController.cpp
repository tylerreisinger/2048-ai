#include "TestController.h"

void TestController::do_turn(Board& board, const GameTime& time) {
    if(board.is_lost()) {
        return;
    }
    double max_score = 0.0;
    ShiftDirection max_dir = ShiftDirection::Left;
    for(int i = 0; i < 4; ++i) {
        auto dir = static_cast<ShiftDirection>(i);
        Board board_copy = board;
        board_copy.do_move(dir);
        double score = compute_score(board_copy, dir);
        if(score > max_score) {
            max_score = score;
            max_dir = dir;
        }
    }

    board.do_move(max_dir);
}

double TestController::compute_score(
        const Board& board, ShiftDirection dir) const {
    auto raw_score = board.compute_score();
    if(board.get_cell(0, board.height() - 1).value == board.max_value()) {
        raw_score *= 6.0;
    }
    if(dir == ShiftDirection::Up) {
        raw_score *= 0.5;
    } else if(dir == ShiftDirection::Right) {
        raw_score *= 0.8;
    }
    raw_score *=
            (static_cast<double>(board.free_spaces()) / board.total_blocks());
    return raw_score;
}
