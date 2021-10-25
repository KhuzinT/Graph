#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>

//********************************************************************************************

typedef uint64_t Vertex;

typedef uint64_t weight_t;

typedef uint64_t dist_t;

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

    [[nodiscard]] virtual const std::vector<Vertex>& GetNeighborsVertex(const Vertex& v) const = 0;

    [[nodiscard]] virtual const std::vector<weight_t>& GetNeighborsWeight(const Vertex& v) const = 0;

    void AddEdge(const Vertex& a, const Vertex& b, const weight_t& w) {
        Add(a - 1, b - 1, w);
    }

};

//********************************************************************************************

class GraphMatrix final : public Graph {
private:

    std::vector<std::vector<Vertex>> matrix;

    void Add(const Vertex& a, const Vertex& b, const weight_t& w) override {
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

    [[nodiscard]] const std::vector<Vertex>& GetNeighborsVertex(const Vertex& v) const override {
        std::vector<Vertex> answer;
        for (Vertex i = 0; i < GetQVertex(); ++i) {
            if (matrix[v][i] != poison) {
                answer.push_back(i);
            }
        }
        return answer;
    }

    [[nodiscard]] const std::vector<weight_t>& GetNeighborsWeight(const Vertex& v) const override {
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
            list.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] const std::vector<Vertex>& GetNeighborsVertex(const Vertex& v) const override {
        return list[v].vertex;
    }

    [[nodiscard]] const std::vector<weight_t>& GetNeighborsWeight(const Vertex& v) const override {
        return list[v].weight;
    }

};

//********************************************************************************************

class GraphAlgorithm {
    std::vector<dist_t> dist;
    std::vector<bool> visit;

    std::vector<Vertex> component;

    GraphAlgorithm(const uint64_t& quantity) {
        dist.clear();
        visit.clear();

        component.clear();

        for (uint64_t i = 0; i < quantity; ++i) {
            dist.push_back(poison);
            visit.push_back(false);
        }
    }

    void BFS(Graph& G, const Vertex& start) {
        std::deque<Vertex> q;

        q.push_back(start);
        dist[start] = 0;

        while (!q.empty()) {
            Vertex v = q.front();
            q.pop_front();
            auto vertex = G.GetNeighborsVertex(v);
            auto weight = G.GetNeighborsWeight(v);
            for (uint64_t i = 0; i < vertex.size(); ++i) {
                if (dist[vertex[i]] > dist[v] + weight[i]) {
                    dist[vertex[i]] = dist[v] + weight[i];
                    if (weight[i] == 0) {
                        q.push_front(vertex[i]);
                    } else {
                        q.push_back(vertex[i]);
                    }
                }
            }
        }
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

    friend std::vector<std::vector<Vertex>> GetComponents(Graph& G);
    friend dist_t ShortPath(Graph& G, const Vertex& start, const Vertex& finish);
};

std::vector<std::vector<Vertex>> GetComponents(Graph& G) {
    std::vector<std::vector<Vertex>> answer;
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
            answer.push_back(comp);
            GInvAlg.component.clear();
        }
    }

    return answer;
}

std::vector<Vertex> GetVertexComponents(Graph& G, std::vector<std::vector<Vertex>>& components) {
    std::vector<Vertex> vertex_comp(G.GetQVertex(), 0);
    for (uint64_t i = 0; i < components.size(); ++i) {
        for (auto &v : components[i]) {
            vertex_comp[v] = i;
        }
    }

    return vertex_comp;
}

GraphList GetCondensationGraph(Graph& G, std::vector<Vertex>& vertex_comp) {
    GraphList GCond(vertex_comp.size(), true);
    for (Vertex v = 0; v < G.GetQVertex(); ++v) {
        auto neighbors = G.GetNeighborsVertex(v);
        for (auto &u : neighbors) {
            if (vertex_comp[v] != vertex_comp[u]) {
                GCond.AddEdge(vertex_comp[v] + 1, vertex_comp[u] + 1, 0);
            }
        }
    }

    return GCond;
}

dist_t ShortPath(Graph& G, const Vertex& start, const Vertex& finish) {
    GraphAlgorithm GAlg(G.GetQVertex());

    GAlg.BFS(G,  start - 1);

    return GAlg.dist[finish - 1];
}

//********************************************************************************************

enum GraphType {
    ORIENTED = true,
    NOT_ORIENTED = false
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    uint64_t q_vertex = 0, q_edge = 0;
    std::cin >> q_vertex >> q_edge;

    GraphList G(q_vertex, ORIENTED);

    for (uint64_t i = 0; i < q_edge; ++i) {
        Vertex a = 0, b = 0;
        std::cin >> a >> b;
        G.AddEdge(a, b, 0);
    }

    auto components = GetComponents(G);
    auto vertex_comp = GetVertexComponents(G, components);
    auto GCond = GetCondensationGraph(G, vertex_comp);

    for (Vertex v = 0; v < GCond.GetQVertex(); ++v) {
        auto neighbors = GCond.GetNeighborsVertex(v);
        for (auto &u : neighbors) {
            GCond.AddEdge(u + 1, v + 1, 1);
        }
    }

    uint64_t q_requests = 0;
    std::cin >> q_requests;
    for (uint64_t i = 0; i < q_requests; ++i) {
        Vertex start = 0, finish = 0;
        std::cin >> start >> finish;
        dist_t answer = ShortPath(GCond, vertex_comp[start - 1] + 1, vertex_comp[finish - 1] + 1);
        if (answer == poison) {
            std::cout << -1 << std::endl;
        } else {
            std::cout << answer << std::endl;
        }
    }
    return 0;
}

//********************************************************************************************