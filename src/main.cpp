#include <GL/gl.h>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengles2_gl2.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <glm/vec3.hpp>
#include <iostream>
#include <ostream>
#include <utility>

#define NUM_OF_SUBSYSTEMS 3
#define PROGRAM_NAME "RedStar"
#define PROGRAM_VERSION "0.1.0"

static int winWidth = 1280;
static int winHeight = 720;
static SDL_Window *programWin = NULL;
static SDL_Renderer *programRenderer = NULL;
static SDL_GLContext gContext = NULL;
static GLuint gProgramID = 0;

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
const std::pair<unsigned int, const char *> SUBSYSTEMS[NUM_OF_SUBSYSTEMS]{
    {SDL_INIT_VIDEO, "VIDEO"}, {SDL_INIT_AUDIO, "AUDIO"}};

// Initializes all subsystems declared in the SUBSYSTEMS pair array
// Will return false if any subsystem fails, returns true otherwise
bool startProgram() {
  SDL_SetAppMetadata(PROGRAM_NAME, PROGRAM_VERSION, NULL);

  // Start all subsystems inorder
  for (int i = 0; i < NUM_OF_SUBSYSTEMS; i++) {
    if (SDL_InitSubSystem(SUBSYSTEMS[i].first) == false) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s SUBSYSTEM FAILED TO INIT: %s\n",
                   SUBSYSTEMS[i].second, SDL_GetError());
      return false;
    }
  }

  // Init OpenGL stuff
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  /*SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Anti ALiasing 4x*/

  programWin =
      SDL_CreateWindow(PROGRAM_NAME, winWidth, winHeight, SDL_WINDOW_OPENGL);
  if (programWin == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "THE PROGRAM WINDOW FAILED TO INIT: %s\n", SDL_GetError());
    return false;
  }

  gContext = SDL_GL_CreateContext(programWin);
  if (gContext == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "OpenGL CONTEXT COULD NOT BE CREATED\n");
    return false;
  }

  // VSync: 0 for off, 1 for on, -1 for adaptive
  SDL_GL_SetSwapInterval(1);
  gProgramID = glCreateProgram();
  return true;
}

void endProgram() {
  // Close things before exiting
  SDL_GL_DestroyContext(gContext);
  SDL_DestroyRenderer(programRenderer);
  SDL_DestroyWindow(programWin);
  SDL_Quit();
}

int main(int argc, char *argv[]) {
  // If any subsystem fails to initialize, end the program
  if (!startProgram()) {
    endProgram();
  }

  SDL_Event event;
  bool finished = false;

  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  // fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  // link shaders
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      -0.5f, -0.5f, 0.0f, // left
      0.5f,  -0.5f, 0.0f, // right
      0.0f,  0.5f,  0.0f  // top
  };

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and
  // then configure vertex attributes(s).
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other VAOs requires a
  // call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
  // VBOs) when it's not directly necessary.
  glBindVertexArray(0);
  while (!finished) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_KEY_DOWN &&
          event.key.scancode == SDL_SCANCODE_ESCAPE) {
        finished = true;
      }

      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glUseProgram(shaderProgram);
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 3);

      SDL_GL_SwapWindow(programWin);
    }
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  endProgram();
}
