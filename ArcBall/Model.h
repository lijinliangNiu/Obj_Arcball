#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

class Model
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    bool gammaCorrection;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    void loadModel(string const &path){
        objl::Loader Loader;
        bool loadout = Loader.LoadFile(path);

        if (loadout) {
            for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
                objl::Mesh curMesh = Loader.LoadedMeshes[i];
                Mesh* mesh = new Mesh(curMesh.Vertices, curMesh.Indices);

                meshes.push_back(*mesh);
            }
        }
        else{
            cout << "Failed to Load File. May have failed to find it or it was not an .obj file.\n";
        }
    }
};
#endif
