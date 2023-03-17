
#version 400

in vec3 n_eye;
in vec4 p_eye;

struct Spotlight {
  vec4 pos; // where the light is located
  vec3 intensity; // light intensity
  vec3 dir; // direction of light
  float exp; // angular attenuation factor
  float angle; // cutoff angle (in degrees)
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

  float _sDotDir= dot(-s, Spot.dir);

  float angle= acos(_sDotDir);
  float cutoff= radians(min(max(Spot.angle, 0.0f), 90.0f));
  vec3 ambient= Spot.intensity * Material.Ka;


  if (angle < cutoff) {
    float spotFactor= pow(_sDotDir, Spot.exp);
    vec3 v= normalize(vec3(-p_eye)); // direction to camera
    vec3 h = normalize(v + s); // half way vector from 
    return ambient +
      spotFactor * Spot.intensity * (
        Material.Kd * max(dot(s, n), 0.0) +
        Material.Ks * pow(max(dot(h, n), 0.0f), Material.alpha));
  } else {
    return ambient;
  }
  
}

void main()
{
  
  FragColor = vec4(phongSpot(), 1.0f);
}