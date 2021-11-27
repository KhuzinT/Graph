#include <iostream>
#include <vector>

//********************************************************************************************

using Vertex = uint32_t;

using ColorT = char;

const uint32_t kInfinity = 1000 * 1000 * 1000 * 1LL;

//********************************************************************************************

class IGraph {
protected:

    Vertex q_vertex = 0;

    bool is_oriented = false;

    virtual void Add(const Vertex& begin, const Vertex& end) = 0;

public:

    [[nodiscard]] Vertex GetQVertex() const {
        return q_vertex;
    }

    [[nodiscard]] virtual std::vector<Vertex> GetNeighbors(const Vertex& vertex) const = 0;

    void AddEdge(const Vertex& begin, const Vertex& end) {
        Add(begin - 1, end - 1);
    }

};

//********************************************************************************************

class [[maybe_unused]] GraphMatrix final : public IGraph {
private:

    std::vector<std::vector<Vertex>> matrix_;

    void Add(const Vertex& begin, const Vertex& end) override {
        matrix_[begin][end] = 1;
        if (!is_oriented) {
            matrix_[end][begin] = 1;
        }
    }

public:

    [[maybe_unused]] explicit GraphMatrix(const Vertex& quantity, bool oriented = false) {
        for (uint32_t i = 0; i < quantity; ++i) {
            std::vector<Vertex> tmp(quantity, kInfinity);
            matrix_.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighbors(const Vertex& vertex) const override {
        std::vector<Vertex> answer;
        for (Vertex next_vertex = 0; next_vertex < GetQVertex(); ++next_vertex) {
            if (matrix_[vertex][next_vertex] != kInfinity) {
                answer.push_back(next_vertex);
            }
        }
        return answer;
    }
};

//********************************************************************************************

class [[maybe_unused]] GraphList final : public IGraph {
private:

    std::vector<std::vector<Vertex>> list_;

    void Add(const Vertex& begin, const Vertex& end) override {
        list_[begin].push_back(end);
        if (!is_oriented) {
            list_[end].push_back(begin);
        }
    }

public:

    [[maybe_unused]] explicit GraphList(const uint32_t& quantity, bool oriented = false) {
        for (uint32_t i = 0; i < quantity; ++i) {
            std::vector<Vertex> tmp;
            list_.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighbors(const Vertex& vertex) const override {
        return list_[vertex];
    }

};

//********************************************************************************************

enum {
    WHITE = 0,
    GREY = 1,
    BLACK = 2
};

ColorT GetNewColor(const ColorT& current_color) {
    ColorT next_color = GREY;
    if (current_color == GREY) {
        next_color = BLACK;
    }
    return next_color;
}

void DFS(IGraph& graph, const Vertex& current_vertex, std::vector<ColorT>& color, bool& is_bipartite) {
    ColorT current_color = color[current_vertex];
    auto neighbors = graph.GetNeighbors(current_vertex);
    for (auto &next_vertex : neighbors) {
        if (color[next_vertex] == WHITE) {
            color[next_vertex] = GetNewColor(current_color);
            DFS(graph, next_vertex, color, is_bipartite);
        } else if (color[next_vertex] == current_color) {
            is_bipartite = false;
        }
    }
}


bool IsBipartiteGraph(IGraph& graph) {
    std::vector<ColorT> color(graph.GetQVertex(), WHITE);
    bool is_bipartite = true;

    for (Vertex current_vertex = 0; current_vertex < graph.GetQVertex(); ++current_vertex) {
        if (color[current_vertex] == WHITE) {
            color[current_vertex] = GREY;
            DFS(graph, current_vertex, color, is_bipartite);
            if (!is_bipartite) {
                return false;
            }
        }
    }
    return true;
}


//********************************************************************************************

int main() {
    uint32_t q_vertex = 0;
    uint32_t q_edge = 0;
    std::cin >> q_vertex >> q_edge;

    GraphMatrix graph(q_vertex);

    for (uint32_t i = 0; i < q_edge; ++i) {
        Vertex begin = 0;
        Vertex end = 0;
        std::cin >> begin >> end;
        graph.AddEdge(begin, end);
    }

    if (IsBipartiteGraph(graph)) {
        std::cout << "YES" << std::endl;
    } else {
        std::cout << "NO" << std::endl;
    }
    return 0;
}

//********************************************************************************************