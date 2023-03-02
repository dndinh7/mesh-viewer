//--------------------------------------------------
// Author: David Dinh
// Date: 2 March 2023
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include "plymesh.h"
#include "osutils.h"

using namespace std;
using namespace glm;
using namespace agl;

class MeshViewer : public Window {
public:
   MeshViewer() : Window() {
   }

   void setup() {
      models= GetFilenamesInDir("../models", "ply");
      mesh.load("../models/" + models[curModel]);
      numModels= models.size();
   }

   void mouseMotion(int x, int y, int dx, int dy) {
   }

   void mouseDown(int button, int mods) {
   }

   void mouseUp(int button, int mods) {
   }

   void scroll(float dx, float dy) {
      // cap at 10, so we don't get inside mesh
      radius= std::max(10.0f, dy+radius);
      std::cout << "changed radius to: " << radius << std::endl;
   }

   void keyUp(int key, int mods) {
      if (key == GLFW_KEY_N) { // next model
         curModel= (curModel + 1) % numModels;
         std::cout << "changed model to: " << models[curModel] << std::endl;
      } else if (key == GLFW_KEY_P) {
         curModel= (curModel - 1 + numModels) % numModels;
         std::cout << "changed model to: " << models[curModel] << std::endl;
      }
      mesh.load("../models/" + models[curModel]);
   }

   void draw() {
      float aspect = ((float)width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
      renderer.rotate(vec3(0,0,0));
      renderer.scale(vec3(0.5,0.5,0.5));
      renderer.translate(vec3(0,0,0));

      
      renderer.mesh(mesh);

      //renderer.cube(); // for debugging!
   }

protected:
   PLYMesh mesh;
   vec3 eyePos = vec3(10, 0, 0);
   vec3 lookPos = vec3(0, 0, 0);
   vec3 up = vec3(0, 1, 0);
   std::vector<string> models;
   int numModels;
   int curModel= 0;
   float radius= 10.0f;
   float elevation= M_PI / 4;
   float azimuth= M_PI / 2;
};

int main(int argc, char** argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}

