#ifndef IGAMECONTROLLER_H_
#define IGAMECONTROLLER_H_

#include <SFML/Window/Event.hpp>

class Board;

class IGameController {
public:
    IGameController() = default;
    virtual ~IGameController() = default;

    virtual void do_turn(Board& board) = 0;
    virtual void handle_event(Board& board, const sf::Event& e){};
};

#endif
