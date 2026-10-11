[scene]
type="//meworla/shader"
name="water"
code_hlsl="//rhea/shaders/water.hlsl"
code_glsl_vs="//rhea/shaders/water_vs.glsl"
code_glsl_fs="//rhea/shaders/water_fs.glsl"

input="standard"
light_type="vertex"
texture_type="standard"

alpha_blending=true
depth_test=true
depth_write=false
cull_mode="none"