#version 330 core

out vec4 Outcolor;

in OUT_struct {
   vec3 fPos;
   vec3 fragNor;
   vec2 vTexCoord;
   vec3 butterflyPosition;
} in_struct;

void main() {
    vec4 ambient = vec4(0.1,0.1,0.1,0.1);
    Outcolor = vec4(in_struct.fragNor, 1) + ambient;
    vec3 lightDirection = in_struct.butterflyPosition - in_struct.fPos;
    Outcolor += pow(2*clamp(dot(vec3(in_struct.fragNor), lightDirection), 0, 1),2) * vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
