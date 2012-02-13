#include "shelby/shelby.h"

using namespace shelby;

namespace shelby {

  ShelbyCenter::ShelbyCenter() {

  }

  ShelbyCenter::~ShelbyCenter() {
    
  }

  void ShelbyCenter::draw() {
    glPushMatrix();
      // Apply the offset
      glTranslatef(this->x_offset, this->y_offset, 0.0f);
      // Rotate in 3D to make the origin in the top left like images
      glScalef(1.0f, -1.0f, 1.0f);
      drawAxis();
      this->drawNorthWestBuilding();
      this->drawSouthWestBuilding();
      this->drawCenterBuilding();

      this->drawGrassArea(275, 20);

      this->drawGrassArea(135, 185);
      this->drawGrassArea(410, 185);

      this->drawGrassArea(135, 285);
      this->drawGrassArea(410, 285);

      this->drawGrassArea(275, 355);
      this->drawGrassArea(275, 425);

      // Setup mirror
      glTranslatef(638.0f, 0.0f, 0.0f);
      glScalef(-1.0f, 1.0f, 1.0f);
      drawAxis();
      this->drawNorthWestBuilding();
      this->drawSouthWestBuilding();
      this->drawCenterBuilding();
    glPopMatrix();
  }

  void ShelbyCenter::drawCenterBuilding() {
    Vertices strip;
    // B 110	120	111	
    glColor3f(110.0/255.0, 120.0/255.0, 111.0/225.0);
    drawTriangle(/*1*/270, 105, /*2*/319, 155, /*3*/319, 105);
    // C 171	175	160	
    glColor3f(171.0/255.0, 175.0/255.0, 160.0/225.0);
    drawTriangle(/*1*/319, 155, /*2*/270, 201, /*3*/319, 201);
    // D 158	162	148	
    glColor3f(158.0/255.0, 162.0/255.0, 148.0/225.0);
    drawTriangle(/*1*/287, 105, /*2*/319, 137, /*3*/319, 105);
    drawQuad(/*1*/287, 105, /*2*/319, 105, /*3*/319, 90, /*4*/287, 90);
    // E 160	164	150	
    glColor3f(160.0/255.0, 164.0/255.0, 150.0/225.0);
    drawTriangle(/*1*/319, 171, /*2*/284, 201, /*3*/319, 201);
    glColor3f(160.0/255.0, 164.0/255.0, 150.0/225.0);
    drawQuad(/*1*/284, 201, /*2*/319, 201, /*3*/319, 208, /*4*/284, 208);
    // G 154	160	146	
    glColor3f(154.0/255.0, 160.0/255.0, 146.0/225.0);
    drawTriangle(/*1*/270, 201, /*2*/270, 105, /*3*/319, 155);
    // A 110	121	106:175	178	163	
    strip.clear();
    strip.push_back(std::make_pair(235, 134));
    strip.push_back(std::make_pair(242, 126));
    strip.push_back(std::make_pair(235+(277-235)/2.0, 134));
    strip.push_back(std::make_pair(270, 126));
    strip.push_back(std::make_pair(277, 134));
    glColor3f(110.0/255.0, 121.0/255.0, 106.0/225.0);
    drawTriangleStrip(strip);
    strip.clear();
    strip.push_back(std::make_pair(235, 134));
    strip.push_back(std::make_pair(242, 140));
    strip.push_back(std::make_pair(235+(277-235)/2.0, 134));
    strip.push_back(std::make_pair(270, 140));
    strip.push_back(std::make_pair(277, 134));
    glColor3f(175.0/255.0, 178.0/255.0, 163.0/225.0);
    drawTriangleStrip(strip);
  }

