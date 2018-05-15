#ifndef AICONTROLLER_H_
#define AICONTROLLER_H_

#include "IGameController.h"

class AiController : public IGameController {
public:
    AiController(int delay_inverval) : m_delay_interval(delay_inverval) {}
    virtual ~AiController() = default;

    int delay_interval() const { return m_delay_interval; }
    void delay_interval(int delay_interval) {
        m_delay_interval = delay_interval;
    }

private:
    int m_delay_interval = 1;
};

#endif
