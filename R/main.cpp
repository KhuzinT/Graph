#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>

//********************************************************************************************

using Vertex = int32_t;

using TimeT = int32_t;

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
    std::vector<TimeT> time_in;
    std::vector<TimeT> time_out;
    std::set<Vertex> component;

    TimeT time = 0;

    explicit GraphInfo(const int32_t& quantity) {
        visit.resize(quantity, false);
        time_in.resize(quantity, kInfinity);
        time_out.resize(quantity, kInfinity);

        time = 0;
    }
};

void DfsForArticulationPoints(IGraph& graph, const Vertex& current_vertex, const Vertex& prev, GraphInfo& info) {
    uint32_t count = 0;

    info.visit[current_vertex] = true;
    info.time_in[current_vertex] = info.time;
    info.time_out[current_vertex] = info.time;
    ++info.time;

    auto neighbors = graph.GetNeighbors(current_vertex);
    for (auto &next_vertex : neighbors) {
        if (next_vertex != prev) {
            if (!info.visit[next_vertex]) {
                DfsForArticulationPoints(graph, next_vertex, current_vertex, info);
                if (info.time_out[current_vertex] > info.time_out[next_vertex]) {
                    info.time_out[current_vertex] = info.time_out[next_vertex];
                }
                if (info.time_out[next_vertex] >= info.time_in[current_vertex] && prev != kInfinity) {
                    info.component.insert(current_vertex + 1);
                }
                ++count;
                continue;
            }
            if (info.time_out[current_vertex] > info.time_in[next_vertex]) {
                info.time_out[current_vertex] = info.time_in[next_vertex];
            }
        }
    }
    if (count > 1 && prev == kInfinity) {
        info.component.insert(current_vertex + 1);
    }
}

std::set<Vertex> FindArticulationPoints(IGraph& graph) {
    GraphInfo info(graph.GetQVertex());
    for (Vertex current_vertex = 0; current_vertex < graph.GetQVertex(); ++current_vertex) {
        if (!info.visit[current_vertex]) {
            Vertex prev = kInfinity;
            DfsForArticulationPoints(graph, current_vertex, prev, info);
        }
    }

    return info.component;
}

//********************************************************************************************

enum GraphType {
    ORIENTED [[maybe_unused]] = true,
    NOT_ORIENTED = false
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int32_t q_vertex = 0;
    int32_t q_edge = 0;
    std::cin >> q_vertex >> q_edge;

    GraphList graph(q_vertex, NOT_ORIENTED);

    for (int32_t i = 0; i < q_edge; ++i) {
        Vertex begin = 0;
        Vertex end = 0;
        std::cin >> begin >> end;
        graph.AddEdge(begin, end);
    }

    auto answer = FindArticulationPoints(graph);
    std::cout << answer.size() << std::endl;
    for (auto &element : answer) {
        std::cout << element << std::endl;
    }

    return 0;
}

//********************************************************************************************