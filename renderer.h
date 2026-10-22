#ifndef RENDERER_H
#define RENDERER_H

// Estructuras base del Pipeline
#pragma once
#include <raylib.h>

typedef enum {
    TRIANGLE_MODE_WIREFRAME,
    TRIANGLE_MODE_BARYCENTRIC,
    TRIANGLE_MODE_SCANLINE
} TriangleMode;

// Estructura para representar un vértice en el espacio
typedef struct {
    Vector3 position; // Posición (x, y, z)
    Color color; // Color base del vértice
} Vertex;

// Prototipos de funciones
void rasterize_triangle(Vertex v0, Vertex v1, Vertex v2, TriangleMode mode, Color* framebuffer, float* zbuffer, int screenWidth, int screenHeight);
void draw_wireframe(Vertex v0, Vertex v1, Vertex v2, Color* framebuffer, int screenWidth, int screenHeight);
void rasterize_barycentric(Vertex v0, Vertex v1, Vertex v2, Color* framebuffer, float* zbuffer, int screenWidth, int screenHeight);
void rasterize_scanline(Vertex v0, Vertex v1, Vertex v2, Color* framebuffer, float* zbuffer, int screenWidth, int screenHeight);

#endif