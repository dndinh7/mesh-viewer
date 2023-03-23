#version 400

struct LightSource {
   vec4 pos; // position of light in eye coordinates
   vec3 La;  // light ambience
   vec3 Ld;  // light diffusion
   vec3 Ls;  // light specular
};

uniform LightSource Light;

struct MaterialProp {
   vec3 Ka; // reflect ambience
   vec3 Kd; // reflect diffusion
   vec3 Ks; // reflect specular
   float alpha; // specular exponent factor
   vec3 color; // original material color
};

uniform MaterialProp Material;

in vec3 n_eye;
in vec4 p_eye;

out vec4 FragColor;

vec3 phong() {
   // vector to the light source
   vec3 s;
   vec3 n= normalize(n_eye);

   if (Light.pos.w == 0.0f) // directional light source
      s= normalize(vec3(Light.pos));
   else // positional light source
      s= normalize(vec3(Light.pos - p_eye));

   vec3 v= normalize(vec3(-p_eye)); // vector to camera

   vec3 ambient= Light.La * Material.Ka; // ambient light

   float sDotn= max(dot(s, n), 0.0f);
   vec3 diffuse=  Light.Ld * Material.Kd * sDotn; // diffuse color

   vec3 r= 2 * (sDotn) * n - s; // reflected light 
   vec3 specular= vec3(0.0f);

   // this condition checks so that we only calculate
   // specular when the angle between light and normal
   // is acute 
   if (sDotn > 0.0f)
      specular= Light.Ls * Material.Ks * pow(max(dot(r, v), 0), Material.alpha);
   return ambient + diffuse + specular;
}

void main()
{
   FragColor = vec4(phong(), 1.0);
}