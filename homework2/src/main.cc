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
#include "shelby/person.h"

using namespace shelby;

ShelbyCenter shelby_center;
Person person;
GLint width = 654;
GLint height = 500;
float move_speed = 5.0f;
float rotation_speed = 5.0f;

void setup() {
  Color c = rgb("#8B8878");
  glClearColor(c.r, c.g, c.b, 1.0f);

  shelby_center.y_offset = height-10;
  shelby_center.x_offset = 10;

  person = Person(330, 200, 0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  std::string help_msg = "Arrow keys and (aswd) move and rotate, - and = scale the person, and ESC quits.";
  glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(5, 5, 0);
    glScalef(0.1f, 0.1f, 0.1f);
    for (int i = 0; i < help_msg.length(); ++i) {
      glutStrokeCharacter(GLUT_STROKE_ROMAN, help_msg[i]);
    }
  glPopMatrix();
  shelby_center.draw();
  person.draw();
  glutSwapBuffers();
}

void resize(GLsizei w, GLsizei h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OnKeyboardPress(unsigned char key, int x, int y) {
    switch(key){
        case 27:
          exit(0);
          break;
        case 97:
          person.rotate(rotation_speed);
          break;
        case 100:
          person.rotate(-rotation_speed);
          break;
        case 119:
          person.move(move_speed);
          break;
        case 115:
          person.move(-move_speed);
          break;
        case 45:
          person.scale(0.9);
          break;
        case 61:
          person.scale(1.1);
          break;
        default:
          printf("   Keyboard %c == %d\n", key, key);
          break;
    }
    glutPostRedisplay();
}

void OnSpecialKeyboardPress(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
          person.rotate(rotation_speed);
          break;
        case GLUT_KEY_RIGHT:
          person.rotate(-rotation_speed);
          break;
        case GLUT_KEY_UP:
          person.move(move_speed);
          break;
        case GLUT_KEY_DOWN:
          person.move(-move_speed);
          break;
        default:
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
  // OpenGL setup
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutCreateWindow("COMP-5/6400 Assignment 2");

  // Setup scene
  setup();

  // Set glut display callback
  glutDisplayFunc(display);

  // Set glut resize callback
  glutReshapeFunc(resize);

  // Setup keypress handlers
  glutKeyboardFunc(OnKeyboardPress);
  glutSpecialFunc(OnSpecialKeyboardPress);

  // Run opengl
  glutMainLoop();

  return 0;
}