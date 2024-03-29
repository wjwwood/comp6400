/***
 * Homework Assignment #5
 * William Woodall
 * Comp 6400 - Computer Graphics
 * Friday April 10th, 2012
 */

/*!
 * Instructions
 * ============
 * ASDW and the arrow keys move the student.
 * Click and drag the mouse to pan and tilt the camera.
 * The scroll wheel or the - and + keys adjust the lights.
 * `l` toggles the lighting setup.
 */

/* Homework Assignment #5:

  Requirements
  ============

  1.  The window size should be at least 800x800.

  2.  The window's title should be "COMP-5/6400 Assignment 5"

  3.    This assignment should be an extension of your assignment 4.  However, 
        you can always improve what you have in assignment 4.

  4.    If you feel that you can create more interesting objects than what you 
        already have, e.g., human-powered vehicles, trash cans, more students, 
        etc., please do so.

  5.    Define normal vectors for all polygons that are used to create objects 
        in your environment.

  6.    Add two light sources to your scene: a white light and a light of 
        another color of your choice.  The white light should be a moving spot 
        light mounted on the main-character student and the other light should 
        be a positional light at a location of your choosing.

  7.    Pressing ‘L’ should cycle through 4 different lighting conditions: 
        White spot light only (default), the other light only, both lights, 
        and no lights (ambient light only).

  8.    Add controls to change the amount of light in the scene.  Use ‘+’ to 
        increase and ‘–‘ to decrease. Both light sources should be 
        controllable.

  9.    Continue the design from assignment 4 that allows users to move the 
        viewpoint around.  The purpose is to see the lighting effect of the 
        environment from different angles.  A simple way to achieve this is to 
        set the view point to that of the student who has the spot light.

  10. Again, use your imagination for design.

  Description
  ===========

  In this assignment, we will explore OpenGL’s lighting model.  You will find 
  that once lighting is enabled, the 3D qualities of the scene become much 
  more apparent.  From this point on, all assignments will expect you to use 
  lighting.

  Useful hint:  Dividing a large polygon into multiple smaller ones will 
  provide much better lighting effect.

*/

/***
 * Includes
 */

// STL includes
#include <sstream>
#include <vector>
#include <iterator>
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
using std::istringstream;
using std::istream_iterator;
using std::copy;
using std::back_inserter;

/***
 * Global variables
 */
GLint window_width = 800;
GLint window_height = 800;

