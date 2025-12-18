#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// Input data structure
struct ArticleInput {
    std::string title;
    std::string body;
    std::string url;
    std::string domain;
};

// Result data structure
struct BiasResult {
    double score;                              // [-1.0, +1.0]
    std::string label;                          // "Moderate Left", "Neutral", etc.
    double confidence;                          // [0.0, 1.0]
    std::vector<std::string> explanations;
};

// Entity mention
struct EntityMention {
    std::string name;
    std::string ideology;  // "left", "right", "neutral", "unknown"
    double sentiment;      // [-1.0, +1.0]
    double emotion;        // [0.0, 1.0] - intensity
};
