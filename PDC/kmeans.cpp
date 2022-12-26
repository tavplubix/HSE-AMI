#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <omp.h>
#include <atomic>

#include <cstdint>

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
#define ASM

inline double Distance(const Point& point1, const Point& point2) {
    double distance_sqr = 0;
    size_t dimensions = point1.size();
#ifdef ASM
    if (dimensions == 2) {
        const double *beg1 = &point1[0];
        const double *beg2 = &point2[0];
        asm(
                "movupd     (%1), %0  \n"
                "subpd      (%2), %0  \n"
                "mulpd      %0, %0          \n"
                "haddpd     %0, %0          \n"

        : "=x" (distance_sqr)     // 0
        : "r" (beg1),             // 1
          "r" (beg2)              // 2
        : );
    } else {
        for (size_t i = 0; i < dimensions; ++i)
            distance_sqr += (point1[i] - point2[i]) * (point1[i] - point2[i]);
    }
#else
    for (size_t i = 0; i < dimensions; ++i)
        distance_sqr += (point1[i] - point2[i]) * (point1[i] - point2[i]);

#endif
    return distance_sqr;
}

size_t FindNearestCentroid(const Points& centroids, const Point& point) {
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
    const Point c1 = centroids[rand() % K];
    const Point c2 = centroids[rand() % K];
    const Point c3 = centroids[rand() % K];
    size_t dimensions = new_position.size();
    //#pragma omp simd
    for (size_t d = 0; d < dimensions; ++d) {
        new_position[d] = (c1[d] + c2[d] + c3[d]) / 3;
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

    std::atomic<bool> converged(false);
    while (!converged) {
        converged = true;
        vector<size_t> clusters_sizes(K);

        #pragma omp parallel
        {

            #pragma omp for
            for (size_t i = 0; i < data_size; ++i) {
                size_t nearest_cluster = FindNearestCentroid(centroids, data[i]);
                if (clusters[i] != nearest_cluster) {
                    clusters[i] = nearest_cluster;
                    converged = false;
                }
            }
            #pragma omp barrier

            #pragma omp single
            centroids = Points(K, dimensions);

            Points local_centroids(K, dimensions);
            vector<size_t> local_clusters_sizes(K);
            #pragma omp for nowait
            for (size_t i = 0; i < data_size; ++i) {
                ++local_clusters_sizes[clusters[i]];
                Point lc = local_centroids[clusters[i]];
                const Point p = data[i];
                for (size_t d = 0; d < dimensions; ++d) {
                    lc[d] += p[d];
                }
            }

            for (size_t i = 0; i < K; ++i) {
                Point c = centroids[i];
                const Point lc = local_centroids[i];
                for (size_t d = 0; d < dimensions; ++d) {
                    #pragma omp atomic
                    c[d] += lc[d];
                }
                #pragma omp atomic
                clusters_sizes[i] += local_clusters_sizes[i];
            }

            #pragma omp barrier

            #pragma omp for
            for (size_t i = 0; i < K; ++i) {
                Point ci = centroids[i];
                if (clusters_sizes[i] != 0) {
                    for (size_t d = 0; d < dimensions; ++d) {
                        ci[d] /= clusters_sizes[i];
                    }
                }
            }
        }


        for (size_t i = 0; i < K; ++i) {
            if (clusters_sizes[i] == 0) {
                Point ci = centroids[i];
                SetRandomPosition(centroids, ci);
            }
        }
    }

    return clusters;
}



// optimized version using atof()
Points ReadPoints(ifstream& input) {
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

    const char* env_var = std::getenv("OMP_NUM_THREADS");
    int max_treads = env_var ? std::max(std::atoi(env_var), 1) : omp_get_max_threads();
    int approx_lines_per_thread = data_size / max_treads;
    const char* borders[max_treads + 1];
    int cur_lines = 0;
    int cur_idx = 1;
    const char* s = content.c_str() + 1;
    borders[0] = s;
    while (cur_idx < max_treads) {
        if (*s++ == '\n') {
            ++cur_lines;
            if (cur_lines == approx_lines_per_thread) {
                borders[cur_idx++] = s;
                cur_lines = 0;
            }
        }
    }
    borders[max_treads] = content.c_str() + content.length();

    #pragma omp parallel for
    for (int i = 0; i < max_treads; ++i) {
        const char* pos = borders[i];
        const char* const end = borders[i + 1];
        size_t bias = i * approx_lines_per_thread;
        size_t line = 0;
        while (pos < end) {
            Point p = data[bias + line];
            for (size_t d = 0; d < dimensions; ++d) {
                char* tmp;
                p[d] = strtod(pos, &tmp);
                pos = tmp;
                ++pos;
            }
            ++line;
        }
    }

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