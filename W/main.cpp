#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>

//********************************************************************************************

typedef uint64_t Vertex;

const uint64_t poison = 1000 * 1000 * 1000 * 1LL;

//********************************************************************************************

class Graph {
protected:

    Vertex q_vertex = 0;

    bool is_oriented = false;

    virtual void Add(const Vertex& a, const Vertex& b) = 0;

    std::vector<bool> used_vertex;

public:

    [[nodiscard]] Vertex GetQVertex() const {
        return q_vertex;
    }

    [[nodiscard]] virtual std::vector<Vertex>& GetNeighborsVertex(const Vertex& v) = 0;

    void AddEdge(const Vertex& a, const Vertex& b) {
        Add(a - 1, b - 1);
    }

    [[nodiscard]] std::vector<bool> GetUsedVertex() const {
        return used_vertex;
    }

};

//********************************************************************************************

class GraphList final : public Graph {
private:

    std::vector<std::vector<Vertex>> list;

    void Add(const Vertex& a, const Vertex& b) override {
        list[a].push_back(b);
        used_vertex[a] = true;
        if (!is_oriented) {
            list[b].push_back(a);
            used_vertex[b] = true;
        }
    }

public:

    explicit GraphList(const Vertex& quantity, bool oriented = false) {
        for (Vertex i = 0; i < quantity; ++i) {
            std::vector<Vertex> tmp;
            list.push_back(tmp);
            used_vertex.push_back(false);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] std::vector<Vertex>& GetNeighborsVertex(const Vertex& v) override {
        return list[v];
    }

};


//********************************************************************************************

class GraphAlgorithm {
    std::vector<bool> visit;
    std::vector<Vertex> component;


    GraphAlgorithm(const uint64_t& quantity) {
        visit.clear();
        component.clear();

        for (uint64_t i = 0; i < quantity; ++i) {
            visit.push_back(false);
        }
    }

    void EulerPath(Graph& G, const Vertex& v) {
        auto it = G.GetNeighborsVertex(v).begin();

        Vertex next = 0;
        while (!G.GetNeighborsVertex(v).empty()) {
            next = *it;
            it = G.GetNeighborsVertex(v).erase(it);
            EulerPath(G, next);
        }

        component.push_back(v + 1);
    }

    void DFS(Graph& G, const Vertex& v) {
        visit[v] = true;

        auto neighbors = G.GetNeighborsVertex(v);
        for (auto &u : neighbors) {
            if (!visit[u]) {
                DFS(G, u);
            }
        }
    }

    friend std::vector<Vertex> FindEuler(Graph& G);
};

std::vector<Vertex> FindEuler(Graph& G) {
    auto used_vertex = G.GetUsedVertex();
    Vertex first_used_vertex = 0;
    for (Vertex v = 0; v < used_vertex.size(); ++v) {
        if (used_vertex[v]) {
            first_used_vertex = v;
            break;
        }
    }

    GraphAlgorithm GAlg(G.GetQVertex());
    GAlg.DFS(G, first_used_vertex);

    for (Vertex v = 0; v < G.GetQVertex(); ++v) {
        if (!GAlg.visit[v] && used_vertex[v]) {
            return {0};
        }
    }


    GAlg.EulerPath(G, first_used_vertex);

    std::vector<Vertex> answer;

    answer = GAlg.component;
    answer.push_back(answer.size());
    if (answer.size() == 2) {
        return {0};
    }

    std::reverse(answer.begin(), answer.end());

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

    uint64_t q_bus = 0, q_square = 0;
    std::cin >> q_bus >> q_square;

    GraphList G(q_square, ORIENTED);

    for (uint64_t i = 0; i < q_bus; ++i) {
        uint64_t q_stop = 0;
        std::cin >> q_stop;
        Vertex start = 0;
        std::cin >> start;
        for (uint64_t j = 0; j < q_stop; ++j) {
            Vertex stop = 0;
            std::cin >> stop;
            G.AddEdge(start, stop);
            start = stop;
        }
    }

    auto answer = FindEuler(G);
    for (auto &element : answer) {
        std::cout << element << ' ';
    }

    return 0;
}

//********************************************************************************************
