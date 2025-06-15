// clay
#include <clay/entity/render/ModelRenderable.h>
// project
#include "application/DemoAppXR.h"
// class
#include "SpaceScene.h"

SpaceScene::SpaceScene(clay::BaseApp& app)
    : clay::BaseScene(app),
      mCameraController_(mpFocusCamera_) {}

SpaceScene::~SpaceScene() {}

void SpaceScene::initialize() {
    assembleResources();

    mpFocusCamera_->setPosition({0,0,5});

    // hands
    {
        // left
        auto* handLeftRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("GloveLeft")
        );
        handLeftRenderable->setScale({0.2f, 0.2f, 0.2f});
        handLeftRenderable->setColor({.95f, .674f, .411f, 1.0f});
        mLeftHandEntity_.addRenderable(handLeftRenderable);

        // right
        auto* handRightRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("GloveRight")
        );
        handRightRenderable->setScale({0.2f, 0.2f, 0.2f});
        handRightRenderable->setColor({.95f, .674f, .411f, 1.0f});
        mRightHandEntity_.addRenderable(handRightRenderable);
    }
    // imgui plane
    {
        auto* modelRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("ImguiPlane")
        );
        mPlaneEntity_.addRenderable(modelRenderable);
        mPlaneEntity_.setPosition({2,0,4});
        mPlaneEntity_.getOrientation() *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mPlaneEntity_.getOrientation() *= glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // sun
    {
        auto* modelRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("Sun")
        );
        mSunSphere_.addRenderable(modelRenderable);
        mSunSphere_.setPosition({0,0,0});
    }
    // earth
    {
        auto* modelRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("Earth")
        );
        mPlanetEntity_.addRenderable(modelRenderable);
        mPlanetEntity_.setPosition({0 ,0, mPlanetOrbitRadius_});
        mPlanetEntity_.setScale({.30f, .30f, .30f});
    }
    // moon
    {
        auto* modelRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("Moon")
        );
        const auto planetPosition = mPlanetEntity_.getPosition();

        mMoonEntity_.addRenderable(modelRenderable);
        mMoonEntity_.setPosition({planetPosition.x,planetPosition.y,planetPosition.z + mMoonOrbitRadius_});
        mMoonEntity_.setScale({.15f, .15f, .15f});
    }
    // skybox
    {
        auto* modelRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("StarSkybox")
        );
        mSkyBoxEntity.addRenderable(modelRenderable);
        mSkyBoxEntity.setPosition({0,0,0});
        mSkyBoxEntity.setScale({1,1,1});
    }
}

