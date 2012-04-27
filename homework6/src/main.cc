/***
 * Homework Assignment #6
 * William Woodall
 * Comp 6400 - Computer Graphics
 * Tuesday April 24th, 2012
 */

/*!
 * Instructions for Use
 * ====================
 * `ASDW` and the arrow keys move the currently selected student.
 * Click and drag the mouse to pan and tilt the camera about the student.
 * Number keys go through the students `1-9`
 * Number key `0` allows you to take control of the camera separately.
 * The scroll wheel or the `-` and `+` keys adjust the brightness of 
   lights currently on.
 * The `,` key zooms in and the `.` key zooms the camera out.
 * `l` toggles the lighting setup.
 * `n` toggles the sun light on and off
 * `p` starts the tour of Shelby, press `p` again to cancel the tour.
 */

/* Homework Assignment #6:

COMP 5/6400 Programming Assignment 6: Collision Avoidance & Animation 

Duration: Two Weeks  (100 points)

Due Date:     Midnight, Monday, April 23, 2012

Requirements: 
  1 The window size should be at least 800x800.
  2 The window's title should be "COMP-5/6400 Assignment 6."
  3 This assignment should be an extension of your assignment 5.  However, you 
    can always improve what you have in assignment 5. 
  4 The main additional requirements are:
    a to allow the user to move the “camera” around the environment
    b to add collision avoidance feature.  This means people or objects cannot 
      penetrate each other and a student cannot pass through a wall.  
    c to define an automatic tour path that will show off interesting features 
      in your design.  This can be achieved by hitting a specific key and the 
      automatic tour takes place
    d to add a spotlight to the camera that aims at the point where the camera 
      is pointing to (you may have this in Assignment 5 already)
    e to allow the user to maneuver multiple students and/or other objects in 
      the environment

  5 Control of objects (students, etc.) and the viewpoint (camera) must be 
    separate, but can be done simultaneously. 
  6 Obviously the user can only choose one mode between 4.a and 4.c. However, 
    the object control for 4.e is independent from 4.a and 4.c.
  7 The viewing must be done using perspective projection.  
  8 The minimum requirements for the camera control include:  move 
    forward/backward, turn right and left, and look upward and downward.  
    Again common sense applies, e.g., cannot pass through any objects.
  9 A switch is needed to switch to/from the spotlight mode.
 10 The lighting control and effects from assignment 5 must be kept.

Description:

  Use your imagination to define areas that are not clearly specified.

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
#include <limits>

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
using std::numeric_limits;

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
  Model() : force_smooth_shading(true), draw_collision(true) {}
  ~Model() {}

  bool loadModelFromFile(string file_name) {
    this->file_name_ = file_name;
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
      if (this->force_smooth_shading || (*obj_it).smooth_shadding) {
        glShadeModel(GL_SMOOTH);
      } else {
        glShadeModel(GL_FLAT);
      }
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
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   material.Kd);
      glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, material.Ns);
      if (0) {
      // if (this->draw_collision && this->file_name_ == "shelby.obj" && obj_it->name == "Cube.019_Cube.021") {
      // if (this->file_name_ == "shelby.obj") {
        // printf(":: %f, %f, %f,, %f, %f, %f\n", obj_it->mbb.max.x, obj_it->mbb.max.y, obj_it->mbb.max.z, obj_it->mbb.min.x, obj_it->mbb.min.y, obj_it->mbb.min.z);
        glBegin(GL_QUADS);
          glVertex3f(obj_it->mbb.max.x, obj_it->mbb.max.y, 20.0);
          glVertex3f(obj_it->mbb.min.x, obj_it->mbb.max.y, 20.0);
          glVertex3f(obj_it->mbb.min.x, obj_it->mbb.min.y, 20.0);
          glVertex3f(obj_it->mbb.max.x, obj_it->mbb.min.y, 20.0);
        glEnd();
      }
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
              glNormal3f(vn.x, vn.z, vn.y);
            }
          }
        glEnd();
        face_count += 1;
      }
      glPopMatrix();
    }
    glPopMatrix();
  }

  bool force_smooth_shading, draw_collision;
public:
  struct Vertex {
    Vertex(GLfloat _x = 0.0f, GLfloat _y = 0.0f, GLfloat _z = 0.0f) {
      x = _x;
      y = _y;
      z = _z;
    }
    GLfloat x, y, z;
  };

  struct Face {
    vector<int> verterx_indices;
    vector<int> texture_indices;
    vector<int> normal_indices;
  };

  struct Material {
    Material() {
      name = "";
      Ns = 0.0f;
      Ka[0] = 0.0f;
      Ka[1] = 0.0f;
      Ka[2] = 0.0f;
      Kd[0] = 0.0f;
      Kd[1] = 0.0f;
      Kd[2] = 0.0f;
      Ks[0] = 0.0f;
      Ks[1] = 0.0f;
      Ks[2] = 0.0f;
    }
    string name;
    GLfloat Ns;
    GLfloat Ka[3];
    GLfloat Kd[3];
    GLfloat Ks[3];
  };

  struct CollisionBox {
    CollisionBox() {
      max = Vertex();
      min = Vertex();
    }
    Vertex max;
    Vertex min;
  };

  struct Object {
    Object() {
      name = "";
      material = "";
      smooth_shadding = true;
      mbb = CollisionBox();
    }
    string name;
    vector<Face> faces;
    string material;
    bool smooth_shadding;
    CollisionBox mbb;
  };

private:
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
        case 'K': // Ka, Kd, or Ks
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

  CollisionBox calculateMBB(Object &obj) {
    CollisionBox mbb;
    float limit = numeric_limits<float>::max();
    mbb.max.x = limit;
    mbb.max.y = limit;
    mbb.max.z = limit;
    mbb.min.x = limit;
    mbb.min.y = limit;
    mbb.min.z = limit;
    // For each face of the object
    vector<Face>::iterator face_it = obj.faces.begin();
    for (; face_it != obj.faces.end(); face_it++) {
      for (int i = 0; i < face_it->verterx_indices.size(); ++i) {
        Vertex vertex;
        try {
          vertex = this->verticies_.at(face_it->verterx_indices[i]);
        } catch (out_of_range &e) {
          continue;
        }
        if (vertex.x < -1000.0 || vertex.y < -1000.0 || vertex.z < -1000.0
         || vertex.x > 1000.0 || vertex.y > 1000.0 || vertex.z > 1000.0) {
          printf("================================================asdf\n");
          printf("Object: [%s]@%i - %f %f %f\n", obj.name.c_str(), face_it->verterx_indices[i], vertex.x, vertex.y, vertex.z);
        }
        if (mbb.max.x == limit || vertex.x > mbb.max.x) {
          mbb.max.x = vertex.x;
        }
        if (mbb.min.x == limit || vertex.x < mbb.min.x) {
          mbb.min.x = vertex.x;
        }
        if (mbb.max.y == limit || vertex.z > mbb.max.y) {
          mbb.max.y = vertex.z;
        }
        if (mbb.min.y == limit || vertex.z < mbb.min.y) {
          mbb.min.y = vertex.z;
        }
        if (mbb.max.z == limit || vertex.y > mbb.max.z) {
          mbb.max.z = vertex.y;
        }
        if (mbb.min.z == limit || vertex.y < mbb.min.z) {
          mbb.min.z = vertex.y;
        }
      }
    }
    return mbb;
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
            vertex.x = 0.0f;
            vertex.y = 0.0f;
            vertex.z = 0.0f;
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
            vertex.x = 0.0f;
            vertex.y = 0.0f;
            vertex.z = 0.0f;
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
    // Calculate the Minimum Bounding Boxes for each object
    vector<Object>::iterator it = this->objects_.begin();
    for (; it != this->objects_.end(); it++) {
      // printf("[%s] Bounding Box: \n", it->name.c_str());
      (*it).mbb = calculateMBB((*it));
      // printf("  max: %f %f %f\n", it->mbb.max.x, it->mbb.max.y, it->mbb.max.z);
      // printf("  min: %f %f %f\n", it->mbb.min.x, it->mbb.min.y, it->mbb.min.z);
    }
    return true;
  }

public:
  vector<Object> objects_;
  vector<Vertex> verticies_;
  vector<Vertex> normals_;
  map<string, Material> materials_;
  string file_name_;
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
  : enabled_(false), gl_light_number_(0)
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
    this->apply();
  }

  void disable() {
    if (this->enabled_) {
      this->enabled_ = false;
      lights_used[this->light_number] = false;
      glDisable(this->gl_light_number_);
    }
  }

  void toggle() {
    if (this->enabled_) {
      this->disable();
    } else {
      this->enable();
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

  void specular(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    this->specular_[0] = r;
    this->specular_[1] = g;
    this->specular_[2] = b;
    this->specular_[3] = a;
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
  }

  size_t light_number;

private:
  bool enabled_;
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

/*!
 * This is a class that represents any object in the scene.
 */
