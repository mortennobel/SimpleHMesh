in vec2 vUv;
in vec3 vNormal;
in vec3 vEcPosition;
in vec4 vShadowMapCoord;
in vec3 dist;

out vec4 fragColor;

uniform vec4 mainColor;
uniform sampler2D mainTexture;

#pragma include "assets/shaders/light.glsl"
#pragma include "assets/shaders/shadowmap.glsl"

void main(void)
{
    float d = min(dist[0],min(dist[1],dist[2]));
    vec3 normal = normalize(vNormal);
    vec3 directionalLight = getDirectionalLightDiffuse(normal,_dLight);
    vec3 pointLight = getPointLightDiffuse(normal,vEcPosition, _pLights);
    float visibility;
    if (SHADOWS){
        visibility = computeLightVisibility(vShadowMapCoord);
    } else {
        visibility = 1.0;
    }
    vec3 color = max((directionalLight+pointLight)*visibility,_ambient.xyz)*mainColor.xyz;

    vec4 wireColor = vec4(0.0,1.0,0.0,1.0);

    fragColor = mix(wireColor, vec4(texture(mainTexture,vUv).xyz*color, 1.0), d);
}
