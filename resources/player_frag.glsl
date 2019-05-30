#version 330 core

out vec4 Outcolor;

in OUT_struct {
   vec3 fPos;
   vec3 fragNor;
   vec2 vTexCoord;
   vec3 butterflyPosition;
} in_struct;

void main() {
    float distance = distance(in_struct.butterflyPosition, in_struct.fPos.xyz);    
    vec3 lightDirection = normalize(in_struct.butterflyPosition - in_struct.fPos);
    float diffuse = 3*pow(1/distance*clamp(dot(vec3(in_struct.fragNor), lightDirection), 0, 1),1.1);

    Outcolor = vec4(in_struct.fragNor, 1);
    Outcolor.rgb += diffuse * vec3(1,1,1);
}
