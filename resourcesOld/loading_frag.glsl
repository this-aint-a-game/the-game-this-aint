#version 330 core

uniform sampler2D loadScreenF0;
uniform sampler2D loadScreenF1;
uniform sampler2D loadScreenF2;
uniform sampler2D loadScreenF3;
uniform int t;

in vec2 texCoord;

out vec4 Outcolor;

void main() {
    vec3 color;

    if(t == 0) { color = texture(loadScreenF0, texCoord).rgb; }
    if(t == 1) { color = texture(loadScreenF1, texCoord).rgb; }
    if(t == 2) { color = texture(loadScreenF2, texCoord).rgb; }
    if(t == 3) { color = texture(loadScreenF3, texCoord).rgb; }
    
    Outcolor = vec4(color, 1.0);
}