// Credits to @MarieEveDube2
// https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

#version 410

in vec3 nearPoint; // nearPoint calculated in vertex shader
in vec3 farPoint;

in mat4 fragView;
in mat4 fragProj;

uniform float mainGridScale;
uniform float secondaryGridScale;

uniform float near;
uniform float far;

out vec4 color;

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));

    vec2 absPos = abs(fragPos3D.xz);

    color.z += step(absPos.x, 0.1 * minimumx);
    color.x += step(absPos.y, 0.1 * minimumz);

    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos, 1.0);
    return (clip_space_pos.z / clip_space_pos.w) * 0.5 + 0.5;
}

float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    color = (grid(fragPos3D, mainGridScale) + grid(fragPos3D, secondaryGridScale)) * float(t > 0); // adding multiple resolution for the grid
    color.a *= fading;
}
