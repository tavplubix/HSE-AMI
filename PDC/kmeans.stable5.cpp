#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <omp.h>
#include <atomic>

#include <chrono>

using namespace std;

//typedef vector<double> Point;
//typedef vector<Point> Points;

class Points;

class Point {
public:
    Point(std::vector<double>& points, const size_t& dem, size_t pointIdx) : data(points.data() + dem * pointIdx), mDem(dem){
    }

    double& operator[](size_t idx) {
        return data[idx];
    }

    const double& operator[](size_t idx) const {
        return data[idx];
    }

    size_t size() const noexcept {
        return mDem;
    }
    Point(const Point& p) : data(data), mDem(p.mDem) {}
    Point& operator=(const Point& p) {
        for (size_t i = 0; i < mDem; ++i)
            data[i] = p[i];
        return *this;
    }
    Point(Point&& p) : data(p.data), mDem(p.mDem){}
    Point& operator = (Point&& p) = delete;

    ~Point() {}

    double* data;
    const size_t& mDem;
};

class Points {
public:
    Points(size_t size, size_t dem) : mSize(size), mDem(dem) {
        mData.resize(size * dem);
    }
    Point operator[](size_t idx) {
        return Point(mData, mDem, idx);
    }
    const Point operator[](size_t idx) const {
        return Point(const_cast<std::vector<double>&>(mData), mDem, idx);
    }
    size_t size() const noexcept {
        return mSize;
    }


    std::vector<double> mData;
    size_t mSize;
    size_t mDem;
};

// Gives random number in range [0..max_value]
unsigned int UniformRandom(unsigned int max_value) {
    unsigned int rnd = ((static_cast<unsigned int>(rand()) % 32768) << 17) |
                       ((static_cast<unsigned int>(rand()) % 32768) << 2) | rand() % 4;
    return ((max_value + 1 == 0) ? rnd : rnd % (max_value + 1));
}

double Distance(const Point& point1, const Point& point2) {
    double distance_sqr = 0;
    size_t dimensions = point1.size();
    for (size_t i = 0; i < dimensions; ++i) {
        distance_sqr += (point1[i] - point2[i]) * (point1[i] - point2[i]);
    }
    return distance_sqr;
}

size_t FindNearestCentroid(const Points& centroids, const Point& point) {
    double dst[centroids.size()];
    double min_distance = Distance(point, centroids[0]);
    size_t centroid_index = 0;
    for (size_t i = 1; i < centroids.size(); ++i) {
        double distance = Distance(point, centroids[i]);
        if (distance < min_distance) {
            min_distance = distance;
            centroid_index = i;
        }
    }
    return centroid_index;
}

// Calculates new centroid position as mean of positions of 3 random centroids
void SetRandomPosition(const Points& centroids, Point& new_position) {
    size_t K = centroids.size();
    int c1 = rand() % K;
    int c2 = rand() % K;
    int c3 = rand() % K;
    size_t dimensions = new_position.size();
    for (size_t d = 0; d < dimensions; ++d) {
        new_position[d] = (centroids[c1][d] + centroids[c2][d] + centroids[c3][d]) / 3;
    }
}


vector<size_t> KMeans(const Points& data, size_t K) {
    size_t data_size = data.size();
    size_t dimensions = data[0].size();
    vector<size_t> clusters(data_size);

    // Initialize centroids randomly at data points
    Points centroids(K, dimensions);
    for (size_t i = 0; i < K; ++i) {
        centroids[i] = data[UniformRandom(data_size - 1)];
    }
    long parallel_time = 0;
    long sum_time = 0;
    long div_time = 0;
    long rand_time = 0;
    auto all_start = std::chrono::steady_clock::now();
    std::atomic<bool> converged(false);
    while (!converged) {
        converged = true;
        auto start = std::chrono::steady_clock::now();
        #pragma omp parallel for
        for (size_t i = 0; i < data_size; ++i) {
            size_t nearest_cluster = FindNearestCentroid(centroids, data[i]);
            if (clusters[i] != nearest_cluster) {
                clusters[i] = nearest_cluster;
                converged = false;
            }
        }
        parallel_time += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
        start = std::chrono::steady_clock::now();
        if (converged) {
            break;
        }



        vector<size_t> clusters_sizes(K);
        centroids = Points(K, dimensions);
        for (size_t i = 0; i < data_size; ++i) {
            Point c = centroids[clusters[i]];
            Point p = data[i];
            for (size_t d = 0; d < dimensions; ++d) {
                c[d] += p[d];
            }
            ++clusters_sizes[clusters[i]];
        }



        sum_time += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
        start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < K; ++i) {
            if (clusters_sizes[i] != 0) {
                for (size_t d = 0; d < dimensions; ++d) {
                    centroids[i][d] /= clusters_sizes[i];
                }
            }
        }
        div_time += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
        start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < K; ++i) {
            if (clusters_sizes[i] == 0) {
                Point ci = centroids[i];
                SetRandomPosition(centroids, ci);
            }
        }
        rand_time += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    }

    std::cerr << "find " << parallel_time << '\n';
    std::cerr << "sum " << sum_time <<'\n';
    std::cerr << "div " << div_time <<'\n';
    std::cerr << "rand " << rand_time <<'\n';
    std::cerr << "all " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - all_start).count() <<'\n';
    return clusters;
}

// void ReadPoints(Points* data, ifstream& input) {
//     size_t data_size;
//     size_t dimensions;
//     input >> data_size >> dimensions;
//     data->assign(data_size, Point(dimensions));
//     for (size_t i = 0; i < data_size; ++i) {
//         for (size_t d = 0; d < dimensions; ++d) {
//             double coord;
//             input >> coord;
//             (*data)[i][d] = coord;
//         }
//     }
// }

// optimized version using atof()
Points ReadPoints(ifstream& input) {
    auto start = std::chrono::steady_clock::now();
    size_t data_size;
    size_t dimensions;
    input >> data_size >> dimensions;
    string content;
    auto cur_pos = input.tellg();
    input.seekg(0, std::ios::end);
    content.resize(input.tellg() - cur_pos);
    input.seekg(cur_pos, std::ios::beg);
    input.read(&content[0], content.size());


    Points data(data_size, dimensions);

    char* pos = &content[0];
    for (size_t i = 0; i < data_size; ++i) {
        Point p = data[i];
        for (size_t d = 0; d < dimensions; ++d) {
            p[d] = strtod(pos, &pos);
            ++pos;
        }
    }
    long read_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    std::cerr << "read_time " << read_time << '\n';
    return data;
}

void WriteOutput(const vector<size_t>& clusters, ofstream& output) {
    for (size_t i = 0; i < clusters.size(); ++i) {
        output << clusters[i] << '\n';
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::printf("Usage: %s number_of_clusters input_file output_file\n", argv[0]);
        return 1;
    }
    size_t K = atoi(argv[1]);

    char* input_file = argv[2];
    ifstream input;
    input.open(input_file, ifstream::in);
    if (!input) {
        cerr << "Error: input file could not be opened\n";
        return 1;
    }

    Points data = ReadPoints(input);
    input.close();

    char* output_file = argv[3];
    ofstream output;
    output.open(output_file, ifstream::out);
    if (!output) {
        cerr << "Error: output file could not be opened\n";
        return 1;
    }

    srand(123); // for reproducible results

    vector<size_t> clusters = KMeans(data, K);

    WriteOutput(clusters, output);
    output.close();

    return 0;
}