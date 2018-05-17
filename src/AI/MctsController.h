#ifndef MCTSCONTROLLER_H_
#define MCTSCONTROLLER_H_

#include "AiController.h"
#include "Board.h"

#include <random>

struct MctsOutput {
    double max_score;
    double avg_score;
    int losses;
};

class MctsController : public AiController {
public:
    MctsController() = default;
    MctsController(uint64_t seed = 0) : AiController(seed) {}
    virtual ~MctsController() = default;

    MctsController(const MctsController& other) = delete;
    MctsController(MctsController&& other) noexcept = default;
    MctsController& operator=(const MctsController& other) = delete;
    MctsController& operator=(MctsController&& other) noexcept = default;

    virtual void do_turn(Board& board, const GameTime& time) override;
    virtual void seed(std::seed_seq& seed) override { m_rng.seed(seed); }

private:
    bool do_trial(Board& board, ShiftDirection dir);
    std::tuple<double, bool> do_trial_to_depth(
            Board board, ShiftDirection start_dir, int depth);
    MctsOutput do_trials(
            Board board, ShiftDirection start_dir, int depth, int count);
    double score_function(const Board& board, ShiftDirection dir) const;


    std::minstd_rand m_rng;
    std::uniform_int_distribution<int> m_move_dist =
            std::uniform_int_distribution<int>(0, 3);
};

#endif
