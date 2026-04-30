#include "TriggerComponent.h"
#include "GameObject.h"
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include "Renderer.h"

namespace portfolio
{
    TriggerComponent::TriggerComponent(GameObject* pOwner, float width, float height)
        : Component(pOwner), m_Width(width), m_Height(height)
    {
    }

    void portfolio::TriggerComponent::Update(float /*deltaTime*/)
    {
        if (!m_Target || !m_OnTriggerEnter) return;

        auto myPos = GetOwner()->GetTransform().GetPosition();
        auto targetPos = m_Target->GetTransform().GetPosition();

        bool isOverlapping = (
            myPos.x < targetPos.x + m_TargetWidth &&
            myPos.x + m_Width > targetPos.x &&
            myPos.y < targetPos.y + m_TargetHeight &&
            myPos.y + m_Height > targetPos.y
            );

        if (m_FirstFrame)
        {
            m_IsInside = isOverlapping;
            m_FirstFrame = false;
            return;
        }

        if (isOverlapping && !m_IsInside)
        {
            m_IsInside = true;
            if (m_OnTriggerEnter)
            {
                m_OnTriggerEnter();
            }
        }
        else if (!isOverlapping && m_IsInside)
        {
            m_IsInside = false;
            if (m_OnTriggerExit)
            {
                m_OnTriggerExit();
            }
        }
    }

    // Add this function anywhere in the .cpp file:
    void portfolio::TriggerComponent::SetOnTriggerExit(std::function<void()> callback)
    {
        m_OnTriggerExit = callback;
    }

    void TriggerComponent::SetTarget(GameObject* target, float targetWidth, float targetHeight)
    {
        m_Target = target;
        m_TargetWidth = targetWidth;
        m_TargetHeight = targetHeight;
    }

    void TriggerComponent::SetOnTriggerEnter(std::function<void()> callback)
    {
        m_OnTriggerEnter = callback;
    }

    void portfolio::TriggerComponent::Render() const
    {
        /*auto renderer = portfolio::Renderer::GetInstance().GetSDLRenderer();
        const auto& pos = GetOwner()->GetTransform().GetPosition();

        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        int thickness = 3;

        for (int i = 0; i < thickness; ++i)
        {
            SDL_FRect rect{};
            rect.x = pos.x + i;
            rect.y = pos.y + i;
            rect.w = m_Width - (i * 2);
            rect.h = m_Height - (i * 2);

            SDL_RenderRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, r, g, b, a);*/
    }
}