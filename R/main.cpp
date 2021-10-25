#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>

//********************************************************************************************

typedef uint64_t Vertex;

typedef uint64_t dist_t;

typedef uint64_t weight_t;

typedef char color_t;

typedef uint64_t time_type;

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

    std::set<Vertex> component;

    Vertex start_cycle = poison;
    Vertex end_cycle = poison;

    time_type time = 0;

    std::vector<time_type> time_in;
    std::vector<time_type> time_out;

    GraphAlgorithm(const uint64_t& quantity) {
        dist.clear();
        parent.clear();
        color.clear();
        visit.clear();

        component.clear();

        time_in.clear();
        time_out.clear();

        for (uint64_t i = 0; i < quantity; ++i) {
            dist.push_back(poison);
            parent.push_back(poison);
            color.push_back(WHITE);
            visit.push_back(false);

            time_in.push_back(poison);
            time_out.push_back(poison);
        }

        start_cycle = poison;
        end_cycle = poison;

        time = 0;
    }

    void DFS(Graph& G, const Vertex& v, const Vertex& prev = poison) {
        uint64_t count = 0;
        visit[v] = true;
        time_in[v] = time_out[v] = time++;
        auto neighbors = G.GetNeighborsVertex(v);
        for (auto &u : neighbors) {
            if (u != prev) {
                if (!visit[u]) {
                    DFS(G, u, v);
                    time_out[v] = std::min(time_out[v], time_out[u]);
                    if (time_out[u] >= time_in[v] && prev != poison) {
                        component.insert(v);
                    }
                    ++count;
                    continue;
                }
                time_out[v] = std::min(time_out[v], time_in[u]);
            }
        }
        if (count > 1 && prev == poison) {
            component.insert(v);
        }
    }

    friend std::set<Vertex> FindArticulationPoints(Graph& G);

};

std::set<Vertex> FindArticulationPoints(Graph& G) {
    GraphAlgorithm GAlg(G.GetQVertex());


    for (Vertex v = 0; v < G.GetQVertex(); ++v) {
        if (!GAlg.visit[v]) {
            GAlg.DFS(G, v);
        }
    }

    std::set<Vertex> answer;
    for (auto &element : GAlg.component) {
        answer.insert(element + 1);
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

    Vertex q_vertex = 0, q_edge = 0;
    std::cin >> q_vertex >> q_edge;

    GraphList G(q_vertex, NOT_ORIENTED);

    for (uint64_t i = 0; i < q_edge; ++i) {
        Vertex a = 0, b = 0;
        std::cin >> a >> b;
        G.AddEdge(a, b, 0);
    }

    auto answer = FindArticulationPoints(G);
    std::cout << answer.size() << std::endl;
    for (auto &element : answer) {
        std::cout << element << std::endl;
    }

    return 0;
}

//********************************************************************************************