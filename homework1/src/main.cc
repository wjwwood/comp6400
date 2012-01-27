// Windows specific
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif

// OpenGL
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "shelby/shelby.h"

using namespace shelby;

void setup() {
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawShelby(std::make_pair(0,0));
  glutSwapBuffers();
}

int main(int argc, char *argv[]) {
  // OpenGL setup
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(800,600);
  glutCreateWindow("COMP-5/6400 Assignment 1");

  // Setup scene
  setup();

  // Set opengl callback
  glutDisplayFunc(display);

  // Run opengl
  glutMainLoop();

  return 0;
}