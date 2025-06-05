/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "pyramid.h"

#include <ncurses.h>

#include <chrono>
#include <cmath>
#include <thread>
#include <utility>
#include <vector>

void Pyramid::drawFrame(const AnimationContext& context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);
    werase(context.window);

    // Tetrahedron vertices (3D)
    struct Vec3 {
        double x, y, z;
    };
    // Equilateral triangle base in XZ, apex above
    const double s = 1.5;  // base size (slightly larger)
    const double h = 3.5;  // apex height (slightly larger)
    // Place base off-center so it orbits world y-axis
    std::vector<Vec3> verts = {
        {s, -s, 0},                             // 0: base
        {-s * 0.5, -s, s * std::sqrt(3) / 2},   // 1: base
        {-s * 0.5, -s, -s * std::sqrt(3) / 2},  // 2: base
        {0, h, 0}                               // 3: apex
    };
    // Edges (pairs of vertex indices)
    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 0},  // base
        {0, 3}, {1, 3}, {2, 3}   // sides
    };

    // Animation parameters
    int frames = 180;
    double yAngleStep = 2 * M_PI / frames;
    double tilt = M_PI / 4.0;  // off-axis tilt
    char edgeChar = '#';

    for (int frame = 0; frame < frames; ++frame) {
        werase(context.window);
        double theta = frame * yAngleStep;
        // Precompute sin/cos
        double cT = cos(theta), sT = sin(theta);
        double cP = cos(tilt), sP = sin(tilt);
        // Project vertices
        std::vector<std::pair<double, double>> projVerts;
        for (const auto& v : verts) {
            double x1 = cT * v.x + sT * v.z;
            double y1 = v.y;
            double z1 = -sT * v.x + cT * v.z;
            double y2 = cP * y1 - sP * z1;
            double z2 = sP * y1 + cP * z1;
            double d = 3.5;
            double px = x1 / (z2 + d);
            double py = y2 / (z2 + d);
            double sx = winWidth / 2 + px * winWidth * 0.48;
            double sy = winHeight / 2 - py * winHeight * 0.60;
            projVerts.emplace_back(sx, sy);
        }
        // For each cell, find min distance to any edge
        std::vector<std::vector<double>> minDist(
            winHeight, std::vector<double>(winWidth, 1e9));
        int substeps = 8;
        for (const auto& e : edges) {
            double x0 = projVerts[e.first].first,
                   y0 = projVerts[e.first].second;
            double x1 = projVerts[e.second].first,
                   y1 = projVerts[e.second].second;
            for (int i = 0; i <= substeps; ++i) {
                double t = (double)i / substeps;
                double sx = x0 + (x1 - x0) * t;
                double sy = y0 + (y1 - y0) * t;
                int ix = (int)std::round(sx);
                int iy = (int)std::round(sy);
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        int cx = ix + dx, cy = iy + dy;
                        if (cx >= 0 && cx < winWidth && cy >= 0 &&
                            cy < winHeight) {
                            double dist =
                                std::hypot(sx - (cx + 0.5), sy - (cy + 0.5));
                            if (dist < minDist[cy][cx])
                                minDist[cy][cx] = dist;
                        }
                    }
                }
            }
        }
        // Draw cells with intensity based on minDist
        double maxDist = 1.0;  // thinner lines (was 1.2)
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                double norm =
                    std::clamp(1.0 - minDist[y][x] / maxDist, 0.0, 1.0);
                int idx = (int)(norm * (intensityChars.size() - 1) + 0.5);
                if (idx > 0)
                    mvwaddch(context.window, y, x, intensityChars[idx]);
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }
    werase(context.window);
    wrefresh(context.window);
}