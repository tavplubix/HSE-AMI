#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <random>

//#define DEBUG
#define TLOG

#ifdef DEBUG
#include <unistd.h>
#endif




class Graph {
  public:
    using Vertex = size_t;
    using VertexSet = std::unordered_set<Vertex>;
    using AdjacencyList = std::unordered_map<Vertex, VertexSet>;

    void AddVertex(Vertex v) {
        adjacency_list_[v];
    }

    void AddEdge(Vertex u, Vertex v) {
        adjacency_list_[u].insert(v);
        adjacency_list_[v].insert(u);
    }

    const VertexSet& AdjacentVertices(Vertex v) const {
        const auto it = adjacency_list_.find(v);
        if (it != adjacency_list_.end()) {
            return it->second;
        } else {
            return empty_set_;
        }
    }

    VertexSet AllVertices() const {
        VertexSet vs;
        vs.reserve(adjacency_list_.size());
        for (const auto& pair : adjacency_list_) {
            const auto& vertex = pair.first;
            vs.insert(vertex);
        }
        return vs;
    }

    const AdjacencyList& AsAdjacencyList() const {
        return adjacency_list_;
    }

    size_t numberOfVertices() const {
        return adjacency_list_.size();
    }

    static const VertexSet empty_set_;
  private:
    AdjacencyList adjacency_list_;
};

const Graph::VertexSet Graph::empty_set_;

class VertexCut {
  public:
    explicit VertexCut(const Graph& graph)
        : graph_(graph), set_A_(graph.AllVertices()), set_B_(Graph::empty_set_) {}

    VertexCut(const VertexCut& other)
            : set_A_(other.set_A_), set_B_(other.set_B_), graph_(other.graph_) {}

    const Graph::VertexSet& GetSetA() const {
        return set_A_;
    }

    const Graph::VertexSet& GetSetB() const {
        return set_B_;
    }

    const int GetDeltaAfterMove(Graph::Vertex v) { // Delta of Max Cut after moving vertex v from one part to other
        int delta = 0;
        if (set_A_.find(v) != set_A_.end()) {
            for (const auto& u : graph_.AdjacentVertices(v)) {
                if (set_A_.find(u) != set_A_.end()) {
                    delta++;
                } else {
                    delta--;
                }
            }
        } else if (set_B_.find(v) != set_B_.end()) {
            for (const auto& u : graph_.AdjacentVertices(v)) {
                if (set_B_.find(u) != set_B_.end()) {
                    delta++;
                } else {
                    delta--;
                }
            }
        }
        return delta;
    }

    void MoveVertex(Graph::Vertex v) {
        if (set_A_.find(v) != set_A_.end()) {
            set_A_.erase(v);
            set_B_.insert(v);
        } else if (set_B_.find(v) != set_B_.end()) {
            set_A_.insert(v);
            set_B_.erase(v);
        }
        return;
    }

    size_t GetSizeOfCut() const {
        size_t cut_size = 0;
        for (const auto& v : set_A_) {
            const auto& neighbours = graph_.AdjacentVertices(v);
            for (const auto& u : set_B_) {
                if (neighbours.find(u) != neighbours.end()) {
                    cut_size++;
                }
            }
        }
        return cut_size;
    }

    const Graph& GetGraph() const {
        return graph_;
    }

    VertexCut& operator = (const VertexCut& v) noexcept {
        set_A_ = v.set_A_;
        set_B_ = v.set_B_;
        const_cast<Graph&>(graph_) = v.graph_;
    }

    VertexCut& operator = (VertexCut&& v) noexcept {
        set_A_ = std::move(v.set_A_);
        set_B_ = std::move(v.set_B_);
        const_cast<Graph&>(graph_) = v.graph_;
    }

  private:

    const Graph& graph_;
    Graph::VertexSet set_A_, set_B_;
};


class MaxCutSolver {
  public:
    virtual VertexCut Solve(const Graph& graph) const = 0;
    virtual ~MaxCutSolver() = default;
};

class HeuristicSolver : public MaxCutSolver {
  protected:
    mutable std::mt19937 randomGenerator;
    mutable std::uniform_int_distribution<> randomBool = std::uniform_int_distribution<>(0, 1);


