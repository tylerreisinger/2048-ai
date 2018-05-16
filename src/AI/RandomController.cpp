#include "RandomController.h"

#include "Board.h"

void RandomController::do_turn(Board& board) {
    if(board.is_lost()) {
        return;
    }
    m_turn += 1;
    if(m_turn < delay_interval()) {
        return;
    } else {
        m_turn = 0;
    }
    std::uniform_int_distribution<int> dist(0, 3);
    auto choice = dist(m_rng);

    auto shift = ShiftDirection::Left;
    switch(choice) {
    case 0:
        shift = ShiftDirection::Left;
        break;
    case 1:
        shift = ShiftDirection::Left;
        break;
    case 2:
        shift = ShiftDirection::Down;
        break;
    case 3:
        shift = ShiftDirection::Down;
        break;
    default:
        std::terminate();
    }

    board.shift_board(shift);
    board.add_new_block();
}
