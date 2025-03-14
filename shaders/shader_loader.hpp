#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <GLES2/gl2.h>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <vector>

class ShaderLoader {
private:
  std::vector<unsigned int> vertexShaders;
  std::vector<unsigned int> fragShaders;

  // Returns the shader file provided (to its path) into a char * repr. of
  // shader source file
  void getShaderSource(const char *filePath, char *shaderSource) {
    std::string fileData;
    std::ifstream file(filePath);
    while (std::getline(file, fileData)) {
      std::cout << fileData << '\n';
    }

    file.close();

    shaderSource = fileData.data();
  };

  bool compileAllInDir(const char *filePath, const char *extension,
                       std::vector<unsigned int> *shaderVector) {
    char *shaderSource;
    unsigned int shader;
    int success;
    char infoLog[512];

    // Compile Vertex Shaders
    for (const auto &item : std::filesystem::directory_iterator(filePath)) {
      // Converting the path to const char * in the
      // subsequent lines
      std::filesystem::path fileName = item.path();
      std::cout << "filepath: " << fileName << '\n';
      //
      // Makes sure that the file is a .vert file
      if (fileName.extension() == extension) {
        std::cout << "File path is valid, getting source...\n";
        // TODO: Log current shader being compiled here
        getShaderSource(fileName.c_str(), shaderSource);
        std::cout << "Creating shader...\n";
        shader = glCreateShader(GL_VERTEX_SHADER);
        std::cout << "Compling shader...\n";
        glCompileShader(shader);

        // TODO: Log any shader compilation error here
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
          glGetShaderInfoLog(shader, 512, NULL, infoLog);
          std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                    << infoLog << std::endl;
          return false;
        }

        // Add shader to list of shaders
        vertexShaders.push_back(shader);
      }
    }

    return true;
  }

  inline void deleteShaders() {
    for (unsigned int shader : vertexShaders) {
      glDeleteShader(shader);
    }

    for (unsigned int shader : fragShaders) {
      glDeleteShader(shader);
    }
  }

  inline void attachShaders(unsigned int *shaderProgram) {
    // Attach vertex shaders
    for (unsigned int shader : vertexShaders) {
      glAttachShader(*shaderProgram, shader);
    }

    // Attach fragment shaders
    for (unsigned int shader : fragShaders) {
      glAttachShader(*shaderProgram, shader);
    }
  };

public:
  // Constructor
  ShaderLoader() { std::cout << std::filesystem::current_path() << '\n'; };

  bool compileShaders(unsigned int *shaderProgram) {
    std::cout << "Now compiling vertex shaders...\n";
    // Compile all vertex shaders in directory
    if (!compileAllInDir("shaders/vert/", ".vert", &vertexShaders)) {
      // TODO: Put error message here
      return false;
    }

    std::cout << "Now compiling fragment shaders...\n";
    // Compile all fragment shaders in directory
    if (!compileAllInDir("shaders/frag/", ".frag", &fragShaders)) {
      // TODO: Put error message here
      return false;
    }

    attachShaders(shaderProgram);
    glLinkProgram(*shaderProgram);

    int success;
    // check for linking errors
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      // TODO: Error message for linking shaders to program
      deleteShaders();
      return false;
    }

    deleteShaders();
    return true;
  };

  // Deconstructor
  ~ShaderLoader() {

  };
};

#endif // !SHADER_LOADER_H
