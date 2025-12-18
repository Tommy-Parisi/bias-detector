#include "../../include/signals/emotional_direction_signal.hpp"
#include <sstream>

double EmotionalDirectionSignal::compute(const NLPContext& ctx,
                                          const ArticleInput& article) {
    double left_emotion_sum = 0.0;
    double right_emotion_sum = 0.0;
    int left_count = 0;
    int right_count = 0;

    for (const auto& entity : ctx.entities) {
        // Weight emotion by sentiment: if entity is spoken of negatively,
        // and it has high emotion, that's a bias signal against that entity
        double weighted_emotion = entity.emotion * (-entity.sentiment);

        if (entity.ideology == "left") {
            left_emotion_sum += weighted_emotion;
            left_count++;
        } else if (entity.ideology == "right") {
            right_emotion_sum += weighted_emotion;
            right_count++;
        }
    }

    left_emotion = (left_count > 0) ? left_emotion_sum / left_count : 0.0;
    right_emotion = (right_count > 0) ? right_emotion_sum / right_count : 0.0;

    // Diff: if left entities have more emotional negativity, it's a right bias
    double diff = left_emotion - right_emotion;

    // Clamp to [-1, 1]
    return std::max(-1.0, std::min(1.0, diff));
}

std::string EmotionalDirectionSignal::explain() const {
    std::ostringstream oss;
    oss << "Emotional direction: left entity emotions=" << left_emotion
        << ", right entity emotions=" << right_emotion;
    
    if (left_emotion > right_emotion) {
        oss << " → more emotional negativity toward left (right bias signal)";
    } else if (right_emotion > left_emotion) {
        oss << " → more emotional negativity toward right (left bias signal)";
    } else {
        oss << " → balanced emotional tone";
    }
    
    return oss.str();
}
