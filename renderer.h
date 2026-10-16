// Estructuras base del Pipeline

#pragma once
#include <raylib.h>

// Estructura para representar un vértice en el espacio
typedef struct {
    Vector3 position; // Posición (x, y, z)
    Vector2 uv; // Coordenadas de textura
    Vector3 normal; // Normal para iluminación
    Color color; // Color base del vértice
} Vertex;

// Estructura para los fragmentos
typedef struct {
    int x;
    int y;
    Color color;
    float depth;
} Fragment;

// Uniforms para pasar matrices o parámetros globales
typedef struct {
    Matrix modelMatrix;
    Matrix viewMatrix;
    Matrix projectionMatrix;
    Color currentColor;
} Uniforms;