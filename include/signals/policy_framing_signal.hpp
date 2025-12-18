#pragma once

#include "../bias_signal.hpp"

/**
 * Signal 3: Policy Framing
 * 
 * Detects which policy language appears more frequently.
 * Left-aligned terms: "inequality", "climate action", "regulation"
 * Right-aligned terms: "freedom", "deregulation", "free market"
 */
class PolicyFramingSignal : public BiasSignal {
public:
    double compute(const NLPContext& ctx,
                  const ArticleInput& article) override;

    std::string explain() const override;
    std::string name() const override { return "PolicyFraming"; }

private:
    int left_terms = 0;
    int right_terms = 0;
};
