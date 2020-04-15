#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#define STL_ASCII 1
#define STL_BINARY 2

//memory in bytes 
#define STL_BINARY_HEADER_SIZE 80
#define STL_BINARY_TRIANGLE_SIZE 50
#define STL_BINARY_TRIANGLE_NORMAL_SIZE 12
#define STL_BINARY_TRIANGLE_VERTEX_SIZE 12
#define STL_BINARY_TRIANGLE_ATTRIBUTE_SIZE 2
#define STL_NUM_VERTEX_PER_FACE 3


#define STL_SOLID_START "solid"
#define STL_SOLID_SIZE 5
#define STL_CHECK_LINE 3

#define STL_FACE "facet"
#define STL_END_FACE "endfacet"

#define STL_VERTEX "vertex"
#define STL_NORMAL "normal"

class STLreader
{
public:
    //construct take the file name as argument
    STLreader(const std::string& filename) {
        std::ifstream inputFile(filename, std::ios::binary);
        if (!inputFile)
        {
            std::cout << "Failed to load the file: " << filename << std::endl;
            return;
        }

        //check for the format of input file 
        fileFormat = checkFormat(inputFile);

        //read the file using respective methods
        if (fileFormat == STL_ASCII)
            readASCIIstl(inputFile);
        else
            readBINARYstl(inputFile);

        inputFile.close();
    }

    int getNumberOfFaces() {
        return numberOfTriangles;
    }
    int getNumberOfVertices() {
        return numberOfvertices;
    }

    std::vector<glm::vec3>& getVertices() {
        return vertices;
    }
    std::vector<glm::vec3>& getNormals() {
        return normals;
    }

private:
    //check whether the given file is ascii or binary
    int checkFormat(std::ifstream& input) {
        //read first 5 bytes
        char format[STL_SOLID_SIZE + 1];
        input.read(format, STL_SOLID_SIZE);
        format[STL_SOLID_SIZE] = '\0';	//write the end character 

        //compare the array of character if it is solid then file format is ascii
        //otherwise it is binary
        if (strcmp(format, STL_SOLID_START) == 0)
        {
            //search for facet word
            if (checkForFacet(input))
                return STL_ASCII;
        }

        return STL_BINARY;
    }

    //check end of file to confirma ascii
    bool checkForFacet(std::ifstream& input) {
        std::string line;
        for (int i = 0; i < STL_CHECK_LINE; i++)
        {
            //check for eof
            if (std::getline(input, line))
            {
                std::string search = STL_FACE;
                size_t found = line.find(search);
                //if found facet in line: "ASCII file format cofirmed"
                if (found != std::string::npos)
                    return true;
            }
        }

        return false;
    }

    //read ascii stl file
    void readASCIIstl(std::ifstream& input) {
        //set the file get pointer to beginning
        input.seekg(0, std::ios::beg);

        //read till end of file
        while (input)
        {
            std::string data;
            input >> data;

            if (data == STL_FACE)
            {
                do
                {
                    input >> data;
                    if (data == STL_VERTEX)
                    {
                        glm::vec3 v;
                        input >> v.x >> v.y >> v.z;
                        vertices.push_back(v);
                    }
                    else if (data == STL_NORMAL)
                    {
                        glm::vec3 n;
                        input >> n.x >> n.y >> n.z;
                        normals.push_back(n);
                    }

                } while (data != STL_END_FACE);

                //one face is read
                numberOfTriangles++;
            }
        }
    }

    //read binary stl file
    void readBINARYstl(std::ifstream& input) {
        //set the file get pointer to beginning
        input.seekg(0, std::ios::beg);

        //read the header whose size is 80 bytes
        char header[STL_BINARY_HEADER_SIZE + 1];
        input.read(header, STL_BINARY_HEADER_SIZE);
        header[STL_BINARY_HEADER_SIZE] = '\0';

        //read 4bytes or 32bit unsinged int for number of triangles
        input.read((char*)&numberOfTriangles, sizeof(uint32_t));

        //read the triangles and data of each faces
        for (uint32_t i = 0; i < numberOfTriangles; i++)
        {
            //read normals which are 3 floats or 12 bytes or 48bit
            glm::vec3 n;
            input.read((char*)& n, STL_BINARY_TRIANGLE_NORMAL_SIZE);
            normals.push_back(n);

            //for each face there will be three vertices
            for (int j = 0; j < STL_NUM_VERTEX_PER_FACE; j++)
            {
                //read vertex which are 3 floats or 12 bytes or 48 bit
                glm::vec3 v;
                input.read((char*)& v, STL_BINARY_TRIANGLE_NORMAL_SIZE);
                vertices.push_back(v);
            }

            //read 2bytes or 16bit attribute count
            uint16_t attributeCount;
            input.read((char*)& attributeCount, STL_BINARY_TRIANGLE_ATTRIBUTE_SIZE);
            if (attributeCount)
                std::cout << "Alert!!!!!!!!!!! Attribute Count: " << attributeCount << std::endl;

        }
    }

private:
    //format
    int fileFormat;

    //number of facet
    uint32_t numberOfTriangles;
    int numberOfvertices;

    //storage
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
};