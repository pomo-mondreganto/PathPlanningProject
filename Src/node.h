#ifndef NODE_H
#define NODE_H

#include <tuple>
#include <memory>


struct Node {
    int i, j;
    double F, g, H;
    std::shared_ptr<Node> parent;
    int di, dj;

    Node(int i, int j, double d) :
            i(i), j(j), F(0), g(d), H(0), parent(nullptr), di(0), dj(0) {}

    bool operator<(const Node &other) const {
        return std::tie(i, j) < std::tie(other.i, other.j);
    }

    std::pair<int, int> v_key() const {
        return std::make_pair(i, j);
    }

    std::tuple<int, int, int, int> full_key() const {
        return std::tie(i, j, di, dj);
    }
};

#endif
