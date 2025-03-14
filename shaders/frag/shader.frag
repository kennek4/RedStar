#version 330 core
out vec4 FragColor;

uniform float red = 1.0f;
uniform float green = 1.0f;
uniform float blue = 1.0f;
uniform float alpha = 1.0f;

vec4 colour;

void main() {
      colour.r = red;
      colour.g = green;
      colour.b = blue;
      colour.a = alpha;
      FragColor = colour; 
}
