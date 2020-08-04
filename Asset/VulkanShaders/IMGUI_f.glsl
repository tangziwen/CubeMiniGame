layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
void main() 
{
  out_Color = fragColor;//vec4( fragColor.rgb, fragColor.a);
}
