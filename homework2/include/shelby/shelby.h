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

#ifndef SHELBY_H
#define SHELBY_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <utility> // For std::pair

typedef std::vector<std::pair<GLint, GLint> > Vertices;

namespace shelby {

  /***** Shelby Center Class *****/
  class ShelbyCenter
  {
  public:
    ShelbyCenter (GLfloat size_x = 800.0f, GLfloat size_y = 600.0f);
    virtual ~ShelbyCenter ();

    void draw();

  private:
    void drawCenter();
    void drawNW();
    void drawSW();
    void drawTriangle(GLint x1, GLint y1,
                      GLint x2, GLint y2,
                      GLint x3, GLint y3,
                      GLfloat r, GLfloat b, GLfloat g);

    void drawQuad(GLint x1, GLint y1,
                  GLint x2, GLint y2,
                  GLint x3, GLint y3,
                  GLint x4, GLint y4,
                  GLfloat r, GLfloat b, GLfloat g);

    void drawTriangleStrip(Vertices &verts,
                           GLfloat r, GLfloat b, GLfloat g);

    // Width and Height
    GLfloat w_, h_;
    bool mirror_x_;
  };

}

#endif // SHELBY_H

