#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <queue>

//********************************************************************************************

typedef uint64_t Vertex;

typedef uint64_t dist_t;

typedef uint64_t weight_t;

const uint64_t poison = 1000 * 1000 * 1000 * 1LL;

//********************************************************************************************

class Graph {
protected:

    Vertex q_vertex = 0;

    bool is_oriented = false;

    virtual void Add(const Vertex& a, const Vertex& b, const weight_t& w) = 0;

    struct Neighbors {
        std::vector<Vertex> vertex;
        std::vector<weight_t> weight;
    };

public:

    [[nodiscard]] Vertex GetQVertex() const {
        return q_vertex;
    }

    [[nodiscard]] virtual Neighbors GetNeighbors(const Vertex& v) const = 0;

    [[nodiscard]] virtual std::vector<Vertex> GetNeighborsVertex(const Vertex& v) const = 0;

    [[nodiscard]] virtual std::vector<weight_t> GetNeighborsWeight(const Vertex& v) const = 0;

    void AddEdge(const Vertex& a, const Vertex& b, const weight_t& w) {
        Add(a - 1, b - 1, w);
    }

};

//********************************************************************************************

class GraphMatrix final : public Graph {
private:

    std::vector<std::vector<Vertex>> matrix;

    void Add(const Vertex& a, const Vertex& b, const weight_t& w) override {
        if (a == b) {
            return;
        }
        if (a < 0 || b < 0) {
            return;
        }
        if (a >= q_vertex || b >= q_vertex) {
            return;
        }
        matrix[a][b] = w;
        if (!is_oriented) {
            matrix[b][a] = w;
        }
    }

public:

    explicit GraphMatrix(const Vertex& quantity, bool oriented = false) {
        for (Vertex i = 0; i < quantity; ++i) {
            std::vector<Vertex> tmp(quantity, poison);
            matrix.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] Neighbors GetNeighbors(const Vertex& v) const override {
        Neighbors answer;
        for (Vertex i = 0; i < GetQVertex(); ++i) {
            if (matrix[v][i] != poison) {
                answer.vertex.push_back(i);
                answer.weight.push_back(matrix[v][i]);
            }
        }
        return answer;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighborsVertex(const Vertex& v) const override {
        std::vector<Vertex> answer;
        for (Vertex i = 0; i < GetQVertex(); ++i) {
            if (matrix[v][i] != poison) {
                answer.push_back(i);
            }
        }
        return answer;
    }

    [[nodiscard]] std::vector<weight_t> GetNeighborsWeight(const Vertex& v) const override {
        std::vector<weight_t> answer;
        for (Vertex i = 0; i < GetQVertex(); ++i) {
            if (matrix[v][i] != poison) {
                answer.push_back(matrix[v][i]);
            }
        }
        return answer;
    }


};

//********************************************************************************************

class GraphList final : public Graph {
private:

    std::unordered_map<Vertex, Neighbors> list;

    void Add(const Vertex& a, const Vertex& b, const weight_t& w) override {
        if (a == b) {
            return;
        }
        if (a < 0 || b < 0) {
            return;
        }
        if (a >= q_vertex || b >= q_vertex) {
            return;
        }
        list[a].vertex.push_back(b);
        list[a].weight.push_back(w);
        if (!is_oriented) {
            list[b].vertex.push_back(a);
            list[b].weight.push_back(w);
        }
    }

public:

    explicit GraphList(const Vertex& quantity, bool oriented = false) {
        for (Vertex i = 0; i < quantity; ++i) {
            Neighbors tmp;
            list[i] = tmp;
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] Neighbors GetNeighbors(const Vertex& v) const override {
        Neighbors answer;
        answer.vertex = list.at(v).vertex;
        answer.weight = list.at(v).weight;
        return answer;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighborsVertex(const Vertex& v) const override {
        std::vector<Vertex> answer;
        answer = list.at(v).vertex;
        return answer;
    }

    [[nodiscard]] std::vector<weight_t> GetNeighborsWeight(const Vertex& v) const override {
        std::vector<weight_t> answer;
        answer = list.at(v).weight;
        return answer;
    }

};

//********************************************************************************************

class GraphAlgorithm {
    std::vector<dist_t> dist;
    std::vector<Vertex> parent;

    explicit GraphAlgorithm(const uint64_t& quantity) {
        dist.clear();
        parent.clear();

        for (uint64_t i = 0; i < quantity; ++i) {
            dist.push_back(poison);
            parent.push_back(poison);
        }
    }

    void BFS(Graph& G, const Vertex& start, const Vertex& finish) {
        std::queue<Vertex> q;

        dist[start] = 0;
        q.push(start);

        while (!q.empty()) {
            Vertex v = q.front();
            q.pop();

            auto neighbors = G.GetNeighborsVertex(v);
            for (auto &u: neighbors) {
                if (dist[u] == poison) {
                    dist[u] = dist[v] + 1;
                    parent[u] = v;
                    q.push(u);
                }
            }
        }
    }

    friend std::vector<Vertex> HorsePath(Graph& G, const Vertex& N, const Vertex& start, const Vertex& finish);
};

std::vector<Vertex> HorsePath(Graph& G, const Vertex& N, const Vertex& start, const Vertex& finish) {
    GraphAlgorithm GAlg(G.GetQVertex());

    GAlg.BFS(G, start - 1 - N, finish - 1 - N);

    std::vector<Vertex> answer;
    if (GAlg.dist[finish - 1 - N] == poison) {
        return answer;
    }

    for (Vertex v = finish - 1 - N; v != poison; v = GAlg.parent[v]) {
        answer.push_back(v);
    }
    std::reverse(answer.begin(), answer.end());

    return answer;
}

//********************************************************************************************

void DesignateNeighbors(Graph& G, const Vertex& v, const Vertex& N) {
    Vertex border = (v - 1) / N;
    border *= N;
    if (border <= v + 1 && v + 1 < border + N) {
        G.AddEdge(v + 2 - N, v, 0);
        G.AddEdge(v + 2 + N, v, 0);
    }
    if (border <= v - 3 && v - 3 < border + N) {
        G.AddEdge(v - 2 - N, v, 0);
        G.AddEdge(v - 2 + N, v, 0);
    }
    if (border <= v && v < border + N) {
        G.AddEdge(v + 1 - 2 * N, v, 0);
        G.AddEdge(v + 1 + 2 * N, v, 0);
    }
    if (border <= v - 2 && v - 2 < border + N) {
        G.AddEdge(v - 1 - 2 * N, v, 0);
        G.AddEdge(v - 1 + 2 * N, v, 0);
    }
}

//********************************************************************************************

int main() {
    Vertex N = 0;
    std::cin >> N;

    GraphMatrix G(N * N);

    for (Vertex v = 1; v <= N * N; ++v) {
        DesignateNeighbors(G, v, N);
    }

    Vertex x_start = 0, y_start = 0;
    Vertex x_finish = 0, y_finish = 0;

    std::cin >> x_start >> y_start;
    std::cin >> x_finish >> y_finish;

    Vertex start = x_start + y_start * N;
    Vertex finish = x_finish + y_finish * N;

    auto answer = HorsePath(G, N, start, finish);
    if (answer.empty()) {
        std::cout << -1;
        return 0;
    }

    std::cout << answer.size() - 1 << std::endl;
    for (auto &element: answer) {
        Vertex x = element % N;
        Vertex y = element / N;
        std::cout << x + 1 << ' ' << y + 1 << std::endl;
    }
    return 0;
}

//********************************************************************************************