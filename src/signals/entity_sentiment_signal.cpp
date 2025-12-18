#include "../../include/signals/entity_sentiment_signal.hpp"
#include <sstream>

double EntitySentimentSignal::compute(const NLPContext& ctx,
                                       const ArticleInput& article) {
    double left_sentiment_sum = 0.0;
    double right_sentiment_sum = 0.0;
    int left_count = 0;
    int right_count = 0;

    for (const auto& entity : ctx.entities) {
        if (entity.ideology == "left") {
            left_sentiment_sum += entity.sentiment;
            left_count++;
        } else if (entity.ideology == "right") {
            right_sentiment_sum += entity.sentiment;
            right_count++;
        }
    }

    left_avg = (left_count > 0) ? left_sentiment_sum / left_count : 0.0;
    right_avg = (right_count > 0) ? right_sentiment_sum / right_count : 0.0;

    // If left entities have negative sentiment and right have positive, it's left bias
    // If right entities have negative sentiment and left have positive, it's right bias
    double diff = left_avg - right_avg;

    // Clamp to [-1, 1]
    return std::max(-1.0, std::min(1.0, diff));
}

std::string EntitySentimentSignal::explain() const {
    std::ostringstream oss;
    oss << "Entity sentiment: left entities avg=" << left_avg
        << ", right entities avg=" << right_avg;
    
    if (left_avg < right_avg) {
        oss << " → more negative toward left entities (right bias signal)";
    } else if (left_avg > right_avg) {
        oss << " → more negative toward right entities (left bias signal)";
    } else {
        oss << " → balanced sentiment (neutral)";
    }
    
    return oss.str();
}
