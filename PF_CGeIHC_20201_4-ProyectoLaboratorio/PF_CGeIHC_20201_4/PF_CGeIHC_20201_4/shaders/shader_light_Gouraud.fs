#version 330 core

in vec3 ambient;
in vec3 diffuse;
in vec3 specular;

out vec4 FragColor;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;

void main()
{
    //Final
    vec3 result = (ambient * ambientColor) + (diffuse * diffuseColor) + (specular * specularColor);
    FragColor = vec4(result, 1.0f);
}