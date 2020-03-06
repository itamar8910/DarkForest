#include "Mouse.h"
#include "Math.h"

u32 Mouse::sprite_template[MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE] = {};
u32 Mouse::current_sprite[MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE] = {};

Mouse::Mouse(const int x, const int y) :
    m_x(x),
    m_y(y),
    m_hidden_by_mouse(MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE)
{
    init_sprite();
    memset(m_hidden_by_mouse.data(), 0, MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE*sizeof(u32));
    m_hidden_by_mouse.set_size(MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE);
}

void Mouse::draw(const RawMouseEvent& event, VGA& vga)
{
    int previous_mouseX = m_x;
    int previous_mouseY = m_y;

    m_x = Math::clamp(m_x + event.delta_x, 0, vga.width() - MOUSE_SPRITE_SIZE - 1);
    m_y = Math::clamp(m_y - event.delta_y, 0, vga.height() - MOUSE_SPRITE_SIZE - 1);

    Vector<u32> tmp_hidden_pixels = m_hidden_by_mouse;

    vga.draw(tmp_hidden_pixels.data(), previous_mouseX, previous_mouseY, MOUSE_SPRITE_SIZE, MOUSE_SPRITE_SIZE); 
    vga.copy_framebuffer_section(m_hidden_by_mouse.data(), m_x, m_y, MOUSE_SPRITE_SIZE, MOUSE_SPRITE_SIZE);

    for(uint32_t i = 0; i < MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE; ++i)
    {
        if(sprite_template[i] != 0)
        {
            current_sprite[i] = sprite_template[i];
            continue;
        }
        current_sprite[i] = m_hidden_by_mouse[i];
    }
    vga.draw(current_sprite, m_x, m_y, MOUSE_SPRITE_SIZE, MOUSE_SPRITE_SIZE); 
}

void Mouse::init_sprite()
{
    char sprite[MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE+1] =  \
        "*******"
        "****** "
        "*****  "
        "****   "
        "*** *  "
        "**   * "
        "*     *";
    
    for(uint32_t i = 0; i < MOUSE_SPRITE_SIZE*MOUSE_SPRITE_SIZE; ++i)
    {
        sprite_template[i] = (sprite[i] == '*') ? 0xffffffff : 0;
    }
}

u16 Mouse::x() const
{
    return m_x;
}

u16 Mouse::y() const
{
    return m_y;
}

Point Mouse::point() const
{
    return {m_x, m_y};
}
