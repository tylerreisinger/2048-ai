#ifndef RANDOMCONTROLLER_H_
#define RANDOMCONTROLLER_H_

#include "AiController.h"

#include <random>

class RandomController : public AiController {
public:
    RandomController() = default;
    RandomController(int delay_interval) : AiController(delay_interval) {}
    virtual ~RandomController() = default;

    RandomController(const RandomController& other) = delete;
    RandomController(RandomController&& other) noexcept = default;
    RandomController& operator=(const RandomController& other) = delete;
    RandomController& operator=(RandomController&& other) noexcept = default;

    virtual void do_turn(Board& board) override;

private:
    std::mt19937 m_rng;
    int m_turn = 0;
};

#endif
