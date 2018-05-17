#include "RandomController.h"

#include "Board.h"

void RandomController::do_turn(Board& board, const GameTime& time) {
    if(board.is_lost()) {
        return;
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

    if(!board.do_move(shift)) {
        board.do_move(ShiftDirection::Right);
    }
}
