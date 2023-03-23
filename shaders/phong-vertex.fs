#version 400

// texture information
uniform sampler2D diffuseTexture;
uniform bool HasUV;
in vec2 uv;
const float uvScale= 3.0f;

in vec3 Intensity;

out vec4 FragColor;

void main()
{
   vec3 color= Intensity;
   if (HasUV) color= color * texture(diffuseTexture, uv * uvScale).xyz;
   
   FragColor = vec4(color, 1.0);
}
