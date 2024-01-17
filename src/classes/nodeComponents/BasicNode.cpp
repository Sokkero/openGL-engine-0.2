#include "BasicNode.h"

#include "../engine/EngineManager.h"
#include "GeometryComponent.h"

#include <iostream>

namespace Engine
{
    unsigned int BasicNode::LASTID = 0;
    std::shared_ptr<WindowManager> BasicNode::WINDOW_MANAGER = nullptr;
    std::shared_ptr<EngineManager> BasicNode::ENGINE_MANAGER = nullptr;
    std::shared_ptr<UserEventManager> BasicNode::USER_EVENT_MANAGER = nullptr;

    BasicNode::BasicNode() : m_parentNode(nullptr), m_nodeId(0)
    {
        m_nodeId = getNewUniqueId();
    }

    std::shared_ptr<BasicNode> BasicNode::getChildNode(int pos) const
    {
        if(pos >= m_childNodes.size())
        {
            return nullptr;
        }
        return m_childNodes[pos];
    }

    void BasicNode::addChild(const std::shared_ptr<BasicNode>& node)
    {
        m_childNodes.emplace_back(node);
        node->setParent(this);

        auto geometry = std::dynamic_pointer_cast<GeometryComponent>(node);
        if(geometry)
        {
            ENGINE_MANAGER->addGeometryToScene(geometry);
        }

        node->start();
        std::cout << "Object [" << node->getName() << "] initialised" << std::endl;
    }

    std::shared_ptr<BasicNode> BasicNode::detatchChild(const std::shared_ptr<BasicNode>& node)
    {
        return detatchChild(node->getNodeId());
    }

    std::shared_ptr<BasicNode> BasicNode::detatchChild(const unsigned int& nodeId)
    {
        const auto& it = m_childNodes.erase(std::remove_if(m_childNodes.begin(), m_childNodes.end(), [nodeId](const auto& childNode)->bool { return nodeId == childNode->getNodeId(); }), m_childNodes.end());

        if(it == m_childNodes.end())
        {
            return nullptr;
        }

        auto geometry = std::dynamic_pointer_cast<GeometryComponent>(*it);
        if(geometry)
        {
            ENGINE_MANAGER->removeGeometryFromScene(geometry);
        }

        (*it)->setParent(nullptr);

        return *it;
    }

    void BasicNode::deleteChild(const std::shared_ptr<BasicNode>& node)
    {
        detatchChild(node->getNodeId());
    }

    void BasicNode::deleteChild(const unsigned int& nodeId)
    {
        detatchChild(nodeId);
    }

    std::vector<std::shared_ptr<BasicNode>> BasicNode::detatchAllChildren()
    {
        for(const auto& child : m_childNodes)
        {
            child->callOnAllChildrenRecursiveAndSelf([](BasicNode* node)->void{ENGINE_MANAGER->removeGeometryFromScene(node);});
            child->setParent(nullptr);
        }

        return std::move(m_childNodes);
    }

    void BasicNode::deleteAllChildren()
    {
        detatchAllChildren();
    }

    void BasicNode::detatchFromParent()
    {
        callOnAllChildrenRecursiveAndSelf([](BasicNode* node)->void{ENGINE_MANAGER->removeGeometryFromScene(node);});
        setParent(nullptr);
    }

    void BasicNode::deleteNode()
    {
        detatchFromParent();
    }

    void BasicNode::callOnAllChildren(const std::function<void(BasicNode*)>& func)
    {
        for(const auto& childNode : m_childNodes)
        {
            func(childNode.get());
        }
    }

    void BasicNode::callOnAllChildrenRecursive(const std::function<void(BasicNode*)>& func)
    {
        for(const auto& childNode : m_childNodes)
        {
            childNode->callOnAllChildrenRecursiveAndSelf(func);
        }
    }

    void BasicNode::callOnAllChildrenAndSelf(const std::function<void(BasicNode*)>& func)
    {
        for(const auto& childNode : m_childNodes)
        {
            func(childNode.get());
        }
        func(this);
    }

    void BasicNode::callOnAllChildrenRecursiveAndSelf(const std::function<void(BasicNode*)>& func)
    {
        for(const auto& childNode : m_childNodes)
        {
            childNode->callOnAllChildrenRecursiveAndSelf(func);
        }
        func(this);
    }

    glm::mat4 BasicNode::getGlobalModelMatrix() const
    {
        if(m_parentNode)
        {
            return m_parentNode->getGlobalModelMatrix() * getModelMatrix();
        }
        return getModelMatrix();
    }

    glm::quat BasicNode::getGlobalRotation() const
    {
        if(m_parentNode)
        {
            return m_parentNode->getGlobalRotation() * getRotationQuat();
        }
        return getRotationQuat();
    }

    glm::vec3 BasicNode::getGlobalPosition() const
    {
        glm::mat4 matrix = getGlobalModelMatrix();
        return matrix[3];
    }

    glm::vec3 BasicNode::getGlobalScale() const
    {
        glm::vec3 scale;
        const auto& matrix = getGlobalModelMatrix();
        scale.x = glm::length(glm::vec3(matrix[0][0], matrix[0][1], matrix[0][2]));
        scale.y = glm::length(glm::vec3(matrix[1][0], matrix[1][1], matrix[1][2]));
        scale.z = glm::length(glm::vec3(matrix[2][0], matrix[2][1], matrix[2][2]));
        return scale;
    }

    glm::vec3 BasicNode::getForward() const
    {
        return glm::normalize(glm::rotate(getGlobalRotation(), glm::vec3(0.0f, 0.0f, -1.0f)));
    }

    glm::vec3 BasicNode::getBackwards() const
    {
        return glm::normalize(glm::rotate(getGlobalRotation(), glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    glm::vec3 BasicNode::getLeft() const
    {
        return glm::normalize(glm::rotate(getGlobalRotation(), glm::vec3(-1.0f, 0.0f, 0.0f)));
    }

    glm::vec3 BasicNode::getRight() const
    {
        return glm::normalize(glm::rotate(getGlobalRotation(), glm::vec3(1.0f, 0.0f, 0.0f)));
    }

    glm::vec3 BasicNode::getDown() const
    {
        return glm::normalize(glm::rotate(getGlobalRotation(), glm::vec3(0.0f, -1.0f, 0.0f)));
    }

    glm::vec3 BasicNode::getUp() const
    {
        return glm::normalize(glm::rotate(getGlobalRotation(), glm::vec3(0.0f, 1.0f, 0.0f)));
    }
} // namespace Engine