#ifndef COLOR_H
#define COLOR_H

// --- gizmo ---

#define GIZMO_NONE_SIZE                     0.05f

#define GIZMO_TRANSLATE_LINE_WIDTH          8
#define GIZMO_TRANSLATE_LINE_COLOR          RGB_F_RGB(1)
#define GIZMO_TRANSLATE_LINE_SPHERES_COLOR  RGB_F_RGB(0.95f)

#define GIZMO_POINT_LIGHT_ROT               VEC3_XYZ(90, 0, 0)
#define GIZMO_POINT_LIGHT_SCL               VEC3(2.35f)
#define GIZMO_POINT_LIGHT_MESH              "gizmos/lightbulb"

// --- info --- 

// @TODO: phys collider colors


// - relation lines -

#define INFO_CHILD_LINE_WIDTH               8
#define INFO_CHILD_LINE_COLOR               RGB_F(0, 1, 1)
#define INFO_CHILD_LINE_SPHERES_COLOR       RGB_F_RGB(0.95f)

#define INFO_PARENT_LINE_WIDTH              INFO_CHILD_LINE_WIDTH
#define INFO_PARENT_LINE_COLOR              RGB_F(1, 1, 0)
#define INFO_PARENT_LINE_SPHERES_COLOR      RGB_F_RGB(0.95f)

// - physics -

#define INFO_COLLIDER_DYNAMIC_COLOR         RGB_F(1, 0, 1)
#define INFO_COLLIDER_STATIC_COLOR          RGB_F(1, 1, 0)
#define INFO_COLLIDER_TRIGGER_COLOR         RGB_F(0, 1, 1)

#define INFO_VELOCITY_LINE_COLOR            RGB_F(1, 1, 0)


#endif
