#include "TypeWriter.h"
#include "../Logger.h"
#include "../Console.h"
#include <algorithm>

bool Typewriter::SetBorderProperties()
{
    m_BorderX = std::clamp(m_x - 2, 0, 127);
    m_BorderY = std::clamp(m_y - 2, 0, 47);

    m_BorderWidth = m_TextWrap + 2;
    m_BorderHeight = static_cast<int>(m_sTextChunks.size()) + 2;

    if (m_BorderHeight <= 2 || m_BorderWidth <= 2)
    {
        TRPG_ERROR("Border Height/Width is too small");
        return false;
    }

    if (m_BorderX + m_BorderWidth > 127 || m_BorderY + m_BorderHeight > 47)
    {
        TRPG_ERROR("Border x or y written beyond buffer");
        return false;
    }

    return true;
}

void Typewriter::DrawBorder()
{
    m_Console.DrawPanel(m_BorderX, m_BorderY, m_BorderWidth + 1, m_BorderHeight + 1, m_BorderColour);
}

void Typewriter::ClearArea()
{
    for (int i = 0; i <= m_BorderHeight; i++)
    {
        for (int j = 0; j <= m_BorderWidth; j++)
        {
            m_Console.Write(m_BorderX + j, m_BorderY + i, L" ");
        }
    }
}

Typewriter::Typewriter(Console& console)
    : Typewriter(console, 200, 1, L"SET TYPEWRITER TEXT!", 200, 1)
{
}

Typewriter::Typewriter(Console& console, int start_x, int start_y, const std::wstring& text, int text_wrap, int speed, WORD textColour, WORD borderColour)
    : m_Console(console), m_sText(text), m_sCurrentText(L""),
    m_x(start_x), m_y(start_y), m_BorderX(0), m_BorderY(0), m_BorderWidth(0), m_BorderHeight(0),
    m_TextSpeed(speed), m_TextWrap(text_wrap), m_CharIndex(0), m_TextIndex(0), m_Index(0), // Initialize m_Index
    m_TextColour(textColour), m_BorderColour(borderColour), m_Timer(), m_bFinished(false)
{
    if (!SetText(text))
    {
        TRPG_ERROR("Failed to initialise text!");
        return;
    }

    ClearArea();
    m_Timer.Start();
}



bool Typewriter::SetText(const std::wstring& text)
{
    m_sText = text;
    m_sTextChunks.clear();

    std::wstring text_holder;

    for (size_t i = 0; i < text.size(); i++)
    {
        text_holder += text[i];
        bool newLine = text[i] == '\n';

        if (text_holder.size() >= static_cast<size_t>(m_TextWrap) || newLine)
        {
            if (!newLine)
            {
                while (i > 0 && text[i] != ' ' && text[i] != '!' && text[i] != '?')
                {
                    i--;
                    if (!text_holder.empty()) text_holder.pop_back();
                }
            }

            if (!text_holder.empty())
            {
                m_sTextChunks.push_back(text_holder);
                text_holder.clear();
            }
        }
    }

    if (!text_holder.empty())
    {
        m_sTextChunks.push_back(text_holder);
    }

    if (!SetBorderProperties())
    {
        TRPG_ERROR("Failed to set Border Properties!");
        return false;
    }

    m_CharIndex = 0;
    m_TextIndex = 0;
    m_bFinished = false;
    m_sCurrentText.clear();

    return true;
}

void Typewriter::UpdateText()
{
    if (!m_Timer.IsRunning() || m_bFinished)
        return;

    if (m_Timer.ElapsedMS() > m_TextSpeed * m_Index &&
        m_TextIndex < m_sTextChunks.size() &&
        m_Index < m_sText.size())
    {
        m_sCurrentText += m_sTextChunks[m_TextIndex][m_CharIndex++];

        if (m_CharIndex >= m_sTextChunks[m_TextIndex].size())
        {
            m_CharIndex = 0;
            m_TextIndex++;
            m_y++;
            m_sCurrentText.clear();
        }

        m_Index++;
    }

    if (m_Index >= m_sText.size() + 1)
    {
        m_Timer.Stop();
        m_bFinished = true;
    }
}


void Typewriter::Draw(bool showBorder)
{
    if (!m_bFinished)
    {
        m_Console.Write(m_x, m_y, m_sCurrentText, m_TextColour);

        if (showBorder)
            DrawBorder();
    }
}
