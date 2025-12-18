#pragma once

#include "../bias_signal.hpp"

/**
 * Signal 4: Emotional Direction
 * 
 * Measures emotional intensity directed at left vs. right entities.
 * High emotion + left entity negative sentiment = left bias
 * High emotion + right entity negative sentiment = right bias
 */
class EmotionalDirectionSignal : public BiasSignal {
public:
    double compute(const NLPContext& ctx,
                  const ArticleInput& article) override;

    std::string explain() const override;
    std::string name() const override { return "EmotionalDirection"; }

private:
    double left_emotion = 0.0;
    double right_emotion = 0.0;
};
