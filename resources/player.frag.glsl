#version 330 core

out vec4 Outcolor;

in OUT_struct {
   vec3 fPos;
   vec3 fragNor;
   vec2 vTexCoord;
   vec3 butterflyPosition;
} in_struct;

void main() {    
    vec3 lightDirection = normalize(in_struct.butterflyPosition - in_struct.fPos);
    vec3 diffuse = pow(2*clamp(dot(vec3(in_struct.fragNor), lightDirection), 0, 1),2) * vec3(1.0f, 1.0f, 1.0f);

    Outcolor = vec4(in_struct.fragNor, 1);
    Outcolor.rgb += diffuse;
}
