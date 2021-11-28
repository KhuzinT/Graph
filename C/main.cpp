#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>

//********************************************************************************************

using Vertex = int32_t;

using DistT = int32_t;

const int32_t kInfinity = 1000 * 1000 * 1000 * 1LL;

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

    [[nodiscard]] virtual std::vector<Vertex> GetNeighbors(const Vertex& current_vertex) const = 0;

    void AddEdge(const Vertex& begin, const Vertex& end) {
        if (ValidVertex(begin - 1) && ValidVertex(end - 1) && begin != end) {
            Add(begin - 1, end - 1);
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
        for (Vertex i = 0; i < quantity; ++i) {
            std::vector<Vertex> tmp(quantity, kInfinity);
            matrix_.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighbors(const Vertex& current_vertex) const override {
        std::vector<Vertex> answer;
        for (Vertex next_vertex = 0; next_vertex < GetQVertex(); ++next_vertex) {
            if (matrix_[current_vertex][next_vertex] != kInfinity) {
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

    [[maybe_unused]] explicit GraphList(const Vertex& quantity, bool oriented = false) {
        for (Vertex i = 0; i < quantity; ++i) {
            std::vector<Vertex> tmp;
            list_.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighbors(const Vertex& current_vertex) const override {
        return list_[current_vertex];
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

std::vector<Vertex> GetAnswer(const Vertex& finish, const int32_t& board_size, std::vector<DistT>& dist, std::vector<Vertex>& parent) {
    if (dist[finish - 1 - board_size] == kInfinity) {
        return {};
    }

    std::vector<Vertex> answer;
    for (Vertex current_vertex = finish - 1 - board_size; current_vertex != kInfinity; current_vertex = parent[current_vertex]) {
        answer.push_back(current_vertex);
    }
    std::reverse(answer.begin(), answer.end());

    return answer;
}

std::vector<Vertex> ShortestDistanceFromStartToFinish(IGraph& graph, const Vertex& start, const Vertex& finish) {
    const auto board_size = static_cast<int32_t>(std::sqrt(graph.GetQVertex()));
    std::vector<DistT> dist(graph.GetQVertex(), kInfinity);
    std::vector<Vertex> parent(graph.GetQVertex(), kInfinity);

    BFS(graph, start - 1 - board_size, finish - 1 - board_size, dist, parent);

    return GetAnswer(finish, board_size, dist, parent);
}

//********************************************************************************************

struct Point {
    int32_t x_coordinate = 0;
    int32_t y_coordinate = 0;

    explicit Point(const int32_t& x, const int32_t& y) {
        x_coordinate = x;
        y_coordinate = y;
    }
};

Point GetPointFromVertex(const Vertex& current_vertex, const int32_t& board_size) {
    int32_t x = current_vertex % board_size;
    int32_t y = current_vertex / board_size;
    return Point(x, y);
}

Vertex GetVertexFromPoint(const int32_t& x_coordinate, const int32_t& y_coordinate, const int32_t& board_size) {
    return x_coordinate + y_coordinate * board_size;
}

bool MoveNotOutOfBoard(const Vertex& current_vertex, const int32_t& left_board_border, const int32_t& right_board_border) {
    return left_board_border <= current_vertex && current_vertex < right_board_border;
}

void MoveThreeRightOneUpAndDown(IGraph& graph, const Vertex& current_vertex, const int32_t& board_size, const int32_t& left_board_border, const int32_t& right_board_border) {
    if (MoveNotOutOfBoard(current_vertex + 1, left_board_border, right_board_border)) {
        Vertex three_right_one_up = current_vertex + 2 + board_size;
        graph.AddEdge(current_vertex, three_right_one_up);

        Vertex three_right_one_down = current_vertex + 2 - board_size;
        graph.AddEdge(current_vertex, three_right_one_down);
    }
}

void MoveThreeLeftOneUpAndDown(IGraph& graph, const Vertex& current_vertex, const int32_t& board_size, const int32_t& left_board_border, const int32_t& right_board_border) {
    if (MoveNotOutOfBoard(current_vertex - 3, left_board_border, right_board_border)) {
        Vertex three_left_one_up = current_vertex - 2 + board_size;
        graph.AddEdge(current_vertex, three_left_one_up);

        Vertex three_left_one_down = current_vertex - 2 - board_size;
        graph.AddEdge(current_vertex, three_left_one_down);
    }
}

void MoveOneRightThreeUpAndDown(IGraph& graph, const Vertex& current_vertex, const int32_t& board_size, const int32_t& left_board_border, const int32_t& right_board_border) {
    if (MoveNotOutOfBoard(current_vertex, left_board_border, right_board_border)) {
        Vertex one_right_three_up = current_vertex + 1 + 2 * board_size;
        graph.AddEdge(current_vertex, one_right_three_up);

        Vertex one_right_three_down = current_vertex + 1 - 2 * board_size;
        graph.AddEdge(current_vertex, one_right_three_down);
    }
}

void MoveOneLeftThreeUpAndDown(IGraph& graph, const Vertex& current_vertex, const int32_t& board_size, const int32_t& left_board_border, const int32_t& right_board_border) {
    if (MoveNotOutOfBoard(current_vertex - 2, left_board_border, right_board_border)) {
        Vertex one_left_three_up = current_vertex - 1 + 2 * board_size;
        graph.AddEdge(current_vertex, one_left_three_up);

        Vertex one_left_three_down = current_vertex - 1 - 2 * board_size;
        graph.AddEdge(current_vertex, one_left_three_down);
    }
}

void DesignateNeighbors(IGraph& graph, const Vertex& current_vertex) {
    const auto board_size = static_cast<int32_t>(std::sqrt(graph.GetQVertex()));
    int32_t left_board_border = ((current_vertex - 1) / board_size);
    left_board_border *= board_size;
    int32_t right_board_border = left_board_border + board_size;

    MoveThreeRightOneUpAndDown(graph, current_vertex, board_size, left_board_border, right_board_border);

    MoveThreeLeftOneUpAndDown(graph, current_vertex, board_size, left_board_border, right_board_border);

    MoveOneRightThreeUpAndDown(graph, current_vertex, board_size, left_board_border, right_board_border);

    MoveOneLeftThreeUpAndDown(graph, current_vertex, board_size, left_board_border, right_board_border);
}

//********************************************************************************************

int main() {
    int32_t board_size = 0;
    std::cin >> board_size;

    GraphMatrix graph(board_size * board_size);

    for (Vertex current_vertex = 1; current_vertex <= board_size * board_size; ++current_vertex) {
        DesignateNeighbors(graph, current_vertex);
    }

    int32_t x_start = 0;
    int32_t y_start = 0;
    int32_t x_finish = 0;
    int32_t y_finish = 0;

    std::cin >> x_start >> y_start;
    std::cin >> x_finish >> y_finish;

    Vertex start = GetVertexFromPoint(x_start, y_start, board_size);
    Vertex finish = GetVertexFromPoint(x_finish, y_finish, board_size);

    auto answer = ShortestDistanceFromStartToFinish(graph, start, finish);
    if (answer.empty()) {
        std::cout << -1;
    } else {
        std::cout << answer.size() - 1 << std::endl;
        for (auto &element: answer) {
            auto elem_point = GetPointFromVertex(element, board_size);
            std::cout << elem_point.x_coordinate + 1 << ' ' << elem_point.y_coordinate + 1 << std::endl;
        }
    }

    return 0;
}

//********************************************************************************************
