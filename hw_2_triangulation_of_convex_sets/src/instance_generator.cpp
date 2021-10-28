#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <stdint.h>

constexpr float pi = 3.141592653589f;

// Taken from Wikipedia: https://en.wikibooks.org/wiki/Algorithm_Implementation/Geometry/Convex_hull/Monotone_chain
// Implementation of Andrew's monotone chain 2_d convex hull algorithm.
// Asymptotic complexity: O(n log n).
// Practical performance: 0.5-1.0 seconds for n=1000000 on a 1_g_hz machine.

struct Point {
    Point() : x(0.0), y(0.0) {}

    Point(float xa, float ya) : x(xa), y(ya) {}

    bool operator<(const Point &p) const {
        return x < p.x || (x == p.x && y < p.y);
    }

    float x, y;
};

// 2_d cross product of OA and OB vectors, i.e. z-component of their 3_d cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
float cross(const Point &O, const Point &A, const Point &B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
std::vector<Point> convex_hull(std::vector<Point> P) {
    int n = P.size(), k = 0;
    std::vector<Point> H(2 * n);

    // Sort points lexicographically
    sort(P.begin(), P.end());

    // Build lower hull
    for (int i = 0; i < n; ++i) {
        while (k >= 2 && cross(H[k - 2], H[k - 1], P[i]) <= 0) {
            k--;
        }
        H[k++] = P[i];
    }

    // Build upper hull
    for (int i = n - 2, t = k + 1; i >= 0; i--) {
        while (k >= t && cross(H[k - 2], H[k - 1], P[i]) <= 0) {
            k--;
        }
        H[k++] = P[i];
    }

    H.resize(k - 1);
    return H;
}

int main(int argc, char *argv[]) {
    int32_t n;
    std::string output_file;

    if (argc != 3) {
        std::cout << "Instance generator for triangulation homework assignment." << std::endl;
        std::cout << std::endl << "Usage:" << std::endl;
        std::cout << "\t" << argv[0] << " NUM_POINTS INSTANCE_PATH" << std::endl;
        return 1;
    } else {
        n = atoi(argv[1]);
        output_file = argv[2];
    }


    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<float> rot(0.0f, 2 * pi);
    std::uniform_real_distribution<float> prolong(0.0f, 0.03f);
    std::uniform_real_distribution<float> axis_length(0.5f, 2.0f);

    std::vector<Point> gen_points;
    float phi = 0.0;
    float rotation = rot(gen);
    float a = axis_length(gen), b = axis_length(gen);
    while (gen_points.size() < n) {
        phi = rot(gen);
        double p = prolong(gen);
        double x = a * std::cos(phi) * (1.0 + p);
        double y = b * std::sin(phi) * (1.0 + p);
        x = x * std::cos(rotation) - y * std::sin(rotation) + 2.0;
        y = x * std::sin(rotation) + y * std::cos(rotation) + 2.0;
        gen_points.emplace_back(x, y);
    }

    // Select convex hull...
    gen_points = convex_hull(gen_points);
    int32_t num_points = gen_points.size();

    std::ofstream bout(output_file.c_str(), std::ofstream::binary | std::ofstream::trunc);
    if (bout) {
        bout.write((char *) &num_points, sizeof(int32_t));
        for (const Point &p: gen_points) {
            bout.write((char *) &p.x, sizeof(float));
            bout.write((char *) &p.y, sizeof(float));
        }
        bout.close();
    } else {
        std::cerr << "Cannot write to file '" << output_file << "'!" << std::endl;
    }

    std::cout << "In total " << gen_points.size() << " points generated..." << std::endl;

    return 0;
}

