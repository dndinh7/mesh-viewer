#version 400

struct LightSource {
   vec4 Pos; // position of light in eye coordinates
   vec3 La;  
   vec3 Ld;  
   vec3 Ls;
};

uniform LightSource Light;

struct MaterialProp {
   vec3 Ka;
   vec3 Kd;
   vec3 Ks;
   float alpha; // specular exponent factor
};

uniform MaterialProp Material;

in vec3 n_eye;
in vec4 p_eye;

out vec4 FragColor;

vec3 phong() {
   // vector to the light source
   vec3 s;
   if (Light.Pos.w == 0.0f) // directional light source
      s= normalize(vec3(Light.Pos));
   else // positional light source
      s= normalize(vec3(Light.Pos - p_eye));
   
   vec3 v= normalize(-vec3(p_eye));

   vec3 ambient= Light.La * Material.Ka;

   float sDotn= max(dot(s, n_eye), 0);
   vec3 diffuse=  Light.Ld * Material.Kd * sDotn;

   vec3 r= 2 * (sDotn) * n_eye - s; 
   
   vec3 specular= Light.Ls * Material.Ks * pow(max(dot(r, v), 0), Material.alpha);
   return ambient + diffuse + specular;
}

void main()
{

   FragColor = vec4(phong(), 1.0);
}