class SceneObject {
public:
  SceneObject() {
    this->position_[0] = 0;
    this->position_[1] = 0;
    this->position_[2] = 0;
    this->rotation_[0] = 0;
    this->rotation_[1] = 0;
    this->rotation_[2] = 0;
  }
  ~SceneObject() {}

  void enterCoordinateFrame() {
    glTranslatef(this->position_[0], this->position_[1], this->position_[2]);
    glRotatef(this->rotation_[0], 1, 0, 0);
    glRotatef(this->rotation_[1], 0, 1, 0);
    glRotatef(this->rotation_[2], 0, 0, 1);
  }

  void draw_only() {
    vector<Model>::iterator it = this->models_.begin();
    for (; it != this->models_.end(); it++) {
      (*it).draw();
    }
  }

  /*!
   * Draws the models in the scene object
   */
  void draw() {
    if (!this->loaded_) {
      this->load();
    }
    glPushMatrix();
      this->enterCoordinateFrame();
      this->draw_only();
    glPopMatrix();
  }

  /*!
   * Loads the model(s) for the scene object
   */
  void load() {
    if (this->loaded_) {
      return;
    }
    vector<string>::iterator it = this->model_paths_.begin();
    for (; it != this->model_paths_.end(); it++) {
      this->models_.push_back(Model());
      this->models_.back().loadModelFromFile((*it));
    }
    this->loaded_ = true;
  }

