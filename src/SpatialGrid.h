#pragma once
#include <vector>
#include <algorithm>
#include <cmath>

class SpatialGrid {
private:
    float cellSize;
    int gridWidth, gridHeight;
    float worldMinX, worldMinY, worldMaxX, worldMaxY;
    std::vector<std::vector<int>> grid;
    
public:
    SpatialGrid(float cellSize, float minX, float minY, float maxX, float maxY) 
        : cellSize(cellSize), worldMinX(minX), worldMinY(minY), worldMaxX(maxX), worldMaxY(maxY) {
        
        gridWidth = static_cast<int>((maxX - minX) / cellSize) + 1;
        gridHeight = static_cast<int>((maxY - minY) / cellSize) + 1;
        grid.resize(gridWidth * gridHeight);
    }
    
    void clear() {
        for (auto& cell : grid) {
            cell.clear();
        }
    }
    
    void addParticle(int particleIndex, float x, float y) {
        int gridX = static_cast<int>((x - worldMinX) / cellSize);
        int gridY = static_cast<int>((y - worldMinY) / cellSize);
        
        // Clamp to grid bounds
        gridX = std::max(0, std::min(gridX, gridWidth - 1));
        gridY = std::max(0, std::min(gridY, gridHeight - 1));
        
        int cellIndex = gridY * gridWidth + gridX;
        grid[cellIndex].push_back(particleIndex);
    }
    
    std::vector<int> getNearbyParticles(float x, float y, float radius) {
        std::vector<int> nearby;
        
        // Calculate grid cell range to check
        int minGridX = static_cast<int>((x - radius - worldMinX) / cellSize);
        int maxGridX = static_cast<int>((x + radius - worldMinX) / cellSize);
        int minGridY = static_cast<int>((y - radius - worldMinY) / cellSize);
        int maxGridY = static_cast<int>((y + radius - worldMinY) / cellSize);
        
        // Clamp to grid bounds
        minGridX = std::max(0, std::min(minGridX, gridWidth - 1));
        maxGridX = std::max(0, std::min(maxGridX, gridWidth - 1));
        minGridY = std::max(0, std::min(minGridY, gridHeight - 1));
        maxGridY = std::max(0, std::min(maxGridY, gridHeight - 1));
        
        // Collect particles from nearby cells
        for (int gy = minGridY; gy <= maxGridY; gy++) {
            for (int gx = minGridX; gx <= maxGridX; gx++) {
                int cellIndex = gy * gridWidth + gx;
                for (int particleIndex : grid[cellIndex]) {
                    nearby.push_back(particleIndex);
                }
            }
        }
        
        return nearby;
    }
};
