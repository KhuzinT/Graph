#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>

//********************************************************************************************

typedef int64_t Vertex;

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

    friend std::vector<Vertex> ShortPath(Graph& G, const Vertex& start, const Vertex& finish);
};

std::vector<Vertex> ShortPath(Graph& G, const Vertex& start, const Vertex& finish) {
    GraphAlgorithm GAlg(G.GetQVertex());

    GAlg.BFS(G, start, finish);

    std::vector<Vertex> answer;
    if (GAlg.dist[finish] == poison) {
        return answer;
    }

    for (Vertex v = finish; v != poison; v = GAlg.parent[v]) {
        answer.push_back(v);
    }
    std::reverse(answer.begin(), answer.end());

    return answer;
}
//********************************************************************************************

bool InWalls(const Vertex& v, std::vector<Vertex>& walls) {
    for (auto &u : walls) {
        if (u == v) {
            return true;
        }
    }
    return false;
}

void DesignateNeighbors(Graph& G, const Vertex& v, const Vertex& M, std::vector<Vertex>& walls) {
    if (InWalls(v, walls)) {
        return;
    }
    Vertex up = v;
    while (true) {
        if (up + M >= G.GetQVertex() || InWalls(up + M, walls)) {
            break;
        }
        up += M;
    }
    Vertex tmp = (up - v) / M;
    tmp /= 2;
    G.AddEdge(v + 1, up - tmp * M + 1, 0);

    Vertex down = v;
    while (true) {
        if (down - M < 0 || InWalls(down - M, walls)) {
            break;
        }
        down -= M;
    }
    tmp = (v - down) / M;
    tmp /= 2;
    G.AddEdge(v + 1, down + tmp * M + 1, 0);

    Vertex bolder = v / M;
    bolder *= M;

    Vertex left = v;
    while (true) {
        if (left - 1 < bolder || InWalls(left - 1, walls)) {
            break;
        }
        --left;
    }
    G.AddEdge(v + 1, left + (v - left) / 2 + 1, 0);

    Vertex right = v;
    while (true) {
        if (right + 1 >= bolder + M || InWalls(right + 1, walls)) {
            break;
        }
        ++right;
    }
    G.AddEdge(v + 1, right - (right - v) / 2 + 1, 0);
}

//********************************************************************************************

enum GraphType {
    ORIENTED = true,
    NOT_ORIENTED = false
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    typedef std::vector<std::vector<char>> Matrix;

    uint64_t N = 0, M = 0;
    std::cin >> N >> M;

    GraphMatrix G(N * M, ORIENTED);

    Matrix track;
    for (uint64_t i = 0; i < N; ++i) {
        std::vector<char> tmp(M, '.');
        track.push_back(tmp);
    }

    std::vector<Vertex> walls;
    Vertex start = 0, finish = 0;
    for (uint64_t i = 0; i < N; ++i) {
        for (uint64_t j = 0; j < M; ++j) {
            std::cin >> track[i][j];
            if (track[i][j] == 'S') {
                start = i * M + j;
            }
            if (track[i][j] == 'T') {
                finish = i * M + j;
            }
            if (track[i][j] == '#') {
                walls.push_back(i * M + j);
            }
        }
    }

    for (Vertex v = 0; v < N * M; ++v) {
        DesignateNeighbors(G, v, M, walls);
    }

    auto answer = ShortPath(G, start, finish);
    if (answer.empty()) {
        std::cout << -1 << std::endl;
    } else {
        std::cout << answer.size() - 1 << std::endl;
    }

    return 0;
}

//********************************************************************************************