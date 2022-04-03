#pragma once

#include "Common.h"
#include "Animation.h"

class Assests
{
	std::map<std::string, sf::Texture>	m_textureMap;
	std::map<std::string, Animation>	m_animationMap;
	std::map<std::string, sf::Font>		m_fontMap;
	std::map<std::string, sf::Shader>	m_fragShaderMap;

	void addTexture(const std::string& textureName, const std::string& path, bool smooth);
	void addAnimation(const std::string& animationName, const std::string& textureName, size_t frameCount, size_t speed);
	void addFont(const std::string& fontName, const std::string& path);
	void addFragShader(const std::string& shaderName, const std::string& path);

public:
	Assests();

	void loadFromFile(const std::string& path);

	const sf::Texture&	getTexture(const std::string& textureMap) const;
	const Animation&	getAnimation(const std::string& animationName) const;
	const sf::Font& getFont(const std::string& fontName) const;
	sf::Shader* getFragShader_ptr(const std::string& shaderName);
};