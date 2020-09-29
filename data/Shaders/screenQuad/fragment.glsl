#version 430 
out vec4 FragColor;
in vec2 TexCoords;

#include "../include/frameConstants.glsl"
#include "../include/tracing.glsl"
#include "../include/pbrt.glsl"
#include "../include/atmosphere.glsl"

uniform sampler2D hdrBuffer;
uniform sampler2D depthBuffer;
uniform float gamma;
uniform float exposure;

vec2 toClipSpace(vec2 uv)
{
	const float x = (2.0f * uv.x) - 1.0f;
	const float y = 1.0f - (2.0f * uv.y);
	return vec2(x, -y);
}
void main()
{
	const vec2 centeredUV = toClipSpace(TexCoords);
	const Ray r = GetCameraRay(centeredUV);


    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

	const Sphere s = Sphere(GetPlanetCenter(), earthRadius + atmosphereThickness);
    vec3 intersect;
	vec4 depth = texture(depthBuffer, TexCoords);
    if(Intersect(r, s, intersect))
    {
    	vec4 intersectionPoint = vec4(r.origin + r.dir*intersect.z, 1.0);
    	vec4 P = frame.viewProjectionMatrix * intersectionPoint;

    	// this means we doesn't hit the geomtery
    	if(P.z < toLinearDepth(depth.x) || toLinearDepth(depth.x) > frame.zFar)
    	{
	   		hdrColor = exp(-GetTotalExtinctionCoefAlongRay(r, intersect.z) / 6000000);
    	}
	}

	//hdrColor = left;
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // Gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);

    //vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    //FragColor = vec4(hdrColor, 1.0);
    //FragColor = vec4(TexCoords, 0, 1.0);
}