//--------------------------------------------------
// Author: David Dinh
// Date: 2 March 2023
// Description: Loads PLY files in ASCII format
// References: https://learnopengl.com/Lighting/Materials    
// http://devernay.free.fr/cours/opengl/materials.html    (different materials)
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
    renderer.loadShader("normals", "../shaders/normals.vs", "../shaders/normals.fs");
    renderer.loadShader("phong-vertex", "../shaders/phong-vertex.vs", "../shaders/phong-vertex.fs");
    renderer.loadShader("phong-pixel", "../shaders/phong-pixel.vs", "../shaders/phong-pixel.fs");
    renderer.loadShader("spotlight", "../shaders/spotlight.vs", "../shaders/spotlight.fs");
    shaders.push_back("normals");
    shaders.push_back("phong-vertex");
    shaders.push_back("phong-pixel");
    shaders.push_back("spotlight");
    numShaders= 4;

    models= GetFilenamesInDir("../models", "ply");
    mesh.load("../models/" + models[curModel]);
    numModels= models.size();

    // change the light position here..
    this->lightPosition= vec4(2.5f, 5.0f, 10.0f, 1.0f); 
    this->spotlightPosition= vec4(2.5f, 10.0f, 2.5f, 1.0f);
  }

  void mouseMotion(int x, int y, int dx, int dy) {
    if (keyIsDown(GLFW_KEY_LEFT_SHIFT) && mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
        // more intuitive left shift mouse click only depending on horizontal movement
        radius= std::max(10.0f, radius - dx);
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
      azimuth= 0;
    } else if (key == GLFW_KEY_P) {
      curModel= (curModel - 1 + numModels) % numModels;
      mesh= PLYMesh();
      mesh.load("../models/" + models[curModel]);
      std::cout << "changed model to: " << models[curModel] << std::endl;
      elevation= 0;
      azimuth= 0;
    } else if (key == GLFW_KEY_S) {
      curShader= (curShader + 1) % numShaders;
      std::cout << "changed shader to: " << shaders[curShader] << std::endl;
    }

  }

  void initShaderVars() {
    // this is phong shader
    if (curShader > 0 && curShader < 3) {
      vec3 La= vec3(0.90f); // light ambience 
      vec3 Ld= vec3(0.90f); // light diffusion
      vec3 Ls= vec3(0.90f); // light specular

      vec4 lightPos_eye= renderer.viewMatrix() * this->lightPosition;
      
      renderer.setUniform("Light.La", La);
      renderer.setUniform("Light.Ld", Ld);
      renderer.setUniform("Light.Ls", Ls);
      renderer.setUniform("Light.pos", lightPos_eye);

      // silver material 
      vec3 Ka= vec3(0.19225f); // reflect ambiance
      vec3 Kd= vec3(0.50754f); // reflect diffusion
      vec3 Ks= vec3(0.508273f); // reflect specular
      float shininess= 128.0f * 0.4f;
      
      renderer.setUniform("Material.Ka", Ka);
      renderer.setUniform("Material.Kd", Kd);
      renderer.setUniform("Material.Ks", Ks);
      renderer.setUniform("Material.alpha", shininess);
    } else if (curShader >= 3) { // spotlight shader
      vec3 lightIntensity= vec3(0.90f);

      vec4 lightPos_eye= renderer.viewMatrix() * this->spotlightPosition;
      vec3 lightDir= normalize(vec3(0) - vec3(this->spotlightPosition));

      float lightExp= 5.0f;
      float lightAngle= 60.0f;

      renderer.setUniform("Spot.pos", lightPos_eye);
      renderer.setUniform("Spot.intensity", lightIntensity);
      renderer.setUniform("Spot.dir", lightDir);
      renderer.setUniform("Spot.exp", lightExp);
      renderer.setUniform("Spot.angle", lightAngle);

      // silver material
      vec3 Ka= vec3(0.19225f); // reflect ambiance
      vec3 Kd= vec3(0.50754f); // reflect diffusion
      vec3 Ks= vec3(0.508273f); // reflect specular
      float shininess= 128.0f * 0.4f;
      
      renderer.setUniform("Material.Ka", Ka);
      renderer.setUniform("Material.Kd", Kd);
      renderer.setUniform("Material.Ks", Ks);
      renderer.setUniform("Material.alpha", shininess);

    }
  }

  void draw() {
    float aspect = ((float)width()) / height();
    vec3 maxBounds= mesh.maxBounds();
    vec3 minBounds= mesh.minBounds();
    

    // in a 10 x 10 x 10 view box
    vec3 scale= (maxBounds - minBounds);

    // do not want NaNs when we scale
    scale.x= (scale.x <= 0.000001f && scale.x >= -0.000001f) ? 1 : 10 / scale.x;
    scale.y= (scale.y <= 0.000001f && scale.y >= -0.000001f) ? 1 : 10 / scale.y;
    scale.z= (scale.z <= 0.000001f && scale.z >= -0.000001f) ? 1 : 10 / scale.z;

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

    renderer.beginShader(shaders[curShader]);

    initShaderVars();

    renderer.mesh(mesh);
    renderer.endShader();
  }

protected:
  PLYMesh mesh;
  vec3 eyePos = vec3(10, 0, 0);
  vec3 lookPos = vec3(0, 0, 0);
  vec3 camX= vec3(1, 0, 0); // x axis of camera
  vec3 camY= vec3(0, 1, 0); // y axis of camera
  vec3 camZ= vec3(0, 0, 1); // z axis of camera

  // determines light position
  // 1.0f is a position, while 0.0f is a direction
  vec4 lightPosition;
  vec4 spotlightPosition;

  std::vector<string> models;
  std::vector<string> shaders;
  int numModels;
  int numShaders;
  int curShader= 0;
  int curModel= 0;
  float radius= 10.0f;
  float elevation= 0;
  float azimuth= 0;
};

int main(int argc, char** argv)
{
  MeshViewer viewer;
  viewer.run();
  return 0;
}

