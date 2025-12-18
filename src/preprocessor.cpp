#include "../include/preprocessor.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

NLPContext Preprocessor::process(const ArticleInput& article) {
    NLPContext ctx;

    // Combine title and body for full text analysis
    std::string full_text = article.title + " " + article.body;

    // Tokenize
    ctx.tokens = tokenize(full_text);

    // Split into sentences
    ctx.sentences = split_sentences(article.body);

    // Extract entities (stub - can be extended with NER model)
    extract_entities(ctx, article);

    // Compute sentiment
    compute_sentiment(ctx);

    // Compute emotion
    compute_emotion(ctx);

    return ctx;
}

std::vector<std::string> Preprocessor::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream stream(text);
    std::string word;

    while (stream >> word) {
        // Remove punctuation from word ends
        while (!word.empty() && !std::isalnum(word.back())) {
            word.pop_back();
        }
        while (!word.empty() && !std::isalnum(word.front())) {
            word = word.substr(1);
        }

        if (!word.empty()) {
            // Lowercase
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            tokens.push_back(word);
        }
    }

    return tokens;
}

std::vector<std::string> Preprocessor::split_sentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::regex sentence_regex(R"([^.!?]+[.!?]+)");
    
    auto begin = std::sregex_iterator(text.begin(), text.end(), sentence_regex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        std::string sentence = it->str();
        // Trim whitespace
        sentence.erase(0, sentence.find_first_not_of(" \t\n\r"));
        sentence.erase(sentence.find_last_not_of(" \t\n\r") + 1);
        if (!sentence.empty()) {
            sentences.push_back(sentence);
        }
    }

    return sentences;
}

void Preprocessor::extract_entities(NLPContext& ctx, const ArticleInput& article) {
    // Stub: In production, use spaCy via Python bindings or a C++ NER model
    // For now, we extract simple heuristics based on common political entities

    std::vector<std::pair<std::string, std::string>> known_entities = {
        // Left-leaning
        {"biden", "left"},
        {"democrats", "left"},
        {"democratic", "left"},
        {"harris", "left"},
        {"obama", "left"},
        {"pelosi", "left"},
        {"schumer", "left"},
        {"progressive", "left"},
        {"climate", "left"},
        {"regulation", "left"},
        
        // Right-leaning
        {"trump", "right"},
        {"republicans", "right"},
        {"republican", "right"},
        {"mcconnell", "right"},
        {"desantis", "right"},
        {"pence", "right"},
        {"cpac", "right"},
        {"conservative", "right"},
        {"freedom", "right"},
        {"market", "right"},
        
        // Neutral
        {"congress", "neutral"},
        {"senate", "neutral"},
        {"house", "neutral"},
        {"bill", "neutral"},
    };

    std::string full_text = article.title + " " + article.body;
    std::transform(full_text.begin(), full_text.end(), full_text.begin(), ::tolower);

    for (const auto& [entity_name, ideology] : known_entities) {
        if (full_text.find(entity_name) != std::string::npos) {
            EntityMention mention{
                .name = entity_name,
                .ideology = ideology,
                .sentiment = 0.0,  // Will be updated in compute_sentiment
                .emotion = 0.0     // Will be updated in compute_emotion
            };
            ctx.add_entity(mention);
        }
    }
}

void Preprocessor::compute_sentiment(NLPContext& ctx) {
    // Stub sentiment analysis
    // In production: use VADER, TextBlob, or fine-tuned model
    
    std::vector<std::pair<std::string, double>> sentiment_words = {
        // Positive
        {"great", 0.5},
        {"excellent", 0.6},
        {"good", 0.4},
        {"wonderful", 0.6},
        {"strong", 0.3},
        
        // Negative
        {"bad", -0.4},
        {"terrible", -0.6},
        {"awful", -0.6},
        {"poor", -0.4},
        {"weak", -0.3},
        {"corrupt", -0.7},
        {"failed", -0.5},
    };

    for (auto& entity : ctx.entities) {
        double total_sentiment = 0.0;
        int count = 0;

        for (const auto& token : ctx.tokens) {
            for (const auto& [word, sentiment] : sentiment_words) {
                if (token == word) {
                    total_sentiment += sentiment;
                    count++;
                }
            }
        }

        if (count > 0) {
            entity.sentiment = total_sentiment / count;
            ctx.cache_sentiment(entity.name, entity.sentiment);
        }
    }
}

void Preprocessor::compute_emotion(NLPContext& ctx) {
    // Stub emotion computation
    // In production: use emotion detection model (NRC, etc.)
    
    std::vector<std::string> emotional_words = {
        "angry", "furious", "outraged",
        "shocking", "devastating", "alarming",
        "beautiful", "inspiring", "wonderful"
    };

    for (auto& entity : ctx.entities) {
        double emotion_score = 0.0;
        for (const auto& token : ctx.tokens) {
            for (const auto& emotion_word : emotional_words) {
                if (token == emotion_word) {
                    emotion_score += 0.3;
                }
            }
        }
        entity.emotion = std::min(emotion_score, 1.0);
    }
}
