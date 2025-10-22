#include "Primitives.h"
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Mesh Primitives::CreateTriangle()
{
    Mesh mesh;

    mesh.num_vertices = 3;
    mesh.vertices = new float[9] {
        -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
        };

    mesh.texCoords = new float[6] {
        0.0f, 0.0f,
            1.0f, 0.0f,
            0.5f, 1.0f
        };

    mesh.num_indices = 3;
    mesh.indices = new unsigned int[3] { 0, 1, 2 };

    return mesh;
}

Mesh Primitives::CreateCube()
{
    Mesh mesh;

    mesh.num_vertices = 24;
    mesh.vertices = new float[72] {
        // Front face
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
            // Back face
            0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,
            // Left face
            -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f,
            // Right face
            0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
            // Top face
            -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f
        };

    mesh.texCoords = new float[48];
    // All faces have the same UVs
    for (int i = 0; i < 6; i++) {
        mesh.texCoords[i * 8 + 0] = 0.0f; mesh.texCoords[i * 8 + 1] = 0.0f;
        mesh.texCoords[i * 8 + 2] = 1.0f; mesh.texCoords[i * 8 + 3] = 0.0f;
        mesh.texCoords[i * 8 + 4] = 1.0f; mesh.texCoords[i * 8 + 5] = 1.0f;
        mesh.texCoords[i * 8 + 6] = 0.0f; mesh.texCoords[i * 8 + 7] = 1.0f;
    }

    mesh.num_indices = 36;
    mesh.indices = new unsigned int[36] {
        0, 1, 2, 2, 3, 0,      // Front
            4, 5, 6, 6, 7, 4,      // Back
            8, 9, 10, 10, 11, 8,   // Left
            12, 13, 14, 14, 15, 12, // Right
            16, 17, 18, 18, 19, 16, // Top
            20, 21, 22, 22, 23, 20  // Bottom
        };

    return mesh;
}

Mesh Primitives::CreatePyramid()
{
    Mesh mesh;

    mesh.num_vertices = 18;  
    mesh.vertices = new float[54] {  
        // Front face (triángulo)
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 0.0f,
            // Right face
            0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.0f, 0.5f, 0.0f,
            // Back face
            0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.5f, 0.0f,
            // Left face
            -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 0.0f,
            // Bottom face (2 triangles = 6 vertices)
            -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f
        };

    mesh.texCoords = new float[36] {  
        // Front
        0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
            // Right
            0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
            // Back
            0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
            // Left
            0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
            // Bottom
            0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
        };

    mesh.num_indices = 18;
    mesh.indices = new unsigned int[18] {
        0, 1, 2,       // Front
            3, 4, 5,       // Right
            6, 7, 8,       // Back
            9, 10, 11,     // Left
            12, 13, 14,    // Bottom triangle 1
            15, 16, 17     // Bottom triangle 2
        };

    return mesh;
}
Mesh Primitives::CreatePlane(float width, float height)
{
    Mesh mesh;

    mesh.num_vertices = 4;
    float halfW = width * 0.5f;
    float halfH = height * 0.5f;

    mesh.vertices = new float[12] {
        -halfW, 0.0f, -halfH,
            halfW, 0.0f, -halfH,
            halfW, 0.0f, halfH,
            -halfW, 0.0f, halfH
        };

    mesh.texCoords = new float[8] {
        0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

    mesh.num_indices = 6;
    mesh.indices = new unsigned int[6] {
        0, 1, 2,
            2, 3, 0
        };

    return mesh;
}

Mesh Primitives::CreateSphere(float radius, unsigned int rings, unsigned int sectors)
{
    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);

    for (unsigned int r = 0; r < rings; r++)
    {
        for (unsigned int s = 0; s < sectors; s++)
        {
            float const y = sin(-M_PI / 2 + M_PI * r * R);
            float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
            float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);

            texCoords.push_back(s * S);
            texCoords.push_back(r * R);
        }
    }

    for (unsigned int r = 0; r < rings - 1; r++)
    {
        for (unsigned int s = 0; s < sectors - 1; s++)
        {
            indices.push_back(r * sectors + s);
            indices.push_back(r * sectors + (s + 1));
            indices.push_back((r + 1) * sectors + (s + 1));

            indices.push_back(r * sectors + s);
            indices.push_back((r + 1) * sectors + (s + 1));
            indices.push_back((r + 1) * sectors + s);
        }
    }

    Mesh mesh;
    mesh.num_vertices = vertices.size() / 3;
    mesh.vertices = new float[vertices.size()];
    memcpy(mesh.vertices, vertices.data(), vertices.size() * sizeof(float));

    mesh.texCoords = new float[texCoords.size()];
    memcpy(mesh.texCoords, texCoords.data(), texCoords.size() * sizeof(float));

    mesh.num_indices = indices.size();
    mesh.indices = new unsigned int[indices.size()];
    memcpy(mesh.indices, indices.data(), indices.size() * sizeof(unsigned int));

    return mesh;
}

Mesh Primitives::CreateCylinder(float radius, float height, unsigned int segments)
{
    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    float halfHeight = height * 0.5f;

    // Top center
    vertices.push_back(0.0f);
    vertices.push_back(halfHeight);
    vertices.push_back(0.0f);
    texCoords.push_back(0.5f);
    texCoords.push_back(0.5f);

    // Bottom center
    vertices.push_back(0.0f);
    vertices.push_back(-halfHeight);
    vertices.push_back(0.0f);
    texCoords.push_back(0.5f);
    texCoords.push_back(0.5f);

    // Top and bottom circle vertices
    for (unsigned int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * i / segments;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        // Top circle
        vertices.push_back(x);
        vertices.push_back(halfHeight);
        vertices.push_back(z);
        texCoords.push_back((float)i / segments);
        texCoords.push_back(0.0f);

        // Bottom circle
        vertices.push_back(x);
        vertices.push_back(-halfHeight);
        vertices.push_back(z);
        texCoords.push_back((float)i / segments);
        texCoords.push_back(1.0f);
    }

    // Top cap
    for (unsigned int i = 0; i < segments; i++)
    {
        indices.push_back(0);
        indices.push_back(2 + i * 2);
        indices.push_back(2 + (i + 1) * 2);
    }

    // Bottom cap
    for (unsigned int i = 0; i < segments; i++)
    {
        indices.push_back(1);
        indices.push_back(3 + (i + 1) * 2);
        indices.push_back(3 + i * 2);
    }

    // Side faces
    for (unsigned int i = 0; i < segments; i++)
    {
        unsigned int topCurrent = 2 + i * 2;
        unsigned int bottomCurrent = 3 + i * 2;
        unsigned int topNext = 2 + (i + 1) * 2;
        unsigned int bottomNext = 3 + (i + 1) * 2;

        indices.push_back(topCurrent);
        indices.push_back(bottomCurrent);
        indices.push_back(topNext);

        indices.push_back(topNext);
        indices.push_back(bottomCurrent);
        indices.push_back(bottomNext);
    }

    Mesh mesh;
    mesh.num_vertices = vertices.size() / 3;
    mesh.vertices = new float[vertices.size()];
    memcpy(mesh.vertices, vertices.data(), vertices.size() * sizeof(float));

    mesh.texCoords = new float[texCoords.size()];
    memcpy(mesh.texCoords, texCoords.data(), texCoords.size() * sizeof(float));

    mesh.num_indices = indices.size();
    mesh.indices = new unsigned int[indices.size()];
    memcpy(mesh.indices, indices.data(), indices.size() * sizeof(unsigned int));

    return mesh;
}