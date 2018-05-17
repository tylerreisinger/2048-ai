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
            board.do_move(ShiftDirection::Left);
            break;
        case sf::Keyboard::Right:
            board.do_move(ShiftDirection::Right);
            break;
        case sf::Keyboard::Up:
            board.do_move(ShiftDirection::Up);
            break;
        case sf::Keyboard::Down:
            board.do_move(ShiftDirection::Down);
            break;
        default:
            break;
        }
        break;
    }
    default: break;
    }
}