  void ShelbyCenter::drawNorthWestBuilding() {
    Vertices strip;
    // A 155	159	145	
    glColor3f(155.0/255.0, 159.0/255.0, 145.0/225.0);
    drawTriangle(/*1*/58, 80, /*2*/93, 119, /*3*/58, 155);
    // C 113	121	110	
    strip.clear();
    strip.push_back(std::make_pair(58,80));
    strip.push_back(std::make_pair(93,119));
    strip.push_back(std::make_pair(58+(241-58)/2.0,80));
    strip.push_back(std::make_pair(205,119));
    strip.push_back(std::make_pair(241,80));
    glColor3f(113.0/255.0, 121.0/255.0, 110.0/225.0);
    drawTriangleStrip(strip);
    // B 155	160	146:136	144	131	
    glColor3f(155.0/255.0, 160.0/255.0, 146.0/225.0);
    drawTriangle(/*1*/58, 77, /*2*/76, 77, /*3*/76, 95);
    glColor3f(136.0/255.0, 144.0/255.0, 131.0/225.0);
    drawTriangle(/*1*/76, 77, /*2*/94, 77, /*3*/76, 95);
    // D 157	161	147:135	143	128	
    glColor3f(157.0/255.0, 161.0/255.0, 147.0/225.0);
    drawTriangle(/*1*/204, 77, /*2*/223, 77, /*3*/223, 96);
    glColor3f(135.0/255.0, 143.0/255.0, 128.0/225.0);
    drawTriangle(/*1*/223, 77, /*2*/240, 77, /*3*/223, 96);
    // E 138	144	130	
    glColor3f(138.0/255.0, 144.0/255.0, 130.0/225.0);
    drawTriangle(/*1*/241, 80, /*2*/205, 119, /*3*/241, 156);
    // G 171	175	160	
    strip.clear();
    strip.push_back(std::make_pair(58, 155));
    strip.push_back(std::make_pair(93, 119));
    strip.push_back(std::make_pair(58+(241-58)/2.0, 155));
    strip.push_back(std::make_pair(205, 119));
    strip.push_back(std::make_pair(241, 155));
    glColor3f(171.0/255.0, 175.0/255.0, 160.0/225.0);
    drawTriangleStrip(strip);
    // F 155	163	147:134	141	129	
    glColor3f(155.0/255.0, 163.0/255.0, 147.0/225.0);
    drawTriangle(/*1*/204, 160, /*2*/223, 160, /*3*/223, 144);
    glColor3f(134.0/255.0, 141.0/255.0, 129.0/225.0);
    drawTriangle(/*1*/223, 160, /*2*/223, 144, /*3*/242, 160);
    // H 159	163	149	
    strip.clear();
    strip.push_back(std::make_pair(93, 123));
    strip.push_back(std::make_pair(60, 155));
    strip.push_back(std::make_pair(93, 123+(235-123)/2.0));
    strip.push_back(std::make_pair(60, 268));
    strip.push_back(std::make_pair(93, 235));
    glColor3f(159.0/255.0, 163.0/255.0, 149.0/225.0);
    drawTriangleStrip(strip);
    // I 136	142	128	
    strip.clear();
    strip.push_back(std::make_pair(93, 123));
    strip.push_back(std::make_pair(126, 155));
    strip.push_back(std::make_pair(93, 123+(235-123)/2.0));
    strip.push_back(std::make_pair(126, 268));
    strip.push_back(std::make_pair(93, 235));
    glColor3f(136.0/255.0, 142.0/255.0, 128.0/225.0);
    drawTriangleStrip(strip);
    // J 170	174	159	
    glColor3f(170.0/255.0, 174.0/255.0, 159.0/225.0);
    drawTriangle(/*1*/60, 268, /*2*/126, 268, /*3*/93, 235);
    // K 113	121	113:168	175	159	
    glColor3f(113.0/255.0, 121.0/255.0, 113.0/225.0);
    drawTriangle(/*1*/55, 233, /*2*/55, 249, /*3*/74, 249);
    glColor3f(168.0/255.0, 175.0/255.0, 159.0/225.0);
    drawTriangle(/*1*/55, 249, /*2*/74, 249, /*3*/55, 263);
    // L 113	122	111:172	177	159	
    glColor3f(113.0/255.0, 122.0/255.0, 111.0/225.0);
    drawTriangle(/*1*/130, 235, /*2*/118, 250, /*3*/130, 250);
    glColor3f(172.0/255.0, 177.0/255.0, 159.0/225.0);
    drawTriangle(/*1*/130, 250, /*2*/118, 250, /*3*/130, 264);
  }