  public:
    VertexCut generateRandomSolution(const Graph& graph) const {
        VertexCut solution(graph);
        for (const auto &vertex : graph.AllVertices())
            if (randomBool(randomGenerator))
                solution.MoveVertex(vertex);
        return solution;
    }
    void setSeed(std::mt19937::result_type seed) {
        randomGenerator.seed(seed);
    }
};



class GreedySolver final: public HeuristicSolver {
  public:
    GreedySolver(int seed = time(nullptr)) {
        setSeed(seed);
    }
    VertexCut Solve(const Graph& graph) const override {
        VertexCut currentSolution = generateRandomSolution(graph);
        int maxDelta;
        do {
            auto currentCutSize = currentSolution.GetSizeOfCut();
            Graph::Vertex vertexToMove;
            maxDelta = 0;
            for (const auto &vertex : graph.AllVertices()) {
                int delta = currentSolution.GetDeltaAfterMove(vertex);
                if (maxDelta < delta) {
                    maxDelta = delta;
                    vertexToMove = vertex;
                }
            }
            if (0 < maxDelta)
                currentSolution.MoveVertex(vertexToMove);
        } while (0 < maxDelta);
        return currentSolution;
    }
    size_t cutSizeSum = 0;
};



class  AnnealingSolver final: public HeuristicSolver {
    mutable std::cauchy_distribution<> randomDistance;
    mutable std::uniform_real_distribution<> randomDouble = std::uniform_real_distribution<>(0, 1);
    double T0;
    double maxIterationsFactor = 0.5;

    VertexCut generateNeighbor(const VertexCut& solution, double T) const {
        typename std::cauchy_distribution<>::param_type param(0, T);
        int dist = 1 + int(std::floor(std::abs(randomDistance(randomGenerator, param))));
#ifdef DEBUG
        std::cout << "\t dist = " << dist;
#endif
        VertexCut newSolution = solution;
        auto allVertices = solution.GetGraph().AllVertices();
        std::vector<Graph::Vertex> sample;
        auto end = std::sample(allVertices.begin(), allVertices.end(),
                               std::back_inserter(sample), std::abs(dist), randomGenerator);   // C++17
        for (const auto & vertex : sample)
            newSolution.MoveVertex(vertex);
        return newSolution;
    };
  public:
    AnnealingSolver(double T0, int seed = time(nullptr), double maxIterationsFactor = 0.5)
            : T0(T0), maxIterationsFactor(maxIterationsFactor) {
        setSeed(seed);
    }

    VertexCut Solve(const Graph& graph) const override {
        double T = T0;
        int k = 1;
        VertexCut solution = generateRandomSolution(graph);
        size_t E = solution.GetSizeOfCut();
        cutSizeAtIteration[0] += E;
        // int iterationWithoutChanges = 0;
        // while (iterationWithoutChanges < maxIterationsFactor * graph.numberOfVertices()) {
        while(k < 5000) {
            VertexCut n = generateNeighbor(solution, T);
            size_t newE = n.GetSizeOfCut();
            int dE = int(newE) - int(E);
            double h = exp(dE / T);
            double alpha = randomDouble(randomGenerator);
            if (alpha < h) {
                solution = n;
                E = newE;
                T = T / k;
                // iterationWithoutChanges = 0;
            }
            // ++iterationWithoutChanges;
            ++k;
            cutSizeAtIteration[k] += newE;
#ifdef DEBUG
            usleep(100000);
            std::cout << " \tk = " << k << "\t T = " << T << "\t h = " << h << "\t dE = " << dE << "\t E = " << E << std::endl;
#endif
        }
        return solution;
    }
    double getT() const {
        return T0;
    }

    mutable std::vector<size_t> cutSizeAtIteration = std::vector<size_t>(5001, 0);     // костыль
};



