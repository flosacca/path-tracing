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
        return num::lessEqual(t.x, t.y) && num::greater(t.y, 0);
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

    template <typename It>
    static Box fromRange(It l, It r) {
        return fromRange(l, r, [] (const Vec& v) { return v; });
    }
};

template <typename T>
struct BVH {
    std::vector<Box> tree;
    std::vector<const T*> leaves;
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
        struct Cache {
            const T* obj;
            Box box;
            Vec mid;
        };
        std::vector<Cache> caches;
        while (first != last) {
            const T& e = *first++;
            Box box = e.box();
            caches.push_back({&e, box, box.p1 + box.p2});
        }
        n = caches.size();
        if (n == 0) {
            return;
        }
        leaves.resize(n);
        tree.resize(size(n));
        auto p = caches.data();
        auto f = [] (const Cache& cache) {
            return cache.mid;
        };
        fun::recursive([&] (auto dfs, int i, int j, int k) -> void {
            if (j - i == 1) {
                leaves[i] = p[i].obj;
                tree[k] = p[i].box;
            } else {
                Vec s = Box::fromRange(p + i, p + j, f).shape();
                double c[3] = {s.x, s.y, s.z};
                int d = std::max_element(c, c + 3) - c;
                int m = (i + j) / 2;
                std::nth_element(p + i, p + m, p + j, [&] (const Cache& a, const Cache& b) {
                    return a.mid[d] < b.mid[d];
                });
                dfs(i, m, k * 2);
                dfs(m, j, k * 2 + 1);
                tree[k] = Box::merge(tree[k * 2], tree[k * 2 + 1]);
            }
        })(0, n, 1);
    }

    void build(const std::vector<T>& a) {
        build(a.begin(), a.end());
    }

    template <typename S>
    void intersect(const Ray& r, S& s) const {
        if (n == 0) {
            return;
        }
        fun::recursive([&] (auto dfs, int i, int j, int k) -> void {
            if (j - i <= 4) {
                for (; i < j; ++i) {
                    leaves[i]->intersect(r, s);
                }
            } else if (tree[k].intersects(r)) {
                int m = (i + j) / 2;
                dfs(i, m, k * 2);
                dfs(m, j, k * 2 + 1);
            }
        })(0, n, 1);
    }
};
