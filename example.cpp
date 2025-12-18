#include <iostream>
#include "include/bias_aggregator.hpp"

int main() {
    // Create aggregator
    BiasAggregator aggregator;

    // Example 1: Left-leaning article (long enough for analysis)
    std::cout << "\n========== EXAMPLE 1: Left-leaning News ==========\n" << std::endl;
    
    ArticleInput article1{
        .title = "Climate Action Bill Passes Senate With Strong Democratic Support",
        .body = "After weeks of intense debate, the Senate passed the comprehensive climate action bill "
                "with overwhelming support from Democratic lawmakers. The progressive legislation aims to "
                "combat inequality in environmental justice by investing in renewable energy and worker "
                "protection programs. Republicans expressed concerns about government overreach and market "
                "deregulation impacts. Senator Biden praised the progressive reforms as essential for "
                "combating climate change. The legislation promotes renewable innovation and worker rights "
                "while strengthening environmental regulations. Critics worry about free market concerns, "
                "but supporters emphasize climate justice and community welfare. The bill represents a "
                "significant progressive victory for climate action and social equity.",
        .url = "https://msnow.com/climate-bill",
        .domain = "msnow.com"
    };

    BiasResult result1 = aggregator.analyze(article1);

    std::cout << "Score: " << result1.score << std::endl;
    std::cout << "Label: " << result1.label << std::endl;
    std::cout << "Confidence: " << result1.confidence << std::endl;
    std::cout << "\nExplanations:" << std::endl;
    for (const auto& explanation : result1.explanations) {
        std::cout << "  - " << explanation << std::endl;
    }

    // Example 2: Right-leaning article
    std::cout << "\n========== EXAMPLE 2: Right-leaning News ==========\n" << std::endl;
    
    ArticleInput article2{
        .title = "Free Market Initiative Promotes Business Growth and Innovation",
        .body = "The Senate approved a business innovation bill that promotes free market entrepreneurship "
                "and reduces government regulations. Conservative lawmakers championed the legislation as "
                "essential for economic growth and innovation. The bill aims to eliminate unnecessary "
                "regulations that stifle business freedom and market competition. Entrepreneurs and "
                "business leaders celebrated the free market approach to fiscal responsibility. "
                "Democrats raised concerns about worker protections and environmental oversight. "
                "The conservative initiative emphasizes traditional business values and entrepreneurial "
                "freedom while critics worry about regulatory protections being weakened. Business "
                "growth advocates support the deregulation and market liberty focus of the proposal.",
        .url = "https://foxnews.com/business-bill",
        .domain = "foxnews.com"
    };

    BiasResult result2 = aggregator.analyze(article2);

    std::cout << "Score: " << result2.score << std::endl;
    std::cout << "Label: " << result2.label << std::endl;
    std::cout << "Confidence: " << result2.confidence << std::endl;
    std::cout << "\nExplanations:" << std::endl;
    for (const auto& explanation : result2.explanations) {
        std::cout << "  - " << explanation << std::endl;
    }

    // Example 3: Neutral article
    std::cout << "\n========== EXAMPLE 3: Neutral News ==========\n" << std::endl;
    
    ArticleInput article3{
        .title = "Congress Debates Economic Policy Proposals",
        .body = "Congress held hearings on proposed economic policy changes this week. "
                "The Senate met to discuss fiscal policy and government budgeting. "
                "Multiple perspectives were presented on tax policy and market regulation. "
                "Republican and Democratic lawmakers offered different approaches to economic growth. "
                "Some emphasized business growth and market freedom while others focused on worker "
                "protection and progressive reform. The House will continue debate on the proposed "
                "legislation next week during sessions in Washington. Various stakeholders provided "
                "testimony on fiscal responsibility concerns and environmental regulation impacts. "
                "Congress members discussed the impacts on business innovation and market competition. "
                "Democratic representatives expressed concerns about deregulation while Republican "
                "members highlighted freedom and market principles. Senate debates continue as bill "
                "moves through committee with extensive discussion of both progressive and conservative "
                "perspectives on policy implementation and economic justice considerations.",
        .url = "https://example.com/congress-debate",
        .domain = "apnews.com"
    };

    BiasResult result3 = aggregator.analyze(article3);

    std::cout << "Score: " << result3.score << std::endl;
    std::cout << "Label: " << result3.label << std::endl;
    std::cout << "Confidence: " << result3.confidence << std::endl;
    std::cout << "\nExplanations:" << std::endl;
    for (const auto& explanation : result3.explanations) {
        std::cout << "  - " << explanation << std::endl;
    }

    return 0;
}
