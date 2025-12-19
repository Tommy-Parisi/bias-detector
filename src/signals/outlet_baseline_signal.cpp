#include "../../include/signals/outlet_baseline_signal.hpp"
#include <sstream>
#include <fstream>
#include <regex>

OutletBaselineSignal::OutletBaselineSignal() {
    load_from_json("config/outlets.json");
}

bool OutletBaselineSignal::load_from_json(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    size_t outlets_pos = content.find("\"outlets\"");
    if (outlets_pos == std::string::npos) {
        return false;
    }

    size_t open_brace = content.find("{", outlets_pos);
    if (open_brace == std::string::npos) {
        return false;
    }

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

    std::string outlets_str = content.substr(open_brace + 1, close_brace - open_brace - 2);

    std::regex pair_regex("\"([^\"]+)\"\\s*:\\s*([+-]?[0-9]*\\.?[0-9]+)");
    std::smatch match;
    std::string::const_iterator search_start(outlets_str.cbegin());

    while (std::regex_search(search_start, outlets_str.cend(), match, pair_regex)) {
        std::string domain = match[1].str();
        double score = std::stod(match[2].str());
        outlet_scores[domain] = score;

        search_start = match.suffix().first;
    }

    return !outlet_scores.empty();
}

double OutletBaselineSignal::compute(const NLPContext& ctx,
                                      const ArticleInput& article) {
    last_score = get_outlet_score(article.domain);
    return last_score;
}

std::string OutletBaselineSignal::explain() const {
    std::ostringstream oss;
    oss << "Outlet baseline bias: " << last_score;
    if (last_score > 0.5) {
        oss << " (strong right-leaning outlet)";
    } else if (last_score > 0.2) {
        oss << " (moderately right-leaning outlet)";
    } else if (last_score < -0.5) {
        oss << " (strong left-leaning outlet)";
    } else if (last_score < -0.2) {
        oss << " (moderately left-leaning outlet)";
    } else {
        oss << " (neutral outlet)";
    }
    return oss.str();
}

double OutletBaselineSignal::get_outlet_score(const std::string& domain) const {
    auto it = outlet_scores.find(domain);
    if (it != outlet_scores.end()) {
        return it->second;
    }
    // Default unknown outlets to neutral
    return 0.0;
}
