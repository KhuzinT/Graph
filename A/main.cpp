#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>

//********************************************************************************************

using Vertex = uint32_t;

using DistT = uint32_t;

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

class GraphList final : public IGraph {
private:

    std::vector<std::vector<Vertex>> list_;

    void Add(const Vertex& begin, const Vertex& end) override {
        list_[begin].push_back(end);
        if (!is_oriented) {
            list_[end].push_back(begin);
        }
    }

public:

    explicit GraphList(const uint32_t& quantity, bool oriented = false) {
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

void BFS(IGraph& graph, const Vertex& start, const Vertex& finish, std::vector<DistT>& dist, std::vector<Vertex>& parent) {
    std::queue<Vertex> queue_of_vertex;

    dist[start] = 0;
    queue_of_vertex.push(start);

    while (!queue_of_vertex.empty()) {
        Vertex current_vertex = queue_of_vertex.front();
        queue_of_vertex.pop();

        auto neighbors = graph.GetNeighbors(current_vertex);
        for (auto &next_vertex: neighbors) {
            if (dist[next_vertex] == kInfinity) {
                dist[next_vertex] = dist[current_vertex] + 1;
                parent[next_vertex] = current_vertex;
                queue_of_vertex.push(next_vertex);
            }
        }
    }
}

std::vector<Vertex> GetAnswer(const Vertex& finish, std::vector<DistT>& dist, std::vector<Vertex>& parent) {
    if (dist[finish - 1] == kInfinity) {
        return {};
    }

    std::vector<Vertex> answer;
    for (Vertex current_vertex = finish - 1; current_vertex != kInfinity; current_vertex = parent[current_vertex]) {
        answer.push_back(current_vertex + 1);
    }
    std::reverse(answer.begin(), answer.end());

    return answer;
}

std::vector<Vertex> ShortestDistanceFromStartToFinish(IGraph& graph, const Vertex& start, const Vertex& finish) {
    std::vector<DistT> dist(graph.GetQVertex(), kInfinity);
    std::vector<Vertex> parent(graph.GetQVertex(), kInfinity);

    BFS(graph, start - 1, finish - 1, dist, parent);

    return GetAnswer(finish, dist, parent);
}


//********************************************************************************************

int main() {
    uint32_t q_vertex = 0;
    uint32_t q_edge = 0;
    std::cin >> q_vertex >> q_edge;

    GraphList graph(q_vertex);

    Vertex start = 0;
    Vertex finish = 0;
    std::cin >> start >> finish;

    for (uint32_t i = 0; i < q_edge; ++i) {
        Vertex begin = 0;
        Vertex end = 0;
        std::cin >> begin >> end;
        graph.AddEdge(begin, end);
    }

    auto answer = ShortestDistanceFromStartToFinish(graph, start, finish);
    if (answer.empty()) {
        std::cout << -1;
        return 0;
    }

    std::cout << answer.size() - 1 << std::endl;
    for (auto &element: answer) {
        std::cout << element << ' ';
    }

    return 0;
}

//********************************************************************************************
