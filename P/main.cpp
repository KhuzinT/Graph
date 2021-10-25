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

    [[nodiscard]] Neighbors GetNeighbors(const Vertex& v) const override {
        Neighbors answer;
        answer.vertex = list[v].vertex;
        answer.weight = list[v].weight;
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


    friend std::vector<std::vector<Vertex>> GetComponents(Graph& G);
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
    std::vector<Vertex> answer(q_vertex, 0);
    for (uint64_t i = 0; i < components.size(); ++i) {
        for (auto &v : components[i]) {
            answer[v] = i + 1;
        }
    }

    std::cout << components.size() << std::endl;
    for (auto &element : answer) {
        std::cout << element << ' ';
    }
    std::cout << std::endl;

    return 0;
}

//********************************************************************************************




