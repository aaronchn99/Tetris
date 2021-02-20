#ifndef TETROMINO_H
#define TETROMINO_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef CUBE_H
#include "cube.h"
#endif

// In clockwise direction
enum angles_t {
  UP, RIGHT, DOWN, LEFT
};

class Tetromino{
protected:
  int x;
  int y;
  int width;
  int height;
  int blocks[4][2];
  int angle;
  virtual void orientUp() {};
  virtual void orientRight() {};
  virtual void orientDown() {};
  virtual void orientLeft() {};
public:
  float *colour;
  virtual Tetromino* clone() const = 0;
  int getX() {return x;};
  int getY() {return y;};
  int getW() {return width;};
  int getH() {return height;};
  int *getBlock(int i) {return blocks[i];};
  void rotate(int turns) {
    angle = angle+turns;
    while (angle < 0) angle += 4;
    angle %= 4;
    switch (angle){
      case UP:
        orientUp();
        break;
      case LEFT:
        orientLeft();
        break;
      case DOWN:
        orientDown();
        break;
      case RIGHT:
        orientRight();
        break;
    }
  };
  void translate(int dx, int dy) {
    x += dx;
    y += dy;
  };
  void display(){
    glPushMatrix();
      glTranslatef(x, y, 0.0f);
      for (int i=0; i < 4; i++){
        glPushMatrix();
          glTranslatef(blocks[i][0], blocks[i][1], 0.0f);
          glColor3fv(border_colour);
          glLineWidth(border_width);
          glCallList(border_handle);
        glPopMatrix();
      }
      for (int i=0; i < 4; i++){
        glPushMatrix();
          glTranslatef(blocks[i][0], blocks[i][1], 0.0f);
          glColor3fv(colour);
          glCallList(cube_handle);
        glPopMatrix();
      }
    glPopMatrix();
  };
};

class IBlock : public Tetromino {
private:
  using Tetromino::orientUp;
  void orientUp(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 1; blocks[1][1] = 0;
    blocks[2][0] = 2; blocks[2][1] = 0;
    blocks[3][0] = 3; blocks[3][1] = 0;
    width = 4;
    height = 1;
  };
  using Tetromino::orientRight;
  void orientRight(){
    blocks[0][1] = 0; blocks[0][0] = 0;
    blocks[1][1] = 1; blocks[1][0] = 0;
    blocks[2][1] = 2; blocks[2][0] = 0;
    blocks[3][1] = 3; blocks[3][0] = 0;
    width = 1;
    height = 4;
  };
  using Tetromino::orientDown;
  void orientDown(){
    orientUp();
  };
  using Tetromino::orientLeft;
  void orientLeft(){
    orientRight();
  };
public:
  IBlock(int xpos, int ypos, float color[3]) {
    x = xpos;
    y = ypos;
    orientUp();
    angle = UP;
    colour = color;
  };
  IBlock* clone() const override{
    return new IBlock(*this);
  }
};

class LBlock : public Tetromino {
private:
  using Tetromino::orientUp;
  void orientUp(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 1; blocks[1][1] = 0;
    blocks[2][0] = 2; blocks[2][1] = 0;
    blocks[3][0] = 2; blocks[3][1] = 1;
    width = 3;
    height = 2;
  };
  using Tetromino::orientRight;
  void orientRight(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 1; blocks[1][1] = 0;
    blocks[2][0] = 0; blocks[2][1] = 1;
    blocks[3][0] = 0; blocks[3][1] = 2;
    width = 2;
    height = 3;
  };
  using Tetromino::orientDown;
  void orientDown(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 0; blocks[1][1] = 1;
    blocks[2][0] = 1; blocks[2][1] = 1;
    blocks[3][0] = 2; blocks[3][1] = 1;
    width = 3;
    height = 2;
  };
  using Tetromino::orientLeft;
  void orientLeft(){
    blocks[0][0] = 1; blocks[0][1] = 0;
    blocks[1][0] = 1; blocks[1][1] = 1;
    blocks[2][0] = 0; blocks[2][1] = 2;
    blocks[3][0] = 1; blocks[3][1] = 2;
    width = 2;
    height = 3;
  };
public:
  LBlock(int xpos, int ypos, float color[3]) {
    x = xpos;
    y = ypos;
    orientUp();
    angle = UP;
    colour = color;
  };
  LBlock* clone() const override{
    return new LBlock(*this);
  }
};

class JBlock : public Tetromino {
private:
  using Tetromino::orientUp;
  void orientUp(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 1; blocks[1][1] = 0;
    blocks[2][0] = 2; blocks[2][1] = 0;
    blocks[3][0] = 0; blocks[3][1] = 1;
    width = 3;
    height = 2;
  };
  using Tetromino::orientRight;
  void orientRight(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 0; blocks[1][1] = 1;
    blocks[2][0] = 0; blocks[2][1] = 2;
    blocks[3][0] = 1; blocks[3][1] = 2;
    width = 2;
    height = 3;
  };
  using Tetromino::orientDown;
  void orientDown(){
    blocks[0][0] = 2; blocks[0][1] = 0;
    blocks[1][0] = 0; blocks[1][1] = 1;
    blocks[2][0] = 1; blocks[2][1] = 1;
    blocks[3][0] = 2; blocks[3][1] = 1;
    width = 3;
    height = 2;
  };
  using Tetromino::orientLeft;
  void orientLeft(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 1; blocks[1][1] = 0;
    blocks[2][0] = 1; blocks[2][1] = 1;
    blocks[3][0] = 1; blocks[3][1] = 2;
    width = 2;
    height = 3;
  };

public:
  JBlock(int xpos, int ypos, float color[3]) {
    x = xpos;
    y = ypos;
    angle = UP;
    orientUp();
    colour = color;
  };
  JBlock* clone() const override{
    return new JBlock(*this);
  }
};

