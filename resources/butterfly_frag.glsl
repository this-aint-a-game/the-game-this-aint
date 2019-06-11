#version 330 core

uniform vec3 MatDif;

out vec4 Outcolor;


void main() {
    Outcolor = vec4(MatDif,1);
}

