#include "Console.h"
#include "Logger.h"
#include <algorithm>
#include <vector>
#include <cassert>

bool Console::SetTextColour(int size, int x, int y, HANDLE handle, WORD colour)
{
    COORD pos = { x, y };

    std::vector<WORD> write(size, colour);

    DWORD written = 0;
    LPDWORD lpNumWritten = &written;

    if (!WriteConsoleOutputAttribute(handle, &write[0], size, pos, lpNumWritten))
    {
        TRPG_ERROR("Unable to change text colour! ");
        return false;
    }

    return true;
}

void Console::DrawBorder()
{
    DrawPanel(1, 0, SCREEN_WIDTH - 2, SCREEN_HEIGHT - 1);
}

Console::Console()
    : m_pScreen(nullptr)
{
    // Initialize the screen buffer
    m_pScreen = std::make_unique<wchar_t[]>(BUFFER_SIZE);

    // Get a handle to the console window
    m_hConsoleWindow = GetConsoleWindow();

    if (!GetWindowRect(m_hConsoleWindow, &m_ConsoleWindowRect))
    {
        throw("Failed to get the Window Rect when creating the console!");
    }

    // Get the font size
    CONSOLE_FONT_INFO font_info;
    if (!GetCurrentConsoleFont(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &font_info))
        throw("Failed to get the console font!");

    COORD font_size = GetConsoleFontSize(GetStdHandle(STD_OUTPUT_HANDLE), font_info.nFont);

    int actual_screen_x = SCREEN_WIDTH * font_size.X + 16;
    int actual_screen_y = SCREEN_HEIGHT * font_size.Y + 48;

    int pos_x = GetSystemMetrics(SM_CXSCREEN) / 2 - (actual_screen_x / 2);
    int pos_y = GetSystemMetrics(SM_CYSCREEN) / 2 - (actual_screen_y / 2);

    if (!MoveWindow(m_hConsoleWindow, pos_x, pos_y, actual_screen_x, actual_screen_y, TRUE))
        throw("Failed to set Console Window!");

    //clear buffer
    ClearBuffer();

    m_hConsole = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL

    );
    if (!m_hConsole)
        throw("Failed!");

    if (!SetConsoleActiveScreenBuffer(m_hConsole))
        throw("Failed!");

    if (!ShowConsoleCursor(false))
        throw("Failed!");
}

Console::~Console()
{


}

void Console::ClearBuffer()
{
    for (int i = 0; i < BUFFER_SIZE; i++)
        m_pScreen[i] = L' ';

}

void Console::Write(int x, int y, const std::wstring& text, WORD colour)
{
    std::vector<wchar_t> invalidCharacters{ L' ', L'\n', L'\t', L'\r' };

    auto is_any_of = [&](wchar_t character) {
        if (text.size() > 1)
            return false;

        if (text.empty())
            return true;

        return character == text[0];
        };

    if (std::find_if(invalidCharacters.begin(), invalidCharacters.end(), is_any_of) == std::end(invalidCharacters))
        SetTextColour(text.size(), x, y, m_hConsole, colour);


    int pos = y * SCREEN_WIDTH + x;

    assert(pos + text.size() < BUFFER_SIZE);

    if (pos + text.size() >= BUFFER_SIZE)
    {
        TRPG_ERROR("This is beyond the buffer size!");
    }

    swprintf(&m_pScreen[pos], BUFFER_SIZE, text.c_str());
}

void Console::Draw()
{
    DrawBorder();

    WriteConsoleOutputCharacter(m_hConsole, m_pScreen.get(), BUFFER_SIZE, { 0, 0 }, &m_BytesWritten);
}

bool Console::ShowConsoleCursor(bool show)
{
    CONSOLE_CURSOR_INFO cursorInfo;
    if (!GetConsoleCursorInfo(m_hConsole, &cursorInfo))
    {
        TRPG_ERROR("Failed");
        return false;
    }
    cursorInfo.bVisible = show;

    return SetConsoleCursorInfo(m_hConsole, &cursorInfo);

}

void Console::DrawPanelHorz(int x, int y, size_t length, WORD colour, const std::wstring& character)
{
    std::wstring sPanelHorz = L"";
    for (int i = 0; i < length; i++)
        sPanelHorz += character;

    Write(x, y, sPanelHorz, colour);
}

void Console::DrawPanelVert(int x, int y, size_t height, WORD colour, const std::wstring& character)
{
    for (int i = 0; i < height; i++)
        Write(x, y + i, character, colour);
}

void Console::DrawPanel(int x, int y, size_t width, size_t height, WORD colour, const std::wstring& width_char, const std::wstring& height_char)
{
    DrawPanelHorz(x, y, width, colour, width_char);
    DrawPanelHorz(x, y + height, width, colour, width_char);

    DrawPanelVert(x, y + 1, height - 1, colour, height_char);
    DrawPanelVert(x + width - 1, y + 1, height - 1, colour, height_char);
}
