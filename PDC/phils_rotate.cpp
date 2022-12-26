#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <exception>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

unsigned debug_flag;

void sleep(unsigned milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

typedef std::chrono::microseconds Microseconds;
typedef std::chrono::steady_clock Clock;
typedef Clock::time_point Time;


constexpr unsigned unknown_philosopher_id = 0;

class Fork {
public:
    Fork(unsigned phil_left, unsigned phil_right, unsigned fork_delay)
            : phil_left(phil_left)
            , phil_right(phil_right)
            , fork_delay(fork_delay)
            , owner(unknown_philosopher_id){
    }

    void trytake(unsigned id, bool& ok) {
        check_adjacent(id);
        ok = mutex.try_lock();
        if (!ok) return;
        owner = id;
        _take(id);
    }

    void take(unsigned id) {
        check_adjacent(id);
        mutex.lock();
        owner = id;
        _take(id);
    }

    void put(unsigned id) {
        _put(id);
        if (id != owner)
            throw std::runtime_error("cannot put fork: owner = " + std::to_string(owner) +
                                     " phil = " + std::to_string(id));
        owner = unknown_philosopher_id;
        mutex.unlock();
    }

    void _take(unsigned id) noexcept {
        if (fork_delay > 0) {
            sleep(fork_delay);
        }
        if (debug_flag) {
            if (id == phil_left) {
                std::printf("[%u] took right fork\n", id);
            } else {
                std::printf("[%u] took left fork\n", id);
            }
        }
    }

    void _put(unsigned id) noexcept {
        if (fork_delay > 0) {
            sleep(fork_delay);
        }
        if (debug_flag) {
            if (id == phil_left) {
                std::printf("[%u] put right fork\n", id);
            } else {
                std::printf("[%u] put left fork\n", id);
            }
        }
    }

private:
    void check_adjacent(unsigned id) const {
        if (id != phil_right && id != phil_left)
            throw std::runtime_error("wrong fork: phil_right = " + std::to_string(phil_right)
                                     + " phil_left = " + std::to_string(phil_left)
                                     + " phil = " + std::to_string(id));
    }

private:
    unsigned phil_left;
    unsigned phil_right;
    unsigned fork_delay;
    std::atomic<unsigned> owner;
    std::mutex mutex;
};

std::atomic<unsigned> bias;
std::atomic<unsigned> cnt(0);

std::atomic<bool> on_barrier(false);

class Philosopher {
public:
    Philosopher(unsigned id, Fork* fork_left, Fork* fork_right, unsigned think_delay,
                unsigned eat_delay, std::deque<Philosopher>& all)
            : id(id)
            , fork_left(fork_left)
            , fork_right(fork_right)
            , r_engine(std::random_device()())
            , think_delay_dist(0, think_delay)
            , eat_delay_dist(0, eat_delay)
            , eat_count(0)
            , wait_time(0)
            , total_time(0)
            , wait_start(Clock::now())
            , stop_flag(false)
            , all(all)
            , wait(false)
            , got(false) {
    }

    void run() {
        Time start = Clock::now();
        N = all.size();
        unsigned next_id = id % N + 1;
        while (!stop_flag) {
            think();

            Fork* first = fork_right;
            Fork* second = fork_left;
            bool inverse = id == (bias % N) + 1;
            if (inverse) {
                printf("%u inverse\n", id);
                std::swap(first, second);
                ++cnt;
            }
            int natempts = 0;
            constexpr int max_attempts = 10;
            while (natempts < max_attempts) {
                first->take(id);
                bool ok;
                second->trytake(id, ok);
                if (ok) {
                    break;
                } else {
                    //std::printf("%u retry\n", id);
                    first->put(id);
                    std::this_thread::yield();
                    ++natempts;
                }
            }
            if (natempts == max_attempts) {
                first->take(id);
                second->take(id);
            }
            //std::printf("%u attempt %d\n", id, natempts);

            eat();

            second->put(id);
            first->put(id);

            if (inverse && cnt % 4 == 0) {
                //printf("id = %u next_id = %u\n", id, next_id);
                all[next_id].wait = true;
                //printf("%u begin wait next\n", id);
                while (!stop_flag &&  !all[next_id].got);
                //printf("%u end wait next\n", id);
                ++bias;
                cnt = 0;
                all[next_id].wait = false;
                all[next_id].got = false;
            }

            if (wait) {
                //printf("%u begin wait\n", id);
                while (!stop_flag && wait) got = true;
                //printf("%u end wait\n", id);
            }

        }
        //got = true;
        //all[next_id].wait = false;

        //if (debug_flag)
            std::printf("[%u] stopped\n", id);
        total_time = std::chrono::duration_cast<Microseconds>(Clock::now() - start).count();
    }

    void stop() {
        stop_flag = true;
    }

    void printStats() const {
        std::printf("[%u] %u %lld %lld\n", id, eat_count, wait_time, total_time);
    }

private:
    void think() {
        if (debug_flag)
            std::printf("[%u] thinking\n", id);
        sleep(think_delay_dist(r_engine));
        if (debug_flag)
            std::printf("[%u] hungry\n", id);
        wait_start = Clock::now();
    }

    void eat() {
        wait_time += std::chrono::duration_cast<Microseconds>(Clock::now() - wait_start).count();
        if (debug_flag)
            std::printf("[%u] eating\n", id);
        sleep(eat_delay_dist(r_engine));
        ++eat_count;
    }

    unsigned id;
    Fork* fork_left;
    Fork* fork_right;
    std::default_random_engine r_engine;
    std::uniform_int_distribution<unsigned> think_delay_dist;
    std::uniform_int_distribution<unsigned> eat_delay_dist;
    unsigned eat_count;
    long long wait_time;
    long long total_time;
    Time wait_start;
    std::atomic<bool> stop_flag;
    std::deque<Philosopher>& all;
    unsigned N;
    std::atomic<bool> wait;
    std::atomic<bool> got;
};

int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::fprintf(stderr,
                     "Usage: %s phil_count duration think_delay eat_delay fork_delay debug_flag\n",
                     argv[0]);
        return 1;
    }

    unsigned N = std::atoi(argv[1]);
    unsigned duration = std::atoi(argv[2]);
    unsigned think_delay = std::atoi(argv[3]);
    unsigned eat_delay = std::atoi(argv[4]);
    unsigned fork_delay = std::atoi(argv[5]);
    debug_flag = std::atoi(argv[6]);

    std::random_device rd;
    bias = std::uniform_int_distribution<unsigned>(1, N)(rd);

    std::setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    // we use deques to avoid defining copy/move constructors
    std::deque<Fork> forks;
    for (unsigned i = 0; i < N; ++i) {
        forks.emplace_back(i + 1, i > 0 ? i : N, fork_delay);
    }
    std::deque<Philosopher> phils;
    for (unsigned i = 0; i < N; ++i) {
        phils.emplace_back(i + 1, &forks[(i + 1) % N], &forks[i], think_delay, eat_delay, phils);
    }

    std::vector<std::thread> threads;
    threads.reserve(N);
    for (auto& phil : phils) {
        threads.emplace_back([&phil] { phil.run(); });
    }

    sleep(duration * 1000);

    for (auto& phil : phils) {
        phil.stop();
    }
    for (auto& thread : threads) {
        thread.join();
    }

    for (const auto& phil : phils) {
        phil.printStats();
    }
}

