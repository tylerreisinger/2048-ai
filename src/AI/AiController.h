#ifndef AICONTROLLER_H_
#define AICONTROLLER_H_

#include "IGameController.h"

#include <random>

class AiController : public IGameController {
public:
    AiController(int delay_inverval, uint64_t seed = 0)
        : m_delay_interval(delay_inverval) {
        std::seed_seq s = {seed};
        this->seed(s);
    }
    virtual ~AiController() = default;

    int delay_interval() const { return m_delay_interval; }
    void delay_interval(int delay_interval) {
        m_delay_interval = delay_interval;
    }

    virtual void seed(std::seed_seq& seed) {}

private:
    int m_delay_interval = 1;
};

#endif
