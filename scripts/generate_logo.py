#!/usr/bin/env python3
"""Generate the brick-wall style title logo: assets/images/logo.png

Self-made asset in the style of assets/images/title.png (red brick letters
with light mortar on a transparent background), using the game's own
joystix.ttf so it matches the in-game text. Run from the project root:

    python scripts/generate_logo.py
"""

from PIL import Image, ImageDraw, ImageFont, ImageFilter

TEXT = "TANK BATTLE"
FONT_PATH = "assets/joystix.ttf"
OUT_PATH = "assets/images/logo.png"

FONT_SIZE = 56
PAD = 8

BRICK_H = 7       # brick row height in px
MORTAR = 2        # mortar gap in px
BRICK_W = 18      # brick length in px (excluding vertical mortar)

BRICK_COLOR = (202, 74, 46)
MORTAR_COLOR = (236, 226, 208, 255)
OUTLINE_COLOR = (12, 10, 10, 255)


def brick_pattern(width, height):
    """RGB brick pattern with staggered rows and slight per-brick shading."""
    img = Image.new("RGBA", (width, height))
    px = img.load()
    period_y = BRICK_H + MORTAR
    period_x = BRICK_W + MORTAR
    for y in range(height):
        row = y // period_y
        in_horizontal_mortar = (y % period_y) >= BRICK_H
        shift = (row % 2) * (period_x // 2)
        for x in range(width):
            if in_horizontal_mortar:
                px[x, y] = MORTAR_COLOR
                continue
            bx = (x + shift) // period_x
            if ((x + shift) % period_x) >= BRICK_W:
                px[x, y] = MORTAR_COLOR
                continue
            # deterministic per-brick brightness variation
            factor = 0.92 + ((bx * 7 + row * 13) % 5) * 0.04
            r = min(255, int(BRICK_COLOR[0] * factor))
            g = min(255, int(BRICK_COLOR[1] * factor))
            b = min(255, int(BRICK_COLOR[2] * factor))
            px[x, y] = (r, g, b, 255)
    return img


def main():
    font = ImageFont.truetype(FONT_PATH, FONT_SIZE)

    # Tight text bounding box
    probe = Image.new("L", (4, 4))
    probe_draw = ImageDraw.Draw(probe)
    left, top, right, bottom = probe_draw.textbbox((0, 0), TEXT, font=font)
    text_w, text_h = right - left, bottom - top

    width, height = text_w + 2 * PAD, text_h + 2 * PAD

    # Letter mask
    mask = Image.new("L", (width, height), 0)
    mask_draw = ImageDraw.Draw(mask)
    mask_draw.text((PAD - left, PAD - top), TEXT, font=font, fill=255)

    # Brick fill clipped by the letter mask
    bricks = brick_pattern(width, height)
    bricks.putalpha(mask)

    # Dark outline from a dilated mask, letters pasted on top
    outline_mask = mask.filter(ImageFilter.MaxFilter(5))
    logo = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    outline = Image.new("RGBA", (width, height), OUTLINE_COLOR)
    outline.putalpha(outline_mask)
    logo.alpha_composite(outline)
    logo.alpha_composite(bricks)

    logo.save(OUT_PATH)
    print(f"wrote {OUT_PATH} ({width}x{height})")


if __name__ == "__main__":
    main()
