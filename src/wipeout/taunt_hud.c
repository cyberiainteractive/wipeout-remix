#include "../types.h"
#include "../utils.h"
#include "../render.h"

#include "taunt_hud.h"
#include "multiplayer/taunt.h"
#include "game.h"
#include "image.h"
#include "ui.h"

static texture_list_t pilot_portraits;

#define TAUNT_HUD_WIDTH        156
#define TAUNT_HUD_HEIGHT       44
#define TAUNT_HUD_AVATAR_SIZE  32

void taunt_hud_load(void)
{
    pilot_portraits =
        image_get_compressed_textures("wipeout/common/pilots.cmp");
}

void taunt_hud_draw(void)
{
    int pilot = taunt_get_active_pilot();

    if (pilot < 0 || pilot >= NUM_PILOTS) {
        return;
    }

    /*
     * Anchor the panel to the middle-right of the screen.
     * All offsets and dimensions use the regular HUD scaling.
     */
    vec2i_t pos = ui_scaled_pos(
        UI_POS_MIDDLE | UI_POS_RIGHT,
        vec2i(-TAUNT_HUD_WIDTH - 8, -TAUNT_HUD_HEIGHT / 2)
    );

    vec2i_t panel_size = ui_scaled(
        vec2i(TAUNT_HUD_WIDTH, TAUNT_HUD_HEIGHT)
    );

    /* Panel background. */
    render_push_2d(
        pos,
        panel_size,
        rgba(0, 0, 0, 192),
        RENDER_NO_TEXTURE
    );

    /*
     * Pilot portraits are stored in the shared pilot texture
     * list. The pilot index selects the corresponding portrait.
     */
    uint16_t portrait = texture_from_list(
        pilot_portraits,
        pilot
    );

    /* Avatar position and size. */
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

    /* TAUNT label. */
    vec2i_t label_pos = ui_scaled_pos(
        UI_POS_MIDDLE | UI_POS_RIGHT,
        vec2i(
            -TAUNT_HUD_WIDTH - 8 + 42,
            -TAUNT_HUD_HEIGHT / 2 + 11
        )
    );

    ui_draw_text(
        "TAUNT",
        label_pos,
        UI_SIZE_8,
        UI_COLOR_ACCENT
    );

    /* Pilot name. */
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
