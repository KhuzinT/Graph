#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>

//********************************************************************************************

using Vertex = uint32_t;

using DistT = uint32_t;

const uint32_t kInfinity = 1000 * 1000 * 1000 * 1LL;

//********************************************************************************************

class IGraph {
private:

    bool ValidVertex(const Vertex& current_vertex) {
        if (current_vertex < 0) {
            return false;
        }
        if (current_vertex >= q_vertex) {
            return false;
        }
        return true;
    }

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
        if (ValidVertex(begin) && ValidVertex(end) && begin != end) {
            Add(begin, end);
        }
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
    if (dist[finish] == kInfinity) {
        return {};
    }

    std::vector<Vertex> answer;
    for (Vertex current_vertex = finish; current_vertex != kInfinity; current_vertex = parent[current_vertex]) {
        answer.push_back(current_vertex);
    }
    std::reverse(answer.begin(), answer.end());

    return answer;
}

std::vector<Vertex> ShortestDistanceFromStartToFinish(IGraph& graph, const Vertex& start, const Vertex& finish) {
    std::vector<DistT> dist(graph.GetQVertex(), kInfinity);
    std::vector<Vertex> parent(graph.GetQVertex(), kInfinity);

    BFS(graph, start, finish, dist, parent);

    return GetAnswer(finish, dist, parent);
}

//********************************************************************************************

bool IsZeroInNumber(const Vertex& current_vertex) {
    auto str = std::to_string(current_vertex);
    return str.find('0') != std::string::npos;
}

void IncreaseFirstDigit(IGraph& graph, const Vertex& number) {
    Vertex first_digit = number / 1000;
    if (first_digit != 9) {
        Vertex number_with_increase_first_digit = number + 1000;
        graph.AddEdge(number, number_with_increase_first_digit);
    }
}

void ReduceLastDigit(IGraph& graph, const Vertex& number) {
    Vertex last_digit = number % 10;
    if (last_digit != 1) {
        Vertex number_with_reduce_last_digit = number - 1;
        graph.AddEdge(number, number_with_reduce_last_digit);
    }
}

void CyclicShiftToLeft(IGraph& graph, const Vertex& number) {
    Vertex first_digit = number / 1000;
    Vertex last_three_digits = number % 1000;
    Vertex number_with_left_shift = last_three_digits * 10 + first_digit;
    graph.AddEdge(number, number_with_left_shift);
}

void CyclingShiftToRight(IGraph& graph, const Vertex& number) {
    Vertex last_digit = number % 10;
    Vertex first_three_digits = number / 10;
    Vertex number_with_right_shift = last_digit * 1000 + first_three_digits;
    graph.AddEdge(number, number_with_right_shift);
}

void DesignateNeighbors(IGraph& graph, const Vertex& number) {
    if (IsZeroInNumber(number)) {
        return;
    }

    IncreaseFirstDigit(graph, number);

    ReduceLastDigit(graph, number);

    CyclicShiftToLeft(graph, number);

    CyclingShiftToRight(graph, number);
}

//********************************************************************************************

enum GraphType {
    ORIENTED = true,
    NOT_ORIENTED [[maybe_unused]] = false
};

int main() {
    const Vertex max_vertex = 10000;

    Vertex start = 0;
    Vertex finish = 0;

    std::cin >> start;
    std::cin >> finish;

    GraphList graph(max_vertex, ORIENTED);

    for (Vertex current_vertex = 1000; current_vertex < max_vertex; ++current_vertex) {
        DesignateNeighbors(graph, current_vertex);
    }

    auto answer = ShortestDistanceFromStartToFinish(graph, start, finish);
    if (answer.empty()) {
        std::cout << -1;
        return 0;
    }

    std::cout << answer.size() << std::endl;
    for (auto &element: answer) {
        std::cout << element << std::endl;
    }

    return 0;
}

//********************************************************************************************