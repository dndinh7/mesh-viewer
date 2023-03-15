#version 400

in vec3 Intensity;

out vec4 FragColor;

void main()
{
   FragColor = vec4(Intensity, 1.0);
}
