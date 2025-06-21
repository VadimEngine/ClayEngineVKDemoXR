# ClayEngineVK Demo XR

Demo application of the ClayEngineVK static library (https://github.com/VadimEngine/ClayEngineVK) showcasing building an XR application with OpenXR, Vulkan, and Android. This application allows loading and using resources to build scenes that the user can navigate and interact with. This application primarily uses OpenXR from ClayEngineVK for the VR functionality to run on Oculus Quest 3.

![alt text](./screenshots/VKXRDemoGif.gif)

## Scenes

### Sandbox Scene

Sandbox scene with controllable hands and a collection of 3d models and text being rendered. An Imgui window is rendered unto a plane and can be interacted with controller. The gui displays the user's controller and headset state. A sample audio can be played with a button and the scene can be swapped. Moving and rotating with the controller joysticks is supported.

### Space Scene

Similar to Sandbox scene but instead, a sun with an orbiting planet and moon is rendered inside a starry skybox. The scene can be paused through the GUI.

### Farm Scene

Similar to Sandbox scene but instead, A nature scene is rendered. 

## Build

Update Submodules
- `git submodule update --init --recursive`

compile shaders
- `./res/compile_shaders.bat`

build CLI:
- `./gradlew clean assembleDebug`
- `./gradlew clean build`

The build `app-debug.apk` will be `.\app\build\intermediates\apk\debug\app-debug.apk` can be deployed to an Oculus device

Alternatively, this can be built and deployed with Android studio.
