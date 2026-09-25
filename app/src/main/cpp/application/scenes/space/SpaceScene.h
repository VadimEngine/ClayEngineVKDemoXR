#pragma once
// clay
#include <clay/graphics/xr/GraphicsContextXR.h>
#include <clay/application/common/BaseScene.h>
#include <clay/gui/xr/ImGuiComponentXR.h>
#include <clay/application/xr/CameraControllerXR.h>
#include <clay/graphics/common/SkyBox.h>
#include <clay/ecs/EntityManager.h>

class SpaceScene : public clay::BaseScene {
public:
    SpaceScene(clay::BaseApp& app);

    ~SpaceScene();

    void initialize() override;

    void update(float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void assembleResources();

    void destroyResources() override;

    clay::ecs::EntityManager mEntityManager_;

    clay::ecs::Entity mLeftHandEntity_;
    clay::ecs::Entity mRightHandEntity_;

    clay::ecs::Entity mPlanetEntity_;
    clay::ecs::Entity mSunSphere_;
    clay::ecs::Entity mMoonEntity_;

    clay::ecs::Entity mPlaneEntity_; // imgui

    float planetOrbitSpeed = -1.0f/60.0f;
    float moonOrbitSpeed = -2.0f/60.0f;
    float mPlanetOrbitRadius_ = 2.0f;
    float mMoonOrbitRadius_ = .5f;

    bool mUpdateSpace_ = true;

    unsigned int mSelectedSceneIdx = 0;

    clay::CameraControllerXR mCameraController_;
};

