#include "../include/outlet_validator.hpp"
#include <fstream>
#include <regex>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

OutletValidator::OutletValidator() = default;

bool OutletValidator::load_name_mapping(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    // Find the "name_to_domain" key
    size_t mapping_pos = content.find("\"name_to_domain\"");
    if (mapping_pos == std::string::npos) {
        return false;
    }

    // Find the opening brace after "name_to_domain"
    size_t open_brace = content.find("{", mapping_pos);
    if (open_brace == std::string::npos) {
        return false;
    }

    // Find matching closing brace
    int brace_count = 1;
    size_t close_brace = open_brace + 1;
    while (close_brace < content.length() && brace_count > 0) {
        if (content[close_brace] == '{') {
            brace_count++;
        } else if (content[close_brace] == '}') {
            brace_count--;
        }
        close_brace++;
    }

    // Extract mapping object
    std::string mapping_str = content.substr(open_brace + 1, close_brace - open_brace - 2);

    // Parse key-value pairs: "name": "domain"
    std::regex pair_regex("\"([^\"]+)\"\\s*:\\s*\"([^\"]+)\"");
    std::smatch match;
    std::string::const_iterator search_start(mapping_str.cbegin());

    while (std::regex_search(search_start, mapping_str.cend(), match, pair_regex)) {
        std::string name = match[1].str();
        std::string domain = match[2].str();
        name_to_domain[name] = domain;

        search_start = match.suffix().first;
    }

    return !name_to_domain.empty();
}

bool OutletValidator::load_allsides(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    // First parse the outlets
    if (!parse_outlets_json(content, allsides_outlets)) {
        return false;
    }

    // Then normalize names to domains
    std::unordered_map<std::string, double> normalized;
    for (auto& [name, score] : allsides_outlets) {
        std::string key = name;
        // Try to find normalized domain name
        if (name_to_domain.find(name) != name_to_domain.end()) {
            key = name_to_domain[name];
        }
        normalized[key] = score;
    }

    allsides_outlets = normalized;
    return true;
}

bool OutletValidator::load_mbfc(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    return parse_outlets_json(content, mbfc_outlets);
}

bool OutletValidator::parse_outlets_json(const std::string& content,
                                        std::unordered_map<std::string, double>& outlets) {
    // Find the "outlets" key
    size_t outlets_pos = content.find("\"outlets\"");
    if (outlets_pos == std::string::npos) {
        return false;
    }

    // Find the opening brace after "outlets"
    size_t open_brace = content.find("{", outlets_pos);
    if (open_brace == std::string::npos) {
        return false;
    }

    // Find matching closing brace
    int brace_count = 1;
    size_t close_brace = open_brace + 1;
    while (close_brace < content.length() && brace_count > 0) {
        if (content[close_brace] == '{') {
            brace_count++;
        } else if (content[close_brace] == '}') {
            brace_count--;
        }
        close_brace++;
    }

    // Extract outlets object
    std::string outlets_str = content.substr(open_brace + 1, close_brace - open_brace - 2);

    // Parse key-value pairs: "domain": score or "domain": {score: ..., ...}
    std::regex pair_regex("\"([^\"]+)\"\\s*:\\s*([+-]?[0-9]*\\.?[0-9]+)");
    std::smatch match;
    std::string::const_iterator search_start(outlets_str.cbegin());

    while (std::regex_search(search_start, outlets_str.cend(), match, pair_regex)) {
        std::string outlet_name = match[1].str();
        double score = std::stod(match[2].str());
        
        // Only add if not already added (in case of nested objects)
        if (outlets.find(outlet_name) == outlets.end()) {
            outlets[outlet_name] = score;
        }

        search_start = match.suffix().first;
    }

    return !outlets.empty();
}

std::vector<OutletValidator::ValidationResult> OutletValidator::validate() {
    validation_results.clear();

    // Collect all unique outlets
    std::unordered_map<std::string, ValidationResult> results_map;

    // Process AllSides outlets
    for (const auto& [outlet, allsides_score] : allsides_outlets) {
        ValidationResult result{
            .outlet = outlet,
            .allsides_score = allsides_score,
            .mbfc_score = NAN,
            .score_diff = 0.0,
            .agreement_confidence = 0.0,
            .status = ""
        };

        if (mbfc_outlets.find(outlet) != mbfc_outlets.end()) {
            result.mbfc_score = mbfc_outlets[outlet];
            result.score_diff = std::abs(result.allsides_score - result.mbfc_score);
        }

        result.status = get_status(result);
        result.agreement_confidence = calculate_confidence(result);
        results_map[outlet] = result;
    }

    // Process MBFC outlets not in AllSides
    for (const auto& [outlet, mbfc_score] : mbfc_outlets) {
        if (results_map.find(outlet) == results_map.end()) {
            ValidationResult result{
                .outlet = outlet,
                .allsides_score = NAN,
                .mbfc_score = mbfc_score,
                .score_diff = 0.0,
                .agreement_confidence = 0.0,
                .status = ""
            };

            result.status = get_status(result);
            result.agreement_confidence = calculate_confidence(result);
            results_map[outlet] = result;
        }
    }

    // Convert to vector and sort
    for (auto& [outlet, result] : results_map) {
        validation_results.push_back(result);
    }

    std::sort(validation_results.begin(), validation_results.end(),
              [](const ValidationResult& a, const ValidationResult& b) {
                  return a.outlet < b.outlet;
              });

    return validation_results;
}

