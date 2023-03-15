#version 400

in vec3 normColor;

out vec4 FragColor;
void main()
{
   FragColor = vec4(normColor, 1.0);
}
