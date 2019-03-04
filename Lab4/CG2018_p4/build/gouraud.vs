//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;

//vars to pass to the pixel shader
varying vec3 v_Color;

//here create uniforms for all the data we need here
uniform vec3 ka;
uniform vec3 ia;
uniform vec3 kd;
uniform vec3 id;
uniform vec3 is;
uniform vec3 ks;
uniform vec3 eye;
uniform vec3 light;
uniform float alpha;

void main()
{	
	//convert local coordinate to world coordinates
	vec3 wPos = (model * vec4( gl_Vertex.xyz, 1.0)).xyz;
	//convert local normal to world coordinates
	vec3 wNormal = (model * vec4( gl_Normal.xyz, 0.0)).xyz;

	vec3 V = normalize(wPos - eye);
	vec3 l = normalize(wPos - light);
	vec3 R = normalize(reflect(l, wNormal));

	float dotLN = dot(l, wNormal);
	dotLN = clamp(dotLN, 0.0, 1.0);

	float dotRV = dot (R, V);
	dotRV = clamp(dotRV, 0.0, 1.0);
	dotRV = pow(dotRV, alpha);

	//pass them to the pixel shader interpolated
	v_Color = ka*ia + kd*dotLN*id + ks*dotRV*is;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * gl_Vertex; //output of the vertex shader
}