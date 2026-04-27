#ifndef MOVE_COMMAND_H
#define MOVE_COMMAND_H

#include "Command.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include <glm/glm.hpp>

namespace dae
{
    class MoveCommand : public Command
    {
    public:
        MoveCommand(GameObject* gameObject, const glm::vec2& direction, float speed)
            : m_pGameObject(gameObject)
            , m_Direction(direction)
            , m_Speed(speed)
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

                float spriteWidth = 88.0f;
                float spriteHeight = 120.0f;

                float minX = 0.0f - spriteWidth/2;
                float maxX = 1366.0f - spriteWidth/2;

                float minY = 0.0f - spriteHeight/2;
                float maxY = 768.0f - spriteHeight/2;

                if (newX < minX) newX = minX;
                if (newX > maxX) newX = maxX;

                if (newY < minY) newY = minY;
                if (newY > maxY) newY = maxY;

                m_pGameObject->SetLocalPosition(newX, newY);

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
    };
}

#endif