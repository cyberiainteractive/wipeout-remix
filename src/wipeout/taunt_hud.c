#include "../types.h"
#include "../utils.h"
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
        vec2i(-TAUNT_HUD_WIDTH - 8, -TAUNT_HUD_HEIGHT / 2)
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
        pilot_portraits,
        pilot
    );

    vec2i_t avatar_pos = ui_scaled_pos(
        UI_POS_MIDDLE | UI_POS_RIGHT,
        vec2i(
            -TAUNT_HUD_WIDTH - 8 + 4,
            -TAUNT_HUD_HEIGHT / 2 + 6
        )
    );

    vec2i_t avatar_size = ui_scaled(
        vec2i(TAUNT_HUD_AVATAR_SIZE, TAUNT_HUD_AVATAR_SIZE)
    );

    render_push_2d(
        avatar_pos,
        avatar_size,
        rgba(255, 255, 255, 255),
        portrait
    );

    vec2i_t label_pos = ui_scaled_pos(
        UI_POS_MIDDLE | UI_POS_RIGHT,
        vec2i(
            -TAUNT_HUD_WIDTH - 8 + 42,
            -TAUNT_HUD_HEIGHT / 2 + 11
        )
    );

    ui_draw_text(
        "VOICE COMM",
        label_pos,
        UI_SIZE_8,
        UI_COLOR_ACCENT
    );

    vec2i_t name_pos = ui_scaled_pos(
        UI_POS_MIDDLE | UI_POS_RIGHT,
        vec2i(
            -TAUNT_HUD_WIDTH - 8 + 42,
            -TAUNT_HUD_HEIGHT / 2 + 23
        )
    );

    ui_draw_text(
        def.pilots[pilot].name,
        name_pos,
        UI_SIZE_8,
        UI_COLOR_DEFAULT
    );
}
