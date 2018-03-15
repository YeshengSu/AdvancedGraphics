#version 150 core 

uniform mat4 modelMatrix; 
uniform mat4 viewMatrix; 
uniform mat4 projMatrix; 
uniform vec4 nodeColour;

in vec3 position; 
in vec2 texCoord; 

out Vertex { 
  vec2 texCoord;
  vec4 colour;
} OUT;

 void main(void) { 
  gl_Position = (projMatrix * viewMatrix * modelMatrix)*vec4(position,1.0);
  
  OUT.texCoord = texCoord;
  OUT.colour = nodeColour; 
}