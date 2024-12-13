#include "Terrain.h"

#include <vector>
#include <iostream>

#include "stb_image.h"
#include "CommonValues.h"
#include "imgui.h"
#include "ImGuizmo.h"

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

Terrain::Terrain()
{
    heightScale = 64.f;
    heightShift = -30.f;
}

void Terrain::LoadTerrain(const char* fileLoc)
{
    // shader setup
    terrainShader = new Shader();
    terrainShader->CreateFromFiles("Shaders/Terrain.vert", "Shaders/Terrain.frag", nullptr,
                                "Shaders/Terrain.tesc", "Shaders/Terrain.tese");
    // -------------------------------------------------------------------------
    // height map 로드
    heightMap = new Texture(fileLoc);
    heightMap->LoadTexture(4, true);
    
    width = heightMap->GetWidth();
    height = heightMap->GetHeight();
    nChannels = heightMap->GetBitDepth();

    unsigned char* heightData = heightMap->GetTexData();

    heights.resize(width);
    for (int i = 0; i < heights.size(); i++)
        heights[i].resize(height);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // retrieve texel for (i,j) tex coord
            unsigned char* texel = heightData + (j + width * i) * nChannels;
            // raw height at coordinate
            GLfloat y = texel[0] / 255.f; // 값의 범위를 [0, 1]로

            heights[j][i] = y;
        }
    }

    stbi_image_free(heightData);
    // -------------------------------------------------------------------------
    // diffuse map 로드    
    diffuseMap = new Texture("Textures/aerial_grass_rock_4k.blend/aerial_grass_rock_diff_4k.jpg");
    diffuseMap->LoadTexture(4);
    // -------------------------------------------------------------------------

    // vertex generation
    rez = 100;
    for (unsigned i = 0; i < rez; i++)
    {
        for (unsigned j = 0; j < rez; j++)
        {
            Vertex vertexA;
            vertexA.Position.x = -width / 2.0f + width * i / (float)rez;
            vertexA.Position.y = 0.f;
            vertexA.Position.z = -height / 2.0f + height * j / (float)rez;
            vertexA.TexCoords.x = i / (float)rez;
            vertexA.TexCoords.y = j / (float)rez;
            vertices.push_back(vertexA);

            Vertex vertexB;
            vertexB.Position.x = -width / 2.0f + width * (i + 1) / (float)rez;
            vertexB.Position.y = 0.f;
            vertexB.Position.z = -height / 2.0f + height * j / (float)rez;
            vertexB.TexCoords.x = (i + 1) / (float)rez;
            vertexB.TexCoords.y = j / (float)rez;
            vertices.push_back(vertexB);

            Vertex vertexC;
            vertexC.Position.x = -width / 2.0f + width * i / (float)rez;
            vertexC.Position.y = 0.f;
            vertexC.Position.z = -height / 2.0f + height * (j + 1) / (float)rez;
            vertexC.TexCoords.x = i / (float)rez;
            vertexC.TexCoords.y = (j + 1) / (float)rez;
            vertices.push_back(vertexC);

            Vertex vertexD;
            vertexD.Position.x = -width / 2.0f + width * (i + 1) / (float)rez;
            vertexD.Position.y = 0.f;
            vertexD.Position.z = -height / 2.0f + height * (j + 1) / (float)rez;
            vertexD.TexCoords.x = (i + 1) / (float)rez;
            vertexD.TexCoords.y = (j + 1) / (float)rez;
            vertices.push_back(vertexD);
        }
    }

    std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
    std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(0);

    // texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void Terrain::DrawTerrain(glm::mat4 viewMat, glm::mat4 projMat)
{
    terrainShader->UseShader();

    heightMap->UseTexture(GL_TEXTURE0);
    terrainShader->setInt("heightSampler", 0);

    diffuseMap->UseTexture(GL_TEXTURE1);
    terrainShader->setInt("diffuseSampler", 1);
    
    glm::mat4 modelMat = GetModelMat();
    glm::mat4 PVM = projMat * viewMat * modelMat;
    loc_PVM = terrainShader->GetPVMLoc();
    glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));

    terrainShader->setMat4("modelViewMat", modelMat * viewMat);
    terrainShader->setMat4("modelMat", modelMat);

    glm::vec2 texelSize(1.f/width, 1.f/height);
    terrainShader->setVec2("texelSize", texelSize);

    terrainShader->setFloat("HEIGHT_SCALE", heightScale);
    terrainShader->setFloat("HEIGHT_SHIFT", heightShift);

    glBindVertexArray(VAO);
    glDrawArrays(GL_PATCHES, 0, 4*rez*rez);

    glBindVertexArray(0);
}

void Terrain::UseShader()
{
    terrainShader->UseShader();
}

std::string Terrain::GetName()
{
    return "Terrain";
}

void Terrain::ShowProperties()
{
    ImGui::Text("Transform");
    ImGui::InputFloat3("Position", &position[0]);
    
    ImGui::Text("Height");
    ImGui::SliderFloat("HeightScale", &heightScale, 0.f, 100.f);
    ImGui::SliderFloat("HeightShift", &heightShift, -100.f, 100.f);
}

glm::mat4 Terrain::GetModelMat()
{
    return glm::translate(glm::mat4(1.f), position);
}

void Terrain::UpdateTransform(glm::mat4 newModelMat)
{
    glm::vec3 T;
    glm::vec3 R;
    glm::vec3 S;
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newModelMat), &T[0], &R[0], &S[0]);

    position = T;
}

GLfloat Terrain::GetHeight(float worldX, float worldZ)
{
    float terrainX = worldX + width/2.f;
    float terrainZ = worldZ + height/2.f;

    int gridX = floorf(terrainX);
    int gridZ = floorf(terrainZ);

    if(gridX >= width-1 || gridZ >= height-1 || gridX < 0 || gridZ < 0)
        return 0.f;

    float xCoord = fmod(terrainX, gridX);
    float zCoord = fmod(terrainZ, gridZ);

    float h00 = heights[gridX][gridZ];
    float h01 = heights[gridX+1][gridZ];
    float h10 = heights[gridX][gridZ+1];
    float h11 = heights[gridX+1][gridZ+1];

    float h0 = glm::mix(h00, h01, xCoord);
    float h1 = glm::mix(h10, h11, xCoord);
    float h = glm::mix(h0, h1, zCoord);

    return h * heightScale + heightShift;
}

Terrain::~Terrain()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
