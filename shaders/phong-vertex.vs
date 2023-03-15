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

vec3 phong(vec4 p_eye, vec3 n_eye, vec4 lightPos_eye) {
   // vector to the light source
   vec3 s;
   if (Light.Pos.w == 0.0f) // directional light source
      s= normalize(vec3(lightPos_eye));
   else // positional light source
      s= normalize(vec3(lightPos_eye - p_eye));
   
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
   // get the normal and vertex position to eye coordinates
   vec3 n_eye= normalize(NormalMatrix * vNormals);
   vec4 p_eye= ModelViewMatrix * vec4(vPos, 1.0);
   vec4 lightPos_eye= ModelViewMatrix * Light.Pos;
   
   Intensity= phong(p_eye, n_eye, lightPos_eye);

   gl_Position = MVP * vec4(vPos, 1.0);
}
