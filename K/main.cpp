#include <iostream>
#include <algorithm>
#include <string>
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

struct GraphInfo {
    std::vector<ColorT> color;
    std::vector<Vertex> parent;

    Vertex start_cycle = kInfinity;
    Vertex end_cycle = kInfinity;

    bool cycle_found = false;

    explicit GraphInfo(const uint32_t& quantity) {
        color.resize(quantity, WHITE);
        parent.resize(quantity, kInfinity);

        start_cycle = kInfinity;
        end_cycle = kInfinity;

        cycle_found = false;
    }
};

void DFS(IGraph& graph, const Vertex& current_vertex, GraphInfo& info) {
    info.color[current_vertex] = GREY;
    auto neighbors = graph.GetNeighbors(current_vertex);
    for (auto &next_vertex : neighbors) {
        if (info.color[next_vertex] == WHITE) {
            info.parent[next_vertex] = current_vertex;
            DFS(graph, next_vertex, info);
            if (info.cycle_found) {
                return;
            }
        } else if (info.color[next_vertex] == GREY) {
            info.start_cycle = next_vertex;
            info.end_cycle = current_vertex;
            info.cycle_found = true;
            return;
        }
    }
    info.color[current_vertex] = BLACK;
}

std::vector<Vertex> GetAnswer(const Vertex& start_cycle, const Vertex& end_cycle, std::vector<Vertex>& parent) {
    if (start_cycle == kInfinity) {
        return {};
    }

    std::vector<Vertex> answer;
    for (Vertex current_vertex = end_cycle; current_vertex != start_cycle; current_vertex = parent[current_vertex]) {
        answer.push_back(current_vertex + 1);
    }
    answer.push_back(start_cycle + 1);
    std::reverse(answer.begin(), answer.end());

    return answer;
}

std::vector<Vertex> FindCycle(IGraph& graph) {
    GraphInfo info(graph.GetQVertex());

    for (Vertex current_vertex = 0; current_vertex < graph.GetQVertex(); ++current_vertex) {
        DFS(graph, current_vertex, info);
        if (info.cycle_found) {
            break;
        }
    }
    return GetAnswer(info.start_cycle, info.end_cycle, info.parent);
}

//********************************************************************************************

enum GraphType {
    ORIENTED = true,
    NOT_ORIENTED [[maybe_unused]] = false
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    uint32_t q_vertex = 0;
    uint32_t q_edge = 0;
    std::cin >> q_vertex >> q_edge;

    GraphList graph(q_vertex, ORIENTED);

    for (uint32_t i = 0; i < q_edge; ++i) {
        Vertex begin = 0;
        Vertex end = 0;
        std::cin >> begin >> end;
        graph.AddEdge(begin, end);
    }

    auto answer = FindCycle(graph);
    if (answer.empty()) {
        std::cout << "NO" << std::endl;
    } else {
        std::cout << "YES" << std::endl;
        for (auto &element : answer) {
            std::cout << element << ' ';
        }
        std::cout << std::endl;
    }
    
    return 0;
}

//********************************************************************************************

