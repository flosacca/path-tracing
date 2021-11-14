#pragma once
#include "common.h"

struct Box {
    Vec p1, p2;

    Box() {}

    Box(const Vec& p1, const Vec& p2) :
        p1(p1), p2(p2) {}

    bool intersects(const Ray& r) const {
        glm::dvec2 t(-INF, +INF);
        for (int i = 0; i < 3; ++i) {
            if (num::nonzero(r.d[i])) {
                glm::dvec2 s(p1[i] - r.o[i], p2[i] - r.o[i]);
                s /= r.d[i];
                if (s.x < s.y) {
                    t = {std::max(t.x, s.x), std::min(t.y, s.y)};
                } else {
                    t = {std::max(t.x, s.y), std::min(t.y, s.x)};
                }
            }
        }
        return num::less(t.x, t.y) && num::greater(t.y, 0);
    }

    Vec shape() const {
        return p2 - p1;
    }

    static Box merge(const Box& a, const Box& b) {
        return {glm::min(a.p1, b.p1), glm::max(a.p2, b.p2)};
    }

    static Box of(const Vec& v1, const Vec& v2) {
        return {glm::min(v1, v2), glm::max(v1, v2)};
    }

    template <typename... Args>
    static Box of(const Vec& v1, const Vec& v2, const Args&... args) {
        Box box = Box::of(v2, args...);
        return {glm::min(v1, box.p1), glm::max(v1, box.p2)};
    }

    template <typename It, typename F>
    static Box fromRange(It l, It r, F f) {
        Vec p1(+INF);
        Vec p2(-INF);
        while (l != r) {
            Vec v = f(*l++);
            p1 = glm::min(p1, v);
            p2 = glm::max(p2, v);
        }
        return {p1, p2};
    }
};

template <typename T>
struct BVH {
    struct Node {
        Box s;
        const T* v;
    };

    std::vector<Node> t;
    int n = 0;

    static int size(int n) {
        while (n != (n & -n)) {
            n += n & -n;
        }
        return n * 2;
    }

    template <typename It>
    void build(It first, It last) {
        It w = first;
        std::vector<Node> leaves;
        std::vector<Vec> mids;
        std::vector<int> indices;
        while (first != last) {
            const T& e = *first++;
            Box box = e.box();
            leaves.push_back({box, &e});
            mids.push_back(box.p1 + box.p2);
            indices.push_back(n++);
        }
        t.resize(size(n));
        auto f = indices.data();
        auto p = [&] (int i) -> const Vec& {
            return mids[i];
        };
        Recursive::of([&] (auto dfs, int i, int j, int k) -> void {
            if (j - i == 1) {
                t[k] = leaves[f[i]];
            } else {
                Vec s = Box::fromRange(f + i, f + j, p).shape();
                double c[3] = {s.x, s.y, s.z};
                int d = std::max_element(c, c + 3) - c;
                int m = (i + j) / 2;
                std::nth_element(f + i, f + m, f + j, [&] (int u, int v) {
                    return p(u)[d] < p(v)[d];
                });
                dfs(i, m, k * 2);
                dfs(m, j, k * 2 + 1);
                t[k].s = Box::merge(t[k * 2].s, t[k * 2 + 1].s);
            }
        })(0, n, 1);
    }

    void intersect(const Ray& r, Intersection& s) const {
        Recursive::of([&] (auto dfs, int i, int j, int k) -> void {
            if (j - i == 1) {
                t[k].v->intersect(r, s);
            } else if (t[k].s.intersects(r)) {
                int m = (i + j) / 2;
                dfs(i, m, k * 2);
                dfs(m, j, k * 2 + 1);
            }
        })(0, n, 1);
    }
};
