@ECHO OFF

C:/VulkanSDK/1.3.296.0/Bin/glslc.exe static.vert -o static_vert.spv
C:/VulkanSDK/1.3.296.0/Bin/glslc.exe static.frag -o static_frag.spv

C:/VulkanSDK/1.3.296.0/Bin/glslc.exe shader.vert -o shader_vert.spv
C:/VulkanSDK/1.3.296.0/Bin/glslc.exe shader.frag -o shader_frag.spv

C:/VulkanSDK/1.3.296.0/Bin/glslc.exe shader_ubo.vert -o shader_ubo_vert.spv
C:/VulkanSDK/1.3.296.0/Bin/glslc.exe shader_ubo.frag -o shader_ubo_frag.spv

C:/VulkanSDK/1.3.296.0/Bin/glslc.exe Renderer2D_Quad.vert -o Renderer2D_Quad_vert.spv
C:/VulkanSDK/1.3.296.0/Bin/glslc.exe Renderer2D_Quad.frag -o Renderer2D_Quad_frag.spv
