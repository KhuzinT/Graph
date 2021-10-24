#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>

//********************************************************************************************

typedef uint64_t Vertex;

typedef uint64_t dist_t;

const uint64_t poison = 1000 * 1000 * 1000 * 1LL;

//********************************************************************************************

class Graph {
protected:

    Vertex q_vertex = 0;

    bool is_oriented = false;

    virtual void Add(const Vertex& a, const Vertex& b) = 0;

public:

    [[nodiscard]] Vertex GetQVertex() const {
        return q_vertex;
    }

    [[nodiscard]] virtual std::vector<Vertex> GetNeighbors(const Vertex& v) const = 0;

    void AddEdge(const Vertex& a, const Vertex& b) {
        Add(a - 1, b - 1);
    }
};

//********************************************************************************************

class GraphMatrix final : public Graph {
private:

    std::vector<std::vector<Vertex>> matrix;

    void Add(const Vertex& a, const Vertex& b) override {
        if (a == b) {
            return;
        }
        if (a < 0 || b < 0) {
            return;
        }
        if (a >= q_vertex || b >= q_vertex) {
            return;
        }
        matrix[a][b] = 1;
        if (!is_oriented) {
            matrix[b][a] = 1;
        }
    }

public:

    explicit GraphMatrix(const Vertex& quantity, bool oriented = false) {
        for (Vertex i = 0; i < quantity; ++i) {
            std::vector<Vertex> tmp(quantity, 0);
            matrix.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighbors(const Vertex& v) const override {
        std::vector<Vertex> answer;
        for (Vertex i = 0; i < GetQVertex(); ++i) {
            if (matrix[v][i] != 0) {
                answer.push_back(i);
            }
        }
        return answer;
    }


};

//********************************************************************************************

class GraphList final : public Graph {
private:

    std::unordered_map<Vertex, std::vector<Vertex>> list;

    void Add(const Vertex& a, const Vertex& b) override {
        if (a == b) {
            return;
        }
        if (a < 0 || b < 0) {
            return;
        }
        if (a >= q_vertex || b >= q_vertex) {
            return;
        }
        list[a].push_back(b);
        if (!is_oriented) {
            list[b].push_back(a);
        }
    }

public:

    explicit GraphList(const Vertex& quantity, bool oriented = false) {
        for (Vertex i = 0; i < quantity; ++i) {
            std::vector<Vertex> tmp;
            list[i] = tmp;
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighbors(const Vertex& v) const override {
        return list.at(v);
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

    void BFS(Graph& G, std::vector<Vertex>& special) {
        std::queue<Vertex> q;

        for (auto &element : special) {
            q.push(element);
            dist[element] = 0;
        }

        while (!q.empty()) {
            Vertex v = q.front();
            q.pop();

            auto neighbors = G.GetNeighbors(v);
            for (auto &u: neighbors) {
                if (dist[u] == poison) {
                    dist[u] = dist[v] + 1;
                    parent[u] = v;
                    q.push(u);
                }
            }
        }
    }

    friend std::vector<Vertex> Path(Graph& G, std::vector<Vertex>& special);
};

std::vector<Vertex> Path(Graph& G, std::vector<Vertex>& special) {
    GraphAlgorithm GAlg(G.GetQVertex());

    GAlg.BFS(G, special);

    return GAlg.dist;
}

//********************************************************************************************

void DesignateNeighbors(Graph& G, const Vertex& v, const Vertex& M) {
    Vertex bolder = v / M;
    bolder *= M;
    if (bolder <= v - 1 && v - 1 < bolder + M) {
        G.AddEdge(v + 1, v);
    }
    if (bolder <= v + 1 && v + 1 < bolder + M) {
        G.AddEdge(v + 1, v + 2);
    }

    G.AddEdge(v + 1, v + 1 + M);
    G.AddEdge(v + 1, v + 1 - M);
}

//********************************************************************************************


enum GraphType {
    ORIENTED = true,
    NOT_ORIENTED = false
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    typedef std::vector<std::vector<uint64_t>> Matrix;

    uint64_t N = 0, M = 0;
    std::cin >> N >> M;

    GraphList G(N * M, NOT_ORIENTED);

    Matrix city;
    for (uint64_t i = 0; i < N; ++i) {
        std::vector<uint64_t> tmp(M, 0);
        city.push_back(tmp);
    }

    std::vector<Vertex> restaurants;
    for (uint64_t i = 0; i < N; ++i) {
        for (uint64_t j = 0; j < M; ++j) {
            std::cin >> city[i][j];
            if (city[i][j] == 1) {
                restaurants.push_back(i * M + j);
            }
        }
    }

    for (Vertex v = 0; v < N * M; ++v) {
        DesignateNeighbors(G, v, M);
    }

    auto answer = Path(G, restaurants);
    for (uint64_t i = 1; i <= answer.size(); ++i) {
        std::cout << answer[i - 1] << ' ';
        if (i % M == 0) {
            std::cout << std::endl;
        }
    }

    return 0;
}

//********************************************************************************************