void SpaceScene::update(float dt) {
    clay::AppXR& appXR = ((clay::AppXR&)mApp_);
    const auto joyDirLeft = appXR.getInputHandler().getJoystickDirection(clay::InputHandlerXR::Hand::LEFT);
    const auto joyDirRight = appXR.getInputHandler().getJoystickDirection(clay::InputHandlerXR::Hand::RIGHT);

    const auto& rightHandPose = appXR.getInputHandler().getAimPose(clay::InputHandlerXR::Hand::RIGHT);
    const glm::quat rightHandOrientation(rightHandPose.orientation.w, rightHandPose.orientation.x, rightHandPose.orientation.y, rightHandPose.orientation.z);
    glm::vec3 rightHandPosition = glm::vec3(rightHandPose.position.x,rightHandPose.position.y,rightHandPose.position.z);

    const auto& leftHandPose = appXR.getInputHandler().getAimPose(clay::InputHandlerXR::Hand::LEFT);
    const glm::quat leftHandOrientation(leftHandPose.orientation.w, leftHandPose.orientation.x, leftHandPose.orientation.y, leftHandPose.orientation.z);
    glm::vec3 leftHandPosition = glm::vec3(leftHandPose.position.x, leftHandPose.position.y, leftHandPose.position.z);

    const auto headPose = appXR.getInputHandler().getHeadPose();

    // update camera with input
    mCameraController_.updateWithJoystickInput(
        {joyDirLeft.x, joyDirLeft.y},
        {joyDirRight.x, joyDirRight.y},
        0.01f * 2.0f,
        1.0f/2.0f * 2.0f,
        headPose
    );
    const glm::vec3 cameraPosition = mpFocusCamera_->getPosition();
    const glm::quat cameraOrientation = mpFocusCamera_->getOrientation();

    // Rotate hands to match camera
    const glm::vec3 rotatedRight = cameraOrientation * rightHandPosition;
    const glm::vec3 rotatedLeft = cameraOrientation * leftHandPosition;

    rightHandPosition = cameraPosition + rotatedRight;
    leftHandPosition = cameraPosition  + rotatedLeft;
    mSkyBoxEntity.setOrientation(glm::conjugate(mpFocusCamera_->getOrientation()));
    {
        // update left hand
        mLeftHandEntity_.setOrientation(cameraOrientation * leftHandOrientation);
        mLeftHandEntity_.setPosition(leftHandPosition);
    }
    {
        // update right hand
        mRightHandEntity_.setOrientation(cameraOrientation * rightHandOrientation);
        mRightHandEntity_.setPosition(rightHandPosition);

    }

    if (mUpdateSpace_) {
        glm::vec3 planetDiff;
        glm::vec3 planetPosition = mPlanetEntity_.getPosition();
        glm::vec3 moonPosition = mMoonEntity_.getPosition();
        {
            glm::vec3 orbitCenter = mSunSphere_.getPosition();
            // Update planet orbit
            glm::vec3 relativePos = planetPosition - orbitCenter;
            // Calculate angle of rotation based on current position
            float planetAngle = glm::atan(relativePos.z, relativePos.x);
            // Calculate distance from center
            float radius = glm::length(glm::vec2(relativePos.x, relativePos.z));
            // Update angle if planet is not at the center
            if (radius > 0.0f) {
                // Adjust the speed of rotation as needed
                planetAngle += planetOrbitSpeed;
            }

            // Calculate new position of the planet
            float x = orbitCenter.x + radius * glm::cos(planetAngle);
            float z = orbitCenter.z + radius * glm::sin(planetAngle);
            glm::vec3 newPlanetPos = glm::vec3(x, orbitCenter.y, z); // Y remains unchanged

            planetDiff = newPlanetPos - planetPosition;

            // Update planet position
            planetPosition = newPlanetPos;
        }
        {
            // Move moon along with the planet's movement
            moonPosition += planetDiff;

            // Orbit moon around planet
            glm::vec3 orbitCenter = planetPosition;
            glm::vec3 relativePos = moonPosition - orbitCenter;

            float moonAngle = glm::atan(relativePos.z, relativePos.x);
            float radius = glm::length(glm::vec2(relativePos.x, relativePos.z));

            // Update angle if moon is not at the center
            if (radius > 0.0f) {
                moonAngle += moonOrbitSpeed;
            }

            // Calculate new position of the moon
            float x = orbitCenter.x + radius * glm::cos(moonAngle);
            float z = orbitCenter.z + radius * glm::sin(moonAngle);
            glm::vec3 newMoonPos = glm::vec3(x, orbitCenter.y, z); // Y remains unchanged

            // Update moon position
            moonPosition = newMoonPos;
        }

        mPlanetEntity_.getOrientation() *= glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mPlanetEntity_.setPosition(planetPosition);

        mMoonEntity_.getOrientation() *= glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mMoonEntity_.setPosition(moonPosition);
    }
}

void SpaceScene::render(VkCommandBuffer cmdBuffer) {
    mSkyBoxEntity.render(cmdBuffer);
    mPlanetEntity_.render(cmdBuffer);
    mSunSphere_.render(cmdBuffer);
    mMoonEntity_.render(cmdBuffer);

    mPlaneEntity_.render(cmdBuffer);

    mLeftHandEntity_.render(cmdBuffer);
    mRightHandEntity_.render(cmdBuffer);
}

