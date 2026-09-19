#include "Rendering/TSAA.h"
#include "Base/Camera.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shaderc/shaderc.hpp>

namespace
{
class TestTSAA : public tzw::TSAA
{
public:
    using TSAA::buildHistoryClipTransform;

    void seedHistory(tzw::Camera* camera)
    {
        m_historyCamera = camera;
        m_hasHistory = true;
        m_framePending = false;
    }

    bool hasHistory() const { return m_hasHistory; }
};

int failureCount = 0;

void expect(bool condition, const char* message)
{
    if(!condition)
    {
        ++failureCount;
        std::cerr << message << '\n';
    }
}

tzw::Matrix44 cameraTransform(glm::vec3 position, float yaw, float pitch)
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
    transform = glm::rotate(transform, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    tzw::Matrix44 result;
    result.copyFromArray(glm::value_ptr(transform));
    return result;
}

void testStaticReprojection()
{
    tzw::Matrix44 projection;
    projection.perspective(60.0f, 1920.0f / 1080.0f, 0.01f, 350.0f);
    const glm::vec3 positions[] = {{0, 0, 0}, {10.4944f, 41.5309f, 0.8129f}, {100, 20, 100}, {1000, 200, 1000}};
    float maxErrorPixels = 0.0f;
    float oldMaxErrorPixels = 0.0f;
    for(auto position : positions)
    {
        for(int angle = 0; angle < 40; ++angle)
        {
            auto camera = cameraTransform(position, 0.3f + angle * 0.002f, 0.15f + angle * 0.001f);
            auto reprojection = TestTSAA::buildHistoryClipTransform(projection, camera, projection, camera);
            auto vp = projection * camera.inverted();
            auto inverseVP = vp.inverted();
            for(float distance : {5.0f, 25.0f, 100.0f})
            {
                float depth = -projection.data()[10] + projection.data()[14] / distance;
                for(float x : {-0.6f, 0.0f, 0.6f})
                {
                    tzw::vec4 clip(x, 0.2f, depth, 1.0f);
                    auto historyClip = reprojection * clip;
                    float error = std::hypot((historyClip.x / historyClip.w - clip.x) * 960.0f,
                        (historyClip.y / historyClip.w - clip.y) * 540.0f);
                    maxErrorPixels = std::max(maxErrorPixels, error);

                    auto world = inverseVP * clip;
                    auto oldClip = vp * tzw::vec4(world.x / world.w, world.y / world.w, world.z / world.w, 1.0f);
                    oldMaxErrorPixels = std::max(oldMaxErrorPixels, std::hypot((oldClip.x / oldClip.w - clip.x) * 960.0f,
                        (oldClip.y / oldClip.w - clip.y) * 540.0f));
                }
            }
        }
    }
    expect(maxErrorPixels < 0.001f, "Static camera reprojection must stay within 0.001 pixels, including translated cameras");
    expect(oldMaxErrorPixels > 0.1f, "Regression cases must reproduce the old world-space precision error");
    std::cout << "Static reprojection max error: " << maxErrorPixels << " px; old path: " << oldMaxErrorPixels << " px\n";
}

void testMovingReprojection()
{
    tzw::Matrix44 previousProjection;
    previousProjection.perspective(60.0f, 1920.0f / 1080.0f, 0.01f, 350.0f);
    float maxErrorPixels = 0.0f;
    for(float fov : {60.0f, 55.0f})
    {
        tzw::Matrix44 projection;
        projection.perspective(fov, 1920.0f / 1080.0f, 0.01f, 350.0f);
        for(float motion : {0.0f, 0.01f, 0.1f})
        {
            auto previousCamera = cameraTransform({100, 20, 100}, 0.3f, 0.15f);
            auto camera = cameraTransform({100 + motion, 20, 100}, 0.3f + motion * 0.1f, 0.15f);
            auto reprojection = TestTSAA::buildHistoryClipTransform(previousProjection, previousCamera, projection, camera);
            for(double distance : {1.0, 5.0, 25.0, 100.0})
            {
                // Project one static world point independently in each camera as the reference.
                glm::dvec4 world = glm::dmat4(glm::make_mat4(camera.data())) * glm::dvec4(0.2, 0.1, -distance, 1.0);
                glm::dvec4 currentClip = glm::dmat4(glm::make_mat4(projection.data()))
                    * glm::inverse(glm::dmat4(glm::make_mat4(camera.data()))) * world;
                glm::dvec4 expected = glm::dmat4(glm::make_mat4(previousProjection.data()))
                    * glm::inverse(glm::dmat4(glm::make_mat4(previousCamera.data()))) * world;
                currentClip /= currentClip.w;
                expected /= expected.w;
                auto actual = reprojection * tzw::vec4(static_cast<float>(currentClip.x), static_cast<float>(currentClip.y),
                    static_cast<float>(currentClip.z), 1.0f);
                float error = static_cast<float>(std::hypot((actual.x / actual.w - expected.x) * 960.0,
                    (actual.y / actual.w - expected.y) * 540.0));
                maxErrorPixels = std::max(maxErrorPixels, error);
            }
        }
    }
    expect(maxErrorPixels < 0.02f, "Translation, rotation and FOV changes must reproject a static world point correctly");
    std::cout << "Moving reprojection max error: " << maxErrorPixels << " px\n";
}

void testHistoryLifetime()
{
    std::unique_ptr<tzw::Camera> camera(tzw::Camera::CreatePerspective(60, 1920, 1080, 0.01f, 350));
    std::unique_ptr<tzw::Camera> otherCamera(tzw::Camera::CreatePerspective(60, 1920, 1080, 0.01f, 350));
    TestTSAA tsaa;
    tsaa.seedHistory(camera.get());
    tsaa.preTick(camera.get());
    expect(tsaa.hasHistory(), "The next frame with the same camera must retain history");
    tsaa.preTick(camera.get());
    expect(!tsaa.hasHistory(), "A skipped resolve must invalidate history");
    tsaa.seedHistory(camera.get());
    tsaa.preTick(otherCamera.get());
    expect(!tsaa.hasHistory(), "Changing cameras must invalidate history");
    tsaa.seedHistory(camera.get());
    tsaa.resetHistory();
    expect(!tsaa.hasHistory(), "Disabling AA must invalidate history");
}

void testResolveShader(const char* path)
{
    std::ifstream file(path);
    expect(file.is_open(), "TSAA shader source must be readable");
    std::string source = "#version 420\n";
    source.append(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    shaderc::Compiler compiler;
    auto result = compiler.CompileGlslToSpv(source, shaderc_glsl_fragment_shader, path);
    expect(result.GetCompilationStatus() == shaderc_compilation_status_success, "TSAA fragment shader must compile to SPIR-V");
    if(result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << result.GetErrorMessage();
    }
}
}

int main(int argc, char** argv)
{
    tzw::EngineDef::isUseVulkan = true;
    testStaticReprojection();
    testMovingReprojection();
    testHistoryLifetime();
    expect(argc == 2, "Expected the TSAA shader source path");
    if(argc == 2) testResolveShader(argv[1]);
    return failureCount == 0 ? 0 : 1;
}
