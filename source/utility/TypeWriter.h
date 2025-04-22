#pragma once

#include <string>
#include <Windows.h>
#include <vector>
#include "timer.h"
#include "Colours.h"

class Console;

class Typewriter
{
private:
    Console& m_Console;
    std::wstring m_sText, m_sCurrentText;
    int m_x, m_y, m_BorderX, m_BorderWidth, m_BorderHeight;
    int m_TextSpeed, m_TextWrap, m_CharIndex, m_TextIndex, m_BorderY, m_Index;
    WORD m_TextColour, m_BorderColour;
    Timer m_Timer;
    bool m_bFinished;

    std::vector<std::wstring> m_sTextChunks;

    bool SetBorderProperties();
    void DrawBorder();
    void ClearArea();

public:
    Typewriter(Console& console);
    Typewriter(Console& console, int start_x, int start_y, const std::wstring& text, int text_wrap, int speed, WORD textColour = WHITE, WORD borderColour = WHITE);

    bool SetText(const std::wstring& text);
    inline void SetBorderColour(WORD colour) { m_BorderColour = colour; }

    void UpdateText();
    void Draw(bool showBorder = true);
    inline const bool IsFinished() const { return m_bFinished; }
};
