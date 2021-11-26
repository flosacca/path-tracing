#pragma once
#include "common.h"

inline void print(const glm::dmat4& m) {
    fprintf(stderr,
        "[[%12.8f, %12.8f, %12.8f, %12.8f],\n"
        " [%12.8f, %12.8f, %12.8f, %12.8f],\n"
        " [%12.8f, %12.8f, %12.8f, %12.8f],\n"
        " [%12.8f, %12.8f, %12.8f, %12.8f]]\n",
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    );
}

struct TriMat {
    double m00, m01, m02;
    double m10, m11, m12;
    double m20, m21, m22;
    int k;

    TriMat() = default;

    TriMat(const Vec& p0, const Vec& e1, const Vec& e2) {
        Vec n = glm::abs(glm::cross(e1, e2));
        Vec f(0);
        if (n.x > n.y && n.x > n.z) {
            k = 0;
            f.x = 1;
        } else if (n.y > n.z) {
            k = 1;
            f.y = 1;
        } else {
            k = 2;
            f.z = 1;
        }

        glm::dmat4 m;
        m[0] = glm::dvec4(e1, 0);
        m[1] = glm::dvec4(e2, 0);
        m[2] = glm::dvec4(f, 0);
        m[3] = glm::dvec4(p0, 1);

        glm::dmat4 inv_m = glm::inverse(m);
        glm::dmat3 t;
        int i = 0;
        for (int j = 0; j < 4; ++j) {
            if (j == k) {
                if (!cmp::zero(glm::distance(inv_m[j], glm::dvec4(0, 0, 1, 0)))) {
                    print(m);
                    print(inv_m);
                }
            } else {
                t[i++] = inv_m[j];
            }
        }

        m00 = t[0][0];
        m01 = t[1][0];
        m02 = t[2][0];
        m10 = t[0][1];
        m11 = t[1][1];
        m12 = t[2][1];
        m20 = t[0][2];
        m21 = t[1][2];
        m22 = t[2][2];
    }

    bool solve(const Vec& o, const Vec& d, Vec& p) const {
        /*
           t = -o.z / d.z
           b1 = o.x + t * d.x
           b2 = o.y + t * d.y
        */

        double t, b1, b2;

        switch (k) {
        case 0:
            t = -(o.x + m20 * o.y + m21 * o.z + m22) / (d.x + m20 * d.y + m21 * d.z);
            if (!(cmp::less(0, t) && t < INF))
                return 0;
            b1 = (m00 * o.y + m01 * o.z + m02) + t * (m00 * d.y + m01 * d.z);
            if (!cmp::inclusive(b1, 0, 1))
                return 0;
            b2 = (m10 * o.y + m11 * o.z + m12) + t * (m10 * d.y + m11 * d.z);
            break;

        case 1:
            t = -(m20 * o.x + o.y + m21 * o.z + m22) / (m20 * d.x + d.y + m21 * d.z);
            if (!(cmp::less(0, t) && t < INF))
                return 0;
            b1 = (m00 * o.x + m01 * o.z + m02) + t * (m00 * d.x + m01 * d.z);
            if (!cmp::inclusive(b1, 0, 1))
                return 0;
            b2 = (m10 * o.x + m11 * o.z + m12) + t * (m10 * d.x + m11 * d.z);
            break;

        case 2:
            t = -(m20 * o.x + m21 * o.y + o.z + m22) / (m20 * d.x + m21 * d.y + d.z);
            if (!(cmp::less(0, t) && t < INF))
                return 0;
            b1 = (m00 * o.x + m01 * o.y + m02) + t * (m00 * d.x + m01 * d.y);
            if (!cmp::inclusive(b1, 0, 1))
                return 0;
            b2 = (m10 * o.x + m11 * o.y + m12) + t * (m10 * d.x + m11 * d.y);
            break;
        }

        if (!cmp::inclusive(b2, 0, 1 - b1))
            return 0;

        p = {t, b1, b2};
        return 1;
    }
};
