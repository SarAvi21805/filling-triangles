Vertex vertex_shader(Vertex vertex, Uniforms uniforms) {
    // Transformar la posición del vértice usando matrices (Model, View, Projection)
    Vertex transformed = vertex;
    // Aplicar transformación matricial a transformed.position
    return transformed;
}

void primitive_assembly(Vertex* vertices, int vertexCount, Triangle* triangles) {
    // Agrupa cada 3 vértices para formar un triángulo
}

// Función para calcular coordenadas baricéntricas de un punto P respecto a un triángulo (A, B, C)
Vector3 barycentric(Vector2 A, Vector2 B, Vector2 C, Vector2 P) {
    Vector2 AB = { B.x - A.x, B.y - A.y };
    Vector2 AC = { C.x - A.x, C.y - A.y };
    Vector2 AP = { P.x - A.x, P.y - A.y };

    float denom = AB.x * AC.y - AB.y * AC.x;
    if (denom == 0) return (Vector3){ -1, -1, -1 }; // Evitar división por cero

    float v = (AP.x * AC.y - AP.y * AC.x) / denom;
    float w = (AB.x * AP.y - AB.y * AP.x) / denom;
    float u = 1.0f - v - w;

    return (Vector3){ u, v, w };
}

void rasterize_triangle(Vertex v0, Vertex v1, Vertex v2, Color* framebuffer, int screenWidth, int screenHeight) {
    // 1. Encontrar la caja delimitadora (Bounding Box) del triángulo para optimizar
    // 2. Recorrer cada píxel dentro de la caja delimitadora
    // 3. Evaluar si el píxel está dentro usando las coordenadas baricéntricas (u >= 0, v >= 0, w >= 0)
    // 4. Pintar el píxel en el framebuffer si cumple la condición
}

Color fragment_shader(Fragment frag) {
    // Aplicar efectos de color o texturas por píxel
    return frag.color;
}