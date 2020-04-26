//
// Created by Roman Nikitin on 15.01.2020.
//

#ifndef PATHPLANNING_AUXILIARY_H
#define PATHPLANNING_AUXILIARY_H

#include "node.h"
#include "gl_const.h"
#include <set>
#include <limits>
#include <memory>
#include <functional>
#include <type_traits>
#include <unordered_set>
#include <chrono>

#define BIG_MAP_THRESHOLD 100000
#define INITIAL_FPSET_SIZE 10000019

template<typename T>
void
hash_combine(std::size_t &seed, T const &key) {
    std::hash<T> hasher;
    seed ^= hasher(key) + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
}

namespace std {
    template<typename T1, typename T2>
    struct hash<std::pair<T1, T2>> {
        std::size_t operator()(std::pair<T1, T2> const &p) const {
            std::size_t seed(0);
            ::hash_combine(seed, p.first);
            ::hash_combine(seed, p.second);
            return seed;
        }
    };

}

struct node_ptr_hasher {
    std::size_t operator()(std::shared_ptr<Node> const &p) const {
        std::size_t seed(0);
        ::hash_combine(seed, p->i);
        ::hash_combine(seed, p->j);
        ::hash_combine(seed, p->di);
        ::hash_combine(seed, p->dj);
        return seed;
    }
};

struct node_ptr_equal_to {
    bool operator()(const std::shared_ptr<Node> &n1, const std::shared_ptr<Node> &n2) const {
        return std::tie(n1->i, n1->j, n1->di, n1->dj) == std::tie(n2->i, n2->j, n2->di, n2->dj);
    }
};

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y, int ulp) {
    return std::abs(x - y) <= std::numeric_limits<T>::epsilon()
                              * std::max(std::abs(x), std::abs(y))
                              * ulp;
}

struct g_node_compare {
    bool operator()(const std::shared_ptr<Node> &n1, const std::shared_ptr<Node> &n2) const {
        if (!almost_equal(n1->g, n2->g, 8)) {
            return n1->g < n2->g;
        }
        return n1->full_key() < n2->full_key();
    }
};

struct f_node_compare {
    int breakingties;

    bool operator()(const std::shared_ptr<Node> &n1, const std::shared_ptr<Node> &n2) const {
        if (!almost_equal(n1->F, n2->F, 8)) {
            return n1->F < n2->F;
        } else if (!almost_equal(n1->g, n2->g, 8)) {
            if (breakingties == CN_SP_BT_GMIN) {
                return n1->g < n2->g;
            } else {
                return n1->g > n2->g;
            }
        }
        return n1->full_key() < n2->full_key();
    }
};

typedef std::set<
        std::shared_ptr<Node>,
        std::function<bool(const std::shared_ptr<Node> &,
                           const std::shared_ptr<Node> &)>> BTSet;

typedef std::unordered_set<std::shared_ptr<Node>, node_ptr_hasher, node_ptr_equal_to> FPSet;

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TP;

#endif //PATHPLANNING_AUXILIARY_H
