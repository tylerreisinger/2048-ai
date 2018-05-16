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
    MctsController(int delay) : AiController(delay) {}
    virtual ~MctsController() = default;

    MctsController(const MctsController& other) = delete;
    MctsController(MctsController&& other) noexcept = default;
    MctsController& operator=(const MctsController& other) = delete;
    MctsController& operator=(MctsController&& other) noexcept = default;

    virtual void do_turn(Board& board) override;

private:
    bool do_trial(Board& board, ShiftDirection dir);
    std::tuple<double, bool> do_trial_to_depth(
            Board board, ShiftDirection start_dir, int depth);
    MctsOutput do_trials(
            Board board, ShiftDirection start_dir, int depth, int count);
    double score_function(const Board& board, ShiftDirection dir) const;


    std::mt19937 m_rng;
    std::uniform_int_distribution<int> m_move_dist =
            std::uniform_int_distribution<int>(0, 3);
    int m_time = 0;
};

#endif