  /*!
   * Adds the model string to the models to be loaded.
   */
  void addModel(const string &path) {
    this->model_paths_.push_back(path);
  }

  void moveTo(GLfloat x, GLfloat y, GLfloat z) {
    this->position_[0] = x;
    this->position_[1] = y;
    this->position_[2] = z;
  }

  void move(GLfloat dx, GLfloat dy, GLfloat dz) {
    this->position_[0] += dx;
    this->position_[1] += dy;
    this->position_[2] += dz;
  }

  GLfloat* getPosition() {
    return this->position_;
  }

  void rotateTo(GLfloat roll, GLfloat pitch, GLfloat yaw) {
    this->rotation_[0] = roll;
    this->rotation_[1] = pitch;
    this->rotation_[2] = yaw;
  }

  void rotate(GLfloat droll, GLfloat dpitch, GLfloat dyaw) {
    this->rotation_[0] += droll;
    this->rotation_[1] += dpitch;
    this->rotation_[2] += dyaw;
  }

  GLfloat* getRotation() {
    return this->rotation_;
  }

  bool collidesWith(SceneObject &obj) {
    GLfloat * my_position = this->getPosition();
    GLfloat * their_position = obj.getPosition();
    vector<Model>::iterator my_model_it = models_.begin();
    for (; my_model_it != models_.end(); my_model_it++) {
      vector<Model>::iterator their_model_it = obj.models_.begin();
      for (; their_model_it != obj.models_.end(); their_model_it++) {
        Model my_model = (*my_model_it);
        Model their_model = (*their_model_it);
        vector<Model::Object>::iterator my_obj_it = my_model.objects_.begin();
        for (; my_obj_it != my_model.objects_.end(); my_obj_it++) {
          vector<Model::Object>::iterator their_obj_it =
            their_model.objects_.begin();
          for (; their_obj_it != their_model.objects_.end(); their_obj_it++) {
            Model::Object my_obj = (*my_obj_it);
            Model::Object their_obj = (*their_obj_it);
            if (compareMBB(my_obj, my_position, their_obj, their_position)) {
              return true;
            }
          }
        }
      }
    }
    return false;
  }

  bool compareMBB(Model::Object &my_obj, GLfloat *my_pos,
                  Model::Object their_obj, GLfloat *their_pos)
  {
    Model::CollisionBox lhs = my_obj.mbb;
    Model::CollisionBox rhs = their_obj.mbb;
    lhs.max.x += my_pos[0];
    lhs.max.y += my_pos[1];
    lhs.max.z += my_pos[2];
    lhs.min.x += my_pos[0];
    lhs.min.y += my_pos[1];
    lhs.min.z += my_pos[2];
    rhs.max.x += their_pos[0];
    rhs.max.y += their_pos[1];
    rhs.max.z += their_pos[2];
    rhs.min.x += their_pos[0];
    rhs.min.y += their_pos[1];
    rhs.min.z += their_pos[2];
    if (lhs.max.x < rhs.min.x) return false;
    if (lhs.min.x > rhs.max.x) return false;
    if (lhs.max.y < rhs.min.y) return false;
    if (lhs.min.y > rhs.max.y) return false;
    if (lhs.max.z < rhs.min.z) return false;
    if (lhs.min.z > rhs.max.z) return false;
    printf("==========\n");
    printf("[%s]\n", my_obj.name.c_str());
    printf("  max: %f %f %f\n", lhs.max.x, lhs.max.y, lhs.max.z);
    printf("  min: %f %f %f\n", lhs.min.x, lhs.min.y, lhs.min.z);
    printf("Collides with:\n");
    printf("[%s]\n", their_obj.name.c_str());
    printf("  max: %f %f %f\n", rhs.max.x, rhs.max.y, rhs.max.z);
    printf("  min: %f %f %f\n", rhs.min.x, rhs.min.y, rhs.min.z);
    printf("==========\n");
    return true;
  }

private:
  bool loaded_;
  vector<string> model_paths_;
  vector<Model> models_;
  GLfloat position_[3];
  GLfloat rotation_[3];

};

