#include "types/vector.h"
#include "types.h"
#include "Widget.h"

struct Point
{
    u16 x;
    u16 y;
};

class TextBox : public Widget
{
public:
    TextBox(u16 x, u16 y, u16 width, u16 height);
    ~TextBox() = default;

    void draw(u32* frame_buffer, const u32 window_width, const u32 window_height) const override;

    void set_cursor_position(Point position);
    Point get_cursor_position();

    void set_character(char c, Point position);

private:
    Point m_cursor_position;
    Vector<char> m_screen_text;
};
