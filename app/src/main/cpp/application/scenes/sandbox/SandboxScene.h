#pragma once
// clay
#include <clay/graphics/common/Mesh.h>
#include <clay/application/common/BaseScene.h>
#include <clay/application/xr/AppXR.h>
#include <clay/graphics/common/Material.h>
#include <clay/entity/Entity.h>
#include <clay/graphics/common/Texture.h>
#include <clay/application/common/Resources.h>
#include <clay/gui/xr/ImGuiComponentXR.h>
#include <clay/application/xr/CameraControllerXR.h>
#include <clay/graphics/common/Font.h>
#include <clay/entity/render/TextRenderable.h>

class SandboxScene : public clay::BaseScene {
public:
    SandboxScene(clay::IApp& app);

    ~SandboxScene();

    void initialize() override;

    void update(float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void assembleResources();

    void destroyResources() override;

    clay::Resources* mpResources_;

    clay::Entity mCenterSphere_;
    clay::Entity mTexturedSphere_;
    // TODO for now populate these and try drawing both (instead of mTexturedSphere_)
    clay::Entity mTexturedSphereStencil_;
    clay::Entity mTexturedSphereSolid_;


    clay::Entity mPlaneEntity_;

    // TODO make hand(s) class to encapsulate the hand logic
    clay::Entity mLeftHandEntity_;
    clay::Entity mRightHandEntity_;

    clay::Entity mTextEntity_;

    unsigned int mSelectedSceneIdx = 0;

    clay::CameraControllerXR mCameraController_;

    bool mHighLight = false;

    clay::Audio* mpBeepDeepAudio_ = nullptr;
};

