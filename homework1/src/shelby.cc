#include "shelby/shelby.h"

using namespace shelby;

namespace shelby {

  ShelbyCenter::ShelbyCenter(GLfloat size_x, GLfloat size_y) {
    this->w_ = size_x;
    this->h_ = size_y;
  }

  ShelbyCenter::~ShelbyCenter() {
    
  }

  void ShelbyCenter::draw() {
    this->drawNW();
    this->drawSW();
    this->drawCenter();
    this->mirror_x_ = true;
    this->drawNW();
    this->drawSW();
    this->drawCenter();
    this->mirror_x_ = false;
  }

  void ShelbyCenter::drawCenter() {
    Vertices strip;
    // B 110	120	111	
    drawTriangle(/*1*/270, 105, /*2*/319, 155, /*3*/319, 105,
                 /*rgb*/110.0/255.0, 120.0/255.0, 111.0/225.0);
    // C 171	175	160	
    drawTriangle(/*1*/319, 155, /*2*/270, 201, /*3*/319, 201,
                 /*rgb*/171.0/255.0, 175.0/255.0, 160.0/225.0);
    // D 158	162	148	
    drawTriangle(/*1*/287, 105, /*2*/319, 137, /*3*/319, 105,
                 /*rgb*/158.0/255.0, 162.0/255.0, 148.0/225.0);
    drawQuad(/*1*/287, 105, /*2*/319, 105, /*3*/319, 90, /*4*/287, 90,
             /*rgb*/158.0/255.0, 162.0/255.0, 148.0/225.0);
    // E 160	164	150	
    drawTriangle(/*1*/319, 171, /*2*/284, 201, /*3*/319, 201,
                 /*rgb*/160.0/255.0, 164.0/255.0, 150.0/225.0);
    drawQuad(/*1*/284, 201, /*2*/319, 201, /*3*/319, 208, /*4*/284, 208,
             /*rgb*/160.0/255.0, 164.0/255.0, 150.0/225.0);
    // G 154	160	146	
    drawTriangle(/*1*/270, 201, /*2*/270, 105, /*3*/319, 155,
                 /*rgb*/154.0/255.0, 160.0/255.0, 146.0/225.0);
    // A 110	121	106:175	178	163	
    strip.clear();
    strip.push_back(std::make_pair(235, 134));
    strip.push_back(std::make_pair(242, 126));
    strip.push_back(std::make_pair(235+(277-235)/2.0, 134));
    strip.push_back(std::make_pair(270, 126));
    strip.push_back(std::make_pair(277, 134));
    drawTriangleStrip(strip, /*rgb*/110.0/255.0, 121.0/255.0, 106.0/225.0);
    strip.clear();
    strip.push_back(std::make_pair(235, 134));
    strip.push_back(std::make_pair(242, 140));
    strip.push_back(std::make_pair(235+(277-235)/2.0, 134));
    strip.push_back(std::make_pair(270, 140));
    strip.push_back(std::make_pair(277, 134));
    drawTriangleStrip(strip, /*rgb*/175.0/255.0, 178.0/255.0, 163.0/225.0);
  }

