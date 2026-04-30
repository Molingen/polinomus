#include <iostream>
#include "src/poly_parser.hpp"

int main() {
    Polynomus p;
    p.addMonom(Monomus(0,0,0,1.0));
    p.addMonom(Monomus(3,0,0,1.0));
    p.addMonom(Monomus(1,0,0,1.0));
    std::cout << p << '\n';
}