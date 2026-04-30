#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "Font.h"

TTF_Font* portfolio::Font::GetFont() const {
	return m_font;
}

portfolio::Font::Font(const std::string& fullPath, float size) : m_font(nullptr)
{
	m_font = TTF_OpenFont(fullPath.c_str(), size);
	if (m_font == nullptr) 
	{
		throw std::runtime_error(std::string("Failed to load font: ") + SDL_GetError());
	}
}

portfolio::Font::~Font()
{
	TTF_CloseFont(m_font);
}
