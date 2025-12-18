#include "../../include/signals/outlet_baseline_signal.hpp"
#include <sstream>

OutletBaselineSignal::OutletBaselineSignal() {
    // Seed with known outlet biases (simplified)
    outlet_scores["foxnews.com"] = 0.7;          // right
    outlet_scores["msnbc.com"] = -0.6;           // left
    outlet_scores["cnn.com"] = -0.2;             // slight left
    outlet_scores["bbc.com"] = -0.05;            // neutral/slight left
    outlet_scores["apnews.com"] = 0.0;           // neutral
    outlet_scores["reuters.com"] = 0.0;          // neutral
    outlet_scores["nytimes.com"] = -0.3;         // left-leaning
    outlet_scores["wsj.com"] = 0.2;              // slight right
    outlet_scores["breitbart.com"] = 0.8;        // far right
    outlet_scores["huffpost.com"] = -0.5;        // left
}

double OutletBaselineSignal::compute(const NLPContext& ctx,
                                      const ArticleInput& article) {
    last_score = get_outlet_score(article.domain);
    return last_score;
}

std::string OutletBaselineSignal::explain() const {
    std::ostringstream oss;
    oss << "Outlet baseline bias: " << last_score;
    if (last_score > 0.5) {
        oss << " (strong right-leaning outlet)";
    } else if (last_score > 0.2) {
        oss << " (moderately right-leaning outlet)";
    } else if (last_score < -0.5) {
        oss << " (strong left-leaning outlet)";
    } else if (last_score < -0.2) {
        oss << " (moderately left-leaning outlet)";
    } else {
        oss << " (neutral outlet)";
    }
    return oss.str();
}

double OutletBaselineSignal::get_outlet_score(const std::string& domain) const {
    auto it = outlet_scores.find(domain);
    if (it != outlet_scores.end()) {
        return it->second;
    }
    // Default unknown outlets to neutral
    return 0.0;
}
