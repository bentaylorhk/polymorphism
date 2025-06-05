/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "pyramid.h"

#include <ncurses.h>

#include <cmath>
#include <string>
#include <thread>
#include <vector>

void Pyramid::drawFrame(const AnimationContext& context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);
    werase(context.window);

    // --- Regular tetrahedron (equilateral 4-sided pyramid), centroid at origin
    // ---
    struct Vec3 {
        double x, y, z;
    };
    const double a = 3.0;  // edge length
    std::vector<Vec3> verts = {
        {a / std::sqrt(3.0), 0, -a / 3.0},
        {-a / (2 * std::sqrt(3.0)), a / 2.0, -a / 3.0},
        {-a / (2 * std::sqrt(3.0)), -a / 2.0, -a / 3.0},
        {0, 0, a}  // apex
    };
    // Center at origin
    double cx = 0, cy = 0, cz = 0;
    for (const auto& v : verts) {
        cx += v.x;
        cy += v.y;
        cz += v.z;
    }
    cx /= verts.size();
    cy /= verts.size();
    cz /= verts.size();
    for (auto& v : verts) {
        v.x -= cx;
        v.y -= cy;
        v.z -= cz;
    }

    // Skew the pyramid in world space for more interesting rotation
    double skew = M_PI / 8.0;
    double cs = cos(skew), ss = sin(skew);
    for (auto& v : verts) {
        double x0 = v.x, z0 = v.z;
        v.x = cs * x0 + ss * z0;
        v.z = -ss * x0 + cs * z0;
    }

    // Edges (pairs of vertex indices)
    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2},
        {2, 0},  // base
        {0, 3}, {1, 3},
        {2, 3}  // sideThe centre of the shape should be the origins
    };

    // Animation parameters
    int frames = 600;
    double yAngleStep = 2 * M_PI / 300;  // slow, smooth rotation
    double d = 14.0;                     // camera distance (was 7.0)
    int substeps = 16;     // was 8, more substeps for denser sampling
    double maxDist = 1.7;  // was 1.0, increase for bolder lines
    const char* polyphonic = "POLYPHONIC";
    int text_len = 10;

    // --- Perspective/Isometric toggle ---
    bool useIsometric = true;  // set to false for perspective

    // Project the 3D origin to screen for text placement
    int text_cx = winWidth / 2;
    int text_cy = winHeight / 2;
    int text_startx = text_cx - text_len / 2;
    int text_y = text_cy;

    for (int frame = 0; frame < frames; ++frame) {
        werase(context.window);
        double theta = frame * yAngleStep;
        double cT = cos(theta), sT = sin(theta);
        // Project vertices (rotate around world y-axis)
        std::vector<Vec3> vproj;
        for (const auto& v : verts) {
            // Rotate around world y-axis
            double x1 = cT * v.x + sT * v.z;
            double y1 = v.y;
            double z1 = -sT * v.x + cT * v.z;
            vproj.push_back({x1, y1, z1});
        }
        // Project to 2D
        std::vector<std::pair<double, double>> projVerts;
        std::vector<double> zVerts;
        for (const auto& v : vproj) {
            double sx, sy;
            if (useIsometric) {
                // Isometric projection
                double iso_x = v.x - v.z;
                double iso_y = (v.x + 2 * v.y + v.z) / std::sqrt(6.0);
                sx = winWidth / 2 + iso_x * winWidth * 0.18;
                sy = winHeight / 2 - iso_y * winHeight * 0.22;
            } else {
                // Perspective projection
                double px = v.x / (v.z + d);
                double py = v.y / (v.z + d);
                sx = winWidth / 2 + px * winWidth * 0.48;
                sy = winHeight / 2 - py * winHeight * 0.60;
            }
            projVerts.emplace_back(sx, sy);
            zVerts.push_back(v.z);
        }
        // Z-buffer for painter's algorithm (for text occlusion)
        std::vector<std::vector<double>> zBuffer(
            winHeight, std::vector<double>(winWidth, 1e9));
        std::vector<std::vector<double>> minDist(
            winHeight, std::vector<double>(winWidth, 1e9));
        for (const auto& e : edges) {
            double x0 = projVerts[e.first].first,
                   y0 = projVerts[e.first].second;
            double x1 = projVerts[e.second].first,
                   y1 = projVerts[e.second].second;
            double z0 = zVerts[e.first], z1v = zVerts[e.second];
            for (int i = 0; i <= substeps; ++i) {
                double t = (double)i / substeps;
                double sx = x0 + (x1 - x0) * t;
                double sy = y0 + (y1 - y0) * t;
                double sz = z0 + (z1v - z0) * t;
                int ix = (int)std::round(sx);
                int iy = (int)std::round(sy);
                if (ix >= 0 && ix < winWidth && iy >= 0 && iy < winHeight) {
                    double dist = std::hypot(sx - (ix + 0.5), sy - (iy + 0.5));
                    if (dist < minDist[iy][ix] || sz < zBuffer[iy][ix]) {
                        minDist[iy][ix] = dist;
                        zBuffer[iy][ix] = sz;
                    }
                }
            }
        }
        // Project 3D origin for text placement
        double text_sx, text_sy;
        if (useIsometric) {
            double iso_x = 0, iso_y = 0;
            text_sx = winWidth / 2 + iso_x * winWidth * 0.18;
            text_sy = winHeight / 2 - iso_y * winHeight * 0.22;
        } else {
            double px = 0.0 / d;
            double py = 0.0 / d;
            text_sx = winWidth / 2 + px * winWidth * 0.48;
            text_sy = winHeight / 2 - py * winHeight * 0.60;
        }
        int text_startx = (int)text_sx - text_len / 2;
        int text_y = (int)text_sy;
        // Draw cells with intensity based on minDist
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                double norm =
                    std::clamp(1.0 - minDist[y][x] / maxDist, 0.0, 1.0);
                int idx = (int)(norm * (intensityChars.size() - 1) + 0.5);
                // If this cell is in the text region, only draw text if zBuffer
                // is behind origin
                bool inText = (y == text_y && x >= text_startx &&
                               x < text_startx + text_len);
                if (inText) {
                    // If the closest edge is behind the origin, draw text
                    if (zBuffer[y][x] > 0.0) {
                        wattron(context.window, COLOR_PAIR(5));
                        mvwaddch(context.window, y, x,
                                 polyphonic[x - text_startx]);
                        wattroff(context.window, COLOR_PAIR(5));
                    } else if (idx > 0) {
                        mvwaddch(context.window, y, x, intensityChars[idx]);
                    }
                } else if (idx > 0) {
                    mvwaddch(context.window, y, x, intensityChars[idx]);
                }
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }
    werase(context.window);
    wrefresh(context.window);
}