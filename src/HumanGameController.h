#ifndef HUMANGAMECONTROLLER_H_
#define HUMANGAMECONTROLLER_H_

#include "IGameController.h"

#include "Board.h"


class HumanGameController : public IGameController {
public:
    HumanGameController() = default;
    virtual ~HumanGameController() = default;

    HumanGameController(const HumanGameController& other) = delete;
    HumanGameController(HumanGameController&& other) noexcept = default;
    HumanGameController& operator=(const HumanGameController& other) = delete;
    HumanGameController& operator=(
            HumanGameController&& other) noexcept = default;

    virtual void do_turn(Board& board, const GameTime& time) override;
    virtual void handle_event(Board& board, const sf::Event& e) override;

private:
};

#endif