/*! A class to represent OpenGL cameras. */
class Camera : public SceneObject {
public:
  Camera() {}
  ~Camera() {}

  void draw() {
    GLfloat * rotation = SceneObject::getRotation();
    GLfloat rel_pos[3];
    rel_pos[0] = this->distance_;
    rel_pos[1] = 0;
    rel_pos[2] = 0;
    GLfloat r = rotation[0]*degrees_to_radians; // roll
    GLfloat p = rotation[1]*degrees_to_radians; // pitch
    GLfloat y = rotation[2]*degrees_to_radians; // yaw
    GLfloat R[3][3]; // 3D rotation matrix
    R[0][0] = cos(p)*cos(y);
    R[0][1] = sin(r)*sin(p)*cos(y) - cos(r)*sin(y);
    R[0][2] = sin(r)*sin(y) + cos(r)*sin(p)*sin(y);
    R[1][0] = cos(p)*sin(y);
    R[1][1] = cos(r)*cos(y) + sin(r)*sin(p)*sin(y);
    R[1][2] = cos(r)*sin(p)*sin(y) - sin(r)*cos(y);
    R[2][0] = -1*sin(p);
    R[2][1] = sin(r)*cos(p);
    R[2][2] = cos(r)*cos(p);
    GLfloat cam_pos[3];
    cam_pos[0] = rel_pos[0]*R[0][0] + rel_pos[1]*R[0][1] + rel_pos[2]*R[0][2];
    cam_pos[0] += this->target_[0];
    cam_pos[1] = rel_pos[0]*R[1][0] + rel_pos[1]*R[1][1] + rel_pos[2]*R[1][2];
    cam_pos[1] += this->target_[1];
    cam_pos[2] = rel_pos[0]*R[2][0] + rel_pos[1]*R[2][1] + rel_pos[2]*R[2][2];
    cam_pos[2] += this->target_[2] + 5.0f;
    gluLookAt(cam_pos[0], cam_pos[1], cam_pos[2],
              this->target_[0], this->target_[1], this->target_[2],
              0.0f, 0.0f, 1.0f);
  }

  void setTarget(GLfloat x, GLfloat y, GLfloat z) {
    this->target_[0] = x;
    this->target_[1] = y;
    this->target_[2] = z;
  }

  void setTarget(SceneObject &so) {
    GLfloat * position = so.getPosition();
    this->target_[0] = position[0];
    this->target_[1] = position[1];
    this->target_[2] = position[2];
  }

  void setDistance(GLfloat distance_to_target) {
    this->distance_ = distance_to_target;
  }

  void distance(GLfloat delta_distance) {
    this->distance_ += delta_distance;
  }

private:
  GLfloat target_[3];
  GLfloat distance_;
  
};

/***
 * Domain Specific Code
 */

/*!
 * This is a class that represents a person in 3D
 */
class Person : public SceneObject {
public:
  Person(int index_ = 0) : index(index_) {
    SceneObject::addModel("student.obj");
  }
  ~Person() {

  }

  /*!
   * Draws the person
   */
  void draw() {
    SceneObject::draw();
  }

  /*!
   * Loads the model(s) for the person
   */
  void load() {
    SceneObject::draw();
  }

  bool collidesWith(Person &person) {
    GLfloat * my_position = this->getPosition();
    GLfloat * their_position = person.getPosition();
    GLfloat distance = sqrt(pow(their_position[0]-my_position[0], 2.0)
                          + pow(their_position[1] - my_position[1], 2.0));
    if (fabs(distance) <= 1.5) {
      return true;
    }
    return false;
  }

  bool collidesWith(SceneObject &so) {
    return SceneObject::collidesWith(so);
  }

  int index;

private:

};

