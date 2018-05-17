#ifndef AICONTROLLER_H_
#define AICONTROLLER_H_

#include "IGameController.h"

#include <random>

class AiController : public IGameController {
public:
    AiController(uint64_t seed = 0) {
        std::seed_seq s = {seed};
        this->seed(s);
    }
    virtual ~AiController() = default;

    virtual void seed(std::seed_seq& seed) {}

private:
};

#endif
