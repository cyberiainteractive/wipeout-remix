
#include "../types.h"
#include "../render.h"

#include "taunt_hud.h"
#include "multiplayer/taunt.h"
#include "game.h"
#include "image.h"
#include "ui.h"

static texture_list_t pilot_portraits[NUM_PILOTS];

#define TAUNT_HUD_WIDTH        156
#define TAUNT_HUD_HEIGHT       44
#define TAUNT_HUD_AVATAR_SIZE  32

static vec2i_t taunt_hud_add(vec2i_t a, vec2i_t b)
{
    return vec2i(a.x + b.x, a.y + b.y);
}

void taunt_hud_load(void)
{
    for (int i = 0; i < NUM_PILOTS; i++) {
        pilot_portraits[i] =
            image_get_compressed_textures(def.pilots[i].portrait);
    }
}

void taunt_hud_draw(void)
{
    int pilot = taunt_get_active_pilot();

    if (pilot < 0 || pilot >= NUM_PILOTS) {
        return;
    }

    vec2i_t pos = ui_scaled_pos(
        UI_POS_MIDDLE | UI_POS_RIGHT,
        vec2i(
            -TAUNT_HUD_WIDTH - 8,
            -TAUNT_HUD_HEIGHT / 2
        )
    );

    vec2i_t panel_size = ui_scaled(
        vec2i(TAUNT_HUD_WIDTH, TAUNT_HUD_HEIGHT)
    );

    render_push_2d(
        pos,
        panel_size,
        rgba(0, 0, 0, 192),
        RENDER_NO_TEXTURE
    );

    uint16_t portrait = texture_from_list(
        pilot_portraits[pilot],
        1
    );

    vec2i_t avatar_pos = taunt_hud_add(
        pos,
        ui_scaled(vec2i(4, 6))
    );

    render_push_2d(
        avatar_pos,
        ui_scaled(
            vec2i(
                TAUNT_HUD_AVATAR_SIZE,
                TAUNT_HUD_AVATAR_SIZE
            )
        ),
        rgba(255, 255, 255, 255),
        portrait
    );

    vec2i_t text_pos = taunt_hud_add(
        pos,
        ui_scaled(vec2i(42, 11))
    );

    ui_draw_text(
        "VOICE COMM",
        text_pos,
        UI_SIZE_8,
        UI_COLOR_ACCENT
    );

    text_pos.y += ui_scaled(vec2i(0, 12)).y;

    ui_draw_text(
        def.pilots[pilot].name,
        text_pos,
        UI_SIZE_8,
        UI_COLOR_DEFAULT
    );
}
