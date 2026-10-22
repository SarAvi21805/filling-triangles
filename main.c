#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "renderer.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Benchmark dinámico según el modo actual
void run_benchmark(Vertex v0, Vertex v1, Vertex v2, TriangleMode mode, Color* fb, float* zb, int w, int h) {
    clock_t start = clock();

    for (int i = 0; i < 100; i++) {
        rasterize_triangle(v0, v1, v2, mode, fb, zb, w, h);
    }

    clock_t end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Identificar el nombre del modo para la consola
    const char* mode_name = "Unknown";
    if (mode == TRIANGLE_MODE_WIREFRAME) mode_name = "Wireframe";
    else if (mode == TRIANGLE_MODE_BARYCENTRIC) mode_name = "Barycentric";
    else if (mode == TRIANGLE_MODE_SCANLINE) mode_name = "Scanline";

    printf("Tiempo para 100 ejecuciones (%s): %f segundos\n", mode_name, cpu_time_used);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Software Renderer - Tarea 1");
    SetTargetFPS(60);

    Color* framebuffer = (Color*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Color));
    float* zbuffer = (float*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(float));

    Image img = {
        .data = framebuffer,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };
    Texture2D texture = LoadTextureFromImage(img);

    TriangleMode current_mode = TRIANGLE_MODE_BARYCENTRIC;
    int triangle_case = 0; 

    while (!WindowShouldClose()) {
        // Controles de teclado
        if (IsKeyPressed(KEY_ONE)) current_mode = TRIANGLE_MODE_WIREFRAME;
        if (IsKeyPressed(KEY_TWO)) current_mode = TRIANGLE_MODE_BARYCENTRIC;
        if (IsKeyPressed(KEY_THREE)) current_mode = TRIANGLE_MODE_SCANLINE;
        
        if (IsKeyPressed(KEY_T)) {
            triangle_case = (triangle_case + 1) % 5;
            printf("Cambiando a caso de triángulo ID: %d\n", triangle_case);
        }

        // Limpiar buffers
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
            framebuffer[i] = BLACK;
            zbuffer[i] = 1000.0f; 
        }

        // Definición de Triángulos según el Caso
        Vertex v0, v1, v2;

        switch (triangle_case) {
            case 0: // Estándar grande
                v0 = (Vertex){ .position = {200, 100, -1}, .color = {255, 182, 193, 255} };
                v1 = (Vertex){ .position = {100, 400, -2}, .color = {230, 230, 250, 255} };
                v2 = (Vertex){ .position = {500, 450, -0.5f}, .color = {135, 206, 250, 255} };
                break;
            case 1: // Pequeño
                v0 = (Vertex){ .position = {400, 300, -1}, .color = {255, 0, 0, 255} };
                v1 = (Vertex){ .position = {420, 320, -1}, .color = {0, 255, 0, 255} };
                v2 = (Vertex){ .position = {380, 340, -1}, .color = {0, 0, 255, 255} };
                break;
            case 2: // Borde horizontal
                v0 = (Vertex){ .position = {300, 200, -1}, .color = {255, 255, 0, 255} };
                v1 = (Vertex){ .position = {200, 400, -1}, .color = {0, 255, 255, 255} };
                v2 = (Vertex){ .position = {500, 400, -1}, .color = {255, 0, 255, 255} };
                break;
            case 3: // Orden inverso
                v0 = (Vertex){ .position = {500, 450, -0.5f}, .color = {135, 206, 250, 255} };
                v1 = (Vertex){ .position = {100, 400, -2}, .color = {230, 230, 250, 255} };
                v2 = (Vertex){ .position = {200, 100, -1}, .color = {255, 182, 193, 255} };
                break;
            case 4: // Fuera del framebuffer
                v0 = (Vertex){ .position = {-50, 200, -1}, .color = {255, 100, 100, 255} };
                v1 = (Vertex){ .position = {400, -50, -1}, .color = {100, 255, 100, 255} };
                v2 = (Vertex){ .position = {900, 700, -1}, .color = {100, 100, 255, 255} };
                break;
        }

        // Ejecutar benchmark del modo activo al presionar B
        if (IsKeyPressed(KEY_B)) {
            run_benchmark(v0, v1, v2, current_mode, framebuffer, zbuffer, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        // Rasterizar triángulo con el modo activo
        rasterize_triangle(v0, v1, v2, current_mode, framebuffer, zbuffer, SCREEN_WIDTH, SCREEN_HEIGHT);

        UpdateTexture(texture, framebuffer);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(texture, 0, 0, WHITE);
        
        // Texto de controles y estado
        DrawText("Controles:", 10, 40, 20, RAYWHITE);
        DrawText("[1] Wireframe  [2] Barycentric  [3] Scanline", 10, 70, 16, LIGHTGRAY);
        DrawText(TextFormat("[T] Cambiar Triangulo (Caso actual: %d)", triangle_case), 10, 95, 16, LIGHTGRAY);
        DrawText("[B] Ejecutar Benchmark en Consola", 10, 120, 16, LIGHTGRAY);
       
        // Rasterizar el triángulo según el modo activo
        if (current_mode == TRIANGLE_MODE_WIREFRAME) {
            // Dibujar el contorno directamente con líneas de Raylib
            DrawLine((int)v0.position.x, (int)v0.position.y, (int)v1.position.x, (int)v1.position.y, v0.color);
            DrawLine((int)v1.position.x, (int)v1.position.y, (int)v2.position.x, (int)v2.position.y, v1.color);
            DrawLine((int)v2.position.x, (int)v2.position.y, (int)v0.position.x, (int)v0.position.y, v2.color);
        } else {
            rasterize_triangle(v0, v1, v2, current_mode, framebuffer, zbuffer, SCREEN_WIDTH, SCREEN_HEIGHT);
            UpdateTexture(texture, framebuffer);
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(texture);
    free(framebuffer);
    free(zbuffer);
    CloseWindow();

    return 0;
}