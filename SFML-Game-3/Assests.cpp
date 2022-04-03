#include "Assests.h"
#include <cassert>

Assests::Assests() {}

void Assests::addTexture(const std::string& textureName, const std::string& path, bool smooth = true)
{
	m_textureMap[textureName] = sf::Texture();

	if (!m_textureMap[textureName].loadFromFile(path))
	{
		std::cerr << "Could not load texture file : " << path << std::endl;
		m_textureMap.erase(textureName);
	}
	else
	{
		m_textureMap[textureName].setSmooth(smooth);
		std::cout << "Loaded Texture : " << path << "\n";
	}
}

void Assests::addAnimation(const std::string& animationName, const std::string& textureName, size_t frameCount, size_t speed)
{
	m_animationMap[animationName] = Animation(animationName, getTexture(textureName), frameCount, speed);
	std::cout << "Animation added : " << animationName << std::endl;
}

void Assests::addFont(const std::string& fontName, const std::string& path)
{
	m_fontMap[fontName] = sf::Font();

	if (!m_fontMap[fontName].loadFromFile(path))
	{
		std::cerr << "Could not load font file : " << path << std::endl;
		m_fontMap.erase(fontName);
	}
	else
	{
		std::cout << "Loaded Font : " << path << "\n";
	}
}

void Assests::addFragShader(const std::string& shaderName, const std::string& path)
{
	if (!m_fragShaderMap[shaderName].loadFromFile(path, sf::Shader::Fragment))
	{
		std::cerr << "Failed loading Fragment Shader" << path << '\n';
	}
	else
	{
		std::cout << "Fragment Shader added : " << path << '\n';
	}
}

void Assests::loadFromFile(const std::string& path)
{
	std::ifstream file(path);
	std::string str;
	while (file.good())
	{
		file >> str;

		if (str == "Texture")
		{
			std::string name, path;
			file >> name >> path;
			addTexture(name, path);
		}
		else if (str == "Animation")
		{
			std::string name, texture;
			size_t frames, speed;
			file >> name >> texture >> frames >> speed;
			addAnimation(name, texture, frames, speed);
		}
		else if (str == "Font")
		{
			std::string name, path;
			file >> name >> path;
			std::cout << "Font : ";
			addFont(name, path);
		}
		else if (str == "ShaderFrag")
		{
			std::string name, path;
			file >> name >> path;
			std::cout << "Shader : ";
			addFragShader(name, path);
		}
		else
		{
			std::cerr << "Unknown Asset Type : " << str << std::endl;
		}
	}
	std::cout << "Everything loaded successfully\n";
}


const sf::Texture& Assests::getTexture(const std::string& textureName) const
{
	assert(m_textureMap.find(textureName) != m_texture.end());
	return m_textureMap.at(textureName);
}

const Animation& Assests::getAnimation(const std::string& animationName) const
{
	assert(m_animationMap.find(animationName) != m_animationMap.end());
	return m_animationMap.at(animationName);
}

const sf::Font& Assests::getFont(const std::string& fontName) const
{
	assert(m_fontMap.find(fontName) != m_fontMap.end());
	return m_fontMap.at(fontName);
}

sf::Shader* Assests::getFragShader_ptr(const std::string& fragShaderName)
{
	assert(m_fragShaderMap.find(shaderName) != m_fragShaderMap.end());
	return &m_fragShaderMap.at(fragShaderName);
}