/*
 * Copyright (c) 2012 William J Woodall <wjwwood@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef SHELBY_DRAWING_TOOLS_H
#define SHELBY_DRAWING_TOOLS_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <utility> // For std::pair
#include <cmath>
#include <string>
#include <sstream>

typedef std::vector<std::pair<GLint, GLint> > Vertices;

// Convenience variables
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
static double radians_to_degrees = 180.0 / M_PI;
static double degrees_to_radians = M_PI / 180.0;

namespace shelby {

  struct Color {
    Color() {}
    Color(GLfloat r_, GLfloat g_, GLfloat b_) {
      this->r = r_;
      this->g = g_;
      this->b = b_;
    }
    GLfloat r, g, b;
  };

  Color rgb(int r, int g, int b);

  Color rgb(std::string css_color);

  /***** Functions that help with drawing OpenGL Primitives *****/
  void drawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

  void drawTriangle(GLint x1, GLint y1,
                    GLint x2, GLint y2,
                    GLint x3, GLint y3);

  void drawQuad(GLint x1, GLint y1,
                GLint x2, GLint y2,
                GLint x3, GLint y3,
                GLint x4, GLint y4);

  void drawTriangleStrip(Vertices &verts);

  void drawAxis();

  void drawSmoothCircle(GLfloat origin[2], GLfloat radius, Color color1, Color color2);
  void drawSmoothQuad(GLfloat top_left[2], GLfloat bottom_right[2], Color color1, Color color2);
  void drawSmoothQuad(GLfloat top_left[2], GLfloat bottom_right[2], std::vector<Color> colors);
}

#endif // SHELBY_H

