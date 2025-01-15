@ECHO OFF

C:/VulkanSDK/1.3.296.0/Bin/glslc.exe static.vert -o static_vert.spv
C:/VulkanSDK/1.3.296.0/Bin/glslc.exe static.frag -o static_frag.spv

C:/VulkanSDK/1.3.296.0/Bin/glslc.exe shader.vert -o shader_vert.spv
C:/VulkanSDK/1.3.296.0/Bin/glslc.exe shader.frag -o shader_frag.spv