  void ShelbyCenter::drawNW() {
    Vertices strip;
    // A 155	159	145	
    drawTriangle(/*1*/58, 80, /*2*/93, 119, /*3*/58, 155,
                 /*rgb*/155.0/255.0, 159.0/255.0, 145.0/225.0);
    // C 113	121	110	
    strip.clear();
    strip.push_back(std::make_pair(58,80));
    strip.push_back(std::make_pair(93,119));
    strip.push_back(std::make_pair(58+(241-58)/2.0,80));
    strip.push_back(std::make_pair(205,119));
    strip.push_back(std::make_pair(241,80));
    drawTriangleStrip(strip, /*rgb*/113.0/255.0, 121.0/255.0, 110.0/225.0);
    // B 155	160	146:136	144	131	
    drawTriangle(/*1*/58, 77, /*2*/76, 77, /*3*/76, 95,
                 /*rgb*/155.0/255.0, 160.0/255.0, 146.0/225.0);
    drawTriangle(/*1*/76, 77, /*2*/94, 77, /*3*/76, 95,
                 /*rgb*/136.0/255.0, 144.0/255.0, 131.0/225.0);
    // D 157	161	147:135	143	128	
    drawTriangle(/*1*/204, 77, /*2*/223, 77, /*3*/223, 96,
                 /*rgb*/157.0/255.0, 161.0/255.0, 147.0/225.0);
    drawTriangle(/*1*/223, 77, /*2*/240, 77, /*3*/223, 96,
                 /*rgb*/135.0/255.0, 143.0/255.0, 128.0/225.0);
    // E 138	144	130	
    drawTriangle(/*1*/241, 80, /*2*/205, 119, /*3*/241, 156,
                 /*rgb*/138.0/255.0, 144.0/255.0, 130.0/225.0);
    // G 171	175	160	
    strip.clear();
    strip.push_back(std::make_pair(58, 155));
    strip.push_back(std::make_pair(93, 119));
    strip.push_back(std::make_pair(58+(241-58)/2.0, 155));
    strip.push_back(std::make_pair(205, 119));
    strip.push_back(std::make_pair(241, 155));
    drawTriangleStrip(strip, /*rgb*/171.0/255.0, 175.0/255.0, 160.0/225.0);
    // F 155	163	147:134	141	129	
    drawTriangle(/*1*/204, 160, /*2*/223, 160, /*3*/223, 144,
                 /*rgb*/155.0/255.0, 163.0/255.0, 147.0/225.0);
    drawTriangle(/*1*/223, 160, /*2*/223, 144, /*3*/242, 160,
                 /*rgb*/134.0/255.0, 141.0/255.0, 129.0/225.0);
    // H 159	163	149	
    strip.clear();
    strip.push_back(std::make_pair(93, 123));
    strip.push_back(std::make_pair(60, 155));
    strip.push_back(std::make_pair(93, 123+(235-123)/2.0));
    strip.push_back(std::make_pair(60, 268));
    strip.push_back(std::make_pair(93, 235));
    drawTriangleStrip(strip, /*rgb*/159.0/255.0, 163.0/255.0, 149.0/225.0);
    // I 136	142	128	
    strip.clear();
    strip.push_back(std::make_pair(93, 123));
    strip.push_back(std::make_pair(126, 155));
    strip.push_back(std::make_pair(93, 123+(235-123)/2.0));
    strip.push_back(std::make_pair(126, 268));
    strip.push_back(std::make_pair(93, 235));
    drawTriangleStrip(strip, /*rgb*/136.0/255.0, 142.0/255.0, 128.0/225.0);
    // J 170	174	159	
    drawTriangle(/*1*/60, 268, /*2*/126, 268, /*3*/93, 235,
                 /*rgb*/170.0/255.0, 174.0/255.0, 159.0/225.0);
    // K 113	121	113:168	175	159	
    drawTriangle(/*1*/55, 233, /*2*/55, 249, /*3*/74, 249,
                 /*rgb*/113.0/255.0, 121.0/255.0, 113.0/225.0);
    drawTriangle(/*1*/55, 249, /*2*/74, 249, /*3*/55, 263,
                 /*rgb*/168.0/255.0, 175.0/255.0, 159.0/225.0);
    // L 113	122	111:172	177	159	
    drawTriangle(/*1*/130, 235, /*2*/118, 250, /*3*/130, 250,
                 /*rgb*/113.0/255.0, 122.0/255.0, 111.0/225.0);
    drawTriangle(/*1*/130, 250, /*2*/118, 250, /*3*/130, 264,
                 /*rgb*/172.0/255.0, 177.0/255.0, 159.0/225.0);
  }

