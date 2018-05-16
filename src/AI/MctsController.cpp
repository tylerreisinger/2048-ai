#include "MctsController.h"

#include <iostream>

void MctsController::do_turn(Board& board) {
    if(board.is_lost()) {
        return;
    }
    m_time += 1;
    if(m_time < delay_interval()) {
        return;
    } else {
        m_time = 0;
    }
    double max_score = 0.0;
    double max_avg_score = 0.0;
    int max_fails = 0;
    int min_fails = 0;
    ShiftDirection max_dir = ShiftDirection::Left;
    ShiftDirection avg_dir = ShiftDirection::Left;
    ShiftDirection loss_dir = ShiftDirection::Left;

    for(int i = 0; i < 4; ++i) {
        ShiftDirection dir = static_cast<ShiftDirection>(i);
        auto out = do_trials(board, dir, 10, 5000);
        if(out.avg_score > max_avg_score) {
            max_avg_score = out.avg_score;
            avg_dir = dir;
        }
        if(out.max_score > max_score) {
            max_score = out.max_score;
            max_dir = dir;
        }
        if(out.losses > max_fails) {
            max_fails = out.losses;
        }
        if(out.losses < min_fails) {
            min_fails = out.losses;
            loss_dir = dir;
        }
    }

    std::cout << "Max Score: " << max_score << "(" << board.compute_score()
              << ") in dir " << max_dir << std::endl;
    std::cout << "\tAvg Score: " << max_avg_score << std::endl;
    std::cout << "\tMax Fails: " << max_fails << " Min Fails: " << min_fails
              << std::endl;

    // if(max_fails > 22 && min_fails < max_fails) {
    //    board.do_move(loss_dir);
    //} else {
    board.do_move(max_dir);
    //}
}

bool MctsController::do_trial(Board& board, ShiftDirection dir) {
    board.do_move(dir);
    return true;
}

std::tuple<double, bool> MctsController::do_trial_to_depth(
        Board board, ShiftDirection start_dir, int depth) {
    auto dir = start_dir;
    for(int i = 0; i < depth; ++i) {
        if(!do_trial(board, start_dir)) {
            i -= 1;
        }
        if(board.is_lost()) {
            return std::make_tuple(0.0, true);
        }
        start_dir = static_cast<ShiftDirection>(m_move_dist(m_rng));
    }
    return std::tuple(score_function(board, dir), false);
}

MctsOutput MctsController::do_trials(
        Board board, ShiftDirection start_dir, int depth, int count) {
    double out_value = 0.0;
    double max_value = 0.0;
    int losses = 0;

    for(int i = 0; i < count; ++i) {
        auto [val, lose] = do_trial_to_depth(board, start_dir, depth);
        losses += lose ? 1 : 0;
        /*
        if(start_dir == ShiftDirection::Left) {
            val *= 2.0;
        }
        else if(start_dir == ShiftDirection::Down) {
            val *= 2.0;
        }
        else if(start_dir == ShiftDirection::Right) {
            val *= 1.25;
        }
        */
        max_value = std::max(max_value, val);
        out_value += val;
        start_dir = static_cast<ShiftDirection>(m_move_dist(m_rng));
    }

    out_value /= count;
    return {max_value, out_value, losses};
}

double MctsController::score_function(
        const Board& board, ShiftDirection dir) const {
    auto score = board.compute_score();
    switch(dir) {
    case ShiftDirection::Down:
        return score * 2.0;
    case ShiftDirection::Left:
        return score * 2.0;
    case ShiftDirection::Right:
        return score * 1.5;
    case ShiftDirection::Up:
        return score * 0.666;
    }
}
