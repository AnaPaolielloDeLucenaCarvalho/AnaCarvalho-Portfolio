#ifndef MOVE_COMMAND_H
#define MOVE_COMMAND_H

#include "Command.h"
#include "GameObject.h"
#include "SpriteComponent.h" 
#include <glm/glm.hpp>
#include <vector>
#include <SDL3/SDL_rect.h>

namespace dae
{
    class MoveCommand : public Command
    {
    public:
        MoveCommand(GameObject* gameObject, const glm::vec2& direction, float speed, const std::vector<SDL_FRect>& walkableZones = {})
            : m_pGameObject(gameObject)
            , m_Direction(direction)
            , m_Speed(speed)
            , m_WalkableZones(walkableZones)
        {
            if (glm::length(m_Direction) > 0)
            {
                m_Direction = glm::normalize(m_Direction);
            }
        }

        void Execute(float deltaTime) override
        {
            if (m_pGameObject)
            {
                auto pos = m_pGameObject->GetTransform().GetPosition();

                glm::vec2 movement = m_Direction * (m_Speed * deltaTime);
                float newX = pos.x + movement.x;
                float newY = pos.y + movement.y;

                if (m_WalkableZones.empty())
                {
                    // Default behavior: Clamp to screen if no zones are provided
                    float minX = 0.0f, maxX = 1366.0f - 88.0f;
                    float minY = 0.0f, maxY = 768.0f - 120.0f;
                    if (newX < minX) newX = minX;
                    if (newX > maxX) newX = maxX;
                    if (newY < minY) newY = minY;
                    if (newY > maxY) newY = maxY;

                    m_pGameObject->SetLocalPosition(newX, newY);
                }
                else
                {
                    float feetOffsetX = 88.0f / 2.0f;
                    float feetOffsetY = 110.0f;

                    bool canWalkX = false;
                    bool canWalkY = false;

                    float testX = newX + feetOffsetX;
                    float testY = pos.y + feetOffsetY;
                    for (const auto& zone : m_WalkableZones)
                    {
                        if (testX >= zone.x && testX <= zone.x + zone.w &&
                            testY >= zone.y && testY <= zone.y + zone.h) canWalkX = true;
                    }

                    testX = pos.x + feetOffsetX;
                    testY = newY + feetOffsetY;
                    for (const auto& zone : m_WalkableZones)
                    {
                        if (testX >= zone.x && testX <= zone.x + zone.w &&
                            testY >= zone.y && testY <= zone.y + zone.h) canWalkY = true;
                    }

                    float finalX = canWalkX ? newX : pos.x;
                    float finalY = canWalkY ? newY : pos.y;
                    m_pGameObject->SetLocalPosition(finalX, finalY);
                }

                auto spriteComp = m_pGameObject->GetComponent<dae::SpriteComponent>();
                if (spriteComp)
                {
                    spriteComp->SetDirection(m_Direction);
                    spriteComp->MarkAsMoved();
                }
            }
        }

    private:
        GameObject* m_pGameObject;
        glm::vec2 m_Direction;
        float m_Speed;
        std::vector<SDL_FRect> m_WalkableZones;
    };
}

#endif