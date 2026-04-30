#pragma once
#include "Component.h"
#include <functional>

namespace portfolio
{
    class TriggerComponent : public Component
    {
    public:
        TriggerComponent(GameObject* pOwner, float width, float height);
        ~TriggerComponent() = default;

        void Update(float deltaTime) override;
        void Render() const override;

        void SetTarget(GameObject* target, float targetWidth, float targetHeight);

        void SetOnTriggerEnter(std::function<void()> callback);

        void SetOnTriggerExit(std::function<void()> callback);
        bool IsInside() const { return m_IsInside; }

    private:
        float m_Width;
        float m_Height;

        GameObject* m_Target{ nullptr };
        float m_TargetWidth{ 0 };
        float m_TargetHeight{ 0 };

        std::function<void()> m_OnTriggerEnter;
        std::function<void()> m_OnTriggerExit;
        bool m_IsInside{ false };

        bool m_FirstFrame{ true };
    };
}