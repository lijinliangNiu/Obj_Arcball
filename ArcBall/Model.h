#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Mesh.h"
#include "Shader.h"
#include "ObjLoader.h"
#include "StlLoader.h"
#include "3DSLoader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory);

class Model {
public:
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model(string const &path, bool gamma = false) : gammaCorrection(gamma) {
        directory = path.substr(0, path.find_last_of('/'));
        string ext = path.substr(path.size() - 4, 4);
        transform(ext.begin(), ext.end(), ext.begin(), tolower);
        if (ext == ".obj") {
            loadObj(path);
        }
        else if (ext == ".stl") {
            Mesh mesh = loadStl(path);
            this->meshes.push_back(mesh);
        }
        else if (ext == ".3ds") {
            load3ds(path);
        }
        else {
            cout << "cannot find the file or the file is unsupported" << endl;
        }
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

    Mesh objl2Mesh(ObjMesh objl_mesh) {
        Material material = objl_mesh.ObjMeshMaterial;
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

    void loadObj(string const &path) {
        ObjLoader Loader(path);
        for (int i = 0; i < Loader.LoadedObjMeshes.size(); i++) {
            ObjMesh curMesh = Loader.LoadedObjMeshes[i];
            this->meshes.push_back(objl2Mesh(curMesh));
        }
    }

    Mesh _3ds2mesh(Simple3DS::Object* obj, vector<Texture> textures) {
        vector<Vertex> Vertices;
        vector<unsigned int> Indices;
        for (int i = 0; i < obj->getNumOfTriangles(); i++) {
            Simple3DS::Triangle* face = obj->getTriangle(i);
            for (int j = 0; j < 3; j++) {
                Vertex vertex;
                Simple3DS::Vertex* v = obj->getVertex(face->index[j]);
                Simple3DS::TextureCoord* tc = obj->getTextureCoord(face->index[j]);
                Simple3DS::Normal* n = obj->getNormal(face->index[j]);
                vertex.Position.x = v->x;
                vertex.Position.y = v->y;
                vertex.Position.z = v->z;
                vertex.TexCoords.x = tc->u;
                vertex.TexCoords.y = tc->v;
                vertex.Normal.x = n->nx;
                vertex.Normal.y = n->ny;
                vertex.Normal.z = n->nz;
                Vertices.push_back(vertex);
            }
        }
        for (int i = 0; i < obj->getNumOfTriangles() * 3; i++) {
            Indices.push_back(i);
        }
        return Mesh(Vertices, Indices, textures);
    }

    Texture material2Texture(Simple3DS::Material* material, string type) {
        Texture texture;
        texture.id = TextureFromFile(material->getFileName().c_str(), this->directory);
        texture.type = type;
        texture.path = material->getFileName().c_str();
        return texture;
    }
    void load3ds(string const& path) {
        // second argument determines if the Y & Z axis should be swapped.
        Simple3DS::Model3DS model(path,true);  
        vector<Texture> textures;
        for (int i = 0; i < model.data.getNumOfMaterials(); i++) {
            Texture texture = material2Texture(model.data.getMaterial(i), "texture_diffuse");
            textures.push_back(texture);
        }
        for (int i = 0; i < model.data.getNumOfObjects(); i++) {
            Simple3DS::Object* obj = model.data.getObject(i);
            this->meshes.push_back(_3ds2mesh(obj, textures));
        }
    }

    Mesh loadStl(string const &path) {
        STLreader* stlLoader = new STLreader(path);
        std::vector<glm::vec3> vertices = stlLoader->getVertices();

        vector<Vertex> mesh_vertices;
        for (const auto& vertex : vertices) {
            Vertex mesh_vertex;
            mesh_vertex.Position = vertex;
            mesh_vertex.Normal = glm::vec3(0.0f);
            mesh_vertex.TexCoords = glm::vec3(0.0f);
            mesh_vertices.push_back(mesh_vertex);

        }
        vector<unsigned int> mesh_indices;
        for (unsigned int i = 0; i < stlLoader->getNumberOfFaces() * 3; i++) {
            mesh_indices.push_back(i);
        }
        vector<Texture> textures;
        return Mesh(mesh_vertices, mesh_indices, textures);
    }
};

unsigned int TextureFromFile(const char *path, const string &directory) {
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
        stbi_image_free(data);
    }

    return textureID;
}