#version 400

in vec3 n_eye;
in vec4 p_eye;


struct LightInfo {
  vec4 pos;
  vec3 intensity;
};

uniform LightInfo Light;

struct MaterialInfo {
  vec3 Ka; // ambient reflectivity
  vec3 Kd; // diffuse reflectivity
  vec3 outlineColor; // color of an edge
};

uniform MaterialInfo Material;

const int levels= 3; // how many "shadings"
const float scaleFactor= 1.0 / levels;
//const float edgeThreshold= 0.25f;

// texture information
uniform sampler2D diffuseTexture;
uniform bool HasUV;
in vec2 uv;
const float uvScale= 3.0f;

out vec4 FragColor;

vec3 toonify() {
  // normal
  vec3 n= normalize(n_eye);

  // dir from vertex to camera
  vec3 v= normalize(vec3(-p_eye));

  // threshold for if a vertex will be an edge or not
  // if (dot(v, n) < edgeThreshold) return Material.outlineColor;

  // dir to light
  vec3 s;
  
  if (Light.pos.w == 0.0f) // directional light source
    s= normalize(vec3(Light.pos));
  else // positional light source
    s= normalize(vec3(Light.pos - p_eye));

  // how closely s and n align
  float sDotn= max(dot(s, n), 0.0f);

  // this essentially categorizes the angle at which the light
  // is hitting the vertex into a level threshold and flooring it
  // to that color so that shading stays the same throughout
  // the range of angles
  vec3 diffuse= Material.Kd * floor(sDotn * levels) * scaleFactor;

  vec3 color= Light.intensity * (Material.Ka + diffuse);
  if (HasUV) {
    color= color * texture(diffuseTexture, uv * uvScale).xyz;
  }

  return color;
}

void main()
{
  FragColor = vec4(toonify(), 1.0);
}