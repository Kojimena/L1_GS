#include <vector>
#include "color.h"  // Asegúrate de que la ruta sea correcta
#include "vertex2.h"  // Asegúrate de que la ruta sea correcta
#include <cmath>

// Tamaño del framebuffer
const int altura = 1600;
const int anchura = 1200;

// Framebuffer
std::vector<Color> framebuffer(anchura * altura);

// Color base del framebuffer
Color clearColor(0, 0, 0);  // Negro por defecto

// Función para limpiar el framebuffer
void clear() {
    std::fill(framebuffer.begin(), framebuffer.end(), clearColor);
}

Color currentColor(255, 255, 255);  // Blanco por defecto


// Función para dibujar un punto
void punto(const Vertex2& vertex) {
    // Asegurarse de que las coordenadas están en el rango correcto
    int x = static_cast<int>(std::round(vertex.x));
    int y = static_cast<int>(std::round(vertex.y));

    if (x < 0 || x >= anchura || y < 0 || y >= altura) {
        return;
    }

    // Calcular el índice en el framebuffer
    int index = y * anchura + x;

    // Poner el punto en el framebuffer
    framebuffer[index] = currentColor;
}

void setClearColor(const Color& color) {
    clearColor = color;
}

void setCurrentColor(const Color& color) {
    currentColor = color;
}

#include <fstream>

void renderBuffer() {
    std::ofstream imageFile("output.bmp", std::ios::binary);

    // Estructura del encabezado de un archivo BMP
    unsigned char bmpFileHeader[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 };
    unsigned char bmpInfoHeader[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0 };

    // Escribir el tamaño de la imagen
    int fileSize = 54 + 3 * anchura * altura;
    bmpFileHeader[2] = (unsigned char)(fileSize);
    bmpFileHeader[3] = (unsigned char)(fileSize >> 8);
    bmpFileHeader[4] = (unsigned char)(fileSize >> 16);
    bmpFileHeader[5] = (unsigned char)(fileSize >> 24);

    // Escribir el ancho y alto de la imagen
    bmpInfoHeader[4] = (unsigned char)(anchura);
    bmpInfoHeader[5] = (unsigned char)(anchura >> 8);
    bmpInfoHeader[6] = (unsigned char)(anchura >> 16);
    bmpInfoHeader[7] = (unsigned char)(anchura >> 24);
    bmpInfoHeader[8] = (unsigned char)(altura);
    bmpInfoHeader[9] = (unsigned char)(altura >> 8);
    bmpInfoHeader[10] = (unsigned char)(altura >> 16);
    bmpInfoHeader[11] = (unsigned char)(altura >> 24);

    // Escribir los encabezados
    imageFile.write((char*)bmpFileHeader, 14);
    imageFile.write((char*)bmpInfoHeader, 40);

    // Escribir los datos del color
    for (Color& pixel : framebuffer) {
        imageFile.put(pixel.b);
        imageFile.put(pixel.g);
        imageFile.put(pixel.r);

    }

    imageFile.close();
}

