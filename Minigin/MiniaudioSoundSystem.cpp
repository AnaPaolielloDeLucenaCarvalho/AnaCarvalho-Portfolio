#include "MiniaudioSoundSystem.h"

#define MA_NO_WASAPI
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <iostream>

namespace dae
{
    struct SoundRequest
    {
        sound_id id;
        float volume;
    };

    class MiniaudioSoundSystem::MiniaudioSoundSystemImpl
    {
    public:
        MiniaudioSoundSystemImpl()
        {
            if (ma_engine_init(NULL, &m_AudioEngine) != MA_SUCCESS) 
            {
                std::cerr << "Failed to initialize miniaudio engine.\n";
            }

#ifndef __EMSCRIPTEN__
            m_Thread = std::jthread(&MiniaudioSoundSystemImpl::ProcessQueue, this);
#endif
        }

        ~MiniaudioSoundSystemImpl()
        {
#ifndef __EMSCRIPTEN__
            m_Quit = true;
            m_Condition.notify_one();
#endif
            for (auto& pair : m_Sounds)
            {
                ma_sound_uninit(pair.second.get());
            }
            ma_engine_uninit(&m_AudioEngine);
        }

        void Play(sound_id id, float volume)
        {
#ifndef __EMSCRIPTEN__
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Queue.push({ id, volume });
            m_Condition.notify_one();
#else
            if (m_Sounds.contains(id)) 
            {
                ma_sound_set_volume(m_Sounds[id].get(), volume);
                if (ma_sound_at_end(m_Sounds[id].get())) 
                {
                    ma_sound_seek_to_pcm_frame(m_Sounds[id].get(), 0);
                }
                ma_sound_start(m_Sounds[id].get());
            }
#endif
        }

        void LoadSound(sound_id id, const std::string& filePath)
        {
            auto sound = std::make_unique<ma_sound>();

            if (ma_sound_init_from_file(&m_AudioEngine, filePath.c_str(), 0, NULL, NULL, sound.get()) == MA_SUCCESS) 
            {
                if (id == 0) 
                {
                    ma_sound_set_looping(sound.get(), MA_TRUE);
                }
                m_Sounds[id] = std::move(sound);
            }
        }

        void ToggleMute()
        {
            m_Muted = !m_Muted;
            ma_engine_set_volume(&m_AudioEngine, m_Muted ? 0.0f : 1.0f);
        }

    private:
#ifndef __EMSCRIPTEN__
        void ProcessQueue()
        {
            while (true)
            {
                std::unique_lock<std::mutex> lock(m_Mutex);
                m_Condition.wait(lock, [this]() { return !m_Queue.empty() || m_Quit; });

                if (m_Quit && m_Queue.empty()) break;

                SoundRequest request = m_Queue.front();
                m_Queue.pop();
                lock.unlock();

                if (m_Sounds.contains(request.id))
                {
                    ma_sound_set_volume(m_Sounds[request.id].get(), request.volume);

                    if (ma_sound_at_end(m_Sounds[request.id].get())) 
                    {
                        ma_sound_seek_to_pcm_frame(m_Sounds[request.id].get(), 0);
                    }

                    ma_sound_start(m_Sounds[request.id].get());
                }
            }
        }

        std::jthread m_Thread;
        std::mutex m_Mutex;
        std::condition_variable m_Condition;
        std::queue<SoundRequest> m_Queue;
        bool m_Quit{ false };
#endif

        ma_engine m_AudioEngine;
        std::unordered_map<sound_id, std::unique_ptr<ma_sound>> m_Sounds;
        bool m_Muted{ false };
    };

    MiniaudioSoundSystem::MiniaudioSoundSystem() : pImpl(std::make_unique<MiniaudioSoundSystemImpl>()) {}
    MiniaudioSoundSystem::~MiniaudioSoundSystem() = default;
    void MiniaudioSoundSystem::play(const sound_id id, const float volume) { pImpl->Play(id, volume); }
    void MiniaudioSoundSystem::loadSound(const sound_id id, const std::string& filePath) { pImpl->LoadSound(id, filePath); }
    void MiniaudioSoundSystem::ToggleMute() { pImpl->ToggleMute(); }
}