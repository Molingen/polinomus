#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <ranges>
#include <unordered_map>
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
        std::vector<Monomus> vec;
        auto it1 = data.begin();
        auto it2 = other.data.begin();

        while (it1 != data.end() && it2 != other.data.end()) {
            if (*it1 > *it2) {
                vec.push_back(*it1);
                ++it1;
            } else if (*it2 > *it1) {
                vec.push_back(*it2);
                ++it2;
            } else {
                double sumc = it1->getCoeff() + it2->getCoeff();
                if (std::abs(sumc) >= EPS) {
                    vec.emplace_back(it1->getDeg(), sumc);
                }
                ++it1;
                ++it2;
            }
        }
        while (it1 != data.end()) {
            vec.push_back(*it1);
            ++it1;
        }
        while (it2 != other.data.end()) {
            vec.push_back(*it2);
            ++it2;
        }

        for (auto & it : std::ranges::reverse_view(vec)) {
            result.data.push_front(it);
        }
        return result;
    }

    Polynomus operator-(const Polynomus& other) const {
        Polynomus result;
        std::vector<Monomus> vec;
        auto it1 = data.begin();
        auto it2 = other.data.begin();

        while (it1 != data.end() && it2 != other.data.end()) {
            if (*it1 > *it2) {
                vec.push_back(*it1);
                ++it1;
            } else if (*it2 > *it1) {
                vec.emplace_back(it2->getDeg(), -it2->getCoeff());
                ++it2;
            } else {
                double diffc = it1->getCoeff() - it2->getCoeff();
                if (std::abs(diffc) >= EPS) {
                    vec.emplace_back(it1->getDeg(), diffc);
                }
                ++it1;
                ++it2;
            }
        }
        while (it1 != data.end()) {
            vec.push_back(*it1);
            ++it1;
        }
        while (it2 != other.data.end()) {
            vec.emplace_back(it2->getDeg(), -it2->getCoeff());
            ++it2;
        }

        for (auto & it : std::ranges::reverse_view(vec)) {
            result.data.push_front(it);
        }
        return result;
    }

    Polynomus operator*(const Polynomus& other) const {
        Polynomus result;
        std::unordered_map<uint32_t, double> tmp;

        for (const auto& m1 : data) {
            for (const auto& m2 : other.data) {
                Monomus prod = m1 * m2;
                tmp[prod.getDeg()] += prod.getCoeff();
            }
        }

        std::vector<Monomus> vec;
        for (const auto& [deg, coeff] : tmp) {
            if (std::abs(coeff) >= EPS) {
                vec.emplace_back(deg, coeff);
            }
        }
        auto cmp = [](const Monomus& a, const Monomus& b) {
            int td1 = a.totalDeg();
            int td2 = b.totalDeg();
            if (td1 != td2) return td1 > td2;
            if (a.x() != b.x()) return a.x() > b.x();
            if (a.y() != b.y()) return a.y() > b.y();
            return a.z() > b.z();
        };
        std::ranges::sort(vec, cmp);
        for (auto & it : std::ranges::reverse_view(vec)) {
            result.data.push_front(it);
        }
        return result;
    }

    Polynomus operator*(double scalar) const {
        if (std::abs(scalar) < EPS) return {};
        Polynomus result;
        std::vector<Monomus> vec;
        for (const auto& m : data) {
            vec.emplace_back(m.getDeg(), m.getCoeff() * scalar);
        }
        for (auto & it : std::ranges::reverse_view(vec)) {
            result.data.push_front(it);
        }
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

private:
    ForwardList<Monomus> data;
    static constexpr double EPS = 1e-9;

};
