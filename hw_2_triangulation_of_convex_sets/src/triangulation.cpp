#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <omp.h>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <cstdint>


// Structure of a point in 2_d plane
struct Point {
    float x, y;
};

// A Dynamic programming based function to find minimum cost for convex polygon triangulation.
// points: vector of points of the convex polygon in counter-clockwise order.
std::tuple<std::vector<std::tuple<int, int, int>>, float> triangulate(const std::vector<Point> &points) {
    float triag_cost = 0.0f;
    std::vector<std::tuple<int, int, int>> triangles;

    // TODO: Implement a parallel dynamic programming approach for triangulation.
    // Fill variables triag_cost and triangles.
    // triag_cost: the cost of the triangulation.
    // triangles: vector of triangles. Each triangle is represented by indices (into points vector) of its corner points.

    return make_tuple(move(triangles), triag_cost);
}

std::vector<Point> read_problem(const std::string &input_file) {
    std::vector<Point> points;
    std::ifstream bin(input_file.c_str(), std::ifstream::binary);
    if (bin) {
        int32_t n = 0;
        bin.read((char *) &n, sizeof(int32_t));
        for (uint64_t p = 0; p < n; ++p) {
            float x = 0.0, y = 0.0;
            bin.read((char *) &x, sizeof(float));
            bin.read((char *) &y, sizeof(float));
            points.push_back({x, y});
        }

        bin.close();
    } else {
        throw std::invalid_argument("Cannot open the input file '" + input_file + "' to read the problem.");
    }

    return points;
}

[[maybe_unused]] void read_result(const std::string &result_file,
                                  int num_points,
                                  float &triag_cost,
                                  std::vector<std::tuple<int, int, int>> &triangles) {
    std::ifstream bin(result_file.c_str(), std::ifstream::binary);
    if (bin) {
        bin.read((char *) &triag_cost, sizeof(float));
        int num_triangles = num_points - 2;
        for (int triangle_idx = 0; triangle_idx < num_triangles; triangle_idx++) {
            int32_t p1, p2, p3;
            bin.read((char *) &p1, sizeof(int32_t));
            bin.read((char *) &p2, sizeof(int32_t));
            bin.read((char *) &p3, sizeof(int32_t));

            triangles.emplace_back(p1, p2, p3);
        }

        bin.close();
    } else {
        throw std::invalid_argument("Cannot open the result file '" + result_file + "' to read the result.");
    }
}

void write_result(float triag_cost,
                  const std::vector<std::tuple<int, int, int>> &triangles,
                  const std::string &result_file) {
    std::ofstream bout(result_file.c_str(), std::ofstream::binary | std::ofstream::trunc);
    if (bout) {
        bout.write((char *) &triag_cost, sizeof(float));
        for (const auto &triangle: triangles) {
            int32_t p1, p2, p3;
            std::tie(p1, p2, p3) = triangle;
            bout.write((char *) &p1, sizeof(int32_t));
            bout.write((char *) &p2, sizeof(int32_t));
            bout.write((char *) &p3, sizeof(int32_t));
        }

        bout.close();
    } else {
        throw std::invalid_argument("Cannot write the results, check the permissions.");
    }
}

void write_image(
        const std::vector<Point> &points,
        const std::vector<std::tuple<int, int, int>> &triangles,
        const std::string &image_filename) {
    constexpr uint32_t num_of_colors = 10;
    std::array<std::string, num_of_colors> colors = {
            "orange", "brown", "purple", "blue", "darksalmon", "yellow", "green", "red", "lime", "aqua"
    };

    float min_x = 10e6, max_x = -10e6, min_y = 10e6, max_y = -10e6;
    for (const Point &p: points) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);
    }

    float mult = 1600.0f / (max_x - min_x), height = std::ceil(mult * (max_y - min_y));

    std::ofstream im(image_filename);
    if (im) {
        im << R"(<svg xmlns="http://www.w3.org/2000/svg" width="1600" height=")" << height << "\">" << std::endl;

        std::default_random_engine generator;
        std::uniform_int_distribution<uint32_t> color_idx(0u, num_of_colors - 1u);
        for (const std::tuple<int, int, int> &t: triangles) {
            int i, j, k;
            std::tie(i, j, k) = t;

            std::array<Point, 3> p{points[i], points[j], points[k]};
            for (uint32_t counter = 0; counter < 3; ++counter) {
                p[counter].x = mult * (p[counter].x - min_x);
                p[counter].y = mult * (p[counter].y - min_y);
            }

            im << "\t<polygon points=\"" << p[0].x << "," << p[0].y << " " << p[1].x << "," << p[1].y << " " << p[2].x
               << "," << p[2].y << "\" ";
            im << "style=\"fill:" << colors[color_idx(generator)] << ";stroke:black;stroke-width=0.3\"/>" << std::endl;
        }

        for (uint32_t i = 0; i < points.size(); ++i) {
            std::array<Point, 2> p{points[i % points.size()], points[(i + 1) % points.size()]};
            for (uint32_t i = 0; i < 2; ++i) {
                p[i].x = mult * (p[i].x - min_x);
                p[i].y = mult * (p[i].y - min_y);
            }

            im << "\t<line x1=\"" << p[0].x << "\" y1=\"" << p[0].y << "\" x2=\"" << p[1].x << "\" y2=\"" << p[1].y
               << "\" ";
            im << R"(stroke-width="2" stroke="black"/>)" << std::endl;
        }

        im << "</svg>" << std::endl;
        im.close();
    } else {
        std::cerr << "Cannot write the result to svg file!" << std::endl;
    }
}

void print_help_page(char *program) {
    std::cout << "Triangulation of a convex polygon." << std::endl;
    std::cout << std::endl << "Usage:" << std::endl;
    std::cout << "\t" << program << " INPUT_PATH OUTPUT_PATH [options]" << std::endl << std::endl;
    std::cout << "General options:" << std::endl;
    std::cout << "\t--output-image IMAGE_PATH, -of IMAGE_PATH" << std::endl;
    std::cout << "\t\t_generates svg file demonstrating the triangulation." << std::endl;
    std::cout << "\t--help, -h" << std::endl;
    std::cout << "\t\t_prints this help." << std::endl;
}

int main(int argc, char *argv[]) {
    std::string image_filename, input_file, result_file;

    if (argc == 1) {
        print_help_page(argv[0]);
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        std::string parg = argv[i];
        if (parg == "--help" || parg == "-h") {
            print_help_page(argv[0]);
            return 0;
        }

        if (parg == "--output-image" || parg == "-of") {
            if (i + 1 < argc) {
                image_filename = argv[++i];
            } else {
                std::cerr << "Expected a filename for the output image!" << std::endl;
                return 1;
            }
        }

        if (!parg.empty() && parg[0] != '-') {
            if (input_file.empty()) {
                input_file = parg;
            } else {
                result_file = parg;
            }
        }
    }

    try {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        float criterion;
        std::vector<std::tuple<int, int, int>> triangles;
        const std::vector<Point> &points = read_problem(input_file);

        tie(triangles, criterion) = triangulate(points);
        double total_duration = std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::high_resolution_clock::now() - start).count();

        if (!result_file.empty()) {
            write_result(criterion, triangles, result_file);
        }

        if (!image_filename.empty()) {
            write_image(points, triangles, image_filename);
        }

        std::cout << "Cost of triangulation: " << criterion << std::endl;
        std::cout << "computational time: " << total_duration << " s" << std::endl;

    } catch (std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 2;
    }

    return 0;
}

