#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <random>

size_t N, K;

std::vector<std::vector<int>> chessboard;
std::vector<size_t> rows;
std::vector<char> is_fixed;


std::vector<std::vector<int>> _chessboard;
std::vector<size_t> _rows;
double _tc;

void add_diag(size_t r, size_t c, int val)
{
    for (size_t i = 1; r + i < N && c + i < N; ++i)
        chessboard[r + i][c + i] += val;
    for (size_t i = 1; r + i < N && c - i < N; ++i)
        chessboard[r + i][c - i] += val;
    for (size_t i = 1; r - i < N && c + i < N; ++i)
        chessboard[r - i][c + i] += val;
    for (size_t i = 1; r - i < N && c - i < N; ++i)
        chessboard[r - i][c - i] += val;
}

void add_queen(size_t r, size_t c)
{
    add_diag(r, c, 1);
}

void remove_queen(size_t r, size_t c)
{
    add_diag(r, c, -1);
}

inline bool same_diag(size_t r1, size_t c1, size_t r2, size_t c2)
{
    size_t rdiff = std::max(r1, r2) - std::min(r1, r2);
    size_t cdiff = std::max(c1, c2) - std::min(c1, c2);
    return rdiff == cdiff;

}

inline int conflicts_if_move(size_t from_row, size_t to_row)
{
    const size_t col = rows[from_row];
    int conflicts = chessboard[to_row][col] - chessboard[from_row][col];
    for (size_t row = 0; row < N; ++row)
    {
        if (row == from_row || row == to_row)
            continue;
        const int col = rows[row];
        if (same_diag(from_row, col, row, col))
            --conflicts;
        if (same_diag(to_row, col, row, col))
            ++conflicts;
    }
    return conflicts;
}

int conflicts_if_swap_rows(size_t row1, size_t row2)
{
    return conflicts_if_move(row1, row2) + conflicts_if_move(row2, row1);
}

int total_conflicts = 0;

void initial_solution()
{
    for (size_t i = 0; i < N; ++i)
    {
        rows[i] = i;
        add_queen(i, i);
    }
    total_conflicts = (N - 1) * N;
}

void swap_rows(size_t r1, size_t r2)
{
    remove_queen(r1, rows[r1]);
    add_queen(r1, rows[r2]);
    remove_queen(r2, rows[r2]);
    add_queen(r2, rows[r1]);
    std::swap(rows[r1], rows[r2]);
}

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> rnd;
std::uniform_real_distribution<> rnd_real(0, 1);

double T0 = 1000000;
double T = T0;
int bestE = 1000000000;
int k = 0;

void step()
{
    if (total_conflicts < bestE)
        bestE = total_conflicts;
    for (size_t i = 0; i < N * N; ++i)
    {
        size_t r1 = rnd(gen);
        while (is_fixed[r1])
            r1 = rnd(gen);
        size_t r2 = rnd(gen);
        while (is_fixed[r2] || r2 == r1)
            r2 = rnd(gen);

        int c = conflicts_if_swap_rows(r1, r2);

        double h = 1.0 / (1.0 + exp(((total_conflicts + c - bestE) / T)));
        double a = rnd_real(gen);
        if (a < h)
        {
            swap_rows(r1, r2);
            total_conflicts +=c;
            ++k;
            T = T0 / k;
            return;
        }
    }
}

int main()
{
    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    std::cin >> N >> K;
    rnd = std::uniform_int_distribution<>(0, N - 1);
    rows.resize(N);
    is_fixed.resize(N, false);
    chessboard.resize(N, std::vector<int>(N, 0));
    initial_solution();
    for (size_t i = 0; i < K; ++i)
    {
        size_t r, c;
        std::cin >> r >> c;
        --r; --c;
        auto row_to_move_from = std::find(rows.cbegin(), rows.cend(), c);
        total_conflicts += conflicts_if_swap_rows(r, row_to_move_from - rows.cbegin());
        swap_rows(r, row_to_move_from - rows.cbegin());
        is_fixed[r] = true;
    }
    _chessboard = chessboard;
    _rows = rows;
    _tc = total_conflicts;

    while (total_conflicts)
        step();

    for (const auto & c : rows)
        std::cout << c + 1 << '\n';
}
