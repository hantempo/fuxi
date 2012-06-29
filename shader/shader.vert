attribute vec3 in_position;
attribute vec3 in_normal;

uniform vec3 light_pos;
uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 inv_model;

varying float out_light_intensity;
varying vec3 out_color;

void main() 
{
    gl_Position = mvp * vec4(in_position, 1.0);

    vec4 temp = inv_model * vec4(in_normal, 1.0);
    vec3 normal_mv = normalize(temp.xyz);

    vec3 pos_mv = (mv * vec4(in_position, 1.0)).xyz;

    vec3 light_dir = normalize(light_pos - pos_mv);
    out_light_intensity = max(dot(light_dir, normal_mv), 0.0);
    out_color = light_dir;
}
