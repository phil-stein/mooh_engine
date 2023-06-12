#include "mui/mui.h"

void mui_test()
{
  {
    // mui_text_l(VEC3_XY(0, 0), "(0, 0)");
    // mui_text_l(VEC2_XY(100, 100), "(100, 100)");
    mui_text(VEC2_XY(w/2, h/2), "(right|up)",     TEXT_RIGHT | TEXT_UP);
    mui_text(VEC2_XY(w/2, h/2), "(right|middle)", TEXT_RIGHT | TEXT_MIDDLE);
    mui_text(VEC2_XY(w/2, h/2), "(right|down)",   TEXT_RIGHT | TEXT_DOWN);
    h += 150;
    mui_text(VEC2_XY(w/2, h/2), "(left|up)",     TEXT_LEFT | TEXT_UP);
    mui_text(VEC2_XY(w/2, h/2), "(left|middle)", TEXT_LEFT | TEXT_MIDDLE);
    mui_text(VEC2_XY(w/2, h/2), "(left|down)",   TEXT_LEFT | TEXT_DOWN);
    h += 150;
    mui_text(VEC2_XY(w/2, h/2), "(center|up)",     TEXT_CENTER | TEXT_UP);
    mui_text(VEC2_XY(w/2, h/2), "(center|middle)", TEXT_CENTER | TEXT_MIDDLE);
    mui_text(VEC2_XY(w/2, h/2), "(center|down)",   TEXT_CENTER | TEXT_DOWN);

    mui_text_l(VEC2_XY(0, 0), "-(0, 0)");
    mui_text_l(VEC2_XY(0.5f, 0.5f), "-(0.5, 0.5)");
    mui_text_l(VEC2_XY(1, 1), "-(1, 1)");
    mui_text_l(VEC2_XY(-0.5f, -0.5f), "-(-0.5, -0.5)");
    mui_text(VEC2_XY(-1, -1), "-(-1, -1)", TEXT_LEFT | TEXT_UP);

  }
  {
    mui_quad(VEC2_XY(0, 0), VEC2(1), RGB_F(0, 1, 1));
    // mui_quad(VEC2_XY(0, 1), VEC2(1), RGB_F(1, 0, 1));
    texture_t* t = assetm_get_texture("#internal/preview_bg.png", true);
    mui_img(VEC2_XY(0, 2), VEC2(1), t);
    mui_img_tint(VEC2_XY(2, 2), VEC2(1), t, RGB_F(0, 1, 1));
  }
}
