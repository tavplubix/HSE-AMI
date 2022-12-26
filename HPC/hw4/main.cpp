#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <boost/mpi.hpp>
#include <chrono>

struct Parameters
{
    double h = 0.02;
    double T = 0.1;
    double dt = 0.0000001;
    double u0 = 1;
    double l = 1;
    double k = 1;

    Parameters(size_t N = 1000, double dt_ = 0.0000001, double T_ = 0.1) : h(1.0 / N), dt(dt_), T(T_)
    {
        if (1 <= k * dt / h / h)
            std::cout << "\n\nERROR 1 <= k * dt / h^2 = \n\n";// throw std::runtime_error("1 <= k * dt / h^2 = " + std::to_string(k * dt / h / h));
    }
};

using Data = std::vector<double>;

struct Solver
{
    Parameters params;
    Data state;
    Data new_state;
    double cur_t = 0;
    size_t offset;

    double solutionByFormula(double x, double t) const
    {
        double sum = 0;
        double prev_sum = -1;
        double m = 0;
        while (prev_sum != sum)
        {
            prev_sum = sum;
            double exp_arg = -params.k * M_PI * M_PI * (2*m + 1)*(2*m + 1) * t / (params.l * params.l);
            double sin_arg = M_PI * (2*m + 1) * x / params.l;
            sum += exp(exp_arg) * sin(sin_arg) / (2*m + 1);
            ++m;
        }
        return sum * 4 * params.u0 / M_PI;
    }

    virtual double leftBound() { return 0; }
    virtual double rightBound() { return 0; }

    Solver(const Parameters & params_, size_t size = 0, size_t offset_ = 0)
    : params(params_)
    , state(size ? size : round(params.l / params.h), params.u0)
    , new_state(state.size(), params.u0)
    , offset(offset_)
    {
        if (state.size() < 2)
            throw std::runtime_error("too small");
    }

    inline double computeNextStateElem(double left, double x, double right) const
    {
        double sum = left - 2 * x + right;
        return x + params.k * params.dt / params.h / params.h * sum;
    }

    virtual void prepareStep()
    {
    }

    virtual void computeOneStep()
    {
        new_state.front() = computeNextStateElem(leftBound(), state[0], state[1]);
        for (size_t i = 1; i < new_state.size() - 1; ++i)
            new_state[i] = computeNextStateElem(state[i - 1], state[i], state[i+1]);
        new_state.back() = computeNextStateElem(state[state.size() - 2], state[state.size() - 1], rightBound());
    }

    void printAndValidate(bool print_solutions = false) const
    {
        Data x_vals;
        Data res;
        Data res_ref;
        for (size_t i = 0; i <= 10; ++i)
        {
            double x = 0.1 * i /* * params.l*/;
            size_t idx = round(x * state.size() /* * l*/);
            if (state.size() <= idx)
                idx = state.size() - 1;
            x_vals.push_back(x);
            res.push_back(state[idx]);
            res_ref.push_back(solutionByFormula(x, cur_t));
        }

        auto print = [&](const std::string & prefix, const Data & arr) {
            std::cout << prefix << "\t";
            for (const auto & v : arr)
                std::cout << v << "\t";
            std::cout << std::endl;
        };

        if (print_solutions)
        {
            print("x", x_vals);
            print("T", res);
            print("Tr", res_ref);
        }

        const double max_diff = 0.1;
        for (size_t i = 0; i < res.size(); ++i)
        {
            if (max_diff < std::abs(res[i] - res_ref[i]) && offset == 0)
                std::cout << "\n\n WARNING \n\n";
        }
    }

    virtual void finalize() {}

