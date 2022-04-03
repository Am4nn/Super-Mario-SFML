#include "Animation.h"

Animation::Animation() {}

Animation::Animation(const std::string& name, const sf::Texture& texture)
	: Animation(name, texture, 1, 0) {}

Animation::Animation(const std::string& name, const sf::Texture& texture, size_t frameCount, size_t speed)
	: m_name(name), m_sprite(texture), m_frameCount(frameCount), m_currentFrame(0), m_speed(speed)
{
	m_size = Vec2((float)texture.getSize().x / frameCount, (float)texture.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
	
	std::cout << name << " " << m_size.x << " " << m_size.y << " " << std::endl;
}

// Update the animation to show the next frame, depending on its speed
// animation loops when it reaches the end
void Animation::update()
{
	// add the speed variable to the current frame
	m_currentFrame++;

	//		1) calculate the current frame of animation to play based on currentFrame and speed
	//		2) set the texture rectange properly (see constructor for sample)
	sf::IntRect rectangle;
	if (m_speed == 0) 
	{
		rectangle = sf::IntRect(0, 0, m_size.x, m_size.y);
	}
	else
	{
		size_t frame = (m_currentFrame / m_speed) % m_frameCount;
		rectangle = sf::IntRect(frame * m_size.x, 0, m_size.x, m_size.y);
	}
	m_sprite.setTextureRect(rectangle);
}

bool Animation::hasEnded() const
{
	// detect when animation has ended (last frame was played) and return true;
	if (m_frameCount == m_currentFrame) { return true; } // check
	return false;
}

const std::string& Animation::getName() const
{
	return m_name;
}

const Vec2& Animation::getSize() const
{
	return m_size;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}