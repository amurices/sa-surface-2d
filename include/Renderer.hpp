#pragma once

#define GL_RG                             0x8227
#define GL_RG_INTEGER                     0x8228
#define GL_R8                             0x8229
#define GL_R16                            0x822A
#define GL_RG8                            0x822B
#define GL_RG16                           0x822C
#define GL_R16F                           0x822D
#define GL_R32F                           0x822E
#define GL_RG16F                          0x822F
#define GL_RG32F                          0x8230
#define GL_R8I                            0x8231
#define GL_R8UI                           0x8232
#define GL_R16I                           0x8233
#define GL_R16UI                          0x8234
#define GL_R32I                           0x8235
#define GL_R32UI                          0x8236
#define GL_RG8I                           0x8237
#define GL_RG8UI                          0x8238
#define GL_RG16I                          0x8239
#define GL_RG16UI                         0x823A
#define GL_RG32I                          0x823B
#define GL_RG32UI                         0x823C

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/colorpicker.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <iostream>
#include <string>

#include <stdio.h>
#include <iostream>
#include <string>

#include "Util.hpp"
#include "IO.hpp"
#include "GlobalState.hpp"

// #include <lemon/list_graph.h>
// #include <FTGL/ftgl.h>

#if defined(NANOGUI_GLAD)
    #if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
        #define GLAD_GLAPI_EXPORT
    #endif

    #include <glad/glad.h>
#else
    #if defined(__APPLE__)
        #define GLFW_INCLUDE_GLCOREARB
    #else
        #define GL_GLEXT_PROTOTYPES
    #endif
#endif

/* Renderer class which abstracts away the drawing of elements. */
class Renderer : public nanogui::Screen
{
  public:
    std::vector<nanogui::Window*> windows;
    std::vector<nanogui::Button*> windowButtons;
    std::vector<nanogui::Widget*> toolses;
    std::vector<nanogui::PopupButton*> popupBtns;
    std::vector<nanogui::TextBox*> textBoxes;
    nanogui::Popup *popup;

    Renderer();
    nanogui::TextBox* makeForm(nanogui::Widget *parent,
                    const std::string &name,
                    const std::string &initialValue,
                    const std::string &unit);
    void makeInputForms(nanogui::Window* targetWindow);
    static int countNumberOfLines();
    void uploadIndices();
    void uploadSurface();
    virtual void drawContents();
    ~Renderer();
    nanogui::GLShader mShader;
};
