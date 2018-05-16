#ifndef TESTCONTROLLER_H_
#define TESTCONTROLLER_H_

#include "AiController.h"

#include <random>

#include "Board.h"

class TestController : public AiController {
public:
    TestController(int delay = 1) : AiController(delay) {}
    virtual ~TestController() = default;

    TestController(const TestController& other) = delete;
    TestController(TestController&& other) noexcept = default;
    TestController& operator=(const TestController& other) = delete;
    TestController& operator=(TestController&& other) noexcept = default;

    virtual void do_turn(Board& board) override;

private:
    double compute_score(const Board& board, ShiftDirection dir) const;


    std::mt19937 m_rgs;
    int m_turn = 0;
};

#endif
