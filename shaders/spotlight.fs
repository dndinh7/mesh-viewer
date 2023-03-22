
#version 400

in vec3 n_eye;
in vec4 p_eye;

struct Spotlight {
  vec4 pos; // where the light is located
  vec3 intensity; // light intensity
  vec3 dir; // direction of light
  float exp; // angular attenuation factor
  float innerCutOff; // this is a inner cut off of the spotlight
  float outerCutOff; // this is the hard cutoff of light
};

uniform Spotlight Spot;

struct MaterialInfo {
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float alpha;
};

uniform MaterialInfo Material;

out vec4 FragColor;

vec3 phongSpot() {
  vec3 s;
  vec3 n= normalize(n_eye);

  if (Spot.pos.w == 0.0f) // directional light source
    s= normalize(vec3(Spot.pos));
  else // positional light source
    s= normalize(vec3(Spot.pos - p_eye));

  float theta= dot(-s, Spot.dir);

  // NOTE: the TERMINOLOGY: inner and outer are based on the ANGLE
  // not the current value, the bigger the angle between [0, 90],
  // the smaller the cos(angle) will be
  float epsilon= Spot.innerCutOff - Spot.outerCutOff;

  // interpolate the light intensity based on distance the theta (our current
  // angle) is between the inner and outer cutoff angles
  float intensity= max(min((theta - Spot.outerCutOff)/epsilon, 1.0f), 0.0f);

  vec3 ambient= Spot.intensity * Material.Ka;



  // normal spotlight calculations
  float spotFactor= pow(theta, Spot.exp);
  vec3 v= normalize(vec3(-p_eye)); // direction to camera
  vec3 h = normalize(v + s); // half way vector from 
  return ambient +
    spotFactor * Spot.intensity * intensity * (
      Material.Kd * max(dot(s, n), 0.0) +
      Material.Ks * pow(max(dot(h, n), 0.0f), Material.alpha));
  
}

void main()
{
  
  FragColor = vec4(phongSpot(), 1.0f);
}