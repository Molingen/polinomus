#pragma once
#include <cstdint>
#include <stdexcept>

class Monomus {
public:
    Monomus() : deg(0), coeff(0) {}
    Monomus(uint32_t x, uint32_t y, uint32_t z, double c) : coeff(c) {
        if (x >= 1024 || y >= 1024 || z >= 1024)
            throw std::overflow_error("Degree overflow");
        deg = (z << 20) | (y << 10) | x;
    }
    Monomus(uint32_t packed, double c) : deg(packed), coeff(c) {}

    Monomus operator+(const Monomus& other) const {
        if (deg != other.deg)
            throw std::logic_error("Different degrees");
        return {deg, coeff + other.coeff};
    }

    Monomus operator*(const Monomus& other) const {
        uint32_t nx = x() + other.x();
        uint32_t ny = y() + other.y();
        uint32_t nz = z() + other.z();
        if (nx >= 1024 || ny >= 1024 || nz >= 1024)
            throw std::overflow_error("Degree overflow");
        return {nx, ny, nz, coeff * other.coeff};
    }

    bool operator==(const Monomus& other) const { return deg == other.deg && std::abs(coeff - other.coeff) < 1e-9; }
    bool operator>(const Monomus& other) const {
        int td1 = totalDeg();
        int td2 = other.totalDeg();
        if (td1 != td2) return td1 > td2;
        if (x() != other.x()) return x() > other.x();
        if (y() != other.y()) return y() > other.y();
        return z() > other.z();
    }
    bool sameDeg(const Monomus& other) const { return deg == other.deg; }

    uint32_t x()        const { return deg & MASK;          }
    uint32_t y()        const { return (deg >> 10) & MASK;  }
    uint32_t z()        const { return (deg >> 20) & MASK;  }
    int totalDeg()      const { return x() + y() + z();     }
    uint32_t getDeg()   const { return deg;                 }
    double getCoeff()   const { return coeff;               }

private:
    uint32_t deg;
    double coeff;

    static constexpr uint32_t MASK = (1u << 10) - 1;
};