std::string OutletValidator::get_status(const ValidationResult& result) {
    if (!std::isnan(result.allsides_score) && !std::isnan(result.mbfc_score)) {
        if (result.score_diff < 0.1) {
            return "agreement";
        } else if (result.score_diff < 0.3) {
            return "slight_diff";
        } else {
            return "major_diff";
        }
    } else if (!std::isnan(result.allsides_score)) {
        return "only_allsides";
    } else {
        return "only_mbfc";
    }
}

double OutletValidator::calculate_confidence(const ValidationResult& result) {
    if (result.status == "agreement") {
        return 0.95;
    } else if (result.status == "slight_diff") {
        return 0.70;
    } else if (result.status == "major_diff") {
        return 0.30;
    } else if (result.status == "only_allsides") {
        return 0.50;
    } else { // only_mbfc
        return 0.60;
    }
}

OutletValidator::Statistics OutletValidator::get_statistics() const {
    Statistics stats{0, 0, 0, 0, 0, 0, 0};

    stats.total_outlets = validation_results.size();

    for (const auto& result : validation_results) {
        if (!std::isnan(result.allsides_score) && !std::isnan(result.mbfc_score)) {
            stats.in_both++;
            if (result.score_diff < 0.1) {
                stats.strong_agreement++;
            } else if (result.score_diff < 0.3) {
                stats.moderate_agreement++;
            } else {
                stats.major_disagreement++;
            }
        } else if (!std::isnan(result.allsides_score)) {
            stats.only_allsides++;
        } else {
            stats.only_mbfc++;
        }
    }

    return stats;
}

void OutletValidator::generate_report(const std::string& output_path) {
    std::ofstream report(output_path);
    if (!report.is_open()) {
        return;
    }

    auto stats = get_statistics();

    report << "=== Outlet Bias Validation Report ===\n\n";
    report << "Statistics:\n";
    report << "  Total unique outlets: " << stats.total_outlets << "\n";
    report << "  In both datasets: " << stats.in_both << "\n";
    report << "  Only in AllSides: " << stats.only_allsides << "\n";
    report << "  Only in MBFC: " << stats.only_mbfc << "\n";
    report << "\n";

    report << "Agreement Statistics (outlets in both):\n";
    report << "  Strong agreement (diff < 0.1): " << stats.strong_agreement << "\n";
    report << "  Moderate agreement (diff 0.1-0.3): " << stats.moderate_agreement << "\n";
    report << "  Major disagreement (diff >= 0.3): " << stats.major_disagreement << "\n";
    report << "\n";

    double agreement_pct = (stats.in_both > 0) ? 
        (100.0 * (stats.strong_agreement + stats.moderate_agreement) / stats.in_both) : 0.0;
    report << "Overall agreement rate: " << std::fixed << std::setprecision(1) << agreement_pct << "%\n";
    report << "\n";

    // Major disagreements
    report << "=== Major Disagreements (diff >= 0.3) ===\n";
    for (const auto& result : validation_results) {
        if (result.status == "major_diff") {
            report << "  " << result.outlet << ": "
                   << std::fixed << std::setprecision(2)
                   << result.allsides_score << " (AllSides) vs "
                   << result.mbfc_score << " (MBFC), diff=" << result.score_diff << "\n";
        }
    }
    report << "\n";

    // Only in one dataset
    report << "=== Only in AllSides ===\n";
    for (const auto& result : validation_results) {
        if (result.status == "only_allsides") {
            report << "  " << result.outlet << ": " 
                   << std::fixed << std::setprecision(2) << result.allsides_score << "\n";
        }
    }
    report << "\n";

    report << "=== Only in MBFC ===\n";
    for (const auto& result : validation_results) {
        if (result.status == "only_mbfc") {
            report << "  " << result.outlet << ": "
                   << std::fixed << std::setprecision(2) << result.mbfc_score << "\n";
        }
    }
    report << "\n";

    report.close();
}

void OutletValidator::generate_merged_outlets(const std::string& output_path) {
    std::ofstream merged(output_path);
    if (!merged.is_open()) {
        return;
    }

    merged << "{\n  \"outlets\": {\n";

    for (size_t i = 0; i < validation_results.size(); ++i) {
        const auto& result = validation_results[i];

        // Use score from dataset with higher confidence
        double final_score = 0.0;
        std::string source;

        if (!std::isnan(result.allsides_score) && !std::isnan(result.mbfc_score)) {
            // Both exist: average them (or weight by agreement)
            final_score = (result.allsides_score + result.mbfc_score) / 2.0;
            source = "both";
        } else if (!std::isnan(result.allsides_score)) {
            final_score = result.allsides_score;
            source = "allsides";
        } else {
            final_score = result.mbfc_score;
            source = "mbfc";
        }

        merged << "    \"" << result.outlet << "\": {\n"
               << "      \"score\": " << std::fixed << std::setprecision(2) << final_score << ",\n"
               << "      \"confidence\": " << std::fixed << std::setprecision(2) << result.agreement_confidence << ",\n"
               << "      \"source\": \"" << source << "\",\n"
               << "      \"status\": \"" << result.status << "\"\n"
               << "    }";

        if (i < validation_results.size() - 1) {
            merged << ",";
        }
        merged << "\n";
    }

    merged << "  }\n}\n";
    merged.close();
}