/* Set this to the directory containing the model files. */
string model_path = "models/";

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
      // if ((*obj_it).smooth_shadding) {
        // glShadeModel(GL_SMOOTH);
      // }
      // Set the material for the object
      if (this->materials_.count((*obj_it).material) == 0) {
        cerr << "Object " << (*obj_it).name
             << " referenced an invalid material: "
             << (*obj_it).material << endl;
        return;
      }
      Material material = this->materials_[(*obj_it).material];
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  material.Ks);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   material.Kd);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   material.Ka);
      glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, material.Ns);
      // For each face of the object
      size_t face_count = 0;
      vector<Face>::iterator face_it = (*obj_it).faces.begin();
      for (; face_it != (*obj_it).faces.end(); face_it++) {
        // Draw triangles using the vertex indices
        size_t number_of_verticies = (*face_it).verterx_indices.size();
        if (number_of_verticies == 3) {
          glBegin(GL_TRIANGLES);
        } else {
          glBegin(GL_QUADS);
        }
          // For vertex in face
          bool do_normals =
            (number_of_verticies == (*face_it).normal_indices.size());
          for (int i = 0; i < number_of_verticies; ++i) {
            size_t index = (*face_it).verterx_indices[i] - 1;
            size_t normal_index = 0;
            Vertex v, vn;
            if (do_normals) {
              normal_index = (*face_it).normal_indices[i] - 1;
            }
            try {
              v = this->verticies_[index];
              if (do_normals) {
                vn = this->normals_[normal_index];
              }
            } catch (out_of_range &oor) {
              cerr << "Face " << face_count
                   << " of object " << (*obj_it).name
                   << " referenced invalid vertex "
                   << index << ", cannot continue drawing." << endl;
              return;
            }
            glVertex3f(v.x, v.z, v.y);
            if (do_normals) {
              // glNormal3f(vn.x, vn.z, vn.y);
            }
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
    vector<int> texture_indices;
    vector<int> normal_indices;
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
            Vertex vertex;
            size_t matches = sscanf(line.c_str(), "vn %f %f %f",
                                    &vertex.x,
                                    &vertex.y,
                                    &vertex.z);
            if (matches != 3) {
              cerr << "Failed to parse vn element at line "
                   << obj_current_line << " of file "
                   << (model_path+obj_file_name) 
                   << ": sscanf matched " << matches
                   << " elements but expected 3." << endl;
              return false;
            }
            this->normals_.push_back(vertex);
          }
          if (line.substr(0, 2) == "v ") { // just a vertex
            Vertex vertex;
            size_t matches = sscanf(line.c_str(), "v %f %f %f",
                                    &vertex.x,
                                    &vertex.y,
                                    &vertex.z);
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
          if (line.substr(0, 2) == "f ") {
            Face face;
            vector<string> elements;
            istringstream iss(line.substr(2));
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter<vector<string> >(elements));
            // Now that you have the "elements" find out what kind
            // i.e.: v OR v/vt/vn OR v//vn
            size_t matches = 0;
            vector<string>::iterator it = elements.begin();
            for (; it != elements.end(); it++) {
              int v,vt,vn;
              string element = (*it);
              if (element.find("/") == string::npos) {
                // No /, therefore just a normal
                matches = sscanf(element.c_str(), "%i", &v);
                if (matches != 1) {
                  cerr << "Failed to parse f element at line "
                       << obj_current_line << " of file "
                       << (model_path+obj_file_name) 
                       << ": sscanf matched " << matches
                       << " elements but expected 1." << endl;
                  cerr << "    Matching `" << element 
                       << "` against `%i`." << endl;
                  return false;
                }
                face.verterx_indices.push_back(v);
              } else {
                // Has a / in it
                matches = 0;
                matches = sscanf(element.c_str(), "%i//%i",
                                 &v, &vn);
                if (matches < 2) {
                  // Try the vt format
                  matches = 0;
                  matches = sscanf(element.c_str(), "%i/%i/%i",
                                   &v, &vt, &vn);
                  if (matches != 3) {
                    cerr << "Failed to parse f element at line "
                         << obj_current_line << " of file "
                         << (model_path+obj_file_name) 
                         << ": sscanf matched " << matches
                         << " elements but expected 2 or 3." << endl;
                    return false;
                  }
                  face.verterx_indices.push_back(v);
                  face.texture_indices.push_back(vt);
                  face.normal_indices.push_back(vn);
                }
                // Otherwise it worked
                face.verterx_indices.push_back(v);
                face.normal_indices.push_back(vn);
              }
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
  vector<Vertex> normals_;
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

/*! A class to represent OpenGL lights. */
class Light {
public:
  static map<int,bool> lights_used;
private:
  static const size_t max_light = 7;
  static const size_t no_lights_left = 8;

  static size_t get_next_available_light_() {
    for (int i = 0; i <= max_light; ++i) {
      if (lights_used.count(i) == 0
       || lights_used[i] == false)
      {
        return i;
      }
    }
    return no_lights_left;
  }

public:
  Light()
  : enabled_(false), applied_(false), gl_light_number_(0)
  {
    this->ambient_[0] = 0;
    this->ambient_[1] = 0;
    this->ambient_[2] = 0;
    this->ambient_[3] = 1;
    this->diffuse_[0] = 1;
    this->diffuse_[1] = 1;
    this->diffuse_[2] = 1;
    this->diffuse_[3] = 1;
    this->specular_[0] = 1;
    this->specular_[1] = 1;
    this->specular_[2] = 1;
    this->specular_[3] = 1;
    this->position_[0] = 0;
    this->position_[1] = 0;
    this->position_[2] = 1;
    this->position_[3] = 0;
    this->attenuation_c_ = 1;
    this->attenuation_l_ = 0;
    this->attenuation_q_ = 0;
    this->direction_[0] = 0;
    this->direction_[1] = 0;
    this->direction_[2] = -1;
    this->spot_cutoff_ = 180;
    this->spot_exponent_ = 0;
  }
  ~Light() {
    this->disable();
  }

  void enable() {
    this->light_number = this->get_next_available_light_();
    if (this->light_number == no_lights_left) {
      throw "No OpenGL lights available.";
    }
    lights_used[this->light_number] = true;
    this->gl_light_number_ = GL_LIGHT0 + this->light_number;
    glEnable(this->gl_light_number_);
    this->enabled_ = true;
    if (!this->applied_) {
      this->apply();
    }
  }

  void disable() {
    if (this->enabled_) {
      this->enabled_ = false;
      lights_used[this->light_number] = false;
      glDisable(this->gl_light_number_);
    }
  }

  static size_t number_of_lights_left() {
    size_t lights_left = 0;
    for (int i = 0; i <= max_light; ++i) {
      if (lights_used.count(i) == 0
       || lights_used[i] == false)
      {
        lights_left++;
      }
    }
    return lights_left;
  }

  void position(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    this->position_[0] = x;
    this->position_[1] = y;
    this->position_[2] = z;
    this->position_[3] = w;
  }

  void spotCutoff(GLfloat spot_cutoff) {
    this->spot_cutoff_ = spot_cutoff;
  }

  void spotExponent(GLfloat spot_exponent) {
    this->spot_exponent_ = spot_exponent;
  }

  void direction(GLfloat x, GLfloat y, GLfloat z) {
    this->direction_[0] = x;
    this->direction_[1] = y;
    this->direction_[2] = z;
  }

  void ambient(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    this->ambient_[0] = r;
    this->ambient_[1] = g;
    this->ambient_[2] = b;
    this->ambient_[3] = a;
  }

  void diffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    this->diffuse_[0] = r;
    this->diffuse_[1] = g;
    this->diffuse_[2] = b;
    this->diffuse_[3] = a;
  }

  void brighter() {
    this->diffuse_[0] *= 1.05;
    this->diffuse_[1] *= 1.05;
    this->diffuse_[2] *= 1.05;
  }

  void dimmer() {
    this->diffuse_[0] /= 1.05;
    this->diffuse_[1] /= 1.05;
    this->diffuse_[2] /= 1.05;
  }

  void apply() {
    if (!this->enabled_) {
      return;
    }
    glLightfv(this->gl_light_number_, GL_POSITION, this->position_);
    glLightfv(this->gl_light_number_, GL_AMBIENT, this->ambient_);
    glLightfv(this->gl_light_number_, GL_DIFFUSE, this->diffuse_);
    glLightfv(this->gl_light_number_, GL_SPECULAR, this->specular_);
    glLightf (this->gl_light_number_, GL_CONSTANT_ATTENUATION,
              this->attenuation_c_);
    glLightf (this->gl_light_number_, GL_LINEAR_ATTENUATION,
              this->attenuation_l_);
    glLightf (this->gl_light_number_, GL_QUADRATIC_ATTENUATION,
              this->attenuation_q_);
    glLightf (this->gl_light_number_, GL_SPOT_CUTOFF, this->spot_cutoff_);
    if (this->spot_cutoff_ < 180.0f) {
        glLightfv(this->gl_light_number_, GL_SPOT_DIRECTION,
                  this->direction_);
        glLightf (this->gl_light_number_, GL_SPOT_EXPONENT,
                  this->spot_exponent_);
    }
    this->applied_ = true;
  }

  size_t light_number;

private:
  bool enabled_;
  bool applied_;
  GLenum gl_light_number_;
  GLfloat position_[4];
  GLfloat ambient_[4];
  GLfloat diffuse_[4];
  GLfloat specular_[4];
  GLfloat attenuation_c_;
  GLfloat attenuation_l_;
  GLfloat attenuation_q_;

  GLfloat spot_cutoff_;
  GLfloat spot_exponent_;
  GLfloat direction_[3];
};
map<int,bool> Light::lights_used = map<int,bool>();

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
Model shelby;
Light positional_light;
Light spot_light;

GLfloat origin[3] = {0.0f, 0.0f, 5.0f};
GLfloat camera[3] = {100.0f, 0.0f, 50.0f};
GLfloat rotation[3] = {0.0f, 0.0f, 0.0f};

GLfloat x_position = 0.0f;
GLfloat y_position = 0.0f;
GLfloat heading = 0.0f;

enum LightingStyle {
  no_selection = -1,
  spotlight = 0,
  other_light = 1,
  both = 2,
  no_lights = 3
};

LightingStyle current_lighting = spotlight;

void toggleLighting() {
  if (current_lighting == no_selection) {
    current_lighting = spotlight;
  } else {
    current_lighting = LightingStyle((int(current_lighting) + 1) % 4);
  }
  positional_light.disable();
  spot_light.disable();
  switch (current_lighting) {
    case spotlight:
      spot_light.enable();
      break;
    case other_light:
      positional_light.enable();
      break;
    case both:
      spot_light.enable();
      positional_light.enable();
      break;
    default:
      break;
  }
  glutPostRedisplay();
}

/*! Sets up the OpenGL environment */
void setup() {
  glShadeModel(GL_SMOOTH); /* enable smooth shading */
  glEnable(GL_LIGHTING); /* enable lighting */
  // This draws things in order of z depth for the camera
  glEnable(GL_DEPTH_TEST);

  spot_light.position(0.0f, 0.0f, 20.0f, 1.0f);
  spot_light.ambient(1.0, 1.0, 1.0, 1.0);
  spot_light.diffuse(1.0, 1.0, 1.0, 1.0);
  spot_light.spotCutoff(10.0f);
  spot_light.spotExponent(64);
  spot_light.direction(0, 0, -1);
  spot_light.enable();

  positional_light.position(0.0f, 20.0f, 10.0f, 1.0f);
  positional_light.ambient(1.0, 1.0, 0.0, 1.0);
  positional_light.diffuse(1.0, 1.0, 0.0, 1.0);

  Color c = rgb("#87CEFF");
  glClearColor(c.r, c.g, c.b, 1.0f);

  string shelby_model = "shelby.obj";
  if (!shelby.loadModelFromFile(shelby_model.c_str())) {
    cerr << "Failed to load ground from file, quitting." << endl;
    exit(1);
  }
  person.load();
}

/*! GLUT display callback */
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  gluLookAt(camera[0], camera[1], camera[2],
            origin[0], origin[1], origin[2],
            0.0f, 0.0f, 1.0f);
  glRotatef(rotation[0], 1.0f, 0.0f, 0.0f);
  glRotatef(rotation[1], 0.0f, 1.0f, 0.0f);
  glRotatef(rotation[2], 0.0f, 0.0f, 1.0f);

  // Draw the environment
  shelby.draw();
  positional_light.apply();

  // move and rotate the student
  glPushMatrix();
    glTranslatef(x_position, y_position, 0.0f);
    glRotatef(heading, 0.0f, 0.0f, 1.0f);

    person.draw();
    spot_light.apply();
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
    case 108: // l
      toggleLighting();
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
      // camera[0] += 2.5f;
      switch (current_lighting) {
        case spotlight:
          spot_light.dimmer();
          break;
        case other_light:
          positional_light.dimmer();
          break;
        case both:
          spot_light.dimmer();
          positional_light.dimmer();
          break;
        default:
          break;
      }
      break;
    case 61: // =
      // camera[0] -= 2.5f;
      switch (current_lighting) {
        case spotlight:
          spot_light.brighter();
          break;
        case other_light:
          positional_light.brighter();
          break;
        case both:
          spot_light.brighter();
          positional_light.brighter();
          break;
        default:
          break;
      }
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
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(window_width, window_height);
  glutCreateWindow("COMP-5/6400 Assignment 5");

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

  display();

  // Run opengl
  glutMainLoop();

  return 0;
}
