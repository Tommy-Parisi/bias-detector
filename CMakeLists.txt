cmake_minimum_required(VERSION 3.10)
project(BiasDetector CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Source files
set(SOURCES
    src/nlp_context.cpp
    src/preprocessor.cpp
    src/signals/outlet_baseline_signal.cpp
    src/signals/entity_sentiment_signal.cpp
    src/signals/policy_framing_signal.cpp
    src/signals/emotional_direction_signal.cpp
    src/bias_aggregator.cpp
)

# Library
add_library(bias_detector ${SOURCES})

# Executable
add_executable(bias_detector_example main.cpp)
target_link_libraries(bias_detector_example PRIVATE bias_detector)

# Enable testing
enable_testing()

# Add tests directory (if tests exist)
add_subdirectory(tests)
