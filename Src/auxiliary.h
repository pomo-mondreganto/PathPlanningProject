//
// Created by Roman Nikitin on 15.01.2020.
//

#ifndef PATHPLANNING_AUXILIARY_H
#define PATHPLANNING_AUXILIARY_H

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

typedef std::unordered_map<std::pair<int, int>, std::shared_ptr<Node>> NMap;

typedef std::set<
        std::shared_ptr<Node>,
        std::function<bool(const std::shared_ptr<Node> &,
                           const std::shared_ptr<Node> &)>> BTSet;

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
        return n1->v_key() < n2->v_key();
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
        return n1->v_key() < n2->v_key();
    }
};


#endif //PATHPLANNING_AUXILIARY_H
