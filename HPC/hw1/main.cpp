#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <queue>
#include <atomic>
#include <utility>
#include <shared_mutex>
#include <iostream>
#include <thread>

struct KnapsackProblem
{
    struct Item
    {
        size_t cost;
        size_t weight;
        size_t idx;
    };

    size_t W;
    std::vector<Item> items;

    void readFromFile(const std::string & file_path)
    {
        std::ifstream in(file_path);
        size_t N;
        in >> N >> W;
        items.resize(N);
        for (size_t i = 0; i < N; ++i)
        {
            in >> items[i].cost >> items[i].weight;
            items[i].idx = i + 1;
        }

        std::sort(items.begin(), items.end(), [](const Item & i1, const Item & i2)
        {
            return static_cast<double>(i1.cost) / i1.weight > static_cast<double>(i2.cost) / i2.weight;
        });
    }

    using CostBounds = std::pair<size_t, double>;

    template<typename Iter>
    static CostBounds greedyUpperBound(Iter beg, Iter end, size_t max_weight)
    {
        size_t cost = 0;
        size_t weight = 0;
        while (beg != end && weight + beg->weight <= max_weight)
        {
            cost += beg->cost;
            weight += beg->weight;
            ++beg;
        }

        if (beg == end)
            return {cost, cost};

        size_t free_space = max_weight - weight;
        double upper_bound = cost + free_space * (static_cast<double>(beg->cost) / beg->weight);
        return {cost, upper_bound};
    }

    struct PartialSolutionState
    {
        size_t cost = 0;
        size_t weight = 0;
        size_t total_cost = 0;
        double bound = std::numeric_limits<double>::max();
        std::vector<bool> items_mask;
    };

    using TaskQueue = std::queue<PartialSolutionState>;
    TaskQueue tasks;

    /// Statistics
    std::atomic_size_t nodes_visited{0};
    std::atomic_size_t rlocks{0};
    std::atomic_size_t wlocks{0};

    std::atomic_size_t global_max_cost{0};
    PartialSolutionState best_solution;

    void dfs(PartialSolutionState & state, std::vector<PartialSolutionState> & good_solutions, size_t max_depth)
    {
        ++nodes_visited;
        CostBounds bounds = greedyUpperBound(items.cbegin() + state.items_mask.size(), items.cend(), W - state.weight);
        double partial_solution_upper_bound = state.cost + bounds.second;
        if (partial_solution_upper_bound <= global_max_cost)
            return;

        if (max_depth == 0)
        {
            good_solutions.emplace_back(state);
            good_solutions.back().bound = partial_solution_upper_bound;
            good_solutions.back().total_cost = state.cost + bounds.first;
            return;
        }

        /// Take item (if we can)
        state.items_mask.push_back(true);
        state.cost += items[state.items_mask.size() - 1].cost;
        state.weight += items[state.items_mask.size() - 1].weight;
        if (state.weight <= W)
            dfs(state, good_solutions, max_depth - 1);

        /// Do not take item
        state.cost -= items[state.items_mask.size() - 1].cost;
        state.weight -= items[state.items_mask.size() - 1].weight;
        state.items_mask.back() = false;
        dfs(state, good_solutions, max_depth - 1);

        /// Restore state
        state.items_mask.pop_back();
    }

    std::shared_mutex mutex;


    void processTask(PartialSolutionState & state, size_t max_depth)
    {
        if (state.bound < global_max_cost)
            return;

        size_t dfs_depth = std::min(max_depth, items.size() - state.items_mask.size());

        std::vector<PartialSolutionState> good_solutions;

        dfs(state, good_solutions, dfs_depth);

        size_t local_max_cost = 0;
        size_t best_idx = 0;

        mutex.lock();
        for (size_t i = 0; i < good_solutions.size(); ++i)
        {
            if (local_max_cost < good_solutions[i].total_cost)
            {
                local_max_cost = good_solutions[i].total_cost;
                best_idx = i;
            }
            tasks.push(good_solutions[i]);
        }

        if (global_max_cost < local_max_cost)
        {
            best_solution = good_solutions[best_idx];
            global_max_cost = local_max_cost;
        }
        mutex.unlock();
    }

    std::atomic_size_t active_threads{0};
    std::condition_variable_any cv;

    void work()
    {
        while (true)
        {
            PartialSolutionState state;
            {
                std::shared_lock<std::shared_mutex> lock(mutex);
                cv.wait(lock, [&]() { return !active_threads || !tasks.empty(); });
                if (!active_threads && tasks.empty())
                    return;
                if (tasks.empty())
                    continue;
                state = tasks.front();
                ++active_threads;
                tasks.pop();
            }
            processTask(state, 1);
            --active_threads;
            cv.notify_one();
        }
    }

    void solveMt(size_t num_threads)
    {
        tasks.push({});
        std::vector<std::thread> threads;
        for (size_t i = 0; i < num_threads; ++i)
            threads.emplace_back([&](){ work(); });

        for (size_t i = 0; i < num_threads; ++i)
            threads[i].join();
    }

    void printSolution() const
    {
        std::cout << "Cost " << best_solution.cost << std::endl;
        std::cout << "Total cost " << best_solution.total_cost << std::endl;
        std::cout << "Weight " << best_solution.weight << std::endl;
        std::cout << "Bound " << best_solution.bound << std::endl;
        std::cout << "Count " << best_solution.items_mask.size() << std::endl;
        std::cout << "Items ";
        for (size_t i = 0; i < best_solution.items_mask.size(); ++i)
            if (best_solution.items_mask[i])
                std::cout << items[i].idx << " ";
    }
};

int main(int argc, char ** argv)
{
    KnapsackProblem p;
    p.readFromFile("small_tests/41.in");
    p.solveMt(2);
    p.printSolution();

}
