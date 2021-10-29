#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>

//********************************************************************************************

typedef uint32_t Vertex;

typedef uint32_t time_type;

const uint32_t poison = 1000 * 1000 * 1000 * 1LL;

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

    [[nodiscard]] virtual std::vector<Vertex> GetNeighborsVertex(const Vertex& v) const = 0;

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
            std::vector<Vertex> tmp(quantity, poison);
            matrix.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
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
};

//********************************************************************************************

class GraphList final : public Graph {
private:

    struct Neighbors {
        Vertex vertex;
        uint32_t number;
        uint32_t quantity_edge = 0;
    };

    std::vector<std::vector<Neighbors>> list;

    uint32_t number_edge = 0;

    void Add(const Vertex& a, const Vertex& b) override {
        if (a < 0 || b < 0) {
            return;
        }
        if (a >= q_vertex || b >= q_vertex) {
            return;
        }

        bool is_in_list = false;
        for (auto &elem : list[a]) {
            if (elem.vertex == b) {
                is_in_list = true;
                elem.quantity_edge++;
                break;
            }
        }
        if (is_in_list && !is_oriented) {
            for (auto &elem : list[b]) {
                if (elem.vertex == a) {
                    elem.quantity_edge++;
                    return;
                }
            }
        }

        Neighbors tmp;
        tmp.vertex = b;
        tmp.number = number_edge;
        tmp.quantity_edge = 1;
        list[a].push_back(tmp);

        if (!is_oriented) {
            tmp.vertex = a;
            tmp.number = number_edge;
            tmp.quantity_edge = 1;
            list[b].push_back(tmp);
        }

        ++number_edge;
    }

public:

    explicit GraphList(const Vertex& quantity, bool oriented = false) {
        for (Vertex i = 0; i < quantity; ++i) {
            std::vector<Neighbors> tmp;
            list.push_back(tmp);
        }
        q_vertex = quantity;
        is_oriented = oriented;
    }

    [[nodiscard]] std::vector<Vertex> GetNeighborsVertex(const Vertex& v) const override {
        std::vector<Vertex> answer;
        for (auto &elem : list[v]) {
            answer.push_back(elem.vertex);
        }
        return answer;
    }

    [[nodiscard]] bool IsNotMultiEdge(const Vertex& v, const uint32_t index) const {
        return list[v][index].quantity_edge == 1;
    }

    [[nodiscard]] uint32_t GetNumber(const Vertex& v, const uint32_t& index) const {
        return list[v][index].number;
    }


};

//********************************************************************************************

class GraphAlgorithm {
    std::vector<bool> visit;

    std::set<uint32_t> answer;

    time_type time = 0;

    std::vector<time_type> time_in;
    std::vector<time_type> time_out;

    GraphAlgorithm(const uint32_t& quantity) {
        visit.clear();
        answer.clear();

        time_in.clear();
        time_out.clear();

        for (uint32_t i = 0; i < quantity; ++i) {
            visit.push_back(false);

            time_in.push_back(poison);
            time_out.push_back(poison);
        }

        time = 0;
    }

    void DFS(GraphList& G, const Vertex& v, const Vertex& prev = poison) {
        visit[v] = true;
        time_in[v] = time_out[v] = time++;
        auto neighbors = G.GetNeighborsVertex(v);
        for (uint32_t i = 0; i < neighbors.size(); ++i) {
            Vertex u = neighbors[i];
            if (u != prev) {
                if (!visit[u]) {
                    DFS(G, u, v);
                    time_out[v] = std::min(time_out[v], time_out[u]);
                    if (time_out[u] > time_in[v]) {
                        if (G.IsNotMultiEdge(v, i)) {
                            answer.insert(G.GetNumber(v, i) + 1);
                        }
                    }
                } else {
                    time_out[v] = std::min(time_out[v], time_in[u]);
                }
            }
        }
    }

    friend std::set<uint32_t> FindBridges(GraphList& G);
};

std::set<uint32_t> FindBridges(GraphList& G) {
    GraphAlgorithm GAlg(G.GetQVertex());

    for (Vertex v = 0; v < G.GetQVertex(); ++v) {
        if (!GAlg.visit[v]) {
            GAlg.DFS(G, v);
        }
    }

    return GAlg.answer;
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

    for (uint32_t i = 0; i < q_edge; ++i) {
        Vertex a = 0, b = 0;
        std::cin >> a >> b;
        G.AddEdge(a, b);
    }

    auto answer = FindBridges(G);
    std::cout << answer.size() << std::endl;
    for (auto &element : answer) {
        std::cout << element << ' ';
    }
    std::cout << std::endl;

    return 0;
}

//********************************************************************************************
