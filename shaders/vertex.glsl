// shaders/vertex.glsl
#version 330 core
layout (location = 0) in vec2 aPos; // 2D vertex position

uniform mat4 projection;   // Orthographic projection
uniform mat3 transform;    // Our 2D transformation matrix (as mat3)

void main()
{
    // Apply the 2D transformation (mat3 * vec3)
    // We treat the 2D position as (x, y, 1) for the multiplication
    vec3 transformedPos = transform * vec3(aPos, 1.0);

    // Apply the projection matrix (mat4 * vec4)
    // Convert the transformed 2D point back to a vec4 for projection (z=0, w=1)
    gl_Position = projection * vec4(transformedPos.xy, 0.0, 1.0);
}