Graph RandomGraph(size_t size, double edge_probability) {
    Graph graph;
    for (Graph::Vertex v = 1; v <= size; ++v) {
        graph.AddVertex(v);
    }
    for (Graph::Vertex v = 1; v <= size; ++v) {
        for (Graph::Vertex u = v + 1; u <= size; ++u) {
            if (double(rand()) / RAND_MAX <= edge_probability) {
                graph.AddEdge(v, u);
            }
        }
    }
    return graph;
}



Graph StarGraph(size_t size) {
    Graph graph;
    for (Graph::Vertex v = 2; v <= size; ++v) {
        graph.AddEdge(1, v);
    }
    return graph;
}



Graph BipartiteGraph(size_t size, double edge_probability) {
    Graph graph;
    Graph::VertexSet left;
    Graph::VertexSet right;
    for (Graph::Vertex v = 1; v <= size; ++v) {
        graph.AddVertex(v);
        if (double(rand()) / RAND_MAX <= 0.5) {
            left.insert(v);
        } else {
            right.insert(v);
        }
    }

    for (const auto& v : left) {
        for (const auto& u : right) {
            if (double(rand()) / RAND_MAX <= edge_probability) {
                graph.AddEdge(v, u);
            }
        }
    }


    return graph;
}



int InitRandSeed(int argc, const char* argv[]) {
    int rand_seed;
    if (argc >= 2) {
        rand_seed = atoi(argv[1]);
    } else {
        rand_seed = time(nullptr);
    }
    srand(rand_seed);
    return rand_seed;
}



size_t TrySolver(const MaxCutSolver& solver, const std::vector<Graph>& graphs) {
    size_t result = 0;
    for (const auto& graph : graphs) {
        const auto vertex_cut = solver.Solve(graph);
        auto cost = vertex_cut.GetSizeOfCut();
        result += cost;
        std::cout << '.' << std::flush;
    }

    std::cout << "\nResult: " << result << std::endl;
}



std::vector<Graph> generateSet(int seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<size_t> randSize(200, 500);
    std::uniform_real_distribution<> randEdgeProb(0, 1);
    std::vector<Graph> graphs;
    for (size_t i = 0; i < 20; ++i) {
        graphs.push_back(RandomGraph(randSize(gen), randEdgeProb(gen)));
        graphs.push_back(BipartiteGraph(randSize(gen), randEdgeProb(gen)));
    }
    for (size_t i = 0; i < 10; ++i) {
        graphs.push_back(StarGraph(randSize(gen)));
    }
    return  graphs;
}


double chooseT(int seed) {
#ifdef TLOG
    std::ofstream tlog("T.log");
#endif
    auto graphsSet = generateSet(seed);
    std::vector<AnnealingSolver> solvers;
    for (int i = -4; i <= 4; ++i)
        solvers.emplace_back(pow(10, i), seed);

    double theBestT = 0;
    size_t maxCutSum = 0;
    for (auto &solver : solvers) {
        std::cout << "*" << std::flush;
        TrySolver(solver, graphsSet);
        if (maxCutSum < solver.cutSizeAtIteration[5000]) {
            std::cout << 'm';
            maxCutSum = solver.cutSizeAtIteration[5000];
            theBestT = solver.getT();
        }
#ifdef TLOG
        tlog << solver.getT() << " : ";
        for (auto i : solver.cutSizeAtIteration)
            tlog << i << ' ';
        tlog << std::endl;
#endif
    }
    return theBestT;
}



int main(int argc, const char* argv[]) {
    int randseed = InitRandSeed(argc, argv);
    std::cout << "Using rand seed: " << randseed << std::endl;

    std::cout << "Choosing T for AnnealingSolver" << std::endl;
    double T = chooseT(randseed);
    std::cout << "\nChosen T = " << T << std::endl;

    randseed = InitRandSeed(argc, argv);
    std::cout << "Using rand seed: " << randseed << std::endl;

    AnnealingSolver annealingSolver(T, randseed);
    GreedySolver greedySolver(randseed);
    auto graphsSet = generateSet(randseed);

    std::cout << "Running AnnealingSolver" << std::endl;
    TrySolver(annealingSolver, graphsSet);

    std::cout << "Running GreedySolver" << std::endl;
    TrySolver(greedySolver, graphsSet);


    return 0;
}