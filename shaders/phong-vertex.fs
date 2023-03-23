#version 400

uniform sampler2D diffuseTexture;

in vec2 uv;
in vec3 Intensity;

uniform bool HasUV;


out vec4 FragColor;

void main()
{
   vec3 color= Intensity;
   if (HasUV) {
      color= color * texture(diffuseTexture, uv * 10.0f).xyz;
   }
   
   FragColor = vec4(color, 1.0);
}
