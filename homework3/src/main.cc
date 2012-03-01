/***
 * Homework Assignment #3
 * William Woodall
 * Comp 6400 - Computer Graphics
 * Wednesday February 29th, 2012
 */

/***
 * Includes
 */

// STL includes
#include <stdlib.h>
#include <sstream>

// Windows specific
#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# define WIN32_EXTRA_LEAN
# include <windows.h>
#endif

// OpenGL/GLUT headers
#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
#endif

/***
 * Global variables
 */
GLint width = 500;
GLint height = 500;

/***
 * Utility Classes and Functions
 */

/*!
 * A generic Color class designed be easy to 
 * specify and set an OpenGl Color.
 */
template <class T>
struct Color {
  Color() {}
  Color(T red, T green, T blue) {
    this->r = red;
    this->g = green;
    this->b = blue;
  }

  /*!
   * Sets the OpenGL color to this color using glColor3f()
   */
  void setGLColor() {
    // Implicit conversions from T to GLfloat should occur
    glColor3f(this->r, this->g, this->b);
  }

  T r, g, b;
};

/*! Convenience function to create a Color struct from 16-bit RGB values */
Color<GLfloat> rgb(int r, int g, int b) {
  Color<GLfloat> result(r/255.0f, g/255.0f, b/255.0f);
  return result;
}

/*! Convenience function to create a Color struct from CSS style RGB values */
Color<GLfloat> rgb(std::string css_color) {
  if (css_color.empty() || css_color.length() < 6) {
    throw "Invalid color string.";
  }
  if (css_color[0] == '#') {
    css_color = css_color.substr(1, css_color.length()-1);
  }
  unsigned int color;
  std::stringstream ss;
  ss << std::hex << css_color;
  ss >> color;
  Color<GLfloat> result;
  result.r = ((color >> 16) & 0xff) / 255.0;
  result.g = ((color >> 8) & 0xff) / 255.0;
  result.b = (color & 0xff) / 255.0;
  return result;
}

/***
 * Domain Specific Code
 */

class Person3D {
public:
  Person3D() {

  }
  ~Person3D() {

  }

  void draw() {
    rgb("#ff0000").setGLColor();
    glutSolidCube(3.0);
  }

private:

};

/***
 * Main program
 */

Person3D person;
GLfloat origin[3] = {0.0f, 0.0f, 0.0f};
GLfloat camera[3] = {15.0f, 0.0f, 5.0f};
GLfloat rotation = 45.0f;

/*! Sets up the OpenGL environment */
void setup() {
  Color<GLfloat> c = rgb("#8B8878");
  glClearColor(c.r, c.g, c.b, 1.0f);
}

/*! GLUT display callback */
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gluLookAt(camera[0], camera[1], camera[2],
            origin[0], origin[1], origin[2],
            0.0f, 0.0f, 1.0f);
  glRotatef(rotation, 0.0f, 0.0f, 1.0f);

  person.draw();

  glutSwapBuffers();
}

/*! GLUT resize callback */
void resize(GLsizei w, GLsizei h) {
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 1000.0);

  glMatrixMode (GL_MODELVIEW);
}

/*! GLUT on normal keyboard press callback */
void OnKeyboardPress(unsigned char key, int x, int y) {
    switch(key){
        case 27: // esc
          exit(0);
          break;
        case 97: // a
          break;
        case 100: // d
          break;
        case 119: // w
          break;
        case 115: // s
          break;
        case 45: // -
          break;
        case 61: // =
          break;
        default:
          printf("   Keyboard %c == %d\n", key, key);
          break;
    }
    glutPostRedisplay();
}

/*! GLUT on special keyboard press callback */
void OnSpecialKeyboardPress(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
          break;
        case GLUT_KEY_RIGHT:
          break;
        case GLUT_KEY_UP:
          break;
        case GLUT_KEY_DOWN:
          break;
        default:
            break;
    }
    glutPostRedisplay();
}

/*! Main function */
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
