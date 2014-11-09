layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 vvUv[];
in vec3 vvNormal[];
in vec4 vvShadowMapCoord[];
in vec3 vvEcPosition[];

out vec2 vUv;
out vec3 vNormal;
out vec4 vShadowMapCoord;
out vec3 vEcPosition;
out vec3 dist;


uniform vec2 _viewport;
//uniform vec2 WIN_SCALE;           // precisely equal to: WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0
//noperspective out vec3 dist;  // Vec3 containing the distances from a vertex to opposite edge

void main(void)
{

        vec2 p0 = _viewport * 0.5 * gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
        vec2 p1 = _viewport * 0.5 * gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
        vec2 p2 = _viewport * 0.5 * gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;

	    vec2 v0 = p2-p1;
	    vec2 v1 = p2-p0;
	    vec2 v2 = p1-p0;
	    float area = abs(v1.x*v2.y - v1.y * v2.x);

//
//        gl_Position = gl_in[0].gl_Position;
//	EmitVertex();

//
//        gl_Position = gl_in[1].gl_Position;
//	EmitVertex();

//
//        gl_Position = gl_in[2].gl_Position;
//	EmitVertex();

    dist = vec3(area/length(v0),0,0);
    vUv = vvUv[0];
    vNormal = vvNormal[0];
    vShadowMapCoord = vvShadowMapCoord[0];
    vEcPosition = vvEcPosition[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    dist = vec3(0,area/length(v1),0);
    vUv = vvUv[1];
    vNormal = vvNormal[1];
    vShadowMapCoord = vvShadowMapCoord[1];
    vEcPosition = vvEcPosition[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    dist = vec3(0,0,area/length(v2));
    vUv = vvUv[2];
    vNormal = vvNormal[2];
    vShadowMapCoord = vvShadowMapCoord[2];
    vEcPosition = vvEcPosition[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
}
