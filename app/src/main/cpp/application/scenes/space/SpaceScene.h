#pragma once
// clay
#include <clay/graphics/xr/GraphicsContextXR.h>
#include <clay/application/common/BaseScene.h>
#include <clay/application/common/Resources.h>
#include <clay/gui/xr/ImGuiComponentXR.h>
#include <clay/application/xr/CameraControllerXR.h>

class SpaceScene : public clay::BaseScene {
public:
    SpaceScene(clay::BaseApp& app);

    ~SpaceScene();

    void initialize() override;

    void update(float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void assembleResources();

    void destroyResources() override;

    clay::Resources* mpResources_;

    clay::Entity mSkyBoxEntity;
    clay::Entity mLeftHandEntity_;
    clay::Entity mRightHandEntity_;

    clay::Entity mPlanetEntity_;
    clay::Entity mSunSphere_;
    clay::Entity mMoonEntity_;

    clay::Entity mPlaneEntity_; // imgui

    float planetOrbitSpeed = -1.0f/60.0f;
    float moonOrbitSpeed = -2.0f/60.0f;
    float mPlanetOrbitRadius_ = 2.0f;
    float mMoonOrbitRadius_ = .5f;

    bool mUpdateSpace_ = true;

    unsigned int mSelectedSceneIdx = 0;

    clay::CameraControllerXR mCameraController_;
};

