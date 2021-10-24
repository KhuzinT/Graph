#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>

//********************************************************************************************

typedef int64_t Vertex;

typedef uint64_t weight_t;

typedef uint64_t dist_t;

typedef char color_t;

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

    std::vector<Neighbors> list;

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
        //list[a].weight.push_back(w);
        if (!is_oriented) {
            list[b].vertex.push_back(a);
            //list[b].weight.push_back(w);
        }
    }

public:

    explicit GraphList(const Vertex& quantity, bool oriented = false) {
        for (Vertex i = 0; i < quantity; ++i) {
            Neighbors tmp;
            list.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] Neighbors GetNeighbors(const Vertex& v) const override {
        Neighbors answer;
        answer.vertex = list[v].vertex;
        //answer.weight = list[v].weight;
        return answer;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighborsVertex(const Vertex& v) const override {
        std::vector<Vertex> answer;
        answer = list[v].vertex;
        return answer;
    }

    [[nodiscard]] std::vector<weight_t> GetNeighborsWeight(const Vertex& v) const override {
        std::vector<weight_t> answer;
        answer = list[v].weight;
        return answer;
    }

};

//********************************************************************************************

enum {
    WHITE = 0,
    GREY = 1,
    BLACK = 2
};

class GraphAlgorithm {
    std::vector<dist_t> dist;
    std::vector<Vertex> parent;
    std::vector<color_t> color;
    std::vector<bool> visit;

    std::vector<Vertex> component;

    Vertex start_cycle = poison;
    Vertex end_cycle = poison;

    GraphAlgorithm(const uint64_t& quantity) {
        dist.clear();
        parent.clear();
        color.clear();
        visit.clear();

        component.clear();

        for (uint64_t i = 0; i < quantity; ++i) {
            dist.push_back(poison);
            parent.push_back(poison);
            color.push_back(WHITE);
            visit.push_back(false);
        }

        start_cycle = poison;
        end_cycle = poison;
    }

    void DFS(Graph& G, const Vertex& v) {
        visit[v] = true;
        auto neighbors = G.GetNeighborsVertex(v);
        for (auto &u : neighbors) {
            if (!visit[u]) {
                DFS(G, u);
            }
        }
        component.push_back(v);
    }

    void DFSComp(Graph& G, const Vertex& v) {
        visit[v] = true;
        component.push_back(v);
        auto neighbors = G.GetNeighborsVertex(v);
        for (auto &u : neighbors) {
            if (!visit[u]) {
                DFSComp(G, u);
            }
        }
    }


    friend uint64_t GetEnds(Graph& G);
};

bool IsInsideVector(std::vector<Vertex>& comp, const Vertex& v) {
    for (auto &u : comp) {
        if (u == v) {
            return true;
        }
    }
    return false;
}

bool IsInsideComp(Graph& G, std::vector<Vertex>& comp) {
    for (auto &v : comp) {
        auto neighbors = G.GetNeighborsVertex(v);
        for (auto &u : neighbors) {
            if (!IsInsideVector(comp, u)) {
                return false;
            }
        }
    }

    return true;
}

uint64_t GetEnds(Graph& G) {
    uint64_t answer = 0;
    GraphList GInv(G.GetQVertex(), true);

    for (Vertex v = 0; v < G.GetQVertex(); ++v) {
        auto neighbors = G.GetNeighborsVertex(v);
        for (auto &u : neighbors) {
            GInv.AddEdge(u + 1, v + 1, 0);
        }
    }

    GraphAlgorithm GAlg(G.GetQVertex());
    for (Vertex v = 0; v < G.GetQVertex(); ++v) {
        if (!GAlg.visit[v]) {
            GAlg.DFS(G, v);
        }
    }

    GraphAlgorithm GInvAlg(GInv.GetQVertex());
    for (Vertex v = 0; v < GInv.GetQVertex(); ++v) {
        Vertex u = GAlg.component[G.GetQVertex() - 1 - v];
        if (!GInvAlg.visit[u]) {
            GInvAlg.DFSComp(GInv, u);
            auto comp = GInvAlg.component;
            if (comp.size() == 1 && G.GetNeighborsVertex(comp[0]).empty()) {
                ++answer;
            } else if (IsInsideComp(G, comp)) {
                ++answer;
            }
            GInvAlg.component.clear();
        }
    }

    return answer;
}


//********************************************************************************************

void DesignateNeighbors(Graph& G, const Vertex& v, const Vertex& M, std::vector<std::vector<uint64_t>>& landscape) {
    Vertex bolder = v / M;
    bolder *= M;

    if (v + M < G.GetQVertex()) {
        uint64_t i_v = v / M;
        uint64_t j_v = v % M;

        uint64_t i = (v + M) / M;
        uint64_t j = (v + M) % M;

        if (landscape[i_v][j_v] >= landscape[i][j]) {
            G.AddEdge(v + 1, v + M + 1, 0);
        }
    }
    if (v - M >= 0) {
        uint64_t i_v = v / M;
        uint64_t j_v = v % M;

        uint64_t i = (v - M) / M;
        uint64_t j = (v - M) % M;

        if (landscape[i_v][j_v] >= landscape[i][j]) {
            G.AddEdge(v + 1, v - M + 1, 0);
        }
    }
    if (v - 1 >= bolder) {
        uint64_t i_v = v / M;
        uint64_t j_v = v % M;

        uint64_t i = (v - 1) / M;
        uint64_t j = (v - 1) % M;

        if (landscape[i_v][j_v] >= landscape[i][j]) {
            G.AddEdge(v + 1, v, 0);
        }
    }
    if (v + 1 < bolder + M) {
        uint64_t i_v = v / M;
        uint64_t j_v = v % M;

        uint64_t i = (v + 1) / M;
        uint64_t j = (v + 1) % M;

        if (landscape[i_v][j_v] >= landscape[i][j]) {
            G.AddEdge(v + 1, v + 2, 0);
        }
    }
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

    GraphList G(N * M, ORIENTED);

    Matrix landscape;
    for (uint64_t i = 0; i < N; ++i) {
        std::vector<uint64_t> tmp(M, poison);
        landscape.push_back(tmp);
    }

    for (uint64_t i = 0; i < N; ++i) {
        for (uint64_t j = 0; j < M; ++j) {
            std::cin >> landscape[i][j];
        }
    }

    for (Vertex v = 0; v < N * M; ++v) {
        DesignateNeighbors(G, v, M, landscape);
    }

    uint64_t answer = GetEnds(G);
    std::cout << answer << std::endl;

    return 0;
}

//********************************************************************************************




