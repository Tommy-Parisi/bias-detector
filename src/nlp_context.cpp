#include "../include/nlp_context.hpp"

size_t NLPContext::token_count() const {
    return tokens.size();
}

size_t NLPContext::entity_count() const {
    return entities.size();
}

size_t NLPContext::sentence_count() const {
    return sentences.size();
}

void NLPContext::add_entity(const EntityMention& entity) {
    entities.push_back(entity);
}

void NLPContext::cache_sentiment(const std::string& text, double score) {
    sentiment_cache[text] = score;
}

double NLPContext::get_cached_sentiment(const std::string& text) const {
    auto it = sentiment_cache.find(text);
    if (it != sentiment_cache.end()) {
        return it->second;
    }
    return 0.0;  // default neutral
}
