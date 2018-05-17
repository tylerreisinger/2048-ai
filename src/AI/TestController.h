#ifndef TESTCONTROLLER_H_
#define TESTCONTROLLER_H_

#include "AiController.h"

#include <random>

#include "Board.h"

class TestController : public AiController {
public:
    TestController(uint64_t seed = 0) : AiController(seed) {}
    virtual ~TestController() = default;

    TestController(const TestController& other) = delete;
    TestController(TestController&& other) noexcept = default;
    TestController& operator=(const TestController& other) = delete;
    TestController& operator=(TestController&& other) noexcept = default;

    virtual void do_turn(Board& board, const GameTime& time) override;
    virtual void seed(std::seed_seq& seed) override { m_rng.seed(seed); }

private:
    double compute_score(const Board& board, ShiftDirection dir) const;


    std::mt19937 m_rng;
};

#endif