class Shelby : public SceneObject {
public:
  Shelby() {
    SceneObject::SceneObject();
    SceneObject::addModel("shelby.obj");

    Model::CollisionBox NE_GRASS;
    NE_GRASS.max.x = 25;
    NE_GRASS.min.x = 6;
    NE_GRASS.max.y = -11;
    NE_GRASS.min.y = -19.4;
    this->mbbs_.push_back(NE_GRASS);
    Model::CollisionBox NW2;
    NW2.max.x = -27;
    NW2.min.x = -41;
    NW2.max.y = -8;
    NW2.min.y = -39;
    this->mbbs_.push_back(NW2);
    Model::CollisionBox NW1;
    NW1.max.x = -8;
    NW1.min.x = -27;
    NW1.max.y = -25;
    NW1.min.y = -39;
    this->mbbs_.push_back(NW1);
    Model::CollisionBox SW;
    SW.max.x = -10.25;
    SW.min.x = -40.75;
    SW.max.y = 31;
    SW.min.y = 16;
    this->mbbs_.push_back(SW);
    Model::CollisionBox SE;
    SE.max.x = 40.43;
    SE.min.x = 9.87;
    SE.max.y = 31.14;
    SE.min.y = 16.49;
    this->mbbs_.push_back(SE);
    Model::CollisionBox NE1;
    NE1.max.x = 27.10;
    NE1.min.x = 7.66;
    NE1.max.y = -25.15;
    NE1.min.y = -39.1;
    this->mbbs_.push_back(NE1);
    Model::CollisionBox NE2;
    NE2.max.x = 40.87;
    NE2.min.x = 26.67;
    NE2.max.y = -8.19;
    NE2.min.y = -39.24;
    this->mbbs_.push_back(NE2);
  }
  ~Shelby() {

  }

  bool collidesWith(Person &person) {
    GLfloat * position = person.getPosition();
    Model::CollisionBox person_mbb;
    person_mbb.max.x = 1 + position[0];
    person_mbb.min.x = -0.5 + position[0];
    person_mbb.max.y = 0.5 + position[1];
    person_mbb.min.y = -0.5 + position[1];
    vector<Model::CollisionBox>::iterator it = this->mbbs_.begin();
    for (; it != this->mbbs_.end(); it++) {
      if (person_mbb.max.x < it->min.x) {
        return false;
      }
      if (person_mbb.min.x > it->max.x) {
        return false;
      }
      if (person_mbb.max.y < it->min.y) {
        return false;
      }
      if (person_mbb.min.y > it->max.y) {
        return false;
      }
    }
    return true;
  }

private:
  vector<Model::CollisionBox> mbbs_;

};

/***
 * Main program
 */

Person * person;
vector<Person> persons;
Shelby shelby;
SceneObject ground;
Camera main_camera;
Light the_sun;
Light positional_light;
Light spot_light;
bool touring;
size_t touring_index;

enum LightingStyle {
  no_selection = -1,
  spotlight = 0,
  other_light = 1,
  both = 2,
  no_lights = 3
};

LightingStyle current_lighting = spotlight;

bool collidesWithShelby(Person &person) {

  return false;
}

bool detectCollisions(Person &person) {
  // Compare with each other person
  vector<Person>::iterator it = persons.begin();
  for (; it != persons.end(); it++) {
    // As long as they are the same person
    if (person.index != it->index) {
      if (person.collidesWith((*it))) {
        printf("Person %i collides with person %i\n", person.index, it->index);
        return true;
      }
    }
  }
  return collidesWithShelby(person);
  // Compare with Shelby
  // if (person.collidesWith(shelby)) {
    // printf("Person %i collides with the Shelby center.\n", person.index);
    // return true;
  // }
  return false;
}

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
  // glutPostRedisplay();
}

vector<Model::Vertex> path, path_rot;