  void ShelbyCenter::drawSW() {
    Vertices strip;
    // A 151	159	146	
    drawTriangle(/*1*/57, 347, /*2*/99, 393, /*3*/57, 433,
                 /*rgb*/151.0/255.0, 159.0/255.0, 146.0/225.0);
    // B 105	115	107	
    strip.clear();
    strip.push_back(std::make_pair(57, 347));
    strip.push_back(std::make_pair(99, 393));
    strip.push_back(std::make_pair(57+(244-57)/2.0, 347));
    strip.push_back(std::make_pair(203, 393));
    strip.push_back(std::make_pair(244, 347));
    drawTriangleStrip(strip, /*rgb*/113.0/255.0, 121.0/255.0, 110.0/225.0);
    // C 133	141	129	
    drawTriangle(/*1*/203, 393, /*2*/244, 347, /*3*/244, 434,
                 /*rgb*/133.0/255.0, 141.0/255.0, 129.0/225.0);
    // D 168	175	161	
    strip.clear();
    strip.push_back(std::make_pair(57, 433));
    strip.push_back(std::make_pair(99, 393));
    strip.push_back(std::make_pair(57+(244-57)/2.0, 433));
    strip.push_back(std::make_pair(203, 393));
    strip.push_back(std::make_pair(244, 433));
    drawTriangleStrip(strip, /*rgb*/168.0/255.0, 175.0/255.0, 161.0/225.0);
    // E 150	158	145:133	141	131	
    drawTriangle(/*1*/58, 344, /*2*/76, 344, /*3*/76, 364,
                 /*rgb*/150.0/255.0, 158.0/255.0, 145.0/225.0);
    drawTriangle(/*1*/76, 344, /*2*/76, 364, /*3*/95, 344,
                 /*rgb*/133.0/255.0, 141.0/255.0, 131.0/225.0);
    // F 150	158	145:133	141	128	
    drawTriangle(/*1*/206, 344, /*2*/225, 344, /*3*/225, 364,
                 /*rgb*/150.0/255.0, 158.0/255.0, 145.0/225.0);
    drawTriangle(/*1*/225, 344, /*2*/225, 364, /*3*/243, 344,
                 /*rgb*/133.0/255.0, 141.0/255.0, 128.0/225.0);
    // G 150	157	146:131	139	128	
    drawTriangle(/*1*/76, 419, /*2*/76, 435, /*3*/58, 435,
                 /*rgb*/150.0/255.0, 157.0/255.0, 146.0/225.0);
    drawTriangle(/*1*/76, 419, /*2*/76, 435, /*3*/93, 435,
                 /*rgb*/131.0/255.0, 139.0/255.0, 128.0/225.0);
    // H 151	157	145:135	143	132	
    drawTriangle(/*1*/207, 435, /*2*/225, 435, /*3*/225, 420,
                 /*rgb*/151.0/255.0, 157.0/255.0, 145.0/225.0);
    drawTriangle(/*1*/225, 420, /*2*/225, 435, /*3*/242, 435,
                 /*rgb*/135.0/255.0, 143.0/255.0, 132.0/225.0);
  }

  void ShelbyCenter::drawTriangle(GLint x1, GLint y1,
                                  GLint x2, GLint y2,
                                  GLint x3, GLint y3,
                                  GLfloat r, GLfloat b, GLfloat g)
  {
    glColor3f(r, b, g);
    glBegin(GL_TRIANGLES);
      if (this->mirror_x_) {
        glVertex2f(1-(x1/(this->w_/2.0)), 1-(y1/(this->h_/2.0)));
        glVertex2f(1-(x2/(this->w_/2.0)), 1-(y2/(this->h_/2.0)));
        glVertex2f(1-(x3/(this->w_/2.0)), 1-(y3/(this->h_/2.0)));
      } else {
        glVertex2f((x1/(this->w_/2.0))-1, 1-(y1/(this->h_/2.0)));
        glVertex2f((x2/(this->w_/2.0))-1, 1-(y2/(this->h_/2.0)));
        glVertex2f((x3/(this->w_/2.0))-1, 1-(y3/(this->h_/2.0)));
      }
    glEnd();
  }

  void ShelbyCenter::drawQuad(GLint x1, GLint y1,
                              GLint x2, GLint y2,
                              GLint x3, GLint y3,
                              GLint x4, GLint y4,
                              GLfloat r, GLfloat b, GLfloat g)
  {
    glColor3f(r, b, g);
    glBegin(GL_QUADS);
      if (this->mirror_x_) {
        glVertex2f(1-(x1/(this->w_/2.0)), 1-(y1/(this->h_/2.0)));
        glVertex2f(1-(x2/(this->w_/2.0)), 1-(y2/(this->h_/2.0)));
        glVertex2f(1-(x3/(this->w_/2.0)), 1-(y3/(this->h_/2.0)));
        glVertex2f(1-(x4/(this->w_/2.0)), 1-(y4/(this->h_/2.0)));
      } else {
        glVertex2f((x1/(this->w_/2.0))-1, 1-(y1/(this->h_/2.0)));
        glVertex2f((x2/(this->w_/2.0))-1, 1-(y2/(this->h_/2.0)));
        glVertex2f((x3/(this->w_/2.0))-1, 1-(y3/(this->h_/2.0)));
        glVertex2f((x4/(this->w_/2.0))-1, 1-(y4/(this->h_/2.0)));
      }
    glEnd();
  }

  void
  ShelbyCenter::drawTriangleStrip(Vertices &verts,
                                  GLfloat r, GLfloat b, GLfloat g)
  {
    glColor3f(r, b, g);
    Vertices::iterator it;
    glBegin(GL_TRIANGLE_STRIP);
    for (it = verts.begin(); it != verts.end(); ++it) {
      if (this->mirror_x_) {
        glVertex2f(1-((*it).first/(this->w_/2.0)),
                   1-((*it).second/(this->h_/2.0)));
      } else {
        glVertex2f(((*it).first/(this->w_/2.0))-1,
                   1-((*it).second/(this->h_/2.0)));
      }
    }
    glEnd();
  }

}

