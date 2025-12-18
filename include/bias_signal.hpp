#pragma once

#include "types.hpp"
#include "nlp_context.hpp"
#include <string>
#include <memory>

/**
 * Abstract base class for bias signals.
 * 
 * Every bias signal must:
 * 1. Compute a normalized score in [-1.0, +1.0]
 * 2. Provide explanation for its reasoning
 * 3. Have a unique name
 * 
 * This is the Strategy pattern - each signal is independent and testable.
 */
class BiasSignal {
public:
    virtual ~BiasSignal() = default;

    /**
     * Compute the bias score for this signal.
     * @param ctx Shared NLP context
     * @param article Original article input
     * @return Score in [-1.0, +1.0] where negative = left bias, positive = right bias
     */
    virtual double compute(const NLPContext& ctx,
                          const ArticleInput& article) = 0;

    /**
     * Produce a human-readable explanation of the score.
     */
    virtual std::string explain() const = 0;

    /**
     * Unique identifier for this signal (used in weighting).
     */
    virtual std::string name() const = 0;
};
