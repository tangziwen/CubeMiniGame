#version 450

layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
void main() {
  out_Color = vec4( fragColor.rgb, 1.0 );
}
