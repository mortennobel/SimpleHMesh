in vec4 position;
in vec3 normal;
in vec2 uv1;

uniform mat4 _mvProj;
uniform mat4 _mv;
uniform mat4 _lightMat;
uniform mat3 _norm;

out vec2 vvUv;
out vec3 vvNormal;
out vec4 vvShadowMapCoord;
out vec3 vvEcPosition;

void main(void) {
    vec4 v = position;
    gl_Position = _mvProj * v;
    vvEcPosition = (_mv * v).xyz;
    vvUv = uv1;
    vvNormal = normalize(_norm * normal);
    vvShadowMapCoord = _lightMat  * v;
}