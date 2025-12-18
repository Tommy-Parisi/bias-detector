#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

/**
 * Validates and merges outlet bias scores from AllSides and MBFC datasets.
 * 
 * Compares scores to identify agreement, flags discrepancies,
 * and generates a merged dataset with confidence scores.
 */
class OutletValidator {
public:
    struct ValidationResult {
        std::string outlet;
        double allsides_score;      // [-1, 1] or NaN if not in dataset
        double mbfc_score;          // [-1, 1] or NaN if not in dataset
        double score_diff;          // |allsides - mbfc|
        double agreement_confidence; // [0, 1] - how much to trust this outlet's score
        std::string status;         // "agreement", "slight_diff", "major_diff", "only_allsides", "only_mbfc"
    };

    struct MergedOutlet {
        std::string outlet;
        double score;               // Final normalized score
        double confidence;          // How confident we are in this score
        std::string primary_source; // Which source we trust more
    };

    OutletValidator();

    /**
     * Load mapping file to normalize outlet names
     */
    bool load_name_mapping(const std::string& config_path);

    /**
     * Load AllSides dataset from JSON
     */
    bool load_allsides(const std::string& config_path);

    /**
     * Load MBFC dataset from JSON
     */
    bool load_mbfc(const std::string& config_path);

    /**
     * Validate all outlets by comparing both datasets
     */
    std::vector<ValidationResult> validate();

    /**
     * Generate human-readable validation report (text)
     */
    void generate_report(const std::string& output_path);

    /**
     * Generate merged outlets.json with confidence scores
     */
    void generate_merged_outlets(const std::string& output_path);

    /**
     * Get statistics about agreement
     */
    struct Statistics {
        int total_outlets;
        int in_both;
        int only_allsides;
        int only_mbfc;
        int strong_agreement;      // diff < 0.1
        int moderate_agreement;    // diff 0.1-0.3
        int major_disagreement;    // diff >= 0.3
    };
    Statistics get_statistics() const;

private:
    std::unordered_map<std::string, double> allsides_outlets;
    std::unordered_map<std::string, double> mbfc_outlets;
    std::unordered_map<std::string, std::string> name_to_domain;  // normalize AllSides names
    std::vector<ValidationResult> validation_results;

    // JSON parsing helpers
    bool parse_outlets_json(const std::string& content, 
                           std::unordered_map<std::string, double>& outlets);

    // Validation helpers
    double calculate_confidence(const ValidationResult& result);
    std::string get_status(const ValidationResult& result);
};
