#include <iostream>
#include "include/outlet_validator.hpp"

int main() {
    OutletValidator validator;

    // Load mapping file
    std::cout << "Loading name-to-domain mapping..." << std::endl;
    if (!validator.load_name_mapping("config/name_to_domain_mapping.json")) {
        std::cerr << "Failed to load name mapping (continuing with defaults)\n";
    }

    // Load datasets
    std::cout << "Loading AllSides dataset..." << std::endl;
    if (!validator.load_allsides("config/allsides_outlets.json")) {
        std::cerr << "Failed to load AllSides dataset\n";
        return 1;
    }

    std::cout << "Loading MBFC dataset..." << std::endl;
    if (!validator.load_mbfc("config/mbfc_outlets.json")) {
        std::cerr << "Failed to load MBFC dataset\n";
        return 1;
    }

    // Validate
    std::cout << "Validating outlets..." << std::endl;
    auto results = validator.validate();

    // Get statistics
    auto stats = validator.get_statistics();

    std::cout << "\n=== Validation Results ===\n";
    std::cout << "Total unique outlets: " << stats.total_outlets << "\n";
    std::cout << "In both datasets: " << stats.in_both << "\n";
    std::cout << "Only in AllSides: " << stats.only_allsides << "\n";
    std::cout << "Only in MBFC: " << stats.only_mbfc << "\n";
    std::cout << "\nAgreement (outlets in both):\n";
    std::cout << "  Strong (diff < 0.1): " << stats.strong_agreement << "\n";
    std::cout << "  Moderate (diff 0.1-0.3): " << stats.moderate_agreement << "\n";
    std::cout << "  Major (diff >= 0.3): " << stats.major_disagreement << "\n";

    if (stats.in_both > 0) {
        double agreement_pct = 100.0 * (stats.strong_agreement + stats.moderate_agreement) / stats.in_both;
        std::cout << "\nOverall agreement rate: " << agreement_pct << "%\n";
    }

    // Generate reports
    std::cout << "\nGenerating validation report..." << std::endl;
    validator.generate_report("config/validation_report.txt");

    std::cout << "Generating merged outlets.json..." << std::endl;
    validator.generate_merged_outlets("config/outlets.json");

    std::cout << "\n✓ Complete!\n";
    std::cout << "  - Validation report: config/validation_report.txt\n";
    std::cout << "  - Merged outlets: config/outlets.json\n";

    return 0;
}
