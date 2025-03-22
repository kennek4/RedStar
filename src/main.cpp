#include "shader.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengles2_gl2.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <cstddef>
#include <glm/vec3.hpp>
#include <iostream>
#include <string>
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

  Shader triangleShader("shaders/vert/vertex.vert", "shaders/frag/shader.frag");

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      // positions          // colors           // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and
  // then configure vertex attributes(s).
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Vertex position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // The colour attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Generating texture

  // Generating texture
  unsigned int texture;
  glGenTextures(1, &texture);

  // All GL_TEXTURE_2D operations will now use this texture
  glBindTexture(GL_TEXTURE_2D, texture);

  // Sets how the texture is wrapped for the S and T axis
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // Sets the texture sampling to be nearest when minifying a texture, and
  // linear when upscaling (magnifying a texture)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Get curr working dir
  std::string texturePath = SDL_GetCurrentDirectory();
  texturePath.append("src/textures/snoopy-esnupi.png");

  /**
   * If above path to the image file is valid
   * we will use a SDL_Surface to "feed" the glTexImage2D function
   * with the necessary pixels to create an OpenGL texture.
   */
  SDL_Surface *esnupi = IMG_Load(texturePath.c_str());
  if (esnupi == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "FAILED TO LOAD IMAGE");
    endProgram();
  }

  SDL_FlipSurface(esnupi, SDL_FLIP_VERTICAL);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, esnupi->w, esnupi->h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, esnupi->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);

  SDL_DestroySurface(esnupi);

  float hOffSet = 0.0f;
  float vOffSet = 0.0f;

  while (!finished) {
    while (SDL_PollEvent(&event)) {

      if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.scancode) {
        case SDL_SCANCODE_ESCAPE:
          finished = true;
          break;

        case SDL_SCANCODE_W:
          vOffSet += 0.1f;
          break;

        case SDL_SCANCODE_A:
          hOffSet -= 0.1f;
          break;

        case SDL_SCANCODE_S:
          vOffSet -= 0.1f;
          break;

        case SDL_SCANCODE_D:
          hOffSet += 0.1f;
          break;

        default:
          break;
        }

        triangleShader.setFloat("hOffSet", hOffSet);
        triangleShader.setFloat("vOffSet", vOffSet);
      }

      // Background colour
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glBindTexture(GL_TEXTURE_2D, texture);

      triangleShader.use();
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      SDL_GL_SwapWindow(programWin);
    }
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  endProgram();
}
