#include "renderer.h"
#include <math.h>
#include <float.h>

// Algoritmo de Wireframe (dibujar contornos)
void draw_wireframe(Vertex v0, Vertex v1, Vertex v2, Color* framebuffer, int screenWidth, int screenHeight) {
    DrawLine((int)v0.position.x, (int)v0.position.y, (int)v1.position.x, (int)v1.position.y, v0.color);
    DrawLine((int)v1.position.x, (int)v1.position.y, (int)v2.position.x, (int)v2.position.y, v1.color);
    DrawLine((int)v2.position.x, (int)v2.position.y, (int)v0.position.x, (int)v0.position.y, v2.color);
}

// Función para calcular coordenadas baricéntricas de un punto P respecto a un triángulo (A, B, C)
Vector3 barycentric(Vector2 A, Vector2 B, Vector2 C, Vector2 P) {
    Vector2 AB = { B.x - A.x, B.y - A.y };
    Vector2 AC = { C.x - A.x, C.y - A.y };
    Vector2 AP = { P.x - A.x, P.y - A.y };

    float denom = AB.x * AC.y - AB.y * AC.x;
    if (fabsf(denom) < 1e-5f) return (Vector3){ -1, -1, -1 }; // Evitar división por cero

    float v = (AP.x * AC.y - AP.y * AC.x) / denom;
    float w = (AB.x * AP.y - AB.y * AP.x) / denom;
    float u = 1.0f - v - w;

    return (Vector3){ u, v, w };
}

// Algoritmo: Coordenadas Baricéntricas
void rasterize_barycentric(Vertex v0, Vertex v1, Vertex v2, Color* framebuffer, float* zbuffer, int screenWidth, int screenHeight) {
    int minX = (int)fmax(0.0f, fmin(v0.position.x, fmin(v1.position.x, v2.position.x)));
    int maxX = (int)fmin((float)(screenWidth - 1), fmax(v0.position.x, fmax(v1.position.x, v2.position.x)));
    int minY = (int)fmax(0.0f, fmin(v0.position.y, fmin(v1.position.y, v2.position.y)));
    int maxY = (int)fmin((float)(screenHeight - 1), fmax(v0.position.y, fmax(v1.position.y, v2.position.y)));

    Vector2 p0 = { v0.position.x, v0.position.y };
    Vector2 p1 = { v1.position.x, v1.position.y };
    Vector2 p2 = { v2.position.x, v2.position.y };

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Vector2 p = { (float)x, (float)y };
            Vector3 bc = barycentric(p0, p1, p2, p);

            if (bc.x >= 0.0f && bc.y >= 0.0f && bc.z >= 0.0f) {
                float z = bc.x * v0.position.z + bc.y * v1.position.z + bc.z * v2.position.z;
                int index = y * screenWidth + x;

                if (z < zbuffer[index]) {
                    zbuffer[index] = z;
                    Color interpolatedColor = {
                        (unsigned char)(bc.x * v0.color.r + bc.y * v1.color.r + bc.z * v2.color.r),
                        (unsigned char)(bc.x * v0.color.g + bc.y * v1.color.g + bc.z * v2.color.g),
                        (unsigned char)(bc.x * v0.color.b + bc.y * v1.color.b + bc.z * v2.color.b),
                        255
                    };
                    framebuffer[index] = interpolatedColor;
                }
            }
        }
    }
}

// Función auxiliar para ordenar tres vértices por coordenada Y ascendente (Scanline)
static void sort_vertices_y(Vertex* v0, Vertex* v1, Vertex* v2) {
    Vertex temp;
    if (v0->position.y > v1->position.y) { temp = *v0; *v0 = *v1; *v1 = temp; }
    if (v0->position.y > v2->position.y) { temp = *v0; *v0 = *v2; *v2 = temp; }
    if (v1->position.y > v2->position.y) { temp = *v1; *v1 = *v2; *v2 = temp; }
}

// Algoritmo: Líneas Horizontales (Scanline)
void rasterize_scanline(Vertex v0, Vertex v1, Vertex v2, Color* framebuffer, float* zbuffer, int screenWidth, int screenHeight) {
    sort_vertices_y(&v0, &v1, &v2);

    int y0 = (int)roundf(v0.position.y);
    int y1 = (int)roundf(v1.position.y);
    int y2 = (int)roundf(v2.position.y);

    int total_height = y2 - y0;
    if (total_height == 0) return; // Caso degenerado (área cero)

    for (int y = y0; y <= y2; y++) {
        if (y < 0 || y >= screenHeight) continue;

        int second_half = y > y1 || y1 == y0;
        int segment_height = second_half ? (y2 - y1) : (y1 - y0);
        if (segment_height == 0) continue;

        float alpha = (float)(y - y0) / (float)total_height;
        float beta = second_half ? (float)(y - y1) / (float)(y2 - y1) : (float)(y - y0) / (float)(y1 - y0);

        // Interpolar puntos A y B en los bordes del triángulo para la fila y
        Vector3 A = {
            v0.position.x + (v2.position.x - v0.position.x) * alpha,
            (float)y,
            v0.position.z + (v2.position.z - v0.position.z) * alpha
        };

        Vector3 B = second_half ? (Vector3){
            v1.position.x + (v2.position.x - v1.position.x) * beta,
            (float)y,
            v1.position.z + (v2.position.z - v1.position.z) * beta
        } : (Vector3){
            v0.position.x + (v1.position.x - v0.position.x) * beta,
            (float)y,
            v0.position.z + (v1.position.z - v0.position.z) * beta
        };

        if (A.x > B.x) { Vector3 t = A; A = B; B = t; }

        int x_start = (int)fmax(0.0f, fminf((float)screenWidth - 1, A.x));
        int x_end = (int)fmax(0.0f, fminf((float)screenWidth - 1, B.x));

        for (int x = x_start; x <= x_end; x++) {
            int index = y * screenWidth + x;
            // Interpolación simple de profundidad lineal a lo largo de la línea horizontal
            float t_x = (B.x == A.x) ? 0.0f : (x - A.x) / (B.x - A.x);
            float z = A.z + (B.z - A.z) * t_x;

            if (z < zbuffer[index]) {
                zbuffer[index] = z;
                // Color sólido o interpolado básico para el modo scanline
                framebuffer[index] = (Color){255, 182, 193, 255}; // Color pastel de ejemplo
            }
        }
    }
}

// Función principal que enruta al modo correspondiente
void rasterize_triangle(Vertex v0, Vertex v1, Vertex v2, TriangleMode mode, Color* framebuffer, float* zbuffer, int screenWidth, int screenHeight) {
    if (mode == TRIANGLE_MODE_WIREFRAME) {
        draw_wireframe(v0, v1, v2, framebuffer, screenWidth, screenHeight);
    } else if (mode == TRIANGLE_MODE_BARYCENTRIC) {
        rasterize_barycentric(v0, v1, v2, framebuffer, zbuffer, screenWidth, screenHeight);
    } else if (mode == TRIANGLE_MODE_SCANLINE) {
        rasterize_scanline(v0, v1, v2, framebuffer, zbuffer, screenWidth, screenHeight);
    }
}