void SpaceScene::renderGUI(VkCommandBuffer cmdBuffer) {
    const uint32_t imguiWidth = 4128;
    const uint32_t imguiHeight = 2208;

    // todo see if pointing at plane
    const auto& rightHandPose = ((clay::AppXR &) mApp_).getInputHandler().getAimPose(clay::InputHandlerXR::Hand::RIGHT);
    const glm::quat rightHandOrientation(rightHandPose.orientation.w, rightHandPose.orientation.x, rightHandPose.orientation.y, rightHandPose.orientation.z);
    glm::vec3 rightHandPosition = glm::vec3(rightHandPose.position.x, rightHandPose.position.y, rightHandPose.position.z);

    const glm::vec3 cameraPosition = mpFocusCamera_->getPosition();
    const glm::quat cameraOrientation = mpFocusCamera_->getOrientation();

    const glm::vec3 rotatedRight = cameraOrientation * rightHandPosition;

    rightHandPosition = cameraPosition + rotatedRight;
    const glm::vec3 rightHandForward = glm::normalize(rightHandOrientation * cameraOrientation * glm::vec3{0, 0, -1});

    glm::vec3 rayOrigin = rightHandPosition;
    glm::vec3 rayDir = rightHandForward;

    glm::vec3 normal = glm::normalize(mPlaneEntity_.getOrientation() * glm::vec3{0, 1, 0});

    float denominator = glm::dot(normal, rayDir);

    ImVec2 calMousePos = {0,0};

    if (glm::abs(denominator) > 1e-6f) {
        glm::vec3 planeToRay = mPlaneEntity_.getPosition() - rayOrigin;
        float t = glm::dot(planeToRay, normal) / denominator;

        if (t > 0.0f) {
            glm::vec3 intersectPoint = rayOrigin + t * rayDir;

            glm::mat4 rotationMatrix = glm::mat4_cast(mPlaneEntity_.getOrientation());
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mPlaneEntity_.getPosition());


            float planeMinX = -0.5f, planeMaxX = 0.5f; // Plane bounds in local space
            float planeMinY = -0.5f, planeMaxY = 0.5f;

            glm::vec4 localPoint = glm::inverse(translationMatrix * rotationMatrix) * glm::vec4(intersectPoint, 1.0f);

            float u = (localPoint.x - planeMinX) / (planeMaxX - planeMinX);
            float v = (localPoint.z - planeMinY) / (planeMaxY - planeMinY);
            int pixelX = static_cast<int>(u * imguiWidth);
            int pixelY = static_cast<int>(v * imguiHeight);

            calMousePos = ImVec2(pixelX, pixelY);
        }
    }


    const float rightTriggerState = ((clay::AppXR &)mApp_).getInputHandler().getTriggerState(clay::InputHandlerXR::Hand::RIGHT);

    // draw imgui
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(imguiWidth, imguiHeight), ImGuiCond_FirstUseEver);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;
    io.MousePos = calMousePos;

    if (rightTriggerState > .1f) {
        io.AddMouseButtonEvent(0, true);  // Simulate mouse down
    }
    if (rightTriggerState < .1f) {
        io.AddMouseButtonEvent(0, false);  // Simulate mouse up
    }

    ImGui::Begin("Plane");
    ImGui::SetWindowFontScale(6.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Separator();
    ImGui::Text("Grab: %f, %f", ((clay::AppXR &)mApp_).getInputHandler().getGrabState(clay::InputHandlerXR::Hand::LEFT), ((clay::AppXR &)mApp_).getInputHandler().getGrabState(clay::InputHandlerXR::Hand::RIGHT));
    ImGui::Text("Trigger: %f, %f", ((clay::AppXR &)mApp_).getInputHandler().getTriggerState(clay::InputHandlerXR::Hand::LEFT), ((clay::AppXR &)mApp_).getInputHandler().getTriggerState(clay::InputHandlerXR::Hand::RIGHT));

    ImGui::Text("Buttons: Y: %i, X: %i, B: %i, A: %i",
        ((clay::AppXR &)mApp_).getInputHandler().getButtonDown(clay::InputHandlerXR::Button::Y),
        ((clay::AppXR &)mApp_).getInputHandler().getButtonDown(clay::InputHandlerXR::Button::X),
        ((clay::AppXR &)mApp_).getInputHandler().getButtonDown(clay::InputHandlerXR::Button::B),
        ((clay::AppXR &)mApp_).getInputHandler().getButtonDown(clay::InputHandlerXR::Button::A)
    );
    const auto& leftGripPose = ((clay::AppXR &)mApp_).getInputHandler().getGripPose(clay::InputHandlerXR::Hand::LEFT);
    ImGui::Text("Left Grip Pose: (%f, %f, %f, %f) (%f, %f, %f)",
        leftGripPose.orientation.x,
        leftGripPose.orientation.y,
        leftGripPose.orientation.z,
        leftGripPose.orientation.w,
        leftGripPose.position.x,
        leftGripPose.position.y,
        leftGripPose.position.z
    );
    const auto& rightGripPose = ((clay::AppXR &)mApp_).getInputHandler().getGripPose(clay::InputHandlerXR::Hand::RIGHT);
    ImGui::Text("Right Grip Pose: (%f, %f, %f, %f) (%f, %f, %f)",
        rightGripPose.orientation.x,
        rightGripPose.orientation.y,
        rightGripPose.orientation.z,
        rightGripPose.orientation.w,
        rightGripPose.position.x,
        rightGripPose.position.y,
        rightGripPose.position.z
    );
    const auto& leftAimPose = ((clay::AppXR &)mApp_).getInputHandler().getAimPose(clay::InputHandlerXR::Hand::LEFT);
    ImGui::Text("Left Aim Pose: (%f, %f, %f, %f) (%f, %f, %f)",
        leftAimPose.orientation.x,
        leftAimPose.orientation.y,
        leftAimPose.orientation.z,
        leftAimPose.orientation.w,
        leftAimPose.position.x,
        leftAimPose.position.y,
        leftAimPose.position.z
    );
    const auto& rightAimPose = ((clay::AppXR &)mApp_).getInputHandler().getAimPose(clay::InputHandlerXR::Hand::RIGHT);
    ImGui::Text("Right Aim Pose: (%f, %f, %f, %f) (%f, %f, %f)",
        rightAimPose.orientation.x,
        rightAimPose.orientation.y,
        rightAimPose.orientation.z,
        rightAimPose.orientation.w,
        rightAimPose.position.x,
        rightAimPose.position.y,
        rightAimPose.position.z
    );
    const auto& leftJoystickDir = ((clay::AppXR &)mApp_).getInputHandler().getJoystickDirection(clay::InputHandlerXR::Hand::LEFT);
    ImGui::Text("Left Joystick dir: (%f, %f) (%f, %f)",
        leftJoystickDir.x,
        leftJoystickDir.y,
        leftJoystickDir.x,
        leftJoystickDir.y
    );
    const auto& rightJoystickDir = ((clay::AppXR &)mApp_).getInputHandler().getJoystickDirection(clay::InputHandlerXR::Hand::RIGHT);
    ImGui::Text("Right Joystick dir: (%f, %f) (%f, %f)",
        rightJoystickDir.x,
        rightJoystickDir.y,
        rightJoystickDir.x,
        rightJoystickDir.y
    );
    const auto& headPose = ((clay::AppXR &)mApp_).getInputHandler().getHeadPose();
    ImGui::Text("Headpose (%f, %f %f, %f) (%f %f %f)",
        headPose.orientation.x,
        headPose.orientation.y,
        headPose.orientation.z,
        headPose.orientation.w,
        headPose.position.x,
        headPose.position.y,
        headPose.position.z
    );
    if (ImGui::Checkbox("Scene Running: ", &mUpdateSpace_)) {}

    ImGui::Separator();

    ImGui::BeginGroup();
    if (ImGui::BeginListBox("##Scenes")) {
        for (unsigned int i = 0; i < ((DemoAppXR&)mApp_).mSceneDetails_.size(); ++i) {
            std::string elementName = "Entity " +  ((DemoAppXR&)mApp_).mSceneDetails_[i].mName_;
            if (ImGui::Selectable(elementName.c_str(), i == mSelectedSceneIdx)) {
                mSelectedSceneIdx = i;
            }
        }
        ImGui::EndListBox();
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    if (mSelectedSceneIdx < ((DemoAppXR&)mApp_).mSceneDetails_.size()) {
        DemoAppXR::SceneDetail& displayScene = ((DemoAppXR&)mApp_).mSceneDetails_[mSelectedSceneIdx];
        ImGui::Text("%s", displayScene.mName_.c_str());
        ImGui::Image(
            (ImTextureID)displayScene.mPreviewTexture_,
            ImVec2(935, 500 ),
            ImVec2(0, 0),
            ImVec2(1, 1)
        );
        if (ImGui::Button("Start")) {
            displayScene.mLoadScene_();
        }
    }
    ImGui::EndGroup();

    ImGui::End();
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
}

void SpaceScene::assembleResources() {
    mpResources_ = new clay::Resources(mApp_.getGraphicsContext());
}

void SpaceScene::destroyResources() {}
