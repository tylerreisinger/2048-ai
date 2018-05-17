#ifndef RANDOMCONTROLLER_H_
#define RANDOMCONTROLLER_H_

#include "AiController.h"

#include <random>

class RandomController : public AiController {
public:
    RandomController() = default;
    RandomController(int delay_interval, uint64_t seed)
        : AiController(delay_interval, seed) {}
    virtual ~RandomController() = default;

    RandomController(const RandomController& other) = delete;
    RandomController(RandomController&& other) noexcept = default;
    RandomController& operator=(const RandomController& other) = delete;
    RandomController& operator=(RandomController&& other) noexcept = default;

    virtual void do_turn(Board& board) override;
    virtual void seed(std::seed_seq& seed) override { m_rng.seed(seed); }

private:
    std::mt19937 m_rng;
    int m_turn = 0;
};

#endif
