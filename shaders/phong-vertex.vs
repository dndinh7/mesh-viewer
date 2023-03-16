#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormals;
layout (location = 2) in vec2 vTextureCoords;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

out vec3 Intensity;

struct LightSource {
  vec4 Pos; // position of light in eye coordinates
  vec3 La;  // light ambiance
  vec3 Ld;  // light diffusion
  vec3 Ls;  // light specular
};

uniform LightSource Light;

struct MaterialProp {
  vec3 Ka;    // reflect ambiance
  vec3 Kd;    // reflect specular
  vec3 Ks;    // reflect specular
  float alpha; // specular exponent factor
};


uniform MaterialProp Material;

vec3 phong(vec4 p_eye, vec3 n_eye) {
  // vector to the light source
  vec3 s;
  if (Light.Pos.w == 0.0f) // directional light source
    s= normalize(vec3(Light.Pos));
  else // positional light source
    s= normalize(vec3(Light.Pos - p_eye));
  
  vec3 v= normalize(vec3(-p_eye)); // vector to camera

  vec3 ambient= Light.La * Material.Ka; // ambient light

  float sDotn= max(dot(s, n_eye), 0);
  vec3 diffuse=  Light.Ld * Material.Kd * sDotn; // diffuse color

  vec3 r= 2 * (sDotn) * n_eye - s; 
  vec3 specular= vec3(0.0f);

  // this condition checks so that we only calculate
  // specular when the angle between the light and normal
  // is acute
  if (sDotn > 0.0f)
    specular= Light.Ls * Material.Ks * pow(max(dot(r, v), 0), Material.alpha);
  return ambient + diffuse + specular;
}

void main()
{
   // get the normal and vertex position to eye coordinates
   vec3 n_eye= normalize(NormalMatrix * vNormals);
   vec4 p_eye= ModelViewMatrix * vec4(vPos, 1.0);
   
   Intensity= phong(p_eye, n_eye);

   gl_Position = MVP * vec4(vPos, 1.0);
}
