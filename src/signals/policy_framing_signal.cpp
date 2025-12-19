#include "../../include/signals/policy_framing_signal.hpp"
#include <sstream>
#include <algorithm>
#include <map>
#include <cmath>

double PolicyFramingSignal::compute(const NLPContext& ctx,
                                     const ArticleInput& article) {
    // Define framing terms with directionality
    // These are high-conviction bias indicators
    std::map<std::string, int> left_frames = {
        {"inequality", 2}, {"climate", 1}, {"action", 1}, {"regulation", 1},
        {"workers", 1}, {"rights", 2}, {"justice", 2}, {"welfare", 1},
        {"progressive", 2}, {"reform", 1}, {"revenue", 1}, {"investment", 1},
        {"equity", 2}, {"protection", 1}, {"safety", 1}, {"access", 1}
    };

    std::map<std::string, int> right_frames = {
        {"freedom", 2}, {"liberty", 2}, {"market", 1}, {"deregulation", 2},
        {"business", 1}, {"growth", 1}, {"entrepreneur", 1}, {"innovation", 1},
        {"conservative", 1}, {"traditional", 1}, {"fiscal", 1}, {"burden", 2},
        {"radical", 2}, {"excessive", 1}, {"control", 1}, {"efficiency", 1}
    };

    // Define entities that carry political meaning
    std::vector<std::string> left_entities = {
        "climate", "environment", "healthcare", "education", "welfare",
        "medicare", "social", "workers", "labor", "unions", "poor", "minorities"
    };

    std::vector<std::string> right_entities = {
        "business", "market", "economy", "tax", "profit", "shareholders",
        "enterprise", "corporations", "regulation", "government", "spending"
    };

    // Negative sentiment amplifiers (used with either frame to boost bias)
    std::vector<std::string> negative_words = {
        "dangerous", "threatens", "costly", "radical", "reckless", "failing",
        "burden", "crisis", "disaster", "extreme", "harmful", "destructive"
    };

    std::vector<std::string> positive_words = {
        "vital", "essential", "innovative", "freedom", "opportunity", "benefits",
        "thriving", "success", "leadership", "growth", "progress", "reform"
    };

    // Calculate bigrams from tokens
    left_terms = 0;
    right_terms = 0;
    double weighted_left_score = 0.0;
    double weighted_right_score = 0.0;

    // Bigram detection for context-rich framing
    std::map<std::string, int> bigram_left_frames = {
        {"climate action", 2}, {"social justice", 2}, {"workers rights", 2},
        {"public investment", 1}, {"healthcare reform", 1}, {"environmental protection", 2}
    };

    std::map<std::string, int> bigram_right_frames = {
        {"free market", 2}, {"economic growth", 1}, {"job creation", 1},
        {"business freedom", 2}, {"government overreach", 2}, {"fiscal responsibility", 2}
    };

    // Check bigrams (two consecutive tokens)
    for (size_t i = 0; i < ctx.tokens.size() - 1; ++i) {
        std::string bigram = ctx.tokens[i] + " " + ctx.tokens[i+1];
        
        if (bigram_left_frames.find(bigram) != bigram_left_frames.end()) {
            weighted_left_score += bigram_left_frames[bigram] * 2;  // Bigrams get 2x weight
            left_terms++;
        }
        if (bigram_right_frames.find(bigram) != bigram_right_frames.end()) {
            weighted_right_score += bigram_right_frames[bigram] * 2;
            right_terms++;
        }
    }

    // Single-token frame detection with context
    for (size_t i = 0; i < ctx.tokens.size(); ++i) {
        const auto& token = ctx.tokens[i];

        // Check left frames
        if (left_frames.find(token) != left_frames.end()) {
            int weight = left_frames[token];
            
            // Check context (previous or next token for sentiment/entity reinforcement)
            int context_boost = 1;
            if (i > 0 && (std::find(negative_words.begin(), negative_words.end(), ctx.tokens[i-1]) != negative_words.end())) {
                context_boost = 2;  // Negative context amplifies left bias
            }
            if (i < ctx.tokens.size() - 1 && (std::find(positive_words.begin(), positive_words.end(), ctx.tokens[i+1]) != positive_words.end())) {
                context_boost = 2;  // Positive context amplifies left bias
            }
            
            left_terms++;
            weighted_left_score += weight * context_boost;
        }

        // Check right frames
        if (right_frames.find(token) != right_frames.end()) {
            int weight = right_frames[token];
            
            // Check context for reinforcement
            int context_boost = 1;
            if (i > 0 && (std::find(negative_words.begin(), negative_words.end(), ctx.tokens[i-1]) != negative_words.end())) {
                context_boost = 2;
            }
            if (i < ctx.tokens.size() - 1 && (std::find(positive_words.begin(), positive_words.end(), ctx.tokens[i+1]) != positive_words.end())) {
                context_boost = 2;
            }
            
            right_terms++;
            weighted_right_score += weight * context_boost;
        }
    }

    // Normalize to [-1, 1]
    // Use weighted scores instead of simple counts
    double total_weight = weighted_left_score + weighted_right_score;
    if (total_weight < 0.1) {
        return 0.0;  // No significant policy framing detected
    }

    // Bias score: (right_weight - left_weight) / total_weight
    // Positive = right-leaning, Negative = left-leaning
    double score = (weighted_right_score - weighted_left_score) / total_weight;
    
    // Clamp to [-1, 1]
    score = std::max(-1.0, std::min(1.0, score));
    
    return score;
}

std::string PolicyFramingSignal::explain() const {
    std::ostringstream oss;
    oss << "Policy framing: " << left_terms << " left-aligned terms, "
        << right_terms << " right-aligned terms";
    
    if (left_terms > right_terms) {
        oss << " → predominantly left-framed language";
    } else if (right_terms > left_terms) {
        oss << " → predominantly right-framed language";
    } else {
        oss << " → balanced language";
    }
    
    return oss.str();
}
