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

ShelbyCenter shelby_center;
GLint width = 637;
GLint height = 500;

void setup() {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Setup Shelby Center Class
  shelby_center = ShelbyCenter(width, height);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shelby_center.draw();
  glutSwapBuffers();
}

int main(int argc, char *argv[]) {
  // OpenGL setup
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutCreateWindow("COMP-5/6400 Assignment 1");

  // Setup scene
  setup();

  // Set opengl callback
  glutDisplayFunc(display);

  // Run opengl
  glutMainLoop();

  return 0;
}