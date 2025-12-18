#include "../../include/signals/policy_framing_signal.hpp"
#include <sstream>
#include <algorithm>

double PolicyFramingSignal::compute(const NLPContext& ctx,
                                     const ArticleInput& article) {
    std::vector<std::string> left_terms_list = {
        "inequality", "climate", "action", "regulation",
        "workers", "rights", "justice", "welfare",
        "progressive", "reform", "taxes", "rich"
    };

    std::vector<std::string> right_terms_list = {
        "freedom", "liberty", "market", "deregulation",
        "business", "growth", "entrepreneur", "innovation",
        "conservative", "traditional", "fiscal", "taxes",
        "government"  // in negative context, but we'll weight it
    };

    left_terms = 0;
    right_terms = 0;

    for (const auto& token : ctx.tokens) {
        for (const auto& term : left_terms_list) {
            if (token == term) {
                left_terms++;
            }
        }
        for (const auto& term : right_terms_list) {
            if (token == term) {
                right_terms++;
            }
        }
    }

    // Normalize to [-1, 1]
    int total = left_terms + right_terms;
    if (total == 0) {
        return 0.0;  // No policy language detected
    }

    double left_ratio = static_cast<double>(left_terms) / total;
    double right_ratio = static_cast<double>(right_terms) / total;

    // Bias score: right_ratio - left_ratio, range [-1, 1]
    // Positive = right-leaning, Negative = left-leaning
    double score = right_ratio - left_ratio;
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
