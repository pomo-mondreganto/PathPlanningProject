#ifndef NODE_H
#define NODE_H

#include <tuple>
#include <memory>


//That's the data structure for storing a single search node.
//You MUST store all the intermediate computations occuring during the search
//incapsulated to Nodes (so NO separate arrays of g-values etc.)

struct Node {
    int i, j; //grid cell coordinates
    double F, g, H; //f-, g- and h-values of the search node
    std::shared_ptr<Node> parent; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)

    Node(int i, int j, double d) :
            i(i), j(j), F(0), g(d), H(0), parent(nullptr) {}

    bool operator<(const Node &other) const {
        return std::tie(i, j) < std::tie(other.i, other.j);
    }

    std::pair<int, int> v_key() const {
        return std::make_pair(i, j);
    }
};

#endif
