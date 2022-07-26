%~2/glslc.exe %~1\engines\graphics\shaders\shader.vert -o %~1\engines\graphics\shaders\vert.spv
%~2/glslc.exe %~1\engines\graphics\shaders\shader.frag -o %~1\engines\graphics\shaders\frag.spv
%~2/glslc.exe %~1\engines\graphics\shaders\point_light.vert -o %~1\engines\graphics\shaders\point_light_vert.spv
%~2/glslc.exe %~1\engines\graphics\shaders\point_light.frag -o %~1\engines\graphics\shaders\point_light_frag.spv