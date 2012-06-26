attribute vec4 av4position;
attribute vec3 av3colour;
attribute vec2 av2texcoord;

uniform mat4 mvp;

varying vec3 vv3colour;
varying vec2 vTexCoord;

void main() 
{
	vv3colour = av3colour;
	vTexCoord = av2texcoord;
	gl_Position = mvp * av4position;
}
