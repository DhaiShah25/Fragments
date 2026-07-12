#include "global.h"
#include <raylib.h>

bool draw_start_animation(float timePassed) {
  if (timePassed > 10.0) {
    return true;
  } else if (timePassed > 6.0) {
    DrawCircleV((Vector2){360, 360}, 40, (Color){0, 0, 0, 255});

    DrawLabel("Why Am I All Alone?", (Vector2){285, 410}, 20, (Color){245, 245, 245, 255 - (timePassed - 6) * 64});
    DrawLabel("Don't Leave Me All Alone", (Vector2){265, 440}, 20,
              (Color){245, 245, 245, 255 - 255 * 0.25 * (timePassed - 8) * (timePassed - 8)});
    DrawLabel("Why Can't I See Anyone?", (Vector2){270, 470}, 20, (Color){245, 245, 245, (timePassed - 6) * 64});
    DrawRectangleRec((Rectangle){0, 0, 720, 720}, (Color){0, 0, 0, (timePassed - 6) * 64});
  } else if (timePassed > 3.0) {
    DrawCircleV((Vector2){360, 360}, 40, (Color){0, 0, 0, 255});

    // Friends
    DrawCircleV((Vector2){320, 390}, 20, (Color){0, 0, 0, 255 - (timePassed - 3) * 85});
    DrawCircleV((Vector2){400, 310}, 20, (Color){0, 0, 0, 255 - (timePassed - 3) * 85});
  } else {
    DrawCircleV((Vector2){100 + (timePassed - 1.5) * (timePassed - 1.5) * -50, 360}, 40, (Color){0, 0, 0, 255});

    if (timePassed > 1.5) {
      DrawRectangle(140 + 220 / 1.5 * (timePassed - 1.5), 360, 20, 4, RAYWHITE);
    }

    if (timePassed > 2.75) {
      DrawCircle(340 - (timePassed - 2.75) * 200, 320, 5, (Color){100, 50, 100, 255});
      DrawCircle(320 - (timePassed - 2.75) * 200, 380, 5, (Color){100, 50, 100, 255});
      DrawCircle(360 - (timePassed - 2.75) * 200, 340, 5, (Color){100, 50, 100, 255});
      DrawCircle(380 - (timePassed - 2.75) * 200, 340, 5, (Color){100, 50, 100, 255});
      DrawCircle(330 - (timePassed - 2.75) * 200, 350, 5, (Color){100, 50, 100, 255});
      DrawCircle(370 - (timePassed - 2.75) * 200, 390, 5, (Color){100, 50, 100, 255});
    }

    DrawCircleV((Vector2){360, 360}, 40, (Color){0, 0, 0, 255});
    // Friends
    DrawCircleV((Vector2){320, 390}, 20, (Color){0, 0, 0, 255});
    DrawCircleV((Vector2){400, 310}, 20, (Color){0, 0, 0, 255});
  }
  return false;
}
