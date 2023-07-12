#pragma once
#include <iostream>
#include <algorithm>

class Color {
public:
    // Constructor que realiza clamping
    Color(int red, int green, int blue)
        : r(clamp(red)), g(clamp(green)), b(clamp(blue)) {}

    // Clamping: asegura que el valor esté en el rango de 0 a 255
    unsigned char clamp(int value) {
        return static_cast<unsigned char>(std::max(0, std::min(value, 255)));
    }

    // Sobrecarga del operador << para la impresión
    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << "Color(R: " << static_cast<int>(color.r) 
           << ", G: " << static_cast<int>(color.g) 
           << ", B: " << static_cast<int>(color.b) << ")";
        return os;
    }

    // Sobrecarga del operador + para la suma de colores
    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b);
    }

    // Sobrecarga del operador * para la multiplicación por un float
    Color operator*(float scalar) const {
        return Color(static_cast<int>(r * scalar), 
                     static_cast<int>(g * scalar), 
                     static_cast<int>(b * scalar));
    }

    // En la definición de la clase Color
    Color& operator=(const Color& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }

    Color() : r(0), g(0), b(0) {}


public:
    unsigned char r;  // Rojo
    unsigned char g;  // Verde
    unsigned char b;  // Azul
};