void linea(Vertex2 inicio, Vertex2 fin) {
    // Convertir a enteros
    int x1 = static_cast<int>(std::round(inicio.x));
    int y1 = static_cast<int>(std::round(inicio.y));
    int x2 = static_cast<int>(std::round(fin.x));
    int y2 = static_cast<int>(std::round(fin.y));

    // Asegurarse de que la línea esté dentro del rango del framebuffer
    x1 = std::max(0, std::min(x1, anchura - 1));
    y1 = std::max(0, std::min(y1, altura - 1));
    x2 = std::max(0, std::min(x2, anchura - 1));
    y2 = std::max(0, std::min(y2, altura - 1));

    // Implementación del algoritmo de Bresenham
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        punto(Vertex2(x1, y1));

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void drawPolygon(const std::vector<Vertex2>& vertices) {
    // Comprobar si hay al menos 2 puntos
    if (vertices.size() < 2) {
        return;
    }

    // Dibujar líneas entre puntos consecutivos
    for (size_t i = 0; i < vertices.size() - 1; ++i) {
        linea(vertices[i], vertices[i + 1]);
    }

    // Conectar el último punto con el primer punto
    linea(vertices.back(), vertices.front());
}

void clearPixel(const Vertex2& vertex) {
    // Ensure coordinates are in correct range
    int x = static_cast<int>(std::round(vertex.x));
    int y = static_cast<int>(std::round(vertex.y));

    if (x < 0 || x >= anchura || y < 0 || y >= altura) {
        return;
    }

    // Calculate index in the framebuffer
    int index = y * anchura + x;

    // Clear the pixel in the framebuffer
    framebuffer[index] = clearColor;
}


void fillPolygon(const std::vector<Vertex2>& vertices, bool isHole = false) {
    if (vertices.size() < 3) {
        return;
    }

    // Encuentra el mínimo y máximo y entre los vértices
    int minY = static_cast<int>(std::round(vertices[0].y));
    int maxY = minY;
    for (const Vertex2& vertex : vertices) {
        int y = static_cast<int>(std::round(vertex.y));
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }

    // Para cada línea horizontal en el rango y
    for (int y = minY; y <= maxY; ++y) {
        // Encuentra las intersecciones con cada línea de polígono
        std::vector<int> intersections;
        for (size_t i = 0; i < vertices.size(); ++i) {
            const Vertex2& vertex1 = vertices[i];
            const Vertex2& vertex2 = vertices[(i + 1) % vertices.size()];

            // Si la línea horizontal intersecta con la línea del polígono
            if ((vertex1.y <= y && y < vertex2.y) ||
                (vertex2.y <= y && y < vertex1.y)) {
                // Calcula la intersección x
                float t = (y - vertex1.y) / (vertex2.y - vertex1.y);
                int x = static_cast<int>(std::round(vertex1.x + t * (vertex2.x - vertex1.x)));
                intersections.push_back(x);
            }
        }

        // Ordena las intersecciones
        std::sort(intersections.begin(), intersections.end());

        // Dibuja líneas entre cada par de intersecciones
        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            if (isHole) { // Si es un agujero, limpia en lugar de dibujar
                for (int x = intersections[i]; x <= intersections[i + 1]; ++x) {
                    clearPixel(Vertex2(x, y));
                }
            } else {
                for (int x = intersections[i]; x <= intersections[i + 1]; ++x) {
                    punto(Vertex2(x, y));
                }
            }
        }
    }
}


std::vector<Vertex2> polygon1() {
    return {{165, 380}, {185, 360}, {180, 330}, {207, 345}, {233, 330}, {230, 360}, {250, 380}, {220, 385}, {205, 410}, {193, 383}};
}

std::vector<Vertex2> polygon2() {
    return {{321, 335}, {288, 286}, {339, 251}, {374, 302}};
}

std::vector<Vertex2> polygon3() {
    return {{377, 249}, {411, 197}, {436, 249}};
}

std::vector<Vertex2> polygon4() {
    return {{413, 177}, {448, 159}, {502, 88}, {553, 53}, {535, 36}, {676, 37}, {660, 52},
            {750, 145}, {761, 179}, {672, 192}, {659, 214}, {615, 214}, {632, 230}, {580, 230},
            {597, 215}, {552, 214}, {517, 144}, {466, 180}};
}

std::vector<Vertex2> polygon5() {
    return {{682, 175}, {708, 120}, {735, 148}, {739, 170}};
}


void render() {
    clear();

    // Dibujar el polígono 1
    setCurrentColor(Color(255, 255, 0));  // Color amarillo
    fillPolygon(polygon1());
    setCurrentColor(Color(255, 255, 255));  // Color blanco
    drawPolygon(polygon1());

    // Dibujar el polígono 2
    setCurrentColor(Color(0, 0, 255));  // Color azul
    fillPolygon(polygon2());
    setCurrentColor(Color(255, 255, 255));  // Color blanco
    drawPolygon(polygon2());

    // Dibujar el polígono 3
    setCurrentColor(Color(255, 0, 0));  // Color rojo
    fillPolygon(polygon3());
    setCurrentColor(Color(255, 255, 255));  // Color blanco
    drawPolygon(polygon3());

    // Dibujar el polígono 4 con el polígono 5 como un agujero
    setCurrentColor(Color(0, 255, 0));  // Color verde
    fillPolygon(polygon4());
    fillPolygon(polygon5(), true);  // Polígono 5 como un agujero
    setCurrentColor(Color(255, 255, 255));  // Color blanco
    drawPolygon(polygon4());
    drawPolygon(polygon5());

    renderBuffer();
}
