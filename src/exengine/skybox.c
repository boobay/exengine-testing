#include "skybox.h"
#include "shader.h"
#include "texture.h"
#include <stdlib.h>
#include <string.h>

GLuint skybox_shader, skybox_vao, skybox_vbo;

void skybox_init()
{
  skybox_shader = shader_compile("data/skyshader.vs", "data/skyshader.fs", NULL);

  // vao for skybox
  glGenVertexArrays(1, &skybox_vao);
  glGenBuffers(1, &skybox_vbo);
  glBindVertexArray(skybox_vao);

  // vertices
  glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices[0], GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, (GLvoid*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

skybox_t* skybox_new(const char *tex_name)
{
  skybox_t *s = malloc(sizeof(skybox_t));
  s->shader = skybox_shader;

  printf("Loading skybox %s\n", tex_name);

  // gen texture
  glGenTextures(1, &s->texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, s->texture);

  // load textures
  for (int i=0; i<6; i++) {
    // file suffix
    char *suffix[6] = {"_right.png", "_left.png",
                       "_top.png",   "_bottom.png",
                       "_back.png",  "_front.png"};
    char name[strlen(tex_name)+strlen(suffix[i])];

    // append suffix to texture name
    strcpy(name, tex_name);
    strcpy(&name[strlen(tex_name)], suffix[i]);

    // load texture
    texture_t *tex = texture_load(name, 1);
    if (tex == NULL) {
      printf("Failed creating skybox\n");
      return NULL;
    }

    // set texture
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
      0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tex->data[0]);

    free(tex->data);
    free(tex);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  
  return s;
}

void skybox_draw(skybox_t *s)
{
  glDepthMask(GL_FALSE);
  glUseProgram(s->shader);
  glClear(GL_COLOR_BUFFER_BIT);

  // render skybox
  glBindVertexArray(skybox_vao);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP, s->texture);
  glCullFace(GL_NONE);
  glUniform1i(glGetUniformLocation(s->shader, "u_skybox"), 3);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glDepthMask(GL_TRUE);
}

void skybox_destroy(skybox_t *s)
{
  glDeleteTextures(1, &s->texture);
  free(s);
}