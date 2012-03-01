/***
 * Homework Assignment #3
 * William Woodall
 * Comp 6400 - Computer Graphics
 * Wednesday February 29th, 2012
 */

// Instructions: Click and drag to rotate, 
//  - and = scale the person, and ESC quits.

/***
 * Includes
 */

// STL includes
#include <stdlib.h>
#include <sstream>
#include <vector>

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

// compatibility with original GLUT
#if !defined(GLUT_WHEEL_UP)
#  define GLUT_WHEEL_UP   3
#  define GLUT_WHEEL_DOWN 4
#endif

using std::vector;

/***
 * Global variables
 */
GLint window_width = 500;
GLint window_height = 500;

/***
 * Utility Classes and Functions
 */

/*!
 * A generic Color class designed be easy to 
 * specify and set an OpenGl Color.
 */
template <class T>
struct ColorG {
  ColorG() {}
  ColorG(T red, T green, T blue) {
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

// For convenience
typedef ColorG<GLfloat> Color;

/*! Convenience function to create a Color struct from 16-bit RGB values */
Color rgb(int r, int g, int b) {
  Color result(r/255.0f, g/255.0f, b/255.0f);
  return result;
}

/*! Convenience function to create a Color struct from CSS style RGB values */
Color rgb(std::string css_color) {
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
  Color result;
  result.r = ((color >> 16) & 0xff) / 255.0;
  result.g = ((color >> 8) & 0xff) / 255.0;
  result.b = (color & 0xff) / 255.0;
  return result;
}

void drawAxis() {
  glLineWidth(3.0f);
  // x is red
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(10.0, 0.0, 0.0);
  glEnd();
  // y is green
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 10.0, 0.0);
  glEnd();
  // z is blue
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 10.0);
  glEnd();
}

void
drawSmoothQuad(GLfloat top_left[2], GLfloat bottom_right[2],
               std::vector<Color> colors)
{
  glPushMatrix();
    glShadeModel(GL_SMOOTH);
    glBegin(GL_QUADS);
      colors[0].setGLColor();
      glVertex2f(top_left[0], top_left[1]);
      colors[1].setGLColor();
      glVertex2f(top_left[0], bottom_right[1]);
      colors[2].setGLColor();
      glVertex2f(bottom_right[0], bottom_right[1]);
      colors[3].setGLColor();
      glVertex2f(bottom_right[0], top_left[1]);
    glEnd();
  glPopMatrix();
}

/*!
 * width = x
 * heigth = y
 * depth = z
 */
void
drawPrism(std::vector<std::vector<Color> > colors,
          GLfloat x, GLfloat y=0.0f, GLfloat z=0.0f)
{
  if (y == 0.0f) y = x;
  if (z == 0.0f) z = y;
  x /= 2.0f;
  y /= 2.0f;
  z /= 2.0f;
  // Push the matrix
  glPushMatrix();
    // Draw the top quad (normal positive z)
    glPushMatrix();
    {
      glTranslatef(0.0f, 0.0f, z);
      GLfloat top_left[2] = {-x, y};
      GLfloat bottom_right[2] = {x, -y};
      drawSmoothQuad(top_left, bottom_right, colors[0]);
    }
    glPopMatrix();
    // Draw the bottom quad (normal negative x)
    glPushMatrix();
    {
      glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
      glTranslatef(0.0f, 0.0f, z);
      GLfloat top_left[2] = {-x, y};
      GLfloat bottom_right[2] = {x, -y};
      drawSmoothQuad(top_left, bottom_right, colors[1]);
    }
    glPopMatrix();
    // Draw the front quad (normal positive x)
    glPushMatrix();
    {
      glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
      glTranslatef(0.0f, 0.0f, x);
      GLfloat top_left[2] = {-z, y};
      GLfloat bottom_right[2] = {z, -y};
      drawSmoothQuad(top_left, bottom_right, colors[2]);
    }
    glPopMatrix();
    // Draw the rear quad (normal negative x)
    glPushMatrix();
    {
      glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
      glTranslatef(0.0f, 0.0f, x);
      GLfloat top_left[2] = {-z, y};
      GLfloat bottom_right[2] = {z, -y};
      drawSmoothQuad(top_left, bottom_right, colors[3]);
    }
    glPopMatrix();
    // Draw the right quad (normal positive y)
    glPushMatrix();
    {
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      glTranslatef(0.0f, 0.0f, y);
      GLfloat top_left[2] = {-x, z};
      GLfloat bottom_right[2] = {x, -z};
      drawSmoothQuad(top_left, bottom_right, colors[4]);
    }
    glPopMatrix();
    // Draw the left quad (normal negative y)
    glPushMatrix();
    {
      glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
      glTranslatef(0.0f, 0.0f, y);
      GLfloat top_left[2] = {-x, z};
      GLfloat bottom_right[2] = {x, -z};
      drawSmoothQuad(top_left, bottom_right, colors[5]);
    }
    glPopMatrix();
  // Pop the matrix
  glPopMatrix();
}

void
drawPrism(std::vector<Color> colors, GLfloat x, GLfloat y=0.0f, GLfloat z=0.0f)
{
  if (colors.size() == 4) {
    vector<vector<Color> > nested_colors(6, colors);
    drawPrism(nested_colors, x, y, z);
  }
  if (colors.size() == 6) {
    vector<vector<Color> > nested_colors;
    for (int i = 0; i < 6; ++i) {
      nested_colors.push_back(vector<Color>(4, colors[i]));
    }
    drawPrism(nested_colors, x, y, z);
  }
}

void
drawPrism(Color color, GLfloat x, GLfloat y=0.0f, GLfloat z=0.0f)
{
  std::vector<Color> colors(4, color);
  drawPrism(colors, x, y, z);
}

