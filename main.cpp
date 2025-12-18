#include <iostream>
#include "include/bias_aggregator.hpp"

int main() {
    // Create aggregator
    BiasAggregator aggregator;

    // Example article
    ArticleInput article{
        .title = "Climate Action Bill Passes Senate With Strong Support",
        .body = "After weeks of debate, the Senate passed the comprehensive climate action bill "
                "with strong support from Democratic lawmakers. Republicans expressed concerns about "
                "market regulations and government overreach. The bill aims to reduce inequality in "
                "environmental justice while promoting green innovation and worker protection.",
        .url = "https://example.com/climate-bill",
        .domain = "msnbc.com"
    };

    // Analyze
    BiasResult result = aggregator.analyze(article);

    // Display results
    std::cout << "=== Bias Detection Results ===" << std::endl;
    std::cout << "Score: " << result.score << std::endl;
    std::cout << "Label: " << result.label << std::endl;
    std::cout << "Confidence: " << result.confidence << std::endl;
    std::cout << "\nExplanations:" << std::endl;
    for (const auto& explanation : result.explanations) {
        std::cout << "  - " << explanation << std::endl;
    }

    return 0;
}
