#include <SDL3/SDL.h>
#include "Texture2D.h"
#include "Renderer.h"
#include <stdexcept>

portfolio::Texture2D::~Texture2D()
{
	SDL_DestroyTexture(m_texture);
}

glm::vec2 portfolio::Texture2D::GetSize() const
{
    float w{}, h{};
    SDL_GetTextureSize(m_texture, &w, &h);
    return { w, h };
}

SDL_Texture* portfolio::Texture2D::GetSDLTexture() const
{
	return m_texture;
}

portfolio::Texture2D::Texture2D(const std::string &fullPath)
{
    SDL_Surface* surface = SDL_LoadPNG(fullPath.c_str());
    if (!surface)
    {
        throw std::runtime_error(
            std::string("Failed to load PNG: ") + SDL_GetError()
        );
    }

    // Magenta (R:255, G:0, B:255) should be transparent!
    const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(surface->format);
    Uint32 transparentColor = SDL_MapRGB(format, nullptr, 255, 0, 255);
    SDL_SetSurfaceColorKey(surface, true, transparentColor);

    m_texture = SDL_CreateTextureFromSurface(
        Renderer::GetInstance().GetSDLRenderer(),
        surface
    );

    SDL_DestroySurface(surface);

    if (!m_texture)
    {
        throw std::runtime_error(
            std::string("Failed to create texture from surface: ") + SDL_GetError()
        );
    }
}

portfolio::Texture2D::Texture2D(SDL_Texture* texture)	: m_texture{ texture } 
{
	assert(m_texture != nullptr);
}

