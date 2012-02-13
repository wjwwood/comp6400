#include "shelby/drawing_tools.h"

shelby::Color shelby::rgb(int r, int g, int b) {
  Color result(r/255.0f, g/255.0f, b/255.0f);
  return result;
}

shelby::Color shelby::rgb(std::string css_color) {
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

void shelby::drawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
  glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
  glEnd();
}

void shelby::drawAxis() {
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

void shelby::drawTriangle(GLint x1, GLint y1,
                          GLint x2, GLint y2,
                          GLint x3, GLint y3)
{
  glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
  glEnd();
}

void shelby::drawQuad(GLint x1, GLint y1,
                      GLint x2, GLint y2,
                      GLint x3, GLint y3,
                      GLint x4, GLint y4)
{
  glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);
  glEnd();
}

void shelby::drawTriangleStrip(Vertices &verts)
{
  Vertices::iterator it;
  glBegin(GL_TRIANGLE_STRIP);
  for (it = verts.begin(); it != verts.end(); ++it) {
    glVertex2f((*it).first, (*it).second);
  }
  glEnd();
}

void shelby::drawSmoothCircle(GLfloat origin[2], GLfloat radius, Color color1, Color color2) {
  glShadeModel(GL_SMOOTH);
  glPushMatrix();
    glTranslatef(origin[0], origin[1], 0.0f);
    glColor3f(color1.r, color1.g, color1.b);
    glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0.0f, 0.0f);
      for (int i = 0; i <= 360; i+=6) {
        glColor3f(color2.r, color2.g, color2.b);
        glVertex2f(radius * cos(i*degrees_to_radians), radius * sin(i*degrees_to_radians));
      }
    glEnd();
  glPopMatrix();
}

void shelby::drawSmoothQuad(GLfloat top_left[2], GLfloat bottom_right[2], Color color1, Color color2) {
  std::vector<Color> colors;
  colors.push_back(color1);
  colors.push_back(color1);
  colors.push_back(color2);
  colors.push_back(color2);
  drawSmoothQuad(top_left, bottom_right, colors);
}

void shelby::drawSmoothQuad(GLfloat top_left[2], GLfloat bottom_right[2], std::vector<Color> colors) {
  glShadeModel(GL_SMOOTH);
  glPushMatrix();
    glBegin(GL_QUADS);
      glColor3f(colors[0].r, colors[0].g, colors[0].b);
      glVertex2f(top_left[0], top_left[1]);
      glColor3f(colors[1].r, colors[1].g, colors[1].b);
      glVertex2f(top_left[0], bottom_right[1]);
      glColor3f(colors[2].r, colors[2].g, colors[2].b);
      glVertex2f(bottom_right[0], bottom_right[1]);
      glColor3f(colors[3].r, colors[3].g, colors[3].b);
      glVertex2f(bottom_right[0], top_left[1]);
    glEnd();
  glPopMatrix();
}
