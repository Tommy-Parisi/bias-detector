#pragma once

#include "types.hpp"
#include "nlp_context.hpp"
#include "preprocessor.hpp"
#include "bias_signal.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

/**
 * BiasAggregator: Main orchestrator.
 * 
 * Responsibilities:
 * 1. Run preprocessor on article
 * 2. Compute all bias signals
 * 3. Aggregate with weights
 * 4. Compute confidence
 * 5. Apply refusal logic
 * 6. Return final BiasResult
 * 
 * This is where the math lives.
 */
class BiasAggregator {
public:
    BiasAggregator();

    /**
     * Main entry point: analyze an article.
     * @param article Raw article input
     * @return BiasResult with score, label, confidence, and explanations
     */
    BiasResult analyze(const ArticleInput& article);

    /**
     * Set custom weights for signals (default: predefined weights)
     * @param signal_name Name from BiasSignal::name()
     * @param weight Value in [0.0, 1.0]
     */
    void set_signal_weight(const std::string& signal_name, double weight);

private:
    Preprocessor preprocessor;
    std::vector<std::unique_ptr<BiasSignal>> signals;
    std::unordered_map<std::string, double> weights;

    // Refusal logic
    bool insufficient_data(const NLPContext& ctx) const;

    // Scoring
    double compute_confidence(const NLPContext& ctx,
                             const std::vector<double>& scores) const;

    // Bucketing
    std::string bucket_label(double score) const;

    // Normalize weights to sum to 1.0
    void normalize_weights();
};
