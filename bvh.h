#pragma once
#include "common.h"

struct Box {
    Vec p1, p2;

    Box(const Vec& p1, const Vec& p2) :
        p1(p1), p2(p2) {}

    Box(const Box& a, const Box& b) :
        p1(glm::min(a.p1, b.p1)),
        p2(glm::max(a.p2, b.p2)) {}

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

    static Box of(const Vec& v1, const Vec& v2) {
        return {glm::min(v1, v2), glm::max(v1, v2)};
    }

    template <typename... Args>
    static Box of(const Vec& v1, const Vec& v2, const Args&... args) {
        Box box = Box::of(v2, args...);
        return {glm::min(v1, box.p1), glm::max(v1, box.p2)};
    }

    template <typename It>
    static Box fromRange(It first, It last) {
        Vec p1(+INF);
        Vec p2(-INF);
        for (; first != last; ++first) {
            p1 = glm::min(p1, *first);
            p2 = glm::max(p2, *first);
        }
        return {p1, p2};
    }
};

template <typename T>
struct BVH {
    std::vector<T> objs;

    BVH(const std::vector<T>& objs) : objs(objs) {}

    void intersect(const Ray& r, Intersection& s) const {
        for (const T& e : objs) {
            if (e.box().intersects(r)) {
                e.intersect(r, s);
            }
        }
    }
};
