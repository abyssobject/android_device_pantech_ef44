/*
 * Pantech CameraParameters members the JB ef44 camera HAL
 * (camera.vendor.msm8960.so) imports. Android 12's libcamera_client has
 * none of them; values come from the cm-14.1 CameraParametersExtra.h, where
 * the colour-extraction accessors were no-ops as well.
 */
#define EXPORT __attribute__((visibility("default")))

EXPORT const char _ZN7android16CameraParameters10H324MVT_ONE[] = "on"; /* H324MVT_ON */
EXPORT const char _ZN7android16CameraParameters11KEY_REFLECTE[] = "pantech-reflect"; /* KEY_REFLECT */
EXPORT const char _ZN7android16CameraParameters11REFLECT_OFFE[] = "off"; /* REFLECT_OFF */
EXPORT const char _ZN7android16CameraParameters11SKY_IPL_OFFE[] = "off"; /* SKY_IPL_OFF */
EXPORT const char _ZN7android16CameraParameters12ANTISHAKE_ONE[] = "on"; /* ANTISHAKE_ON */
EXPORT const char _ZN7android16CameraParameters12CAMNOTE_BL_BE[] = "bl-b"; /* CAMNOTE_BL_B */
EXPORT const char _ZN7android16CameraParameters12CAMNOTE_WB_BE[] = "wb-b"; /* CAMNOTE_WB_B */
EXPORT const char _ZN7android16CameraParameters12CAMNOTE_WB_CE[] = "wb-c"; /* CAMNOTE_WB_C */
EXPORT const char _ZN7android16CameraParameters13ANTISHAKE_OFFE[] = "off"; /* ANTISHAKE_OFF */
EXPORT const char _ZN7android16CameraParameters13KEY_ANTISHAKEE[] = "pantech-antishake"; /* KEY_ANTISHAKE */
EXPORT const char _ZN7android16CameraParameters13REFLECT_WATERE[] = "water"; /* REFLECT_WATER */
EXPORT const char _ZN7android16CameraParameters14CAMNOTE_NORMALE[] = "normal"; /* CAMNOTE_NORMAL */
EXPORT const char _ZN7android16CameraParameters14REFLECT_MIRRORE[] = "mirror"; /* REFLECT_MIRROR */
EXPORT const char _ZN7android16CameraParameters14SKY_IPL_BEAUTYE[] = "pantech-ipl-beauty"; /* SKY_IPL_BEAUTY */
EXPORT const char _ZN7android16CameraParameters14SKY_IPL_SKETCHE[] = "pantech-ipl-sketch"; /* SKY_IPL_SKETCH */
EXPORT const char _ZN7android16CameraParameters15SCENE_MODE_TEXTE[] = "text"; /* SCENE_MODE_TEXT */
EXPORT const char _ZN7android16CameraParameters15SKY_IPL_CAMNOTEE[] = "pantech-ipl-camnote"; /* SKY_IPL_CAMNOTE */
EXPORT const char _ZN7android16CameraParameters16KEY_CAMNOTE_MODEE[] = "camnote-mode"; /* KEY_CAMNOTE_MODE */
EXPORT const char _ZN7android16CameraParameters16KEY_SKY_IPL_MODEE[] = "pantech-ipl-mode"; /* KEY_SKY_IPL_MODE */
EXPORT const char _ZN7android16CameraParameters16SKY_COLOREXT_OFFE[] = "off"; /* SKY_COLOREXT_OFF */
EXPORT const char _ZN7android16CameraParameters16SKY_COLOREXT_REDE[] = "colorext-red"; /* SKY_COLOREXT_RED */
EXPORT const char _ZN7android16CameraParameters17KEY_SUPPORTED_HDRE[] = "hdr-values"; /* KEY_SUPPORTED_HDR */
EXPORT const char _ZN7android16CameraParameters17KEY_SUPPORTED_OJTE[] = "pantech-supported-ojt"; /* KEY_SUPPORTED_OJT */
EXPORT const char _ZN7android16CameraParameters17SCENE_MODE_INDOORE[] = "indoor"; /* SCENE_MODE_INDOOR */
EXPORT const char _ZN7android16CameraParameters17SKY_COLOREXT_BLUEE[] = "colorext-blue"; /* SKY_COLOREXT_BLUE */
EXPORT const char _ZN7android16CameraParameters17SKY_IPL_MINIATUREE[] = "pantech-ipl-miniature"; /* SKY_IPL_MINIATURE */
EXPORT const char _ZN7android16CameraParameters17WHITE_BALANCE_OFFE[] = "off"; /* WHITE_BALANCE_OFF */
EXPORT const char _ZN7android16CameraParameters18SKY_COLOREXT_GREENE[] = "colorext-green"; /* SKY_COLOREXT_GREEN */
EXPORT const char _ZN7android16CameraParameters18SKY_COLOREXT_TOUCHE[] = "colorext-touch"; /* SKY_COLOREXT_TOUCH */
EXPORT const char _ZN7android16CameraParameters19EFFECT_BLACKBOARD_CE[] = "blackboard-c"; /* EFFECT_BLACKBOARD_C */
EXPORT const char _ZN7android16CameraParameters19EFFECT_WHITEBOARD_CE[] = "whiteboard-c"; /* EFFECT_WHITEBOARD_C */
EXPORT const char _ZN7android16CameraParameters19KEY_SKY_IPL_VALUE_1E[] = "value-1"; /* KEY_SKY_IPL_VALUE_1 */
EXPORT const char _ZN7android16CameraParameters19KEY_SKY_IPL_VALUE_2E[] = "value-2"; /* KEY_SKY_IPL_VALUE_2 */
EXPORT const char _ZN7android16CameraParameters19SKY_COLOREXT_YELLOWE[] = "colorext-yellow"; /* SKY_COLOREXT_YELLOW */
EXPORT const char _ZN7android16CameraParameters19SKY_IPL_CAM_CARTOONE[] = "pantech-ipl-cam-cartoon"; /* SKY_IPL_CAM_CARTOON */
EXPORT const char _ZN7android16CameraParameters20KEY_FACE_FILTER_RECTE[] = "pantech-face-filter-rect"; /* KEY_FACE_FILTER_RECT */
EXPORT const char _ZN7android16CameraParameters20REFLECT_MIRROR_WATERE[] = "mirror-water"; /* REFLECT_MIRROR_WATER */
EXPORT const char _ZN7android16CameraParameters21KEY_SUPPORTED_REFLECTE[] = "pantech-supported-reflect"; /* KEY_SUPPORTED_REFLECT */
EXPORT const char _ZN7android16CameraParameters22FLASH_MODE_TORCH_FLASHE[] = "tourch-flash"; /* FLASH_MODE_TORCH_FLASH */
EXPORT const char _ZN7android16CameraParameters22KEY_SKY_COLOREXT_RGB_BE[] = "pantech-colorextraction-rgb-b"; /* KEY_SKY_COLOREXT_RGB_B */
EXPORT const char _ZN7android16CameraParameters22KEY_SKY_COLOREXT_RGB_GE[] = "pantech-colorextraction-rgb-g"; /* KEY_SKY_COLOREXT_RGB_G */
EXPORT const char _ZN7android16CameraParameters22KEY_SKY_COLOREXT_RGB_RE[] = "pantech-colorextraction-rgb-r"; /* KEY_SKY_COLOREXT_RGB_R */
EXPORT const char _ZN7android16CameraParameters22SKY_IPL_WIDGET_CARTOONE[] = "pantech-ipl-widget-cartoon"; /* SKY_IPL_WIDGET_CARTOON */
EXPORT const char _ZN7android16CameraParameters23KEY_SKY_COLOREXTRACTIONE[] = "pantech-colorextraction"; /* KEY_SKY_COLOREXTRACTION */
EXPORT const char _ZN7android16CameraParameters23KEY_SUPPORTED_ANTISHAKEE[] = "pantech-antishake-values"; /* KEY_SUPPORTED_ANTISHAKE */
EXPORT void _ZN7android16CameraParameters23setColorExtractionRGB_BEii(void) {} /* no-op, as in cm-14.1 */
EXPORT void _ZN7android16CameraParameters23setColorExtractionRGB_GEii(void) {} /* no-op, as in cm-14.1 */
EXPORT void _ZN7android16CameraParameters23setColorExtractionRGB_REii(void) {} /* no-op, as in cm-14.1 */
EXPORT const char _ZN7android16CameraParameters25FLASH_MODE_ZSL_TORCH_AUTOE[] = "torch-zsl-auto"; /* FLASH_MODE_ZSL_TORCH_AUTO */
EXPORT const char _ZN7android16CameraParameters28KEY_SKY_COLOREXT_COORDINATESE[] = "T"; /* KEY_SKY_COLOREXT_COORDINATES */
EXPORT void _ZN7android16CameraParameters29setColorExtractionCoordinatesEii(void) {} /* no-op, as in cm-14.1 */
EXPORT const char _ZN7android16CameraParameters30KEY_SKY_COLOREXT_COORDINATES_BE[] = "pantech-color-extraction-coordinates-b"; /* KEY_SKY_COLOREXT_COORDINATES_B */
EXPORT const char _ZN7android16CameraParameters30KEY_SKY_COLOREXT_COORDINATES_GE[] = "pantech-color-extraction-coordinates-g"; /* KEY_SKY_COLOREXT_COORDINATES_G */
EXPORT const char _ZN7android16CameraParameters30KEY_SKY_COLOREXT_COORDINATES_RE[] = "pantech-color-extraction-coordinates-r"; /* KEY_SKY_COLOREXT_COORDINATES_R */
EXPORT const char _ZN7android16CameraParameters33KEY_SUPPORTED_SKY_COLOREXTRACTIONE[] = "T"; /* KEY_SUPPORTED_SKY_COLOREXTRACTION */
EXPORT const char _ZN7android16CameraParameters6HDR_ONE[] = "hdr-on"; /* HDR_ON */
EXPORT const char _ZN7android16CameraParameters6KEY_VTE[] = "pantech-vt"; /* KEY_VT */
EXPORT const char _ZN7android16CameraParameters6OJT_ONE[] = "on"; /* OJT_ON */
EXPORT const char _ZN7android16CameraParameters6VTS_ONE[] = "on"; /* VTS_ON */
EXPORT const char _ZN7android16CameraParameters6VT_OFFE[] = "off"; /* VT_OFF */
EXPORT const char _ZN7android16CameraParameters7HDR_OFFE[] = "hdr-off"; /* HDR_OFF */
EXPORT const char _ZN7android16CameraParameters7HDVT_ONE[] = "off"; /* HDVT_ON */
EXPORT const char _ZN7android16CameraParameters7KEY_HDRE[] = "hdr"; /* KEY_HDR */
EXPORT const char _ZN7android16CameraParameters7KEY_OJTE[] = "pantech-ojt"; /* KEY_OJT */
EXPORT const char _ZN7android16CameraParameters7KEY_VTSE[] = "pantech-vts"; /* KEY_VTS */
EXPORT const char _ZN7android16CameraParameters7OJT_OFFE[] = "off"; /* OJT_OFF */
EXPORT const char _ZN7android16CameraParameters7VTS_OFFE[] = "off"; /* VTS_OFF */
EXPORT const char _ZN7android16CameraParameters9KEY_FLASHE[] = "pantech-flash"; /* KEY_FLASH */
EXPORT void _ZNK7android16CameraParameters23getColorExtractionRGB_BEPiS1_(void) {} /* no-op, as in cm-14.1 */
EXPORT void _ZNK7android16CameraParameters23getColorExtractionRGB_GEPiS1_(void) {} /* no-op, as in cm-14.1 */
EXPORT void _ZNK7android16CameraParameters23getColorExtractionRGB_REPiS1_(void) {} /* no-op, as in cm-14.1 */
EXPORT void _ZNK7android16CameraParameters29getColorExtractionCoordinatesEPiS1_(void) {} /* no-op, as in cm-14.1 */