  void ShelbyCenter::drawSouthWestBuilding() {
    Vertices strip;
    // A 151	159	146	
    glColor3f(151.0/255.0, 159.0/255.0, 146.0/225.0);
    drawTriangle(/*1*/57, 347, /*2*/99, 393, /*3*/57, 433);
    // B 105	115	107	
    strip.clear();
    strip.push_back(std::make_pair(57, 347));
    strip.push_back(std::make_pair(99, 393));
    strip.push_back(std::make_pair(57+(244-57)/2.0, 347));
    strip.push_back(std::make_pair(203, 393));
    strip.push_back(std::make_pair(244, 347));
    glColor3f(113.0/255.0, 121.0/255.0, 110.0/225.0);
    drawTriangleStrip(strip);
    // C 133	141	129	
    glColor3f(133.0/255.0, 141.0/255.0, 129.0/225.0);
    drawTriangle(/*1*/203, 393, /*2*/244, 347, /*3*/244, 434);
    // D 168	175	161	
    strip.clear();
    strip.push_back(std::make_pair(57, 433));
    strip.push_back(std::make_pair(99, 393));
    strip.push_back(std::make_pair(57+(244-57)/2.0, 433));
    strip.push_back(std::make_pair(203, 393));
    strip.push_back(std::make_pair(244, 433));
    glColor3f(168.0/255.0, 175.0/255.0, 161.0/225.0);
    drawTriangleStrip(strip);
    // E 150	158	145:133	141	131	
    glColor3f(150.0/255.0, 158.0/255.0, 145.0/225.0);
    drawTriangle(/*1*/58, 344, /*2*/76, 344, /*3*/76, 364);
    glColor3f(133.0/255.0, 141.0/255.0, 131.0/225.0);
    drawTriangle(/*1*/76, 344, /*2*/76, 364, /*3*/95, 344);
    // F 150	158	145:133	141	128	
    glColor3f(150.0/255.0, 158.0/255.0, 145.0/225.0);
    drawTriangle(/*1*/206, 344, /*2*/225, 344, /*3*/225, 364);
    glColor3f(133.0/255.0, 141.0/255.0, 128.0/225.0);
    drawTriangle(/*1*/225, 344, /*2*/225, 364, /*3*/243, 344);
    // G 150	157	146:131	139	128	
    glColor3f(150.0/255.0, 157.0/255.0, 146.0/225.0);
    drawTriangle(/*1*/76, 419, /*2*/76, 435, /*3*/58, 435);
    glColor3f(131.0/255.0, 139.0/255.0, 128.0/225.0);
    drawTriangle(/*1*/76, 419, /*2*/76, 435, /*3*/93, 435);
    // H 151	157	145:135	143	132	
    glColor3f(151.0/255.0, 157.0/255.0, 145.0/225.0);
    drawTriangle(/*1*/207, 435, /*2*/225, 435, /*3*/225, 420);
    glColor3f(135.0/255.0, 143.0/255.0, 132.0/225.0);
    drawTriangle(/*1*/225, 420, /*2*/225, 435, /*3*/242, 435);
  }

  void ShelbyCenter::drawGrassArea(GLfloat x, GLfloat y) {
    std::vector<Color> colors;
    colors.push_back(rgb("#556B2F"));
    colors.push_back(rgb("#6E8B3D"));
    colors.push_back(rgb("#556B2F"));
    colors.push_back(rgb("#6E8B3D"));
    GLfloat top_left[2] = {0.0f, 0.0f};
    GLfloat bottom_right[2] = {90.0f, 50.0f};
    glPushMatrix();
      glTranslatef(x, y, 0.0f);
      drawSmoothQuad(top_left, bottom_right, colors);
    glPopMatrix();
  }

}

