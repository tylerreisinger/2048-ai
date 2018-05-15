#include "HumanGameController.h"

#include <iostream>

#include <SFML/Window.hpp>

void HumanGameController::do_turn(Board& board) {}

void HumanGameController::handle_event(Board& board, const sf::Event& e) {
    if(board.is_lost()) {
        return;
    }
    switch(e.type) {
    case sf::Event::KeyPressed: {
        switch(e.key.code) {
        case sf::Keyboard::Left:
            board.shift_board(ShiftDirection::Left);
            board.add_new_block();
            break;
        case sf::Keyboard::Right:
            board.shift_board(ShiftDirection::Right);
            board.add_new_block();
            break;
        case sf::Keyboard::Up:
            board.shift_board(ShiftDirection::Up);
            board.add_new_block();
            break;
        case sf::Keyboard::Down:
            board.shift_board(ShiftDirection::Down);
            board.add_new_block();
            break;
        default:
            break;
        }
        break;
    }
    }
}
