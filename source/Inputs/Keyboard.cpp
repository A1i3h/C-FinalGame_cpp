#include "Keyboard.h"
#include "../Logger.h"

Keyboard::Keyboard()
{

}

void Keyboard::Update()
{
	for (int i = 0; i < KEY_LAST; i++)
	{
		m_keys[i].m_bIsJustPressed = false;
		m_keys[i].m_bIsJustPressed = false;
	}
}

void Keyboard::OnKeyDown(int key)
{
	if (key > KEY_LAST)
	{
		TRPG_ERROR("[" + std::to_string(key) + "] - Is not defined!");
		return;
	}
	m_keys[key].Update(true);
}

void Keyboard::OnKeyUp(int key)
{
	if (key > KEY_LAST)
	{
		TRPG_ERROR("[" + std::to_string(key) + "] - Is not defined!");
		return;
	}
	m_keys[key].Update(false);
}

bool Keyboard::IsKeyHeld(int key) const
{
	if (key > KEY_LAST)
	{
		TRPG_ERROR("[" + std::to_string(key) + "] - Is not defined!");
		return false;
	}
	return m_keys[key].m_bIsDown;
}

bool Keyboard::IsKeyJustPressed(int key) const
{
	if (key > KEY_LAST)
	{
		TRPG_ERROR("[" + std::to_string(key) + "] - Is not defined!");
		return false;
	}
	return m_keys[key].m_bIsJustPressed;
}

bool Keyboard::IsKeyJustReleased(int key) const
{
	if (key > KEY_LAST)
	{
		TRPG_ERROR("[" + std::to_string(key) + "] - Is not defined!");
		return false;
	}
	return m_keys[key].m_bIsJustReleased;
}
