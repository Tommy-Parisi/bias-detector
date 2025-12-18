#!/bin/bash
# Build script for BiasDetector

set -e

echo "Building BiasDetector..."

# Create build directory if it doesn't exist
mkdir -p build

# Create object files in build/
clang++ -std=c++17 -I. -c src/nlp_context.cpp -o build/nlp.o
clang++ -std=c++17 -I. -c src/preprocessor.cpp -o build/prep.o
clang++ -std=c++17 -I. -c src/bias_aggregator.cpp -o build/agg.o
clang++ -std=c++17 -I. -c src/signals/outlet_baseline_signal.cpp -o build/outlet.o
clang++ -std=c++17 -I. -c src/signals/entity_sentiment_signal.cpp -o build/entity.o
clang++ -std=c++17 -I. -c src/signals/policy_framing_signal.cpp -o build/policy.o
clang++ -std=c++17 -I. -c src/signals/emotional_direction_signal.cpp -o build/emotional.o

# Link library
ar rcs build/libbias_detector.a build/*.o

# Build example
clang++ -std=c++17 -I. -c example.cpp -o build/example.o
clang++ -std=c++17 build/example.o build/libbias_detector.a -o build/bias_detector_example

echo "✓ Compilation successful"
echo "✓ Executable: ./build/bias_detector_example"
echo "✓ Library: ./build/libbias_detector.a"
echo "✓ Object files: ./build/*.o"
