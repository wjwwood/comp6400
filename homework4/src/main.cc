/***
 * Homework Assignment #4
 * William Woodall
 * Comp 6400 - Computer Graphics
 * Friday March 22nd, 2012
 */

/* Homework Assignment #4:

  Requirements
  ============

  1.The window size should be at least 800x800.

  2.The window's title should be "COMP-5/6400 Assignment 4"

  3.Create a 3D environment (changing your 2-D Shelby Complex into 3-D) for
  the student you have created in assignment 3.

  4.Continue the work from assignments 1, 2 & 3 and allow the student to move
  around the environment.

  5.Use perspective projection, instead of orthographic projection.

  6.Allow the user to move the viewpoint around the environment. 

  Description
  ===========

  You can change your 2-D Shelby Complex in Assignments 1 & 2 into a 3-D
  version. There are usually many people, like students, faculty, staff, and
  visitors, etc. around the complex.  Common sense should apply here.  You do
  not need to consider object collision yet (i.e., allow an object, e.g., a
  student, to move through other objects) at this moment.  This problem will
  be solved in the next assignment.

  The design you have from assignment 3 should be enhanced.  A user should be
  able to direct at least one person, i.e., the main student character, to do
  certain maneuvers in the complex.

  As design tips, you may allow the user to zoom in and out the view,
  duplicate people (we are in virtual world …), etc.

  The design of your interface must be stated clearly in the comments of the
  source code.  Place the comments, in the beginning of your code.

*/

/*!
 * Instructions
 * ============
 * Set the model path string below to the the directory containing 
 * models like ground.obj.
 */

/***
 * Includes
 */

// STL includes
#include <stdlib.h>
#include <cstdio>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cmath>

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

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif
double degrees_to_radians = M_PI / 180.0;

using std::vector;
using std::map;
using std::string;
using std::fstream;
using std::cout;
using std::cerr;
using std::endl;
using std::getline;
using std::out_of_range;

/***
 * Global variables
 */
GLint window_width = 800;
GLint window_height = 800;

/* Set this to the directory containing the model files. */
string model_path = "/Users/william/devel/comp6400/homework4/models/";

/***
 * Utility Classes and Functions
 */

/*!
 * A class for loading and drawing obj files.
 */
class Model {
public:
  Model() {}
  ~Model() {}

  bool loadModelFromFile(string file_name) {
    // Parse it
    if (!this->parse_obj_(file_name)) {
      cerr << "Could not open the resource file: " << file_name << endl;
      return false;
    }
    return true;
  }

  void draw() {
    glPushMatrix();
    // For each object
    vector<Object>::iterator obj_it = this->objects_.begin();
    for (; obj_it != this->objects_.end(); obj_it++) {
      glPushMatrix();
      // Set smooth shading if set
      if ((*obj_it).smooth_shadding) {
        glShadeModel(GL_SMOOTH);
      }
      // Set the material for the object
      if (this->materials_.count((*obj_it).material) == 0) {
        cerr << "Object " << (*obj_it).name
             << " referenced an invalid material: "
             << (*obj_it).material << endl;
        return;
      }
      Material material = this->materials_[(*obj_it).material];
      // glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  material.Ks);
      // glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   material.Kd);
      // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   material.Ka);
      // glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, material.Ns);
      glColor4fv(material.Kd);
      // For each face of the object
      size_t face_count = 0;
      vector<Face>::iterator face_it = (*obj_it).faces.begin();
      for (; face_it != (*obj_it).faces.end(); face_it++) {
        // Draw triangles using the vertex indices
        glBegin(GL_QUADS);
          // For vertex in face
          for (int i = 0; i < 4; ++i) {
            size_t index = (*face_it).verterx_indices[i] - 1;
            Vertex v;
            try {
              v = this->verticies_[index];
            } catch (out_of_range &oor) {
              cerr << "Face " << face_count
                   << " of object " << (*obj_it).name
                   << " referenced invalid vertex "
                   << index << ", cannot continue drawing." << endl;
              return;
            }
            glVertex3f(v.x, v.y, v.z);
          }
        glEnd();
        face_count += 1;
      }
      glPopMatrix();
    }
    glPopMatrix();
  }

private:
  struct Vertex {
    GLfloat x, y, z;
  };

  struct Face {
    vector<int> verterx_indices;
  };

  struct Material {
    string name;
    GLfloat Ns;
    GLfloat Ka[3];
    GLfloat Kd[3];
    GLfloat Ks[3];
  };

  struct Object {
    string name;
    vector<Face> faces;
    string material;
    bool smooth_shadding;
  };

