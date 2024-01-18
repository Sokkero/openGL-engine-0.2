#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

namespace Engine
{
    class BasicNode;
    class RenderManager;
    class CameraComponent;
    class GeometryComponent;
    class GridShader;

    class EngineManager
    {
        public:
            EngineManager();
            ~EngineManager() = default;

            bool engineStart();
            void engineUpdate();
            void engineDraw();

            void drawNode(std::shared_ptr<GeometryComponent> node);

            void setScene(std::shared_ptr<BasicNode> sceneNode);

            std::shared_ptr<BasicNode> getScene() const { return m_sceneNode; };

            std::shared_ptr<CameraComponent> getCamera() const { return m_camera; };

            void setCamera(std::shared_ptr<CameraComponent> camera) { m_camera = std::move(camera); };

            std::shared_ptr<RenderManager> getRenderManager() const { return m_renderManager; };

            void setDeltaTime();
            float getDeltaTime() const;

            void setClearColor(const float color[4]);

            float* getClearColor() { return m_clearColor; };

            void updateFps();

            int getFpsCount() const { return m_fpsCount; };

            bool isGridVisible() const { return m_showGrid; };
            void setGridVisibility(bool showGrid) { m_showGrid = showGrid; };

            void addGeometryToScene(std::shared_ptr<GeometryComponent>& node);
            void removeGeometryFromScene(std::shared_ptr<GeometryComponent>& node);
            void removeGeometryFromScene(std::shared_ptr<BasicNode>& node);
            void removeGeometryFromScene(BasicNode* node);
            void removeGeometryFromScene(const unsigned int& nodeId);

        private:
            void depthSortNodes();

            static bool nodeSortingAlgorithm(std::shared_ptr<GeometryComponent> a, std::shared_ptr<GeometryComponent> b, const glm::vec3& cameraPosition);

            std::vector<std::shared_ptr<GeometryComponent>> m_sceneGeometry;
            std::shared_ptr<RenderManager> m_renderManager;
            std::shared_ptr<BasicNode> m_sceneNode;
            std::shared_ptr<CameraComponent> m_camera;

            bool m_showGrid;
            std::shared_ptr<GridShader> m_gridShader;

            double m_deltaTime;
            double m_currentFrameTimestamp;
            double m_lastFrameTimestamp;
            double m_lastFpsCalc;
            int m_fpsCount;
            int m_frames;
            float m_clearColor[4];
    };

} // namespace Engine
