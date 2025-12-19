#include "../include/bias_aggregator.hpp"
#include "../include/signals/outlet_baseline_signal.hpp"
#include "../include/signals/entity_sentiment_signal.hpp"
#include "../include/signals/policy_framing_signal.hpp"
#include "../include/signals/emotional_direction_signal.hpp"
#include "../include/signals/semantic_bias_signal.hpp"
#include <numeric>
#include <sstream>

BiasAggregator::BiasAggregator() {
    // Register all signals
    signals.push_back(std::make_unique<OutletBaselineSignal>());
    signals.push_back(std::make_unique<EntitySentimentSignal>());
    signals.push_back(std::make_unique<PolicyFramingSignal>());
    signals.push_back(std::make_unique<EmotionalDirectionSignal>());
    signals.push_back(std::make_unique<SemanticBiasSignal>());

    // Set default weights
    weights = {
        {"OutletBaseline", 0.15},
        {"EntitySentiment", 0.30},
        {"PolicyFraming", 0.20},
        {"EmotionalDirection", 0.15},
        {"SemanticBias", 0.20}  // New semantic layer
    };

    // Normalize weights
    normalize_weights();
}

BiasResult BiasAggregator::analyze(const ArticleInput& article) {
    // Step 1: Preprocess
    NLPContext ctx = preprocessor.process(article);

    // Step 2: Refusal logic
    if (insufficient_data(ctx)) {
        return BiasResult{
            .score = 0.0,
            .label = "Insufficient Data",
            .confidence = 0.0,
            .explanations = {"Article is too short or has too few entities for reliable analysis"}
        };
    }

    // Step 3: Compute all signals
    std::vector<double> signal_scores;
    std::vector<std::string> explanations;

    for (auto& signal : signals) {
        double score = signal->compute(ctx, article);
        signal_scores.push_back(score);
        explanations.push_back(signal->explain());
    }

    // Step 4: Weighted aggregate
    double weighted_sum = 0.0;
    double weight_sum = 0.0;

    size_t i = 0;
    for (auto& signal : signals) {
        auto it = weights.find(signal->name());
        double weight = (it != weights.end()) ? it->second : 0.0;
        
        weighted_sum += signal_scores[i] * weight;
        weight_sum += weight;
        i++;
    }

    double aggregate_score = (weight_sum > 0) ? weighted_sum / weight_sum : 0.0;

    // Clamp to [-1, 1]
    aggregate_score = std::max(-1.0, std::min(1.0, aggregate_score));

    // Step 5: Compute confidence
    double confidence = compute_confidence(ctx, signal_scores);

    // Step 6: Bucket label
    std::string label = bucket_label(aggregate_score);

    return BiasResult{
        .score = aggregate_score,
        .label = label,
        .confidence = confidence,
        .explanations = explanations
    };
}

void BiasAggregator::set_signal_weight(const std::string& signal_name, double weight) {
    weights[signal_name] = weight;
    normalize_weights();
}

bool BiasAggregator::insufficient_data(const NLPContext& ctx) const {
    // Thresholds: minimum token count and entity count
    return ctx.token_count() < 100 || ctx.entity_count() < 1;
}

double BiasAggregator::compute_confidence(const NLPContext& ctx,
                                          const std::vector<double>& scores) const {
    if (scores.empty()) {
        return 0.0;
    }

    // Confidence factors:
    // 1. Agreement between signals (lower variance = higher confidence)
    // 2. Data quantity (more entities/tokens = higher confidence)
    // 3. Entity diversity

    // Calculate signal agreement
    double mean_score = std::accumulate(scores.begin(), scores.end(), 0.0) / scores.size();
    double variance = 0.0;
    for (double score : scores) {
        variance += (score - mean_score) * (score - mean_score);
    }
    variance /= scores.size();

    // Agreement: 1 / (1 + variance) bounded to [0, 1]
    double agreement_confidence = 1.0 / (1.0 + variance);

    // Data quantity: sigmoid on token count and entity count
    double token_ratio = std::min(1.0, static_cast<double>(ctx.token_count()) / 1000.0);
    double entity_ratio = std::min(1.0, static_cast<double>(ctx.entity_count()) / 10.0);
    double data_confidence = (token_ratio + entity_ratio) / 2.0;

    // Combined confidence (average)
    return (agreement_confidence + data_confidence) / 2.0;
}

std::string BiasAggregator::bucket_label(double score) const {
    if (score >= 0.6) {
        return "Strong Right";
    } else if (score >= 0.3) {
        return "Moderate Right";
    } else if (score >= 0.1) {
        return "Slight Right";
    } else if (score > -0.1) {
        return "Neutral";
    } else if (score >= -0.3) {
        return "Slight Left";
    } else if (score >= -0.6) {
        return "Moderate Left";
    } else {
        return "Strong Left";
    }
}

void BiasAggregator::normalize_weights() {
    double weight_sum = 0.0;
    for (const auto& [name, weight] : weights) {
        weight_sum += weight;
    }

    if (weight_sum > 0) {
        for (auto& [name, weight] : weights) {
            weight /= weight_sum;
        }
    }
}
