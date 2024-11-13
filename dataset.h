#include <vector>
#include <iostream>
#include <random>
#include <stdexcept>

std::vector<std::vector<float>> generateRandomVectors(size_t numVectors, size_t vectorDim) {
    std::vector<std::vector<float>> vectors(numVectors, std::vector<float>(vectorDim));

    // Random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0, 1.0);

    for (auto& vector : vectors) {
        for (auto& element : vector) {
            element = dist(gen);
        }
    }

    return vectors;
}
