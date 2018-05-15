#ifndef GAME_H_
#define GAME_H_

#include "Board.h"

class Game {
public:
    Game() = default;
    ~Game() = default;

    Game(const Game& other) = delete;
    Game(Game&& other) noexcept = default;
    Game& operator=(const Game& other) = delete;
    Game& operator=(Game&& other) noexcept = default;

private:
    Board m_board;
};

#endif
