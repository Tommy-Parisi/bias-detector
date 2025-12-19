#include "../../include/signals/semantic_bias_signal.hpp"
#include <sstream>
#include <cmath>
#include <numeric>
#include <algorithm>

SemanticBiasSignal::SemanticBiasSignal() {
    build_reference_vectors();
}

SemanticBiasSignal::~SemanticBiasSignal() = default;

void SemanticBiasSignal::build_reference_vectors() {
    // Left-wing semantic space: progressive political terminology
    // Dimensions represent different semantic aspects
    embedding_dim = 20;
    left_vector.resize(embedding_dim, 0.0f);
    right_vector.resize(embedding_dim, 0.0f);

    // Left vector: emphasizes collective values, regulation, progress, equity
    // Dimension breakdown:
    // 0-4:   Collectivism/community (high positive)
    // 5-9:   Individualism (negative)
    // 10-14: Regulation/government (positive)
    // 15-19: Progress/change (positive)
    
    for (int i = 0; i < embedding_dim; ++i) {
        if (i < 5)       left_vector[i] = 0.8f;     // High collectivism
        else if (i < 10) left_vector[i] = -0.5f;    // Low individualism
        else if (i < 15) left_vector[i] = 0.7f;     // Pro-regulation
        else             left_vector[i] = 0.75f;    // Pro-progress
    }

    // Right vector: emphasizes individual liberty, market, tradition, stability
    // Same dimension breakdown but opposite values
    
    for (int i = 0; i < embedding_dim; ++i) {
        if (i < 5)       right_vector[i] = -0.8f;   // Low collectivism
        else if (i < 10) right_vector[i] = 0.85f;   // High individualism
        else if (i < 15) right_vector[i] = -0.7f;   // Anti-regulation/pro-market
        else             right_vector[i] = -0.7f;   // Pro-tradition/stability
    }
}

std::vector<float> SemanticBiasSignal::embed_text(const std::string& text) {
    // Create a semantic embedding based on keyword presence
    // This is a simplified approach; in production, use transformer embeddings
    
    std::vector<float> embedding(embedding_dim, 0.0f);
    
    // Political terminology database
    struct Term {
        std::string word;
        int dimension;  // Which semantic dimension
        float polarity; // Positive = left-wing, negative = right-wing
    };
    
    std::vector<Term> left_terms = {
        {"equality", 0, 0.9f},      {"justice", 0, 0.9f},
        {"community", 1, 0.8f},     {"collective", 1, 0.85f},
        {"workers", 5, -0.7f},      {"rights", 5, -0.8f},
        {"welfare", 10, 0.8f},      {"regulation", 10, 0.75f},
        {"healthcare", 10, 0.7f},   {"environment", 10, 0.6f},
        {"progress", 15, 0.8f},     {"reform", 15, 0.75f},
        {"change", 15, 0.7f},       {"innovation", 15, 0.6f}
    };
    
    std::vector<Term> right_terms = {
        {"freedom", 5, -0.9f},      {"liberty", 5, -0.9f},
        {"individual", 5, -0.85f},  {"personal", 5, -0.8f},
        {"market", 10, -0.85f},     {"business", 10, -0.8f},
        {"deregulation", 10, -0.9f},{"growth", 10, -0.7f},
        {"tradition", 15, -0.8f},   {"family", 15, -0.7f},
        {"stability", 15, -0.75f},  {"strength", 15, -0.7f}
    };

    // Count occurrences and accumulate contributions
    int total_terms_found = 0;
    
    for (const auto& term : left_terms) {
        size_t pos = 0;
        int count = 0;
        while ((pos = text.find(term.word, pos)) != std::string::npos) {
            count++;
            pos += term.word.length();
        }
        if (count > 0) {
            // Add to collectivism dimension if term is left-aligned
            embedding[term.dimension] += count * 0.3f;
            total_terms_found += count;
        }
    }
    
    for (const auto& term : right_terms) {
        size_t pos = 0;
        int count = 0;
        while ((pos = text.find(term.word, pos)) != std::string::npos) {
            count++;
            pos += term.word.length();
        }
        if (count > 0) {
            // Add to individualism dimension if term is right-aligned
            embedding[term.dimension] += count * -0.3f;
            total_terms_found += count;
        }
    }

    // Normalize by document length
    if (total_terms_found > 0) {
        float norm = std::sqrt(std::accumulate(embedding.begin(), embedding.end(), 0.0f,
                                               [](float a, float b) { return a + b*b; }));
        if (norm > 1e-6) {
            for (auto& val : embedding) val /= norm;
        }
    }

    return embedding;
}

double SemanticBiasSignal::cosine_similarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size() || a.empty()) return 0.0;
    
    double dot_product = 0.0;
    double norm_a = 0.0;
    double norm_b = 0.0;

    for (size_t i = 0; i < a.size(); ++i) {
        dot_product += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }

    norm_a = std::sqrt(norm_a);
    norm_b = std::sqrt(norm_b);

    if (norm_a < 1e-6 || norm_b < 1e-6) return 0.0;
    
    return dot_product / (norm_a * norm_b);
}

std::vector<std::string> SemanticBiasSignal::extract_nouns_verbs(const NLPContext& ctx) {
    // Simple filtering: keep substantial tokens
    std::vector<std::string> nouns_verbs;
    
    for (const auto& token : ctx.tokens) {
        if (token.length() > 2) {
            nouns_verbs.push_back(token);
        }
    }
    
    return nouns_verbs;
}

double SemanticBiasSignal::compute(const NLPContext& ctx, const ArticleInput& article) {
    // Combine article text
    std::string full_text = article.title + " " + article.body;
    
    // Convert to lowercase for matching
    std::transform(full_text.begin(), full_text.end(), full_text.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    // Get semantic embedding for article
    auto article_embedding = embed_text(full_text);
    
    // Calculate similarity to political vectors
    last_left_similarity = cosine_similarity(article_embedding, left_vector);
    last_right_similarity = cosine_similarity(article_embedding, right_vector);
    
    // Score: right vs left alignment
    // Positive = right-leaning, Negative = left-leaning
    double score = last_right_similarity - last_left_similarity;
    
    // Clamp to [-1, 1]
    score = std::max(-1.0, std::min(1.0, score));
    
    return score;
}

std::string SemanticBiasSignal::explain() const {
    std::ostringstream oss;
    oss << "Semantic bias: left=" << last_left_similarity 
        << ", right=" << last_right_similarity
        << " → " << (last_right_similarity > last_left_similarity ? "right" : "left")
        << "-leaning semantic space";
    return oss.str();
}
