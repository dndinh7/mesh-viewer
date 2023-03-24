//--------------------------------------------------
// Author: David Dinh
// Date: 2 March 2023
// Description: Renders the model and shader
// Orbit around the model by dragging left click
// Zoom in and out using the scroll wheel, or by shift + left click drag
// Change the model by press 'n' for next and 'p' for previous
// Change the shader by pressing 's'
// Change the texture by pressing 't'
// Make the light move/stop by pressing 'm'
// References: https://learnopengl.com/Lighting/Materials    
// http://devernay.free.fr/cours/opengl/materials.html    (different materials)
// https://learnopengl.com/Lighting/Light-casters (spotlight)
// --------------------------------------------------------------------
// MODEL FOR THE FLASHLIGHT: created by DJMaesen
// https://sketchfab.com/3d-models/flashlight-12d6911e84154d2ab485d983361df76f
//--------------------------------------------------
// TEXTURES:
// PINK MARBLE: https://i.pinimg.com/736x/c0/de/97/c0de974be4d6051e8c4efc2e4eb3d896.jpg
// CHESS BOARD: https://static.vecteezy.com/system/resources/thumbnails/004/249/098/small/abstract-background-black-and-white-chessboard-pattern-optical-illusion-texture-for-your-design-free-vector.jpg

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
    shaders.push_back("normals");
    shaders.push_back("phong-vertex");
    shaders.push_back("phong-pixel");
    shaders.push_back("spotlight");
    shaders.push_back("toon");
    shaders.push_back("fog");
    numShaders= shaders.size();
    
    for (string shaderName: shaders) {
      string path= "../shaders/" + shaderName;
      renderer.loadShader(shaderName, path + ".vs", path + ".fs");
    }

    // this is for rendering objects with a specified color (not part of the main)
    renderer.loadShader("only-color", "../shaders/only-color.vs", "../shaders/only-color.fs");

    textures.push_back("default-white");
    textures.push_back("chess-board");
    textures.push_back("pink-marble");
    numTextures= textures.size();

    for (string textureName: textures) {
      renderer.loadTexture(textureName, "../textures/" + textureName + ".png", 0);
    }


    models= GetFilenamesInDir("../models", "ply");
    mesh.load("../models/" + models[curModel]);
    numModels= models.size();

    // change the light positions here
    this->lightPosition= vec4(0.0f, 0.0f, -10.0f, 1.0f); 
    this->lightIntensity= vec3(0.9f);
  }

  void mouseMotion(int x, int y, int dx, int dy) {
    if (keyIsDown(GLFW_KEY_LEFT_SHIFT) && mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
        // more intuitive left shift mouse click only depending on horizontal movement
        // restrict it to 10 and the wall radius
        radius= std::min(std::max(10.0f, radius - dx), wallScale/2);
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
    radius= std::min(std::max(10.0f, radius-dy), wallScale/2);
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
    } else if (key == GLFW_KEY_T) {
      curTexture= (curTexture + 1) % numTextures;
      std::cout << "changed texture to: " << textures[curTexture] << std::endl;
    } else if (key == GLFW_KEY_M) {
      moveLight= !moveLight;
    }
  }

  void changeLightPos() {
    theta+= dt()*2;
    phi+= 0.4f*dt();

    this->lightPosition.x= lightRadius * sin(theta) * cos(phi);
    this->lightPosition.y= lightRadius * sin(phi);
    this->lightPosition.z= lightRadius * cos(theta) * cos(phi);

  }

  // this method sets the uniform variables for the shaders
  // default material color is red plastic
  void initShaderVars(string shaderType,
    vec3 Ka= vec3(0.1f), 
    vec3 Kd= vec3(0.775f, 0.0f, 0.0f),
    vec3 Ks= vec3(0.9f, 0.7f, 0.7f)) {

    float shininess= 128.0f * 0.25f;

    // this is phong shader
    if (shaderType == "phong-vertex" || shaderType == "phong-pixel") {

      // to get desired light position in eye coordinates (so the MV * lightPos)
      // undoes it
      vec4 lightPos_eye= renderer.viewMatrix() * this->lightPosition;
      
      renderer.setUniform("Light.intensity", lightIntensity);
      renderer.setUniform("Light.pos", lightPos_eye);
      
      renderer.setUniform("Material.Ka", Ka);
      renderer.setUniform("Material.Kd", Kd);
      renderer.setUniform("Material.Ks", Ks);
      renderer.setUniform("Material.alpha", shininess);
    } else if (shaderType == "spotlight") { // spotlight shader

      // these positions and direction indicate the spotlight
      // is right at the camera and the direction is forward
      //vec4 lightPos_eye= vec4(0.0, 0.0, 0.0, 1.0f);
      //vec3 lightDir= normalize(vec3(0, 0, -1));

      // spotlight position and direction
      vec4 lightPos_eye= renderer.viewMatrix() * this->lightPosition;
      vec3 lightDir= renderer.viewMatrix() * vec4(normalize(vec3(0) - 
        vec3(this->lightPosition)), 0.0f);

      float lightExp= 1.0f;
      float innerCutOff= cos(radians(15.0f));
      float outerCutOff= cos(radians(22.5f));

      renderer.setUniform("Spot.pos", lightPos_eye);
      renderer.setUniform("Spot.intensity", lightIntensity);
      renderer.setUniform("Spot.dir", lightDir);
      renderer.setUniform("Spot.exp", lightExp);
      renderer.setUniform("Spot.innerCutOff", innerCutOff);
      renderer.setUniform("Spot.outerCutOff", outerCutOff);
      
      renderer.setUniform("Material.Ka", Ka);
      renderer.setUniform("Material.Kd", Kd);
      renderer.setUniform("Material.Ks", Ks);
      renderer.setUniform("Material.alpha", shininess);
    } else if (shaderType == "toon") { // toon shader

      // to get desired light position in eye coordinates (so the MV * lightPos)
      // undoes it
      vec4 lightPos_eye= renderer.viewMatrix() * this->lightPosition;
     
      renderer.setUniform("Light.pos", lightPos_eye);
      renderer.setUniform("Light.intensity", lightIntensity);

      // green material 
      vec3 Ka= vec3(0.19225f); // reflect ambiance
      vec3 Kd= vec3(0.75, 0.6332, 0.11); // reflect diffusion
      vec3 outlineColor= vec3(1.0f);

      renderer.setUniform("Material.Ka", Ka);
      renderer.setUniform("Material.Kd", Kd);
      renderer.setUniform("Material.outlineColor", outlineColor);
    } else if (shaderType == "fog") { // fog
      // to get desired light position in eye coordinates (so the MV * lightPos)
      // undoes it
      vec4 lightPos_eye= renderer.viewMatrix() * this->lightPosition;
      
      renderer.setUniform("Light.intensity", lightIntensity);
      renderer.setUniform("Light.pos", lightPos_eye);
      
      renderer.setUniform("Material.Ka", Ka);
      renderer.setUniform("Material.Kd", Kd);
      renderer.setUniform("Material.Ks", Ks);
      renderer.setUniform("Material.alpha", shininess);

      renderer.setUniform("Fog.maxDist", wallScale * 0.90f);
      renderer.setUniform("Fog.minDist", wallScale/2);
      // this is gray fog
      vec3 c= vec3(0xab/255.0f, 0xae/255.0f, 0xb0/255.0f);
      // but I like the black fog better
      c= vec3(0.1f);
      renderer.setUniform("Fog.color", c);
    }
  }

  void draw() {
    float aspect = ((float)width()) / height();
    

    // this is to set the camera
    renderer.perspective(glm::radians(60.0f), aspect, 0.1f, wallScale*1.5f);

    // getting the camera pos
    eyePos.x= radius * sin(azimuth) * cos(elevation);
    eyePos.y= radius * sin(elevation);
    eyePos.z= radius * cos(azimuth) * cos(elevation);

    camZ= eyePos - lookPos;
    camX= cross(vec3(0, 1, 0), camZ);
    camY= cross(camZ, camX);

    renderer.lookAt(eyePos, lookPos, camY);

    // get the bounding box
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

    renderer.push();
      renderer.rotate(vec3(0,0,0));
      renderer.scale(scale);
      renderer.translate(midPoint);

      renderer.beginShader(shaders[curShader]);
        initShaderVars(shaders[curShader]);
        renderer.texture("diffuseTexture", textures[curTexture]);
        renderer.mesh(mesh);
      renderer.endShader();
    renderer.pop();

    if (moveLight) {
      changeLightPos();
    }

    renderer.push();
      renderer.translate(this->lightPosition);
      renderer.scale(vec3(1.75f));
      renderer.beginShader("only-color");
        renderer.setUniform("color", lightIntensity);
        renderer.cube();
      renderer.endShader();
    renderer.pop();
    

    renderer.beginShader("fog");
      renderer.texture("diffuseTexture", "chess-board");
      initShaderVars("fog", vec3(0.1f), vec3(0.5f), vec3(0.9f));


      vec3 floorCoords= vec3(0, -wallScale/2, 0);
      vec3 ceilingCoords= vec3(0, wallScale/2, 0.0f); 
      vec3 posXCoords= vec3(wallScale/2, 0, 0.0f);
      vec3 negativeXCoords= vec3(-wallScale/2, 0, 0.0f);
      vec3 posZCoords= vec3(0.0f, 0, wallScale/2);
      vec3 negativeZCoords= vec3(0.0f, 0, -wallScale/2);
      // flooring
      renderer.push();
        renderer.translate(floorCoords);
        renderer.scale(vec3(wallScale, 0.1f, wallScale));
        renderer.cube();
      renderer.pop();
      // ceiling
      renderer.push();
        renderer.translate(ceilingCoords);
        renderer.scale(vec3(wallScale, 0.1f, wallScale));
        renderer.cube();
      renderer.pop();

      // x-dir walls
      renderer.push();
        renderer.translate(posXCoords);
        renderer.scale(vec3(0.1f, wallScale, wallScale));
        renderer.cube();
      renderer.pop();

      renderer.push();
        renderer.translate(negativeXCoords);
        renderer.scale(vec3(0.1f, wallScale, wallScale));
        renderer.cube();
      renderer.pop();

      // z-dir walls
      renderer.push();
        renderer.translate(posZCoords);
        renderer.scale(vec3(wallScale, wallScale, 0.1f));
        renderer.cube();
      renderer.pop();

      renderer.push();
        renderer.translate(negativeZCoords);
        renderer.scale(vec3(wallScale, wallScale, 0.1f));
        renderer.cube();
      renderer.pop();
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
  vec3 lightIntensity;
  float lightRadius= 10.0f;
  bool moveLight= false; // will determine if the light moves or not

  // for the light to move
  float theta= M_PI;
  float phi= 0.0f;

  // distance between walls
  float wallScale= 60.0f;


  std::vector<string> models;
  std::vector<string> shaders;
  std::vector<string> textures;
  int numModels;
  int numShaders;
  int numTextures;
  int curShader= 0;
  int curModel= 0;
  int curTexture= 0;

  // sphere orbiting angles
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

