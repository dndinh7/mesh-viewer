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
      if (keyIsDown(GLFW_KEY_LEFT_SHIFT) && mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
         radius= std::max(10.0f, radius + dx + dy);
         cout << "radius: " << radius << endl;
      } else if (mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
         azimuth+= (float)dx * 0.01f;
         elevation+= (float)dy * 0.01f;

         // clamp between (-pi/2, pi/2), don't want the bounds since it might lead to weird rotation
         elevation= std::max(elevation, (float) (-M_PI/2) + 0.00001f);
         elevation= std::min(elevation, (float) (M_PI/2)  - 0.00001f);
      }
   }


   void mouseDown(int button, int mods) {
   }

   void mouseUp(int button, int mods) {
   }

   void scroll(float dx, float dy) {
      // cap at 10, so we don't get inside mesh
      radius= std::max(10.0f, radius-dy);
      cout << "radius: " << radius << endl;
   }

   void keyUp(int key, int mods) {
      if (key == GLFW_KEY_N) { // next model
         curModel= (curModel + 1) % numModels;
         mesh= PLYMesh();
         mesh.load("../models/" + models[curModel]);
         std::cout << "changed model to: " << models[curModel] << std::endl;
         elevation= 0;
         azimuth= M_PI;
      } else if (key == GLFW_KEY_P) {
         curModel= (curModel - 1 + numModels) % numModels;
         mesh= PLYMesh();
         mesh.load("../models/" + models[curModel]);
         std::cout << "changed model to: " << models[curModel] << std::endl;
         elevation= 0;
         azimuth= 0;
      }
   }

   void draw() {
      float aspect = ((float)width()) / height();
      vec3 maxBounds= mesh.maxBounds();
      vec3 minBounds= mesh.minBounds();
      

      // in a 10 x 10 x 10 view box
      vec3 scale= (maxBounds - minBounds);

      // do not want NaNs when we scale
      scale.x= (scale.x <= 0.000001f && scale.x >= -0.000001f) ? 1 : 9 / scale.x;
      scale.y= (scale.y <= 0.000001f && scale.y >= -0.000001f) ? 1 : 9 / scale.y;
      scale.z= (scale.z <= 0.000001f && scale.z >= -0.000001f) ? 1 : 9 / scale.z;

      // then we only want to take the minimum scale to ensure that it is within the box
      // but also scales each dimension by the same factor
      float min_scale= std::min(std::min(scale.x, scale.y), scale.z);
      scale.x= min_scale;
      scale.y= min_scale;
      scale.z= min_scale;

      // we then want to translate it lookPos - midPoint or just simply -midPoint
      vec3 midPoint= (maxBounds + minBounds) * 0.5f;
      midPoint= -midPoint;

      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
      renderer.rotate(vec3(0,0,0));

      renderer.scale(scale);
      renderer.translate(midPoint);
      eyePos.x= radius * sin(azimuth) * cos(elevation);
      eyePos.y= radius * sin(elevation);
      eyePos.z= radius * cos(azimuth) * cos(elevation);

      camZ= eyePos - lookPos;

      camX= cross(vec3(0, 1, 0), camZ);

      camY= cross(camZ, camX);

      renderer.lookAt(eyePos, lookPos, camY);
      renderer.mesh(mesh);

   }

protected:
   PLYMesh mesh;
   vec3 eyePos = vec3(10, 0, 0);
   vec3 lookPos = vec3(0, 0, 0);
   vec3 camX= vec3(1, 0, 0); // x axis of camera
   vec3 camY= vec3(0, 1, 0); // y axis of camera
   vec3 camZ= vec3(0, 0, 1); // z axis of camera
   std::vector<string> models;
   int numModels;
   int curModel= 0;
   float radius= 10.0f;
   float elevation= 0;
   float azimuth= M_PI;
};

int main(int argc, char** argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}