    virtual size_t run()
    {

        auto start = std::chrono::system_clock::now();
        auto prev = start;
        size_t iter = 0;
        while (cur_t < params.T)
        {
            prepareStep();
            computeOneStep();
            std::swap(state, new_state);
            cur_t += params.dt;
            ++iter;

            auto tmp = std::chrono::system_clock::now();
            if (offset == 0 && 5000 < std::chrono::duration_cast<std::chrono::milliseconds>(tmp - prev).count())
            {
                std::cout << "Iter " << iter << " t = " << cur_t << " of " << params.T << std::endl;
                prev = tmp;
            }
        }

        finalize();

        auto end = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

};

namespace mpi = boost::mpi;

struct MPISolver : public Solver
{
    mpi::environment & env;
    mpi::communicator & comm;
    size_t total_size;
    bool is_first;
    bool is_last;

    static std::pair<size_t, size_t> getSizeAndOffset(const Parameters & params_, size_t nodes, size_t rank)
    {
        size_t t_size = round(params_.l / params_.h);
        size_t per_node = t_size / nodes;
        size_t offset = per_node * rank;
        if (rank + 1 == nodes)
            per_node = t_size - offset;
        return std::make_pair(per_node, offset);
    }

    MPISolver(const Parameters & params_, mpi::environment & env_, mpi::communicator & comm_)
    : Solver(params_,
             getSizeAndOffset(params_, comm_.size(), comm_.rank()).first,
             getSizeAndOffset(params_, comm_.size(), comm_.rank()).second)
    , env(env_)
    , comm(comm_)
    {
        std::cout << "Node " << comm.rank() + 1 << " of " << comm.size() << ": offset = " << offset << ", size = " << state.size() << std::endl;
        is_first = comm.rank() == 0;
        is_last = comm.rank() == comm.size() - 1;
    }

    static constexpr size_t LEFT_BOUND_TAG = 1;
    static constexpr size_t RIGHT_BOUND_TAG = 2;
    static constexpr size_t RESULT_TAG = 3;

    void prepareStep() override
    {
        if (!is_last)
            comm.send(comm.rank() + 1, LEFT_BOUND_TAG, state.back());
        if (!is_first)
            comm.send(comm.rank() - 1, RIGHT_BOUND_TAG, state.front());
    }


    double leftBound() override
    {
        if (is_first)
            return Solver::leftBound();
        double res;
        comm.recv(comm.rank() - 1, LEFT_BOUND_TAG, res);
        return res;
    }

    double rightBound() override
    {
        if (is_last)
            return Solver::leftBound();
        double res;
        comm.recv(comm.rank() + 1, RIGHT_BOUND_TAG, res);
        return res;
    }

    void finalize() override
    {
        if (comm.rank())
        {
            comm.send(0, RESULT_TAG, state);
        }
        else
        {
            Data whole_state(state);
            for (size_t i = 1; i < comm.size(); ++i)
            {
                Data remote_state;
                comm.recv(i, RESULT_TAG, remote_state);
                whole_state.insert(whole_state.end(), remote_state.begin(), remote_state.end());
            }
            state = whole_state;
        }
    }

};

void run_all(mpi::environment & env, mpi::communicator & comm)
{

    for (auto N : {50, 2000, 10000, 50000})
    {
        if (8 <= comm.size() && N == 50)
            continue;
        if (comm.rank() == 0)
            std::cout << "\n\nRUN FOR " << N << " ELEMENTS\n\n" << std::endl;

        double dt = 0.1 / N / N;
        double T = 0.1;
        if (N == 10000)
            T = 0.01;
        if (N == 50000)
            T = 0.001;

        MPISolver solver(Parameters(N, dt, T), env, comm);
        //Solver solver(Parameters{});
        auto t = solver.run();

        if (comm.rank() == 0)
        {
            std::cout << "Time: " << t << " ms" << std::endl;
            solver.printAndValidate(true);
            if (N != 50)
                std::cerr << t << "\t";
        }

        sleep(2);
    }
    if (comm.rank() == 0)
        std::cerr << std::endl;
}

int main(int argc, char ** argv)
{
    try
    {
        mpi::environment env(argc, argv);
        mpi::communicator world;

        run_all(env, world);
    }
    catch (const std::exception & e)
    {
        std::cout << "ERROR: " << e.what();
    }

    return 0;
}
