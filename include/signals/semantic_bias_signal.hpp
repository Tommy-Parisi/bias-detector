#pragma once

#include "../bias_signal.hpp"
#include <vector>
#include <memory>

/**
 * Signal: Semantic Bias Detection using Vector Spaces
 * 
 * Creates left-wing and right-wing semantic vectors from political terminology.
 * Compares article content to these reference vectors using cosine similarity.
 * 
 * Future: Will integrate ONNX Runtime to use distilbert embeddings
 */
class SemanticBiasSignal : public BiasSignal {
public:
    SemanticBiasSignal();
    ~SemanticBiasSignal();

    double compute(const NLPContext& ctx, const ArticleInput& article) override;
    std::string explain() const override;
    std::string name() const override { return "SemanticBias"; }

private:
    // Reference vectors
    std::vector<float> left_vector;   // Average embedding of left-wing terms
    std::vector<float> right_vector;  // Average embedding of right-wing terms
    int embedding_dim = 20;

    // Internal methods
    std::vector<float> embed_text(const std::string& text);
    double cosine_similarity(const std::vector<float>& a, const std::vector<float>& b);
    std::vector<std::string> extract_nouns_verbs(const NLPContext& ctx);
    void build_reference_vectors();

    // Tracking
    double last_left_similarity = 0.0;
    double last_right_similarity = 0.0;
};
