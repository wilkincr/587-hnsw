#include "dataset.h"
#include <math.h>

int node_highest_level(float mlog, int max_level) {
    // Initialize random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Draw a random probability
    float p = dis(gen);

    // Determine level based on exponential distribution
    int level = static_cast<int>(std::floor(-std::log(p) * mlog));

    // Limit the level to max_level
    return std::min(level, max_level);
}


#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <limits>
#include <unordered_map>
#include <algorithm>

// Node class to store vector, level, and neighbors
struct Node {
    int id;
    std::vector<float> vector;
    std::vector<std::vector<int>> neighborsByLevel; // stores neighbors for each level
};

// Function to calculate the squared L2 distance between two vectors
float squaredL2Distance(const std::vector<float>& v1, const std::vector<float>& v2) {
    float distance = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        distance += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return distance;
}

// Search function to find neighbors on a specific level
std::vector<int> searchLevel(const std::vector<Node>& layer, const Node& entryPoint, const Node& target, int ef, int level) {
    using DistanceNodePair = std::pair<float, int>;
    std::priority_queue<DistanceNodePair> candidateQueue;

    // Initialize the queue with the entry point
    candidateQueue.push({-squaredL2Distance(entryPoint.vector, target.vector), entryPoint.id});

    std::vector<int> results;

    while (!candidateQueue.empty() && results.size() < ef) {
        int nodeId = candidateQueue.top().second;
        candidateQueue.pop();

        results.push_back(nodeId);

        for (int neighborId : layer[nodeId].neighborsByLevel[level]) {
            float dist = squaredL2Distance(layer[neighborId].vector, target.vector);
            candidateQueue.push({-dist, neighborId});
        }
    }
    return results;
}

// Generate random level for a node based on m_L
int generateRandomLevel(float m_L) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    int level = 0;
    while (dis(gen) < m_L && level < 16) { // 16 can be the maximum level, for example
        ++level;
    }
    return level;
}

// Insert function to add a new node into the HNSW graph
void insertNode(std::vector<std::vector<Node>>& layers, Node& targetNode, int m_L, int M, int efConstruction) {
    // Step 1: Define the starting entry point (uppermost layer)
    int epId = layers.back().front().id;  // entry point id from the top layer
    int targetLevel = generateRandomLevel(m_L);

    // Step 2: Navigate down to target level+1, updating the entry point
    for (int level = layers.size() - 1; level > targetLevel; --level) {
        auto nearestNeighborIds = searchLevel(layers[level], layers[level][epId], targetNode, 1, level);
        if (!nearestNeighborIds.empty()) {
            epId = nearestNeighborIds[0];
        }
    }

    // Step 3: Insert down to level 0, connecting neighbors
    for (int level = targetLevel; level >= 0; --level) {
        auto candidates = searchLevel(layers[level], layers[level][epId], targetNode, efConstruction, level);
        std::vector<int> neighbors;

        // Keep M nearest neighbors
        for (int neighborId : candidates) {
            neighbors.push_back(neighborId);
            if (neighbors.size() >= M) break;
        }

        // Connect neighbors with the target node and add it to the layer
        targetNode.neighborsByLevel.push_back(neighbors);
        for (int neighborId : neighbors) {
            layers[level][neighborId].neighborsByLevel[level].push_back(targetNode.id);

            // If the neighbor list exceeds M, remove the farthest one
            if (layers[level][neighborId].neighborsByLevel[level].size() > M) {
                auto& neighborList = layers[level][neighborId].neighborsByLevel[level];
                neighborList.erase(std::max_element(neighborList.begin(), neighborList.end(),
                                                    [&](int a, int b) {
                                                        return squaredL2Distance(targetNode.vector, layers[level][a].vector) <
                                                               squaredL2Distance(targetNode.vector, layers[level][b].vector);
                                                    }));
            }
        }
    }


int main() {
    // Example usage: create a dataset with 1000 vectors, each with 128 dimensions
    size_t numVectors = 1000;
    size_t vectorDim = 128;
    size_t numLayers = ceil(log2(numVectors));
    float mlog = 1.0f;
    std::vector<std::vector<float>> dataset = generateRandomVectors(numVectors, vectorDim);

    // Print dataset information
    std::cout << "Dataset size: " << dataset.size() << " vectors\n";
    std::cout << "Number of layers: " << numLayers << "\n";
    std::cout << "Dimension of each vector: " << vectorDim << "\n";

    // Access and print the first vector
    const std::vector<float>& vector = dataset[0];
    std::cout << "First vector: ";
    for (size_t i = 0; i < std::min(vector.size(), static_cast<size_t>(5)); ++i) {
        std::cout << vector[i] << " ";
    }
    std::cout << "...\n";

    // Highest level for first vector
    int highestLevel = node_highest_level(mlog, numLayers);
    std::cout << "Highest level for first vector: " << highestLevel << "\n";

    // Initialize the HNSW graph
    HNSWGraph hnswGraph(mlog, highestLevel);

    // Load dataset and build the graph
    for (const auto& vec : dataset) {
        hnswGraph.addNode(vec);
    }

    // Display neighbors of each node
    for (const Node& node : hnswGraph.nodes) {
        std::cout << "Node " << node.id << " at level " << node.level << " has neighbors:\n";
        for (int l = 0; l <= node.level; ++l) {
            std::cout << "  Level " << l << ": ";
            for (int neighbor : node.neighbors[l]) {
                std::cout << neighbor << " ";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}