/***
 * Domain Specific Code
 */

/*!
 * This is a class that represents a person in 3D
 */
class Person3D {
public:
  Person3D() {

  }
  ~Person3D() {

  }

  /*!
   * Draws the person
   */
  void draw() {
    drawAxis();
    // Push the matrix
    glPushMatrix();
      // Draw the head
      glPushMatrix();
      {
        glTranslatef(0.0f, 0.0f, 7.0f);
        drawPrism(rgb("#DEB887"), 2.0f, 2.0f, 2.0f);
      }
      glPopMatrix();
      // Draw the left arm
      glPushMatrix();
      {
        glTranslatef(0.0f, 1.5f, 4.75f);
        drawPrism(rgb("#DEB887"), 0.75f, 0.75f, 2.0f);
      }
      glPopMatrix();
      // Draw the right arm
      glPushMatrix();
      {
        glTranslatef(0.0f, -1.5f, 4.75f);
        drawPrism(rgb("#DEB887"), 0.75f, 0.75f, 2.0f);
      }
      glPopMatrix();
      // Draw the backpack
      glPushMatrix();
      {
        glTranslatef(-1.0f, 0.0f, 4.75f);
        drawPrism(rgb("#ff0000"), 1.2f, 2.2f, 3.0f);
      }
      glPopMatrix();
      // Draw the torso
      glPushMatrix();
      {
        glTranslatef(0.0f, 0.0f, 4.75f);
        drawPrism(rgb("#ff8c00"), 1.0f, 2.0f, 3.0f);
      }
      glPopMatrix();
      // Draw the left sleeve
      glPushMatrix();
      {
        glTranslatef(0.0f, 1.5f, 5.75f);
        drawPrism(rgb("#ff8c00"), 1.0f, 1.0f, 1.0f);
      }
      glPopMatrix();
      // Draw the right sleeve
      glPushMatrix();
      {
        glTranslatef(0.0f, -1.5f, 5.75f);
        drawPrism(rgb("#ff8c00"), 1.0f, 1.0f, 1.0f);
      }
      glPopMatrix();
      // Draw the left leg
      glPushMatrix();
      {
        glTranslatef(0.0f, 0.85f/2.0f, 2.0f);
        drawPrism(rgb("#0000CD"), 0.75f, 0.75f, 4.0f);
      }
      glPopMatrix();
      // Draw the right leg
      glPushMatrix();
      {
        glTranslatef(0.0f, -0.85f/2.0f, 2.0f);
        drawPrism(rgb("#0000CD"), 0.75f, 0.75f, 4.0f);
      }
      glPopMatrix();
    // Pop the matrix
    glPopMatrix();
  }

private:

};

/***
 * Main program
 */

Person3D person;
GLfloat origin[3] = {0.0f, 0.0f, 5.0f};
GLfloat camera[3] = {15.0f, 0.0f, 5.0f};
GLfloat rotation[3] = {0.0f, 0.0f, 0.0f};

/*! Sets up the OpenGL environment */
void setup() {
  Color c = rgb("#8B8878");
  glClearColor(c.r, c.g, c.b, 1.0f);
}

/*! GLUT display callback */
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  std::string help_msg = "Click and drag to rotate, - "
                         "and = scale the person, and ESC quits.";
  glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(5, 5, 0);
    glScalef(0.1f, 0.1f, 0.1f);
    for (int i = 0; i < help_msg.length(); ++i) {
      glutStrokeCharacter(GLUT_STROKE_ROMAN, help_msg[i]);
    }
  glPopMatrix();

  glPushMatrix();
    gluLookAt(camera[0], camera[1], camera[2],
              origin[0], origin[1], origin[2],
              0.0f, 0.0f, 1.0f);
    glRotatef(rotation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(rotation[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rotation[2], 0.0f, 0.0f, 1.0f);

    person.draw();
  glPopMatrix();

  glutSwapBuffers();
}

/*! GLUT resize callback */
void resize(GLsizei w, GLsizei h) {
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 1000.0);

  glMatrixMode(GL_MODELVIEW);
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
          camera[0] += 2.5f;
          break;
        case 61: // =
          camera[0] -= 2.5f;
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

int old_y = 0;
int old_x = 0;
bool dragging = false;

/*! GLUT on mouse button click callback */
void onMouseButton(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      dragging = true;
      old_y = y;
      old_x = x;
    } else {
      dragging = false;
      old_y = 0;
      old_x = 0;
    }
  }
  // Scrolling
  if (button == GLUT_WHEEL_UP || button == GLUT_WHEEL_DOWN) {
    if (state == GLUT_UP) return;
    if (button == GLUT_WHEEL_UP) camera[0] += 0.5f;
    if (button == GLUT_WHEEL_DOWN) camera[0] -= 0.5f;
  }
}

/*! GLUT on active (button pressed) mouse motion callback */
void onActiveMouseMotion(int x, int y) {
  if (dragging) {
    rotation[1] -= (old_y - y)/5.0f;
    rotation[2] -= (old_x - x)/5.0f;
    old_y = y;
    old_x = x;
    glutPostRedisplay();
  }
}

/*! Main function */
int main(int argc, char *argv[]) {
  // OpenGL setup
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(window_width, window_height);
  glutCreateWindow("COMP-5/6400 Assignment 3");

  // Setup scene
  setup();

  // Set glut display callback
  glutDisplayFunc(display);

  // Set glut resize callback
  glutReshapeFunc(resize);

  // Setup keypress handlers
  glutKeyboardFunc(OnKeyboardPress);
  glutSpecialFunc(OnSpecialKeyboardPress);
  // Setup Mouse handlers
  glutMouseFunc(onMouseButton);
  glutMotionFunc(onActiveMouseMotion);

  // Run opengl
  glutMainLoop();

  return 0;
}
