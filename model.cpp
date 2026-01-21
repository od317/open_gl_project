#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>  // Add this for std::transform
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Helper function declarations (add these at the top)
std::string getFileName(const std::string& path);
unsigned int createDefaultTexture();

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader) {
    // Bind textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = textures[i].type;

        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Model::Model(const std::string& path) {
    std::cout << "\n=== LOADING MODEL: " << path << " ===" << std::endl;
    loadModel(path);

    // Print summary
    std::cout << "\n=== MODEL LOAD SUMMARY ===" << std::endl;
    std::cout << "Total meshes: " << meshes.size() << std::endl;

    unsigned int totalTextures = 0;
    for (const auto& mesh : meshes) {
        totalTextures += mesh.textures.size();
    }
    std::cout << "Total textures: " << totalTextures << std::endl;

    if (meshes.empty()) {
        throw std::runtime_error("No meshes loaded from: " + path);
    }
}

void Model::Draw(Shader& shader) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader);
    }
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // Process all the node's meshes
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // Then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Positions
        vertex.Position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        // Normals
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }

        // Texture coordinates
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // **IMPORTANT: Improved material processing**
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // 1. Try to get material name
        aiString matName;
        if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, matName)) {
            std::cout << "  Material: " << matName.C_Str() << std::endl;
        }

        // 2. Get diffuse color
        aiColor3D diffuseColor(0.f, 0.f, 0.f);
        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor)) {
            std::cout << "  Diffuse color: ("
                << diffuseColor.r << ", "
                << diffuseColor.g << ", "
                << diffuseColor.b << ")" << std::endl;
        }

        // 3. Load textures with better path handling
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // 4. Also try other texture types
        std::vector<Texture> specularMaps = loadMaterialTextures(material,
            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // If no textures found, create a default color
        if (textures.empty()) {
            std::cout << "  No textures found for this material." << std::endl;
        }
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        // Check if texture was loaded before
        bool skip = false;
        for (unsigned int j = 0; j < textures.size(); j++) {
            if (std::strcmp(textures[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
        }
    }

    return textures;
}

// ====== HELPER FUNCTIONS ======

// Helper function to get just the filename from a path
std::string getFileName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return path;
}

// Create a default texture if none found
unsigned int createDefaultTexture() {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // Create 2x2 checkerboard pattern
    unsigned char data[] = {
        255, 0, 0, 255,     0, 255, 0, 255,
        0, 255, 0, 255,     255, 0, 0, 255
    };

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return textureID;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory) {
    std::string filename = std::string(path);

    std::cout << "  Looking for texture: " << filename << std::endl;

    // Try different paths
    std::vector<std::string> tryPaths;

    // 1. Original path relative to model
    tryPaths.push_back(directory + "/" + filename);

    // 2. Just the filename (might be in same directory)
    tryPaths.push_back(directory + "/" + getFileName(filename));

    // 3. Check common subdirectories for car models
    tryPaths.push_back(directory + "/car/" + filename);
    tryPaths.push_back(directory + "/car/" + getFileName(filename));

    tryPaths.push_back(directory + "/skinhp/" + filename);
    tryPaths.push_back(directory + "/skinhp/" + getFileName(filename));

    // Try skin00 - skin07 folders
    for (int i = 0; i <= 7; i++) {
        std::string skinFolder = "/skin0" + std::to_string(i) + "/";
        tryPaths.push_back(directory + skinFolder + filename);
        tryPaths.push_back(directory + skinFolder + getFileName(filename));
    }

    // 4. Try without any path modifications
    tryPaths.push_back(filename);

    unsigned int textureID = 0;

    for (const auto& tryPath : tryPaths) {
        std::cout << "    Trying: " << tryPath << std::endl;

        int width, height, nrComponents;
        unsigned char* data = stbi_load(tryPath.c_str(), &width, &height, &nrComponents, 0);

        if (data) {
            std::cout << "      SUCCESS! Loaded texture: " << tryPath
                << " (" << width << "x" << height << ")" << std::endl;

            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            else {
                stbi_image_free(data);
                continue;
            }

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
            return textureID;
        }
        else {
            std::cout << "      Failed to load." << std::endl;
        }
    }

    std::cout << "ERROR: Could not load texture: " << filename << std::endl;
    std::cout << "Creating default texture instead." << std::endl;

    // Create a default checkerboard texture
    return createDefaultTexture();
}