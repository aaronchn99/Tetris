#ifndef CUBE_H
#define CUBE_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stddef.h>
#include "cross_product.h"

float vertices[8][3] = {
  {0.0f, 0.0f, 0.0f},
  {1.0f, 0.0f, 0.0f},
  {1.0f, 1.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 1.0f},
  {1.0f, 0.0f, 1.0f},
  {1.0f, 1.0f, 1.0f},
  {0.0f, 1.0f, 1.0f}
};
unsigned faces[6][4] = {
  {4, 5, 6, 7},
  {5, 1, 2, 6},
  {0, 1, 2, 3},
  {0, 4, 7, 3},
  {7, 6, 2, 3},
  {4, 5, 1, 0}
};

static float WHITE[3] = {1.0f, 1.0f, 1.0f};
static float* border_colour = WHITE;
static float border_width = 2.0f;

size_t cube_handle;
size_t border_handle;

// For set cubes in the game board
class Cube{
private:
  float *colour;
public:
  Cube(float color[3]) : colour(color) {};
  void display(int x, int y){
    glPushMatrix();
      glTranslatef(x, y, 0.0f);
      glColor3fv(border_colour);
      glLineWidth(border_width);
      glCallList(border_handle);

      glColor3fv(colour);
      glCallList(cube_handle);
    glPopMatrix();
  };
};

size_t make_cube(){
  size_t handle = glGenLists(1);
  glNewList(handle, GL_COMPILE);
  glBegin(GL_QUADS);
    for (unsigned i=0; i<6; i++){
      float v1[3] = {vertices[faces[i][0]][0] - vertices[faces[i][1]][0],
                    vertices[faces[i][0]][1] - vertices[faces[i][1]][1],
                    vertices[faces[i][0]][2] - vertices[faces[i][1]][2]};
      float v2[3] = {vertices[faces[i][0]][0] - vertices[faces[i][2]][0],
                    vertices[faces[i][0]][1] - vertices[faces[i][2]][1],
                    vertices[faces[i][0]][2] - vertices[faces[i][2]][2]};
      float n[3];
      cross_product(v1, v2, n);
      // cout << "(" << v1[0] << "," << v1[1] << "," << v1[2] << ")" << "\t"
      //       << "(" << v2[0] << "," << v2[1] << "," << v2[2] << ")" << "\t"
      //       << "(" << n[0] << "," << n[1] << "," << n[2] << ")" << "\t"
      //       << endl;
      glNormal3fv(n);
      for (unsigned j=0; j<4; j++){
        glVertex3fv(vertices[faces[i][j]]);
      }
    }
  glEnd();
  glEndList();

  return handle;
}

size_t make_cube_frame(){
  size_t handle = glGenLists(1);
  glNewList(handle, GL_COMPILE);
    for (unsigned i=0; i<6; i++){
      glBegin(GL_LINE_LOOP);
        for (unsigned j=0; j<4; j++){
          glVertex3fv(vertices[faces[i][j]]);
        }
      glEnd();
    }
  glEndList();

  return handle;
}

void init_cubes(){
  cube_handle = make_cube();
  border_handle = make_cube_frame();
}

#endif
