#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "forward_list.hpp"
#include "monomus.hpp"

class Polynomus {
public:
    Polynomus() = default;

    bool isZero() const {
        return data.begin() == data.end();
    }

    void addMonom(const Monomus& m) {
        if (std::abs(m.getCoeff()) < EPS) return;

        auto prev = data.before_begin();
        auto curr = data.begin();

        while (curr != data.end() && (*curr > m)) {
            ++prev;
            ++curr;
        }

        if (curr != data.end() && curr->getDeg() == m.getDeg()) {
            Monomus sum = *curr + m;

            if (std::abs(sum.getCoeff()) < EPS) {
                data.erase_after(prev);
            } else {
                *curr = sum;
            }
        } else {
            data.insert_after(prev, m);
        }
    }

    Polynomus operator+(const Polynomus& other) const {
        Polynomus result;

        auto it1 = data.begin();
        auto it2 = other.data.begin();

        while (it1 != data.end() && it2 != other.data.end()) {
            if (it1->getDeg() > it2->getDeg()) {
                result.addMonom(*it1++);
            } else if (it1->getDeg() < it2->getDeg()) {
                result.addMonom(*it2++);
            } else {
                Monomus sum = *it1 + *it2;
                if (std::abs(sum.getCoeff()) >= EPS)
                    result.addMonom(sum);
                ++it1;
                ++it2;
            }
        }
        while (it1 != data.end()) result.addMonom(*it1++);
        while (it2 != other.data.end()) result.addMonom(*it2++);
        result.normalize();
        return result;
    }

    Polynomus operator-(const Polynomus& other) const {
        Polynomus result = *this;
        for (auto & it : other.data) {
            result.addMonom(Monomus(it.getDeg(), -it.getCoeff()));
        }
        result.normalize();
        return result;
    }

    Polynomus operator*(const Polynomus& other) const {
        Polynomus result;
        for (auto & it1 : data) {
            for (auto & it2 : other.data) {
                result.addMonom(it1 * it2);
            }
        }
        result.normalize();
        return result;
    }

    Polynomus operator*(double scalar) const {
        if (std::abs(scalar) < EPS) return {};
        Polynomus result;
        for (auto & it : data) {
            result.addMonom(Monomus(it.getDeg(), it.getCoeff() * scalar));
        }
        result.normalize();
        return result;
    }

    friend Polynomus operator*(double scalar, const Polynomus& p) {
        return p * scalar;
    }

    double evaluate(double x, double y, double z) const {
        double res = 0;
        for (auto & it : data) {
            double term = it.getCoeff();
            for (uint32_t i = 0; i < it.x(); ++i) term *= x;
            for (uint32_t i = 0; i < it.y(); ++i) term *= y;
            for (uint32_t i = 0; i < it.z(); ++i) term *= z;
            res += term;
        }
        return res;
    }

    bool operator==(const Polynomus& other) const {
        auto it1 = data.begin();
        auto it2 = other.data.begin();
        while (it1 != data.end() && it2 != other.data.end()) {
            if (it1->getDeg() != it2->getDeg()) return false;
            if (std::abs(it1->getCoeff() - it2->getCoeff()) > EPS) return false;
            ++it1;
            ++it2;
        }
        return it1 == data.end() && it2 == other.data.end();
    }

    friend std::ostream& operator<<(std::ostream& os, const Polynomus& p) {
        if (p.data.begin() == p.data.end()) {
            os << "0";
            return os;
        }
        bool firstPrinted = false;
        for (auto & it : p.data) {
            double c = it.getCoeff();
            if (std::abs(c) < EPS) continue;
            if (firstPrinted) {
                os << (c >= 0 ? " + " : " - ");
            } else {
                if (c < 0) os << "-";
            }
            double absC = std::abs(c);
            uint32_t px = it.x();
            uint32_t py = it.y();
            uint32_t pz = it.z();
            bool isConst = (px == 0 && py == 0 && pz == 0);
            if (isConst || std::abs(absC - 1.0) > EPS) {
                os << absC;
            }
            auto printVar = [&](char var, uint32_t deg) {
                if (deg == 0) return;
                os << var;
                if (deg > 1) os << "^" << deg;
            };
            printVar('x', px);
            printVar('y', py);
            printVar('z', pz);
            firstPrinted = true;
        }
        return os;
    }

    void normalize() {
        std::vector<Monomus> v;
        for (auto & it : data) if (std::abs(it.getCoeff()) > EPS) v.push_back(it);
        auto cmp = [](const Monomus& a, const Monomus& b) {
            int td1 = a.x() + a.y() + a.z();
            int td2 = b.x() + b.y() + b.z();
            if (td1 != td2) return td1 > td2;
            if (a.x() != b.x()) return a.x() > b.x();
            if (a.y() != b.y()) return a.y() > b.y();
            return a.z() > b.z();
        };
        std::ranges::sort(v, cmp);
        data.clear();
        for (int i = static_cast<int>(v.size()) - 1; i >= 0; --i) {
            data.push_front(v[i]);
        }
    }

private:
    ForwardList<Monomus> data;
    static constexpr double EPS = 1e-9;

};