void setupPath() {
  path.push_back(Model::Vertex(41.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(40.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(39.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(38.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(37.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(36.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(35.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(34.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(33.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(32.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(31.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(30.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(29.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(28.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(27.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(26.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(25.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(24.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(23.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(22.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(21.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(20.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(19.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(18.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(17.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(16.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(15.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(14.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(13.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(12.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(11.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(10.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(9.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(8.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(7.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(6.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(5.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(4.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(3.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -170.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -165.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -160.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -155.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -150.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -145.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -140.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -130.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -125.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -120.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -115.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -110.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -105.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -100.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -95.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(2.076340, 38.258221, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.163496, 37.262028, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.250651, 36.265835, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.337807, 35.269642, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.424963, 34.273449, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.512119, 33.277256, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.599275, 32.281063, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.686430, 31.284868, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.773586, 30.288673, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.860742, 29.292479, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(2.947898, 28.296284, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.035054, 27.300089, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.122210, 26.303894, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.209365, 25.307699, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.296521, 24.311504, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.383677, 23.315310, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.470833, 22.319115, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.557989, 21.322920, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.645144, 20.326725, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.732300, 19.330530, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.819456, 18.334335, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.906612, 17.338140, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(3.993768, 16.341946, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(4.080924, 15.345751, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(4.168079, 14.349556, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(4.255235, 13.353361, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(4.342391, 12.357166, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(4.429547, 11.360971, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -95.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -100.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -105.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -110.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -115.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -120.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -125.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -130.000000));
  path.push_back(Model::Vertex(4.516703, 10.364777, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(3.809596, 9.657670, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(3.102489, 8.950563, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(2.395382, 8.243457, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(1.688275, 7.536350, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(0.981169, 6.829244, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(0.274062, 6.122137, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-0.433045, 5.415030, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-1.140152, 4.707924, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-1.847259, 4.000817, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-2.554365, 3.293710, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-3.261472, 2.586604, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-3.968579, 1.879497, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-4.675686, 1.172390, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-5.382792, 0.465283, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-6.089899, -0.241824, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-6.797006, -0.948930, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-7.504112, -1.656037, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-8.211219, -2.363144, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-8.918325, -3.070251, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-9.625432, -3.777358, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-10.332539, -4.484464, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-11.039645, -5.191571, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-11.746752, -5.898677, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-12.453858, -6.605784, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -130.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -125.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -120.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -115.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -110.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -105.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -100.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -95.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -80.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -75.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -70.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -65.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -60.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -55.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -50.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -45.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -40.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -35.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -30.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -25.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -20.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -15.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -10.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -5.000000));
  path.push_back(Model::Vertex(-13.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-12.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-11.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-10.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-9.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-8.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-7.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-6.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-5.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-4.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-3.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-2.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -5.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -10.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -15.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -20.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -25.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -30.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -35.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -40.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -45.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -50.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -55.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -60.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -65.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -70.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -75.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -80.000000));
  path.push_back(Model::Vertex(-1.160965, -7.312891, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-1.073809, -8.309085, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.986654, -9.305280, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.899498, -10.301475, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.812342, -11.297669, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.725186, -12.293864, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.638030, -13.290059, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.550875, -14.286254, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.463719, -15.282449, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.376563, -16.278643, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.289407, -17.274837, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.202252, -18.271032, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.115096, -19.267227, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(-0.027940, -20.263422, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(0.059216, -21.259617, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -80.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -75.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -70.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -65.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -60.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -55.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -50.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -45.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -40.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -35.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -30.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -25.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -20.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -15.000000));
  path.push_back(Model::Vertex(0.146371, -22.255812, 0));
  path_rot.push_back(Model::Vertex(0, 0, -10.000000));
  path.push_back(Model::Vertex(1.131179, -22.429461, 0));
  path_rot.push_back(Model::Vertex(0, 0, -10.000000));
  path.push_back(Model::Vertex(1.131179, -22.429461, 0));
  path_rot.push_back(Model::Vertex(0, 0, -5.000000));
  path.push_back(Model::Vertex(2.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, -5.000000));
  path.push_back(Model::Vertex(2.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(3.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(4.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(5.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(6.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(7.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(8.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(9.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(10.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(11.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(12.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(13.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(14.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(15.127374, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(16.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(17.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(18.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(19.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(20.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(21.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(22.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(23.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(24.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 5.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 10.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 15.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 20.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 25.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 30.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 35.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 40.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 45.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 50.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 55.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 60.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 65.000000));
  path.push_back(Model::Vertex(25.127373, -22.516617, 0));
  path_rot.push_back(Model::Vertex(0, 0, 70.000000));
  path.push_back(Model::Vertex(25.469393, -21.576923, 0));
  path_rot.push_back(Model::Vertex(0, 0, 70.000000));
  path.push_back(Model::Vertex(25.469393, -21.576923, 0));
  path_rot.push_back(Model::Vertex(0, 0, 75.000000));
  path.push_back(Model::Vertex(25.728212, -20.610998, 0));
  path_rot.push_back(Model::Vertex(0, 0, 75.000000));
  path.push_back(Model::Vertex(25.728212, -20.610998, 0));
  path_rot.push_back(Model::Vertex(0, 0, 80.000000));
  path.push_back(Model::Vertex(25.901861, -19.626190, 0));
  path_rot.push_back(Model::Vertex(0, 0, 80.000000));
  path.push_back(Model::Vertex(25.901861, -19.626190, 0));
  path_rot.push_back(Model::Vertex(0, 0, 85.000000));
  path.push_back(Model::Vertex(25.989017, -18.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 85.000000));
  path.push_back(Model::Vertex(25.989017, -18.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -17.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -16.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -15.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -14.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -13.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -12.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -11.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -10.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -9.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -8.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -7.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -6.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -5.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -4.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 90.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 85.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 80.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 75.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 70.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 65.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 60.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 55.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 50.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 45.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 40.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 35.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 30.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 25.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 20.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 15.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 10.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 5.000000));
  path.push_back(Model::Vertex(25.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(26.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(27.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(28.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(29.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(30.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(31.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(32.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(33.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(34.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(35.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(36.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(37.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(38.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(39.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(40.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, 0.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -5.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -10.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -15.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -20.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -25.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -30.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -35.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -40.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -45.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -50.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -55.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -60.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -65.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -70.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -75.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -80.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -85.000000));
  path.push_back(Model::Vertex(41.989017, -3.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -4.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -5.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -6.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -7.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -8.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -9.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -10.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -11.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -12.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -13.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -14.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -15.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -16.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -17.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -18.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -19.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -20.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -21.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -22.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -23.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -24.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -25.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -26.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -27.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -28.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -29.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -30.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -31.629995, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -32.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -33.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -34.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -35.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -36.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -37.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -38.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -39.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -40.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -90.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -95.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -100.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -105.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -110.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -115.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -120.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -125.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -130.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -135.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -140.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -145.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -150.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -155.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -160.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -165.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -170.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -185.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(41.989017, -41.629997, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(40.992825, -41.717152, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(39.996632, -41.804306, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(39.000439, -41.891460, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(38.004246, -41.978615, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(37.008053, -42.065769, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(36.011860, -42.152924, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(35.015667, -42.240078, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(34.019474, -42.327232, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(33.023281, -42.414387, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(32.027088, -42.501541, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(31.030893, -42.588696, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(30.034698, -42.675850, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(29.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -175.000000));
  path.push_back(Model::Vertex(29.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(28.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(27.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(26.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(25.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(24.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(23.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(22.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(21.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(20.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(19.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(18.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(17.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(16.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(15.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(14.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(13.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(12.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(11.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(10.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(9.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(8.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(7.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(6.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(5.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(4.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(3.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(2.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(1.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -180.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -185.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -190.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -195.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -200.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -205.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -210.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -215.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -220.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -225.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -230.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -235.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -240.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -245.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -250.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -255.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -260.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -265.000000));
  path.push_back(Model::Vertex(0.038504, -42.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -41.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -40.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -39.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -38.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -37.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -36.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -35.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -34.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -33.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -32.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -31.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -30.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -29.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -28.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -27.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -26.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -25.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -24.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -23.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -22.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -21.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -20.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -19.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -18.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -17.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -16.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -15.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -14.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -13.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -12.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -11.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -10.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -9.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
  path.push_back(Model::Vertex(0.038504, -8.763004, 0));
  path_rot.push_back(Model::Vertex(0, 0, -270.000000));
}

/*! Sets up the OpenGL environment */
void setup() {
  glShadeModel(GL_SMOOTH); /* enable smooth shading */
  glEnable(GL_LIGHTING); /* enable lighting */
  // This draws things in order of z depth for the camera
  glEnable(GL_DEPTH_TEST);

  touring = false;
  setupPath();

  main_camera.setTarget(0.0f, 0.0f, 0.0f);
  main_camera.rotate(0.0f, -60.0f, 180.0f);
  main_camera.setDistance(20.0f);

  spot_light.position(0.0f, 0.0f, 20.0f, 1.0f);
  spot_light.ambient(1.0, 1.0, 1.0, 1.0);
  spot_light.diffuse(1.0, 1.0, 1.0, 1.0);
  spot_light.spotCutoff(10.0f);
  spot_light.spotExponent(64);
  spot_light.direction(0, 0, -1);
  spot_light.enable();

  the_sun.position(0.0f, 0.0f, 75.0f, 0.0f);
  the_sun.ambient(1.0f, 1.0f, 1.0f, 1.0f);
  the_sun.diffuse(1.0f, 1.0f, 1.0f, 1.0f);
  the_sun.specular(0.0f, 0.0f, 0.0f, 1.0f);
  the_sun.enable();

  positional_light.position(0.0f, 20.0f, 10.0f, 1.0f);
  positional_light.ambient(0.0, 0.0, 0.0, 1.0);
  positional_light.diffuse(1.0, 1.0, 0.0, 1.0);

  Color c = rgb("#87CEFF");
  glClearColor(c.r, c.g, c.b, 1.0f);

  shelby.moveTo(0, 0, -0.1);
  ground.addModel("ground.obj");
  ground.moveTo(0, 0, -0.1);
  // Load the people
  for (int i = 0; i < 9; ++i) {
    persons.push_back(Person(i));
    persons.back().load();
    persons.back().moveTo(i*2.5, i*2.5, 0);
  }
  person = &persons[0];
}

/*! GLUT display callback */
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  main_camera.setTarget((*person));
  main_camera.draw();

  // Draw the environment
  shelby.draw();
  ground.draw();
  positional_light.apply();

  vector<Person>::iterator it = persons.begin();
  for (; it != persons.end(); it++) {
    (*it).draw();
  }
  if (person) {
    glPushMatrix();
      person->enterCoordinateFrame();
      spot_light.apply();
    glPopMatrix();
  }

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
  GLfloat heading, dx, dy;
  if (touring) {
    if (key == 'p') {
      touring = false;
      return;
    }
    return;
  }
  switch(key) {
    case 112: // p
      person = reinterpret_cast<Person *>(&main_camera);
      touring_index = 0;
      touring = true;
      return;
      break;
    case 48: // 0
      person = reinterpret_cast<Person *>(&main_camera);
      break;
    case 49: // 1
    case 50: // 2
    case 51: // 3
    case 52: // 4
    case 53: // 5
    case 54: // 6
    case 55: // 7
    case 56: // 8
    case 57: // 9
      person = &persons[key-49];
      break;
    case 27: // esc
      exit(0);
      break;
    case 97: // a
      person->rotate(0, 0, rotate);
      if (detectCollisions((*person))) {
        person->rotate(0, 0, -rotate);
      }
      break;
    case 100: // d
      person->rotate(0, 0, -rotate);
      if (detectCollisions((*person))) {
        person->rotate(0, 0, rotate);
      }
      break;
    case 108: // l
      toggleLighting();
      break;
    case 119: // w
      heading = person->getRotation()[2];
      dx = travel*cos(heading*degrees_to_radians);
      dy = travel*sin(heading*degrees_to_radians);
      person->move(dx, dy, 0);
      if (detectCollisions((*person))) {
        person->move(-dx, -dy, 0);
      }
      break;
    case 115: // s
      heading = person->getRotation()[2];
      dx = travel*cos(heading*degrees_to_radians);
      dy = travel*sin(heading*degrees_to_radians);
      person->move(-dx, -dy, 0);
      if (detectCollisions((*person))) {
        person->move(dx, dy, 0);
      }
      break;
    case 45: // -
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
    case 44: // ,
      main_camera.distance(1.0);
      break;
    case 46: // .
      main_camera.distance(-1.0);
      break;
    case 110: // n
      the_sun.toggle();
      break;
    default:
      printf("   Keyboard %c == %d\n", key, key);
      break;
  }
  GLfloat * position = person->getPosition();
  GLfloat * rotation = person->getRotation();
  // printf("path.push_back(Model::Vertex(%f %f 0));\n", position[0], position[1]);
  // printf("path_rot.push_back(Model::Vertex(0 0 %f));\n", rotation[2]);
  glutPostRedisplay();
}

/*! GLUT on special keyboard press callback */
void OnSpecialKeyboardPress(int key, int x, int y) {
  double travel = 1.0f;
  double rotate = 5.0f;
  GLfloat heading, dx, dy;
  if (touring) {
    return;
  }
  switch (key) {
    case GLUT_KEY_LEFT:
      person->rotate(0, 0, rotate);
      if (detectCollisions((*person))) {
        person->rotate(0, 0, -rotate);
      }
      break;
    case GLUT_KEY_RIGHT:
      person->rotate(0, 0, -rotate);
      if (detectCollisions((*person))) {
        person->rotate(0, 0, rotate);
      }
      break;
    case GLUT_KEY_UP:
      heading = person->getRotation()[2];
      dx = travel*cos(heading*degrees_to_radians);
      dy = travel*sin(heading*degrees_to_radians);
      person->move(dx, dy, 0);
      if (detectCollisions((*person))) {
        person->move(-dx, -dy, 0);
      }
      break;
    case GLUT_KEY_DOWN:
      heading = person->getRotation()[2];
      dx = travel*cos(heading*degrees_to_radians);
      dy = travel*sin(heading*degrees_to_radians);
      person->move(-dx, -dy, 0);
      if (detectCollisions((*person))) {
        person->move(dx, dy, 0);
      }
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
}

/*! GLUT on active (button pressed) mouse motion callback */
void onActiveMouseMotion(int x, int y) {
  if (dragging) {
    GLfloat dpitch = (old_y - y)/5.0f;
    GLfloat dyaw = (old_x - x)/5.0f;
    main_camera.rotate(0.0f, dpitch, dyaw);
    old_y = y;
    old_x = x;
    glutPostRedisplay();
  }
}

/*! GLUT on idle callback */
void onIdle() {
    glutPostRedisplay();
}

void onTimer(int val) {
  if (!touring) {
    glutTimerFunc(100, onTimer, 0);
    return;
  }
  if (touring_index == path.size()) {
    touring = false;
    touring_index = 0;
  }
  main_camera.setTarget(main_camera);
  main_camera.moveTo(path[touring_index].x, path[touring_index].y, path[touring_index].z);
  touring_index++;
  glutTimerFunc(100, onTimer, 0);
}

/*! Main function */
int main(int argc, char *argv[]) {
  // OpenGL setup
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(window_width, window_height);
  glutCreateWindow("COMP-5/6400 Assignment 6");

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

  // Setup GLUT idle function
  glutIdleFunc(onIdle);

  // Setup timer function
  glutTimerFunc(100, onTimer, 0);

  display();

  // Run opengl
  glutMainLoop();

  return 0;
}
