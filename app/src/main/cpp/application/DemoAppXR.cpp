#include "DemoAppXR.h"
#include "application/scenes/sandbox/SandboxScene.h"
#include "application/scenes/space/SpaceScene.h"
#include "application/scenes/farm/FarmScene.h"


DemoAppXR::DemoAppXR(android_app* pAndroidApp) : AppXR(pAndroidApp) {

}

void DemoAppXR::initialize() {

}

void DemoAppXR::CreateResources() {
    AppXR::CreateResources();

    {
        // load font
        auto fontData = loadFileToMemory_XR("fonts/runescape_uf.ttf");
        auto vertexData = loadFileToMemory_XR("shaders/Text.vert.spv");
        auto fragmentData = loadFileToMemory_XR("shaders/Text.frag.spv");

        mpResources_->addResource<clay::Font>(
            std::make_unique<clay::Font>(*mpGraphicsContext_, fontData, vertexData, fragmentData),
            "Runescape"
        );
    }

    mpResources_->loadResource<clay::Audio>({"audio/beep_deep_1.wav"}, "DeepBeep");

    // create default sampler
    mpResources_->addResource<VkSampler>(
        std::make_unique<VkSampler>(mpGraphicsContext_->createSampler()),
        "Default"
    );
    {
        // linear sampler
        VkSampler linearSampler;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(mpGraphicsContext_->mPhysicalDevice_, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        // Use linear filtering for smoother texture sampling
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        // Wrapping mode – repeat or clamp depending on texture type
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        // Enable anisotropic filtering
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; // Only used with CLAMP_TO_BORDER
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        // Enable mipmapping
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = VK_LOD_CLAMP_NONE; // Or set based on mip levels
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler( mpGraphicsContext_->mDevice_, &samplerInfo, nullptr, &linearSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }

        mpResources_->addResource<VkSampler>(
            std::make_unique<VkSampler>(linearSampler),
            "Linear"
        );
    }
    // SandboxPreview Textures
    {
        auto vImageData = loadImageFileToMemory_XR("textures/SandboxPreview.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mpResources_->getResource<VkSampler>("Default"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pVTexture), "SandboxPreview");
    }
    // SpacePreview Textures
    {
        auto vImageData = loadImageFileToMemory_XR("textures/SpacePreview.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mpResources_->getResource<VkSampler>("Default"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pVTexture), "SpacePreview");
    }
    // FarmPreview Textures
    {
        auto vImageData = loadImageFileToMemory_XR("textures/FarmPreview.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mpResources_->getResource<VkSampler>("Default"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pVTexture), "FarmPreview");
    }
    // VImage Textures
    {
        auto vImageData = loadImageFileToMemory_XR("textures/V.png");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mpResources_->getResource<VkSampler>("Default"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pVTexture), "VTexture");
    }
    // solid texture
    {
        // solid image
        clay::utils::ImageData singleRGBA;
        singleRGBA.width = 1;
        singleRGBA.height = 1;
        singleRGBA.channels = 4; // RGBA
        singleRGBA.pixels = std::make_unique<unsigned char[]>(4);

        singleRGBA.pixels[0] = 255;
        singleRGBA.pixels[1] = 255;
        singleRGBA.pixels[2] = 255;
        singleRGBA.pixels[3] = 255;

        auto* pSolidTexture = new clay::Texture(*mpGraphicsContext_);
        pSolidTexture->initialize(singleRGBA);
        pSolidTexture->setSampler(*mpResources_->getResource<VkSampler>("Default"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pSolidTexture), "SolidTexture");
    }
    // Sun texture
    {
        auto vImageData = loadImageFileToMemory_XR("textures/sunmap.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mpResources_->getResource<VkSampler>("Linear"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pVTexture), "Sun");
    }
    // Moon texture
    {
        auto vImageData = loadImageFileToMemory_XR("textures/2k_moon.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mpResources_->getResource<VkSampler>("Linear"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pVTexture), "Moon");
    }
    // Earth texture
    {
        auto vImageData = loadImageFileToMemory_XR("textures/earthmap1k.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mpResources_->getResource<VkSampler>("Linear"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pVTexture), "Earth");
    }
    // Stars
    {
        auto vImageData = loadImageFileToMemory_XR("textures/8k_stars_milky_way.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mpResources_->getResource<VkSampler>("Linear"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pVTexture), "Stars");
    }
    // Cloudy sky
    {
        auto vImageData = loadImageFileToMemory_XR("textures/CloudSky.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mpResources_->getResource<VkSampler>("Linear"));

        mpResources_->addResource(std::unique_ptr<clay::Texture>(pVTexture), "CloudySky");
    }
    // sphere mesh
    {
        mpResources_->loadResource<clay::Mesh>({"models/Sphere.obj"}, "Sphere");
    }
    // cube mesh
    {
        mpResources_->loadResource<clay::Mesh>({"models/Cube.obj"}, "Cube");
    }
    // plane mesh
    {
        mpResources_->loadResource<clay::Mesh>({"models/Plane.obj"}, "Plane");
    }
    // hand meshes
    {
        mpResources_->loadResource<clay::Mesh>({"models/GloveLeft.obj"}, "GloveLeft");
        mpResources_->loadResource<clay::Mesh>({"models/GloveRight.obj"}, "GloveRight");
    }

    // pipeline (TextureDepth)
    {
        // shaders
        auto vertexFileData = loadFileToMemory_XR("shaders/Texture.vert.spv");
        VkShaderModule vertexShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_VERTEX_BIT, vertexFileData.data.get(), vertexFileData.size}
        );
        auto fragmentFileData = loadFileToMemory_XR("shaders/Texture.frag.spv");
        VkShaderModule fragmentShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_FRAGMENT_BIT, fragmentFileData.data.get(), fragmentFileData.size}
        );

        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShader,
                .pName = "main"
            }
        };
        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4)
            },
            {
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 64,
                .size = sizeof(glm::vec4)
            }
        };
        pipelineConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        pipelineConfig.imageCreateInfos = {
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            }
        };

        pipelineConfig.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        mpResources_->addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "TextureDepth"
        );
        // TODO delete after all pipelines are done with this
        vkDestroyShaderModule(mpGraphicsContext_->mDevice_, vertexShader, nullptr);
        vkDestroyShaderModule(mpGraphicsContext_->mDevice_, fragmentShader, nullptr);
    }
    // pipeline (TextureDepthStencil)
    {
        // shaders
        auto vertexFileData = loadFileToMemory_XR("shaders/Texture.vert.spv");
        VkShaderModule vertexShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_VERTEX_BIT, vertexFileData.data.get(), vertexFileData.size}
        );
        auto fragmentFileData = loadFileToMemory_XR("shaders/Texture.frag.spv");
        VkShaderModule fragmentShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_FRAGMENT_BIT, fragmentFileData.data.get(), fragmentFileData.size}
        );

        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShader,
                .pName = "main"
            }
        };
        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4)
            },
            {
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 64,
                .size = sizeof(glm::vec4)
            }
        };
        pipelineConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        pipelineConfig.imageCreateInfos = {
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            }
        };

        VkStencilOpState stencilState = {};
        stencilState.failOp = VK_STENCIL_OP_KEEP;
        stencilState.passOp = VK_STENCIL_OP_REPLACE;
        stencilState.depthFailOp = VK_STENCIL_OP_KEEP;
        stencilState.compareOp = VK_COMPARE_OP_ALWAYS;  // Equivalent to glStencilFunc(ALWAYS, ref, mask)
        stencilState.compareMask = 0xFF;
        stencilState.writeMask = 0xFF;
        stencilState.reference = 0xFF; // Set at draw time via vkCmdSetStencilReference if dynamic

        pipelineConfig.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_TRUE,
            .front = stencilState,
            .back = stencilState
        };

        pipelineConfig.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        mpResources_->addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "TextureDepthStencil"
        );
        vkDestroyShaderModule(mpGraphicsContext_->mDevice_, vertexShader, nullptr);
        vkDestroyShaderModule(mpGraphicsContext_->mDevice_, fragmentShader, nullptr);
    }
    // pipeline (TextureNoDepth)
    {
        // shaders
        auto vertexFileData = loadFileToMemory_XR("shaders/Texture.vert.spv");
        VkShaderModule vertexShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_VERTEX_BIT, vertexFileData.data.get(), vertexFileData.size}
        );
        auto fragmentFileData = loadFileToMemory_XR("shaders/Texture.frag.spv");
        VkShaderModule fragmentShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_FRAGMENT_BIT, fragmentFileData.data.get(), fragmentFileData.size}
        );

        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShader,
                .pName = "main"
            }
        };
        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4)
            },
            {
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 64,
                .size = sizeof(glm::vec4)
            }
        };

        pipelineConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        pipelineConfig.imageCreateInfos = {
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            }
        };

        pipelineConfig.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_FALSE,
            .depthWriteEnable = VK_FALSE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        mpResources_->addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "TextureNoDepth"
        );
        vkDestroyShaderModule(mpGraphicsContext_->mDevice_, vertexShader, nullptr);
        vkDestroyShaderModule(mpGraphicsContext_->mDevice_, fragmentShader, nullptr);
    }
    // pipeline (flat)
    {
        auto vertexFileData = loadFileToMemory_XR("shaders/Flat.vert.spv");
        VkShaderModule vertexShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_VERTEX_BIT, vertexFileData.data.get(), vertexFileData.size}
        );
        auto fragmentFileData = loadFileToMemory_XR("shaders/Flat.frag.spv");
        VkShaderModule fragmentShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_FRAGMENT_BIT, fragmentFileData.data.get(), fragmentFileData.size}
        );
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShader,
                .pName = "main"
            }
        };
        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4)
            },
            {
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 64,
                .size = sizeof(glm::vec4)
            }
        };

        pipelineConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        pipelineConfig.imageCreateInfos = {};

        pipelineConfig.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        mpResources_->addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "Flat"
        );
    }
    // pipeline (SolidStencil)
    {
        auto vertexFileData = loadFileToMemory_XR("shaders/Solid.vert.spv");
        VkShaderModule vertexShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_VERTEX_BIT, vertexFileData.data.get(), vertexFileData.size}
        );
        auto fragmentFileData = loadFileToMemory_XR("shaders/Solid.frag.spv");
        VkShaderModule fragmentShader = mpGraphicsContext_->createShader(
            {VK_SHADER_STAGE_FRAGMENT_BIT, fragmentFileData.data.get(), fragmentFileData.size}
        );
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShader,
                .pName = "main"
            }
        };
        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4)
            },
            {
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 64,
                .size = sizeof(glm::vec4)
            }
        };

        pipelineConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        pipelineConfig.imageCreateInfos = {};

        VkStencilOpState stencilState = {};
        stencilState.failOp = VK_STENCIL_OP_KEEP;
        stencilState.passOp = VK_STENCIL_OP_KEEP;
        stencilState.depthFailOp = VK_STENCIL_OP_KEEP;
        stencilState.compareOp = VK_COMPARE_OP_NOT_EQUAL;  // Equivalent to glStencilFunc(ALWAYS, ref, mask)
        stencilState.compareMask = 0xFF;
        stencilState.writeMask = 0x00;
        stencilState.reference = 0xFF;

        pipelineConfig.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_FALSE,
            .depthWriteEnable = VK_FALSE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_TRUE,
            .front = stencilState,
            .back = stencilState
        };

        pipelineConfig.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        mpResources_->addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "SolidStencil"
        );
    }

    // material
    // V Texture
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mpResources_->getResource<clay::Texture>("VTexture")->mImageView_,
                .sampler = mpResources_->getResource<clay::Texture>("VTexture")->mSampler_,
                .binding = 1
            }
        };

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "VTexture"
        );
    }
    // TextureDepthStencil
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("TextureDepthStencil")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mpResources_->getResource<clay::Texture>("VTexture")->mImageView_,
                .sampler = mpResources_->getResource<clay::Texture>("VTexture")->mSampler_,
                .binding = 1
            }
        };

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "VTextureStencil"
        );
    }
    // Single White
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mpResources_->getResource<clay::Texture>("SolidTexture")->mImageView_,
                .sampler = mpResources_->getResource<clay::Texture>("SolidTexture")->mSampler_,
                .binding = 1
            }
        };

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "SolidTexture"
        );
    }
    // Sun
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mpResources_->getResource<clay::Texture>("Sun")->mImageView_,
                .sampler = mpResources_->getResource<clay::Texture>("Sun")->mSampler_,
                .binding = 1
            }
        };

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Sun"
        );
    }
    // Moon
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mpResources_->getResource<clay::Texture>("Moon")->mImageView_,
                .sampler = mpResources_->getResource<clay::Texture>("Moon")->mSampler_,
                .binding = 1
            }
        };

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Moon"
        );
    }
    // Earth
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mpResources_->getResource<clay::Texture>("Earth")->mImageView_,
                .sampler = mpResources_->getResource<clay::Texture>("Earth")->mSampler_,
                .binding = 1
            }
        };

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Earth"
        );
    }
    // Stars
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("TextureNoDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mpResources_->getResource<clay::Texture>("Stars")->mImageView_,
                .sampler = mpResources_->getResource<clay::Texture>("Stars")->mSampler_,
                .binding = 1
            }
        };

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Stars"
        );
    }
    // cloudy
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("TextureNoDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mpResources_->getResource<clay::Texture>("CloudySky")->mImageView_,
                .sampler = mpResources_->getResource<clay::Texture>("CloudySky")->mSampler_,
                .binding = 1
            }
        };

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "CloudySky"
        );
    }
    // Flat
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("Flat")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {};

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Flat"
        );
    }
    // solid-stencil
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("SolidStencil")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {};

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "SolidStencil"
        );
    }
    // imgui frame
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mpResources_->getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = imguiImageView,
                .sampler = *mpResources_->getResource<VkSampler>("Linear"),
                .binding = 1
            }
        };

        mpResources_->addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "ImguiFrame"
        );
    }

    // Models
    // solid sphere
    {
        std::unique_ptr<clay::Model> pSolidSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pSolidSphereModel->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Sphere"));
        pSolidSphereModel->mMaterials_.push_back(mpResources_->getResource<clay::Material>("SolidTexture"));

        mpResources_->addResource<clay::Model>(std::move(pSolidSphereModel), "SolidSphere");
    }
    // v sphere
    {
        std::unique_ptr<clay::Model> pVSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pVSphereModel->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Sphere"));
        pVSphereModel->mMaterials_.push_back(mpResources_->getResource<clay::Material>("VTexture"));

        mpResources_->addResource<clay::Model>(std::move(pVSphereModel), "VSphere");
    }
    // v sphere-stencil
    {
        std::unique_ptr<clay::Model> pVSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pVSphereModel->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Sphere"));
        pVSphereModel->mMaterials_.push_back(mpResources_->getResource<clay::Material>("VTextureStencil"));

        mpResources_->addResource<clay::Model>(std::move(pVSphereModel), "VSphereStencil");
    }
    // v sphere solid highlight
    {
        std::unique_ptr<clay::Model> pVSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pVSphereModel->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Sphere"));
        pVSphereModel->mMaterials_.push_back(mpResources_->getResource<clay::Material>("SolidStencil"));

        mpResources_->addResource<clay::Model>(std::move(pVSphereModel), "VSphereSolid");
    }
    // sun
    {
        std::unique_ptr<clay::Model> pRenderable = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pRenderable->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Sphere"));
        pRenderable->mMaterials_.push_back(mpResources_->getResource<clay::Material>("Sun"));

        mpResources_->addResource<clay::Model>(std::move(pRenderable), "Sun");
    }
    // moon
    {
        std::unique_ptr<clay::Model> pRenderable = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pRenderable->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Sphere"));
        pRenderable->mMaterials_.push_back(mpResources_->getResource<clay::Material>("Moon"));

        mpResources_->addResource<clay::Model>(std::move(pRenderable), "Moon");
    }
    // earth
    {
        std::unique_ptr<clay::Model> pRenderable = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pRenderable->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Sphere"));
        pRenderable->mMaterials_.push_back(mpResources_->getResource<clay::Material>("Earth"));

        mpResources_->addResource<clay::Model>(std::move(pRenderable), "Earth");
    }
    // skybox (star)
    {
        std::unique_ptr<clay::Model> pRenderable = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pRenderable->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Cube"));
        pRenderable->mMaterials_.push_back(mpResources_->getResource<clay::Material>("Stars"));

        mpResources_->addResource<clay::Model>(std::move(pRenderable), "StarSkybox");
    }
    // skybox (cloudy)
    {
        std::unique_ptr<clay::Model> pRenderable = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pRenderable->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Sphere"));
        pRenderable->mMaterials_.push_back(mpResources_->getResource<clay::Material>("CloudySky"));

        mpResources_->addResource<clay::Model>(std::move(pRenderable), "CloudySkybox");
    }
    // Hands
    {
        std::unique_ptr<clay::Model> pLeftHandModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pLeftHandModel->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("GloveLeft"));
        pLeftHandModel->mMaterials_.push_back(mpResources_->getResource<clay::Material>("Flat"));
        pLeftHandModel->mColor_ = {.95f, .674f, .411f, 1.0f};

        mpResources_->addResource<clay::Model>(std::move(pLeftHandModel), "GloveLeft");

        std::unique_ptr<clay::Model> pRightHandModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pRightHandModel->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("GloveRight"));
        pRightHandModel->mMaterials_.push_back(mpResources_->getResource<clay::Material>("Flat"));
        pRightHandModel->mColor_ = {.95f, .674f, .411f, 1.0f};

        mpResources_->addResource<clay::Model>(std::move(pRightHandModel), "GloveRight");
    }
    // imgui
    {
        std::unique_ptr<clay::Model> pVSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pVSphereModel->mMeshes_.push_back(mpResources_->getResource<clay::Mesh>("Plane"));
        pVSphereModel->mMaterials_.push_back(mpResources_->getResource<clay::Material>("ImguiFrame"));
        mpResources_->addResource<clay::Model>(std::move(pVSphereModel), "ImguiPlane");
    }

    mSandboxImguiTextureId = ImGui_ImplVulkan_AddTexture(
        mpResources_->getResource<clay::Texture>("SandboxPreview")->mSampler_,
        mpResources_->getResource<clay::Texture>("SandboxPreview")->mImageView_,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
    mSpaceImguiTextureId = ImGui_ImplVulkan_AddTexture(
        mpResources_->getResource<clay::Texture>("SpacePreview")->mSampler_,
        mpResources_->getResource<clay::Texture>("SpacePreview")->mImageView_,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
    mFarmImguiTextureId = ImGui_ImplVulkan_AddTexture(
        mpResources_->getResource<clay::Texture>("FarmPreview")->mSampler_,
        mpResources_->getResource<clay::Texture>("FarmPreview")->mImageView_,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );

    // scene details
    {
        mSceneDetails_ = {
            {
                "Sandbox Scene",
                "Scene for experimenting features.",
                mSandboxImguiTextureId,
                [this]{
                    //mScenes_.front()->setRemove(true);
                    setScene(new SandboxScene(*this));
                }
            },
            {
                "Space Scene",
                "Space simulation with an orbiting solar system.",
                mSpaceImguiTextureId,
                [this]{
                    //mScenes_.front()->setRemove(true);
                    setScene(new SpaceScene(*this));
                }
            },
            {
                "Farm Scene",
                "Simple Nature Scene with future plans to include farming.",
                mFarmImguiTextureId,
                [this]{
                    //mScenes_.front()->setRemove(true);
                    setScene(new FarmScene(*this));
                }
            }
        };
    }

    mScenes_.front()->initialize();
}