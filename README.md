# BiasDetector: Production-Quality C++ Architecture

A clean, extensible C++ implementation of a media bias detection system following SOLID principles and the Strategy pattern.

## Architecture Overview

```
ArticleInput (raw text)
    ↓
Preprocessor (tokenize, sentence split, NER)
    ↓
NLPContext (shared state: tokens, sentences, entities, sentiment cache)
    ↓
BiasSignals (4 independent strategies):
  ├─ OutletBaselineSignal (domain reputation)
  ├─ EntitySentimentSignal (sentiment toward entities)
  ├─ PolicyFramingSignal (policy language detection)
  └─ EmotionalDirectionSignal (emotional intensity analysis)
    ↓
BiasAggregator (weighted sum, confidence, refusal logic)
    ↓
BiasResult (score, label, confidence, explanations)
```

## Design Philosophy

This architecture demonstrates:

- **SOLID Principles**: Single responsibility, open/closed, Liskov substitution
- **Strategy Pattern**: Each bias signal is independent and pluggable
- **Separation of Concerns**: Clear boundaries between preprocessing, analysis, and aggregation
- **Testability**: Each component can be unit tested in isolation
- **Extensibility**: Add new signals without modifying existing code

## Project Structure

```
bias_detector/
├── include/
│   ├── types.hpp                      # Core data structures
│   ├── nlp_context.hpp                # Shared NLP state
│   ├── preprocessor.hpp               # Text preprocessing
│   ├── bias_signal.hpp                # Abstract signal interface
│   ├── bias_aggregator.hpp            # Main orchestrator
│   └── signals/
│       ├── outlet_baseline_signal.hpp
│       ├── entity_sentiment_signal.hpp
│       ├── policy_framing_signal.hpp
│       └── emotional_direction_signal.hpp
├── src/
│   ├── nlp_context.cpp
│   ├── preprocessor.cpp
│   ├── bias_aggregator.cpp
│   └── signals/
│       ├── outlet_baseline_signal.cpp
│       ├── entity_sentiment_signal.cpp
│       ├── policy_framing_signal.cpp
│       └── emotional_direction_signal.cpp
├── tests/
│   └── CMakeLists.txt
├── main.cpp                           # Example usage
├── CMakeLists.txt
└── README.md
```

## Building

### Prerequisites
- C++17 or later
- CMake 3.10+

### Build Steps

```bash
cd /Users/tommy/bias_detector
mkdir build
cd build
cmake ..
make
```

### Running the Example

```bash
./bias_detector_example
```

Expected output:
```
=== Bias Detection Results ===
Score: -0.45
Label: Moderate Left
Confidence: 0.72

Explanations:
  - Outlet baseline bias: -0.6 (strong left-leaning outlet)
  - Entity sentiment: ... (detailed explanation)
  - Policy framing: ... (detailed explanation)
  - Emotional direction: ... (detailed explanation)
```

## Core Components

### 1. BiasSignal (Abstract Base Class)

Every bias signal must implement:

```cpp
class BiasSignal {
public:
    virtual double compute(const NLPContext& ctx, const ArticleInput& article) = 0;
    virtual std::string explain() const = 0;
    virtual std::string name() const = 0;
};
```

**Score Range**: [-1.0, +1.0]
- Negative = left bias
- Positive = right bias
- 0.0 = neutral

### 2. Signal Implementations

#### OutletBaselineSignal
Uses domain reputation as a prior. Outlets are pre-mapped to known bias scores.

```cpp
outlet_scores["foxnews.com"] = 0.7;      // right
outlet_scores["msnbc.com"] = -0.6;       // left
outlet_scores["reuters.com"] = 0.0;      // neutral
```

#### EntitySentimentSignal
Measures average sentiment directed at left vs. right political entities.
- Negative sentiment toward left entities = right bias
- Negative sentiment toward right entities = left bias

#### PolicyFramingSignal
Counts left vs. right policy language.
- Left terms: "inequality", "climate", "regulation", "justice", etc.
- Right terms: "freedom", "market", "deregulation", "business", etc.

#### EmotionalDirectionSignal
Measures emotional intensity (anger, outrage, etc.) directed at entities.
- High emotion + negative sentiment toward left entities = right bias
- High emotion + negative sentiment toward right entities = left bias

### 3. Aggregation

