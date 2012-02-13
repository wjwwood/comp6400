#include "shelby/person.h"

#include <cmath>

#include "shelby/drawing_tools.h"

using namespace shelby;

Person::Person (GLfloat initial_x, GLfloat initial_y, GLfloat initial_rotation) {
  this->x_ = initial_x;
  this->y_ = initial_y;
  this->rotation_ = initial_rotation;
  this->scale_ = 1.0f;
  this->left_foot_forward = true;
}

Person::~Person () {

}

void Person::draw() {
  glPushMatrix();
    glTranslatef(this->x_, this->y_, 0.0f);
    glRotatef(this->rotation_, 0.0f, 0.0f, 1.0f);
    glScalef(this->scale_, this->scale_, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    // Draw axis
    drawAxis();
    // Draw Sholders
    glColor3f(1.0f, 0.0f, 0.0f);
    drawQuad(-2, 7, 2, 7, 2, -7, -2, -7);
    // Draw legs
    GLfloat top_left[2] = {0.0f, 3.0f};
    GLfloat bottom_right[2] = {8.0f, 1.0f};
    Color leg_start = rgb("#A9A9A9");
    Color leg_end = rgb("#00008B");
    if (this->left_foot_forward) {
      bottom_right[0] *= -1;
    }
    drawSmoothQuad(top_left, bottom_right, leg_start, leg_end);
    top_left[1] *= -1;
    bottom_right[1] *= -1;
    bottom_right[0] *= -1;
    drawSmoothQuad(top_left, bottom_right, leg_start, leg_end);
    // Draw head
    GLfloat origin[2] = {0.0f,0.0f};
    drawSmoothCircle(origin, 5.0f, rgb("#CD950C"), rgb("#8B6508"));
  glPopMatrix();
}

void Person::move(float distance) {
  this->x_ += distance*cos(this->rotation_*degrees_to_radians);
  this->y_ += distance*sin(this->rotation_*degrees_to_radians);
  this->left_foot_forward = !this->left_foot_forward;
}

void Person::rotate(float degrees) {
  this->rotation_ += degrees;
}

void Person::scale(float scalar) {
  this->scale_ *= scalar;
}
