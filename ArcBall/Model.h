#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Mesh.h"
#include "Shader.h"
#include "ObjLoader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model {
public:
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model(string const &path, bool gamma = false) : gammaCorrection(gamma) {
        loadModel(path);
    }

    void Draw(Shader shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    Texture material2Texture(string path, string type) {
        Texture texture;
        texture.id = TextureFromFile(path.c_str(), this->directory);
        texture.type = type;
        texture.path = path.c_str();
        return texture;
    }

    Mesh objl2Mesh(objl::Mesh objl_mesh) {
        objl::Material material = objl_mesh.MeshMaterial;
        vector<Texture> textures;

        Texture diffuse_texture = material2Texture(material.map_Kd, "texture_diffuse");
        textures.push_back(diffuse_texture);

        Texture specular_texture = material2Texture(material.map_Ks, "texture_specular");
        textures.push_back(specular_texture);

        Texture ambient_texture = material2Texture(material.map_Ka, "texture_height");
        textures.push_back(ambient_texture);

        Texture bump_texture = material2Texture(material.map_bump, "texture_normal");
        textures.push_back(bump_texture);

        return Mesh(objl_mesh.Vertices, objl_mesh.Indices, textures);
    }

    void loadModel(string const &path) {
        objl::Loader Loader;
        bool loadout = Loader.LoadFile(path);

        directory = path.substr(0, path.find_last_of('/'));

        if (loadout) {
            for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
                objl::Mesh curMesh = Loader.LoadedMeshes[i];
                this->meshes.push_back(objl2Mesh(curMesh));
            }
        }
        else {
            cout << "Failed to Load File. May have failed to find it or it was not an .obj file.\n";
        }
    }
};

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma) {
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}