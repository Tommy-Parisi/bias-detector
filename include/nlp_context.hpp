#pragma once

#include "types.hpp"
#include <vector>
#include <unordered_map>
#include <string>

/**
 * Central shared state for NLP analysis.
 * Avoids recomputing expensive NLP operations.
 */
class NLPContext {
public:
    // Tokenized words
    std::vector<std::string> tokens;

    // Sentences (splitted from body)
    std::vector<std::string> sentences;

    // Extracted entities and their properties
    std::vector<EntityMention> entities;

    // Cached sentiment scores (text -> score)
    std::unordered_map<std::string, double> sentiment_cache;

    // Query methods
    size_t token_count() const;
    size_t entity_count() const;
    size_t sentence_count() const;

    // Utility
    void add_entity(const EntityMention& entity);
    void cache_sentiment(const std::string& text, double score);
    double get_cached_sentiment(const std::string& text) const;
};
