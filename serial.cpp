#include "dataset.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>

// int node_highest_level(float mlog, int max_level) {
//     // Initialize random number generator
//     static std::random_device rd;
//     static std::mt19937 gen(rd());
//     std::uniform_real_distribution<> dis(0.0, 1.0);

//     // Draw a random probability
//     float p = dis(gen);

//     // Determine level based on exponential distribution
//     int level = static_cast<int>(std::floor(-std::log(p) * mlog));

//     // Limit the level to max_level
//     return std::min(level, max_level);
// }

// Node class to store vector, level, and neighbors
class Node {
public:
    int id;
    std::vector<float> vector;
    std::vector<std::vector<int>> neighborsByLevel;
    
    Node() : id(-1) {}
    Node(int id, const std::vector<float>& vec, int numLevels)
        : id(id), vector(vec), neighborsByLevel(numLevels) {}  // Initialize with the required number of levels
};

class HNSW {
public:
    std::vector<std::vector<Node>> layers;
    int M_max;
    int efConstruction;
    int numLayers;
    int entryPoint;

    HNSW(int M_max, int efConstruction, int numLayers) : M_max(M_max), efConstruction(efConstruction), numLayers(numLayers), entryPoint(-1), layers() {
        layers.resize(numLayers);
    }
    void insertNode(std::vector<Node>& dataset, Node& targetNode, int layer, int M_max, int efConstruction) {
        // TODO: implement the insertNode function
        std::vector<int> W;
        std::cout << layers.size() << std::endl;
        for (auto layer : layers) {
            if (layer.empty()) {
                layer.push_back(targetNode);
                entryPoint = targetNode.id;
            }
            else {
                for (auto node : layer) {
                    searchLayer(targetNode, entryPoint, efConstruction, layer);
        
                }
            }
        }
    }
    void searchLayer(Node& targetNode, int entryPoint, int efConstruction, std::vector<Node>& layer) {
        for 
    }
private:
};

// Function to calculate the squared L2 distance between two vectors
float squaredL2Distance(const std::vector<float>& v1, const std::vector<float>& v2) {
    float distance = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        distance += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return distance;
}

int main() {
    // Example usage: create a dataset with 1000 vectors, each with 128 dimensions
    size_t numNodes = 1000;
    size_t vectorDim = 128;
    int numLayers = ceil(log2(numNodes));
    int efConstruction = 10;
    int M_max = 10;
    float mlog = 1.0f;
    std::vector<std::vector<float>> vectors = generateRandomVectors(numNodes, vectorDim);

    // Initialize the dataset
   
    std::cout << "CHECK RIGHT" << std::endl;


    std::vector<Node> dataset;
    for (int i = 0; i < numNodes; ++i) {
        std::vector<float> vec(vectorDim, static_cast<float>(i)); // Simple vector initialization
        dataset.emplace_back(i, vec, numLayers);
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    HNSW hnsw = HNSW(M_max, efConstruction, numLayers);
    // Draw a random probability
    for (auto &node : dataset) {
        float p = dis(gen);
        int insertLayer = std::min(numLayers, static_cast<int>(std::floor(-std::log(p) * mlog)));
        std::cout << "Inserting node " << node.id << " at level " << insertLayer << "\n";
        hnsw.insertNode(dataset, node, insertLayer, M_max, efConstruction);
        break;

    }



    // Print dataset information
    std::cout << "Dataset size: " << dataset.size() << " vectors\n";
    std::cout << "Number of layers: " << numLayers << "\n";
    std::cout << "Dimension of each vector: " << vectorDim << "\n";



    return 0;
}