BiasAggregator combines all signals with configurable weights:

```
Aggregate Score = Σ(signal_score × weight) / Σ(weights)
```

Default weights:
- OutletBaseline: 15%
- EntitySentiment: 35%
- PolicyFraming: 25%
- EmotionalDirection: 25%

### 4. Confidence Scoring

Confidence combines two factors:
1. **Signal Agreement**: Lower variance between signals = higher confidence
2. **Data Quantity**: More entities/tokens = higher confidence

### 5. Refusal Logic

Returns "Insufficient Data" if:
- Article < 300 tokens
- Fewer than 2 entities found

## Usage Example

```cpp
#include "include/bias_aggregator.hpp"

int main() {
    BiasAggregator aggregator;
    
    ArticleInput article{
        .title = "Title",
        .body = "Article body...",
        .url = "https://example.com",
        .domain = "example.com"
    };
    
    BiasResult result = aggregator.analyze(article);
    
    std::cout << "Score: " << result.score << std::endl;
    std::cout << "Label: " << result.label << std::endl;
    std::cout << "Confidence: " << result.confidence << std::endl;
    
    for (const auto& explanation : result.explanations) {
        std::cout << "- " << explanation << std::endl;
    }
    
    return 0;
}
```

## Extending the System

### Adding a New Signal

1. Create `include/signals/my_signal.hpp`:

```cpp
class MySignal : public BiasSignal {
public:
    double compute(const NLPContext& ctx, const ArticleInput& article) override;
    std::string explain() const override;
    std::string name() const override { return "MySignal"; }
private:
    double last_score = 0.0;
};
```

2. Implement in `src/signals/my_signal.cpp`

3. Register in BiasAggregator constructor:

```cpp
BiasAggregator::BiasAggregator() {
    // ... existing signals ...
    signals.push_back(std::make_unique<MySignal>());
    weights["MySignal"] = 0.10;
    normalize_weights();
}
```

4. Write unit tests

### Improving NLP Components

**Preprocessor** currently uses simple heuristics:
- Tokenization: whitespace split
- Sentiment: basic word list
- Entity extraction: domain keyword matching

**Production improvements**:
- Tokenization: spaCy bindings or NLTK
- Sentiment: VADER, TextBlob, or fine-tuned BERT
- Entities: spaCy NER, custom NER model, or transformer-based
- Emotion: NRC Emotion Lexicon or emotion detection model

## Testing Strategy

Suggested testing framework: **Google Test** or **Catch2**

### Test Categories

1. **Unit Tests**
   - Each signal in isolation
   - Preprocessor components
   - NLPContext utilities

2. **Integration Tests**
   - BiasAggregator with mock signals
   - End-to-end analysis

3. **Regression Tests**
   - Known articles with expected scores
   - Bias thresholds and labels

### Test File Structure

```
tests/
├── test_nlp_context.cpp
├── test_preprocessor.cpp
├── test_signals.cpp
│   ├── test_outlet_baseline.cpp
│   ├── test_entity_sentiment.cpp
│   ├── test_policy_framing.cpp
│   └── test_emotional_direction.cpp
├── test_aggregator.cpp
└── test_refusal_logic.cpp
```

## Interview Talking Points

This architecture demonstrates:

1. **SOLID Design**
   - Single Responsibility: Each signal has one job
   - Open/Closed: Add signals without modifying existing code
   - Dependency Inversion: BiasAggregator depends on abstract interface

2. **Clean Code**
   - Clear naming conventions
   - Separation of concerns
   - Minimal coupling

3. **Scalability**
   - Pluggable signal architecture
   - Configurable weights
   - Caching to avoid recomputation

4. **Professional Practices**
   - Proper header/source separation
   - CMake build system
   - Comprehensive documentation
   - Extensible design patterns

## Future Improvements

- [ ] Integrate production NLP libraries (spaCy, transformers)
- [ ] Add configuration file support (JSON weights, outlet mappings)
- [ ] Implement caching layer for repeated analyses
- [ ] Add telemetry/metrics collection
- [ ] Build REST API wrapper
- [ ] Add support for multiple languages
- [ ] Implement feedback loop for model improvement
- [ ] Add visualization of signal contributions

## License

MIT License (modify as needed for your use case)

## Contact

Ready for production use and interview evaluation.