  bool parse_mtl_(string mtl_file_name) {
    fstream file_stream;
    file_stream.open((model_path+mtl_file_name).c_str(), fstream::in);
    if (!file_stream.is_open()) {
      cerr << "Cannot parse mtl file (" << mtl_file_name
           << "): not open." << endl;
      return false;
    }
    size_t mtl_current_line = 0;
    Material current_mtl;
    current_mtl.name = "";
    while (file_stream.good()) {
      string line;
      getline(file_stream, line);
      mtl_current_line += 1;
      // Skip the line if it is empty
      if (line.empty() || line.length() < 2) {
        continue;
      }
      string element_type = "";
      switch (line[0]) {
        case '#': // comment, skip
          break;
        case 'n': // newmtl
          if (line.substr(0, 7) == "newmtl ") {
            // Store the previous material if it is valid
            if (current_mtl.name != "") {
              this->materials_[current_mtl.name] = current_mtl;
            }
            // Make a new material with the name given on this line
            current_mtl = Material();
            current_mtl.name = line.substr(7);
          }
          break;
        case 'K': // Ka,Kd,orKs
          size_t matches;
          if (line.substr(0, 2) == "Ka") { // a Ka
            element_type = "Ka";
            matches = sscanf(line.c_str(), "Ka %f %f %f",
                   &current_mtl.Ka[0],
                   &current_mtl.Ka[1],
                   &current_mtl.Ka[2]);
          }
          if (line.substr(0, 2) == "Kd") { // a Kd
            element_type = "Kd";
            matches = sscanf(line.c_str(), "Kd %f %f %f",
                   &current_mtl.Kd[0],
                   &current_mtl.Kd[1],
                   &current_mtl.Kd[2]);
          }
          if (line.substr(0, 2) == "Ks") { // a Ks
            element_type = "Ks";
            matches = sscanf(line.c_str(), "Ks %f %f %f",
                   &current_mtl.Ks[0],
                   &current_mtl.Ks[1],
                   &current_mtl.Ks[2]);
          }
          if (!element_type.empty() && matches != 3) {
            cerr << "Failed to parse " << element_type
                 << " element at line "
                 << mtl_current_line << " of file "
                 << (model_path+mtl_file_name) 
                 << ": sscanf matched " << matches
                 << " elements but expected 3." << endl;
            return false;
          }
          break;
        case 'N': // Ns element
          if (line.substr(0, 3) == "Ns ") {
            size_t matches = sscanf(line.c_str(), "Ns %f", &current_mtl.Ns);
            if (matches != 1) {
              cerr << "Failed to parse Ns element at line "
                 << mtl_current_line << " of file "
                 << (model_path+mtl_file_name) 
                 << ": sscanf matched " << matches
                 << " elements but expected 1." << endl;
              return false;
            }
          }
          break;
        default:
          break;
      }
    }
    // Make sure the last material isn't lost
    if (!current_mtl.name.empty()) {
      this->materials_[current_mtl.name] = current_mtl;
    }
    return true;
  }

  bool parse_obj_(string obj_file_name) {
    fstream file_stream;
    file_stream.open((model_path+obj_file_name).c_str(), fstream::in);
    if (!file_stream.is_open()) {
      cerr << "Cannot parse obj file (" << obj_file_name
           << "): not open." << endl;
      return false;
    }
    if (!file_stream.is_open()) {
      cerr << "Cannot parse, obj file not open." << endl;
      return false;
    }
    // While not EOF or other error, parse lines into data used in drawing
    size_t obj_current_line = 0;
    Object current_object;
    current_object.name = "";
    while (file_stream.good()) {
      string line;
      getline(file_stream, line);
      obj_current_line += 1;
      // Skip the line if it is empty
      if (line.empty() || line.length() < 2) {
        continue;
      }
      switch (line[0]) {
        case '#': // comment, skip
          break;
        case 'o':
          if (line.substr(0,2) == "o ") {
            if (!current_object.name.empty()) {
              this->objects_.push_back(current_object);
            }
            current_object = Object();
            current_object.name = line.substr(2);
          }
          break;
        case 'm': // mtllib
          if (line.substr(0, 7) == "mtllib ") {
            string mtl_file_name = line.substr(7);
            if (mtl_file_name.empty()) {
              cerr << "No mtl file specified in mtllib element at line: "
                   << obj_current_line << endl;
            } else {
              this->parse_mtl_(mtl_file_name);
            }
          }
          break;
        case 'v': // vertex or vertex normal
          if (line.substr(0, 2) == "vn") { // a vertex normal

          }
          if (line.substr(0, 2) == "v ") { // just a vertex
            Vertex vertex;
            size_t matches = sscanf(line.c_str(), "v %f %f %f",
                                    &vertex.x,
                                    &vertex.z,
                                    &vertex.y);
            if (matches != 3) {
              cerr << "Failed to parse v element at line "
                 << obj_current_line << " of file "
                 << (model_path+obj_file_name) 
                 << ": sscanf matched " << matches
                 << " elements but expected 3." << endl;
              return false;
            }
            this->verticies_.push_back(vertex);
          }
          break;
        case 'f': // face
          if (line.substr(0, 2) == "f ") { // just a vertex
            Face face;
            face.verterx_indices.resize(4);
            size_t matches = sscanf(line.c_str(), "f %i %i %i %i",
                                    &face.verterx_indices[0],
                                    &face.verterx_indices[1],
                                    &face.verterx_indices[2],
                                    &face.verterx_indices[3]);
            if (matches != 4) {
              cerr << "Failed to parse f element at line "
                 << obj_current_line << " of file "
                 << (model_path+obj_file_name) 
                 << ": sscanf matched " << matches
                 << " elements but expected 4." << endl;
              return false;
            }
            current_object.faces.push_back(face);
          }
          break;
        case 'u': // usemtl line
          if (line.substr(0, 7) == "usemtl ") {
            current_object.material = line.substr(7);
          }
          break;
        case 's':
          if (line.substr(0,2) == "s ") {
            if (line.substr(2) == "off") {
              current_object.smooth_shadding = false;
            }
            if (line.substr(2) == "on") {
              current_object.smooth_shadding = true;
            }
          }
          break;
        default:
          break;
      }
    }
    // Make sure the last object isn't lost
    if (!current_object.name.empty()) {
      this->objects_.push_back(current_object);
    }
    return true;
  }