class ZBlock : public Tetromino {
private:
  using Tetromino::orientUp;
  void orientUp(){
    blocks[0][0] = 1; blocks[0][1] = 0;
    blocks[1][0] = 2; blocks[1][1] = 0;
    blocks[2][0] = 0; blocks[2][1] = 1;
    blocks[3][0] = 1; blocks[3][1] = 1;
    width = 3;
    height = 2;
  };
  using Tetromino::orientRight;
  void orientRight(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 0; blocks[1][1] = 1;
    blocks[2][0] = 1; blocks[2][1] = 1;
    blocks[3][0] = 1; blocks[3][1] = 2;
    width = 2;
    height = 3;
  };
  using Tetromino::orientDown;
  void orientDown(){
    orientUp();
  };
  using Tetromino::orientLeft;
  void orientLeft(){
    orientRight();
  };
public:
  ZBlock(int xpos, int ypos, float color[3]) {
    x = xpos;
    y = ypos;
    orientUp();
    angle = UP;
    colour = color;
  };
  ZBlock* clone() const override{
    return new ZBlock(*this);
  }
};

class SBlock : public Tetromino {
private:
  using Tetromino::orientUp;
  void orientUp(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 1; blocks[1][1] = 0;
    blocks[2][0] = 1; blocks[2][1] = 1;
    blocks[3][0] = 2; blocks[3][1] = 1;
    width = 3;
    height = 2;
  };
  using Tetromino::orientRight;
  void orientRight(){
    blocks[0][0] = 1; blocks[0][1] = 0;
    blocks[1][0] = 0; blocks[1][1] = 1;
    blocks[2][0] = 1; blocks[2][1] = 1;
    blocks[3][0] = 0; blocks[3][1] = 2;
    width = 2;
    height = 3;
  };
  using Tetromino::orientDown;
  void orientDown(){
    orientUp();
  };
  using Tetromino::orientLeft;
  void orientLeft(){
    orientRight();
  };
public:
  SBlock(int xpos, int ypos, float color[3]) {
    x = xpos;
    y = ypos;
    orientUp();
    angle = UP;
    colour = color;
  };
  SBlock* clone() const override{
    return new SBlock(*this);
  }
};

class TBlock : public Tetromino {
private:
  using Tetromino::orientUp;
  void orientUp(){
    blocks[0][0] = 1; blocks[0][1] = 0;
    blocks[1][0] = 0; blocks[1][1] = 1;
    blocks[2][0] = 1; blocks[2][1] = 1;
    blocks[3][0] = 2; blocks[3][1] = 1;
    width = 3;
    height = 2;
  };
  using Tetromino::orientRight;
  void orientRight(){
    blocks[0][0] = 1; blocks[0][1] = 0;
    blocks[1][0] = 0; blocks[1][1] = 1;
    blocks[2][0] = 1; blocks[2][1] = 1;
    blocks[3][0] = 1; blocks[3][1] = 2;
    width = 2;
    height = 3;
  };
  using Tetromino::orientDown;
  void orientDown(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 1; blocks[1][1] = 0;
    blocks[2][0] = 2; blocks[2][1] = 0;
    blocks[3][0] = 1; blocks[3][1] = 1;
    width = 3;
    height = 2;
  };
  using Tetromino::orientLeft;
  void orientLeft(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 0; blocks[1][1] = 1;
    blocks[2][0] = 1; blocks[2][1] = 1;
    blocks[3][0] = 0; blocks[3][1] = 2;
    width = 2;
    height = 3;
  };
public:
  TBlock(int xpos, int ypos, float color[3]) {
    x = xpos;
    y = ypos;
    orientUp();
    angle = UP;
    colour = color;
  };
  TBlock* clone() const override{
    return new TBlock(*this);
  }
};

class SqBlock : public Tetromino {
private:
  using Tetromino::orientUp;
  void orientUp(){
    blocks[0][0] = 0; blocks[0][1] = 0;
    blocks[1][0] = 1; blocks[1][1] = 0;
    blocks[2][0] = 0; blocks[2][1] = 1;
    blocks[3][0] = 1; blocks[3][1] = 1;
  };
  using Tetromino::orientRight;
  void orientRight(){
    orientUp();
  };
  using Tetromino::orientDown;
  void orientDown(){
    orientUp();
  };
  using Tetromino::orientLeft;
  void orientLeft(){
    orientUp();
  };
public:
  SqBlock(int xpos, int ypos, float color[3]) {
    x = xpos;
    y = ypos;
    width = 2;
    height = 2;
    orientUp();
    angle = UP;
    colour = color;
  };
  SqBlock* clone() const override{
    return new SqBlock(*this);
  }
};

#endif
