#pragma once

#include "../bias_signal.hpp"

/**
 * Signal 2: Entity Sentiment
 * 
 * Measures sentiment directed at left-aligned vs. right-aligned entities.
 * Left entities with negative sentiment = left bias signal
 * Right entities with negative sentiment = right bias signal
 */
class EntitySentimentSignal : public BiasSignal {
public:
    double compute(const NLPContext& ctx,
                  const ArticleInput& article) override;

    std::string explain() const override;
    std::string name() const override { return "EntitySentiment"; }

private:
    double left_avg = 0.0;
    double right_avg = 0.0;
};
