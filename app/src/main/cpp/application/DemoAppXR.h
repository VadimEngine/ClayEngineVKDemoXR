#pragma once
// class
#include <clay/application/xr/AppXR.h>

class DemoAppXR : public clay::AppXR  {
public:
    struct SceneDetail {
        std::string mName_;
        std::string mDetails_;
        VkDescriptorSet mPreviewTexture_ = VK_NULL_HANDLE;
        std::function<void()> mLoadScene_;
    };

    DemoAppXR(clay::XRSystem* pAndroidApp);

    void initialize() override;

    void CreateResources() override;

// private:

    std::vector<SceneDetail> mSceneDetails_;

    VkDescriptorSet mSandboxImguiTextureId;
    VkDescriptorSet mSpaceImguiTextureId;
    VkDescriptorSet mFarmImguiTextureId;

};