  vector<Object> objects_;
  vector<Vertex> verticies_;
  map<string, Material> materials_;
};

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
    this->person_model_.draw();
  }

  /*!
   * Loads the model(s) for the person
   */
  void load() {
    this->person_model_.loadModelFromFile("student.obj");
  }

private:
  Model person_model_;

};

/***
 * Main program
 */

Person3D person;
Model ground;
GLfloat origin[3] = {0.0f, 0.0f, 5.0f};
GLfloat camera[3] = {15.0f, 0.0f, 5.0f};
GLfloat rotation[3] = {0.0f, 0.0f, 0.0f};

GLfloat x_position = 0.0f;
GLfloat y_position = 0.0f;
GLfloat heading = 0.0f;

/*! Sets up the OpenGL environment */
void setup() {
  Color c = rgb("#8B8878");
  glClearColor(c.r, c.g, c.b, 1.0f);

  // GLfloat light_ambient[] = { 10.0, 10.0, 10.0, 10.0 };
  // GLfloat light_position[] = { 0.0, 0.0, 10.0, 0.0 };
  // glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  // glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  // glEnable(GL_LIGHTING);
  // glEnable(GL_LIGHT0);

  // This draws things in order of z depth for the camera
  glEnable(GL_DEPTH_TEST);

  string ground_model = "ground.obj";
  if (!ground.loadModelFromFile(ground_model.c_str())) {
    cerr << "Failed to load ground from file, quitting." << endl;
    exit(1);
  }
  person.load();
}

/*! GLUT display callback */
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
    gluLookAt(camera[0], camera[1], camera[2],
              origin[0], origin[1], origin[2],
              0.0f, 0.0f, 1.0f);
    glRotatef(rotation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(rotation[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rotation[2], 0.0f, 0.0f, 1.0f);

    // Draw the environment
    ground.draw();

    // move and rotate the student
    glPushMatrix();
      glTranslatef(x_position, y_position, 0.0f);
      glRotatef(heading, 0.0f, 0.0f, 1.0f);

      person.draw();
    glPopMatrix();
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
  double travel = 1.0f;
  double rotate = 5.0f;
  switch(key){
    case 27: // esc
      exit(0);
      break;
    case 97: // a
      heading += rotate;
      break;
    case 100: // d
      heading -= rotate;
      break;
    case 119: // w
      x_position += travel*cos(heading*degrees_to_radians);
      y_position += travel*sin(heading*degrees_to_radians);
      break;
    case 115: // s
      x_position -= travel*cos(heading*degrees_to_radians);
      y_position -= travel*sin(heading*degrees_to_radians);
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
  double travel = 1.0f;
  double rotate = 5.0f;
  switch (key) {
    case GLUT_KEY_LEFT:
      heading += rotate;
      break;
    case GLUT_KEY_RIGHT:
      heading -= rotate;
      break;
    case GLUT_KEY_UP:
      x_position += travel*cos(heading*degrees_to_radians);
      y_position += travel*sin(heading*degrees_to_radians);
      break;
    case GLUT_KEY_DOWN:
      x_position -= travel*cos(heading*degrees_to_radians);
      y_position -= travel*sin(heading*degrees_to_radians);
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
