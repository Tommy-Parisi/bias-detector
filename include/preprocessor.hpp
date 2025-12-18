#pragma once

#include "types.hpp"
#include "nlp_context.hpp"
#include <string>
#include <vector>

/**
 * Preprocessor: Transforms raw ArticleInput into structured NLPContext.
 * 
 * Responsibilities:
 * - Tokenization
 * - Sentence splitting
 * - Entity extraction
 * - Sentiment computation
 * 
 * Can be extended with better NLP models (spaCy bindings, etc.)
 */
class Preprocessor {
public:
    /**
     * Main entry point: processes an article and returns populated NLPContext
     */
    NLPContext process(const ArticleInput& article);

private:
    // Tokenization: simple whitespace-based for MVP
    std::vector<std::string> tokenize(const std::string& text);

    // Sentence splitting: simple regex for MVP
    std::vector<std::string> split_sentences(const std::string& text);

    // Extract named entities (stub for now)
    void extract_entities(NLPContext& ctx, const ArticleInput& article);

    // Compute sentiment per sentence/entity
    void compute_sentiment(NLPContext& ctx);

    // Compute emotion scores
    void compute_emotion(NLPContext& ctx);
};
