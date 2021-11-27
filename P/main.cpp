#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>

//********************************************************************************************

using Vertex = int32_t;

const int32_t kInfinity = 1000 * 1000 * 1000 * 1LL;

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
        for (int32_t i = 0; i < quantity; ++i) {
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

    [[maybe_unused]] explicit GraphList(const int32_t& quantity, bool oriented = false) {
        for (int32_t i = 0; i < quantity; ++i) {
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

struct GraphInfo {
    std::vector<bool> visit;
    std::vector<Vertex> component;

    explicit GraphInfo(const int32_t& quantity) {
        visit.resize(quantity, false);
    }
};

void DfsForTopSort(IGraph& graph, const Vertex& current_vertex, GraphInfo& info) {
    info.visit[current_vertex] = true;
    auto neighbors = graph.GetNeighbors(current_vertex);
    for (auto &next_vertex : neighbors) {
        if (!info.visit[next_vertex]) {
            DfsForTopSort(graph, next_vertex, info);
        }
    }
    info.component.push_back(current_vertex);
}

void DfsForTranspositionGraph(IGraph& graph, const Vertex& current_vertex, GraphInfo& info) {
    info.visit[current_vertex] = true;
    info.component.push_back(current_vertex);
    auto neighbors = graph.GetNeighbors(current_vertex);
    for (auto &next_vertex : neighbors) {
        if (!info.visit[next_vertex]) {
            DfsForTranspositionGraph(graph, next_vertex, info);
        }
    }
}

GraphList GetInvertedOrientedGraphList(IGraph& graph) {
    GraphList inv_graph(graph.GetQVertex(), true);
    for (Vertex current_vertex = 0; current_vertex < graph.GetQVertex(); ++current_vertex) {
        for (auto &next_vertex : graph.GetNeighbors(current_vertex)) {
            inv_graph.AddEdge(next_vertex + 1, current_vertex + 1);
        }
    }

    return inv_graph;
}

std::vector<std::vector<Vertex>> GetComponents(IGraph& graph) {
    auto inv_graph = GetInvertedOrientedGraphList(graph);

    GraphInfo info(graph.GetQVertex());
    GraphInfo inv_info(inv_graph.GetQVertex());

    for (Vertex current_vertex = 0; current_vertex < graph.GetQVertex(); ++current_vertex) {
        if (!info.visit[current_vertex]) {
            DfsForTopSort(graph, current_vertex, info);
        }
    }

    std::vector<std::vector<Vertex>> answer;

    for (Vertex current_vertex = inv_graph.GetQVertex() - 1; current_vertex >= 0; --current_vertex) {
        Vertex next_vertex = info.component[current_vertex];
        if (!inv_info.visit[next_vertex]) {
            DfsForTranspositionGraph(inv_graph, next_vertex, inv_info);
            answer.push_back(inv_info.component);
            inv_info.component.clear();
        }
    }

    return answer;
}

std::vector<uint32_t> FindInWhichComponentVertex(IGraph& graph) {
    auto components = GetComponents(graph);

    std::vector<uint32_t> answer(graph.GetQVertex() + 1, 0);
    answer[0] = components.size();

    for (uint32_t index = 0; index < components.size(); ++index) {
        for (auto &vertex : components[index]) {
            answer[vertex + 1] = index + 1;
        }
    }

    return answer;
}

//********************************************************************************************

enum GraphType {
    ORIENTED = true,
    NOT_ORIENTED [[maybe_unused]] = false
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int32_t q_vertex = 0;
    int32_t q_edge = 0;
    std::cin >> q_vertex >> q_edge;

    GraphList graph(q_vertex, ORIENTED);

    for (int32_t i = 0; i < q_edge; ++i) {
        Vertex begin = 0;
        Vertex end = 0;
        std::cin >> begin >> end;
        graph.AddEdge(begin, end);
    }

    auto answer = FindInWhichComponentVertex(graph);
    uint32_t size = answer[0];
    std::cout << size << std::endl;
    if (size) {
        for (uint32_t i = 1; i < answer.size(); ++i) {
            std::cout << answer[i] << ' ';
        }
        std::cout << std::endl;
    }

    return 0;
}

//********************************************************************************************