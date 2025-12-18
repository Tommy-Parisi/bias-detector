#pragma once

#include "../bias_signal.hpp"
#include <unordered_map>

/**
 * Signal 1: Outlet Baseline
 * 
 * Based on known outlet bias (domain reputation).
 * Example: outlets.push_back({"foxnews.com", 0.6});  // right-leaning
 */
class OutletBaselineSignal : public BiasSignal {
public:
    OutletBaselineSignal();

    double compute(const NLPContext& ctx,
                  const ArticleInput& article) override;

    std::string explain() const override;
    std::string name() const override { return "OutletBaseline"; }

private:
    // Domain -> bias score
    std::unordered_map<std::string, double> outlet_scores;
    double last_score = 0.0;

    double get_outlet_score(const std::string& domain) const;
};
