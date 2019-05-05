#include "Renderer.hpp"

Renderer::Renderer() : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test")
{
    windows.resize(64);
    windows[0] = new nanogui::Window(this, "Button demo");
    windows[0]->setPosition(Eigen::Vector2i(15, 15));
    windows[0]->setLayout(new nanogui::GroupLayout());

    /* No need to store a pointer, the data structure will be automatically
        freed when the parent window is deleted */
    new nanogui::Label(windows[0], "Push buttons", "sans-bold");
    windowButtons.push_back(new nanogui::Button (windows[0], "Plain button"));
    windowButtons.resize(64);
    windowButtons[0]->setCallback([] { std::cout << "pushed!" << std::endl; });
    windowButtons[0]->setTooltip("short tooltip");

    windowButtons[1] = windows[0]->add<nanogui::Button>("Styled", ENTYPO_ICON_ROCKET);
    windowButtons[1]->setBackgroundColor(nanogui::Color(0, 0, 255, 25));
    windowButtons[1]->setCallback([] { std::cout << "pushed!" << std::endl; });
    windowButtons[1]->setTooltip("This button has a fairly long tooltip. It is so long, in "
            "fact, that the shown text will span several lines.");

    new nanogui::Label(windows[0], "Toggle buttons", "sans-bold");
    windowButtons[2] = new nanogui::Button(windows[0], "Toggle me");
    windowButtons[2]->setFlags(nanogui::Button::ToggleButton);
    windowButtons[2]->setChangeCallback([](bool state) { std::cout << "Toggle button state: " << state << std::endl; });

    new nanogui::Label(windows[0], "Radio buttons", "sans-bold");
    windowButtons[3] = new nanogui::Button(windows[0], "Radio button 1");
    windowButtons[3]->setFlags(nanogui::Button::RadioButton);

    windowButtons[4] = new nanogui::Button(windows[0], "Radio button 2");
    windowButtons[4]->setFlags(nanogui::Button::RadioButton);

    new nanogui::Label(windows[0], "A tool palette", "sans-bold");
    toolses.resize(64);
    toolses[0] = new nanogui::Widget(windows[0]);
    toolses[0]->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                    nanogui::Alignment::Middle, 0, 6));

    windowButtons[5] = new nanogui::ToolButton(toolses[0], ENTYPO_ICON_CLOUD);
    windowButtons[6] = new nanogui::ToolButton(toolses[0], ENTYPO_ICON_CONTROLLER_FAST_FORWARD);
    windowButtons[7] = new nanogui::ToolButton(toolses[0], ENTYPO_ICON_COMPASS);
    windowButtons[8] = new nanogui::ToolButton(toolses[0], ENTYPO_ICON_INSTALL);

    /* Allocating a new popupButton gives out a segmentation fault, fuck knows why :shrug: */
    popupBtns.resize(64);
    new nanogui::Label(windows[0], "Popup buttons", "sans-bold");
    popupBtns[0] = new nanogui::PopupButton(windows[0], "Popup", ENTYPO_ICON_EXPORT);
    popup = popupBtns[0]->popup();
    popup->setLayout(new nanogui::GroupLayout());
    new nanogui::Label(popup, "Arbitrary widgets can be placed here");
    new nanogui::CheckBox(popup, "A check box");
    // popup right
    popupBtns[1] = new nanogui::PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
    nanogui::Popup *popupRight = popupBtns[1]->popup();
    popupRight->setLayout(new nanogui::GroupLayout());
    new nanogui::CheckBox(popupRight, "Another check box");
    // popup left
    popupBtns[2] = new nanogui::PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
    popupBtns[2]->setSide(nanogui::Popup::Side::Left);
    nanogui::Popup *popupLeft = popupBtns[2]->popup();
    popupLeft->setLayout(new nanogui::GroupLayout()); 
    new nanogui::CheckBox(popupLeft, "Another check box");

    windows[1] = new nanogui::Window(this, "Basic widgets");
    windows[1]->setPosition(Eigen::Vector2i(200, 15));
    windows[1]->setLayout(new nanogui::GroupLayout());

    new nanogui::Label(windows[1], "Message dialog", "sans-bold");
    toolses[1] = new nanogui::Widget(windows[1]);
    toolses[1]->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                    nanogui::Alignment::Middle, 0, 6));
    windowButtons[9] = new nanogui::Button(toolses[1], "Info");
    windowButtons[9]->setCallback([&] {
        auto dlg = new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Title", "This is an information message");
        dlg->setCallback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
    });
    windowButtons[10] = new nanogui::Button(toolses[1], "Warn");
    windowButtons[10]->setCallback([&] {
        auto dlg = new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Warning, "Title", "This is a warning message");
        dlg->setCallback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
    });
    windowButtons[11] = new nanogui::Button(toolses[1], "Ask");
    windowButtons[11]->setCallback([&] {
        auto dlg = new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Warning, "Title", "This is a question message", "Yes", "No", true);
        dlg->setCallback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
    });
    performLayout();

    mShader.init(
            /* An identifying name */
            "a_simple_shader",

            /* Vertex shader */
            "#version 330\n"
            "uniform mat4 modelViewProj;\n"
            "in vec3 position;\n"
            "void main() {\n"
            "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
            "}",

            /* Fragment shader */
            "#version 330\n"
            "out vec4 color;\n"
            "uniform float intensity;\n"
            "void main() {\n"
            "    color = vec4(vec3(intensity), 1.0);\n"
            "}"
        );

        nanogui::MatrixXu indices(3, 2); /* Draw 2 triangles */
        indices.col(0) << 0, 1, 2;
        indices.col(1) << 2, 3, 0;

        nanogui::MatrixXf positions(3, 4);
        positions.col(0) << -1, -1, 0;
        positions.col(1) <<  1, -1, 0;
        positions.col(2) <<  1,  1, 0;
        positions.col(3) << -1,  1, 0;

        mShader.bind();
        mShader.uploadIndices(indices);
        mShader.uploadAttrib("position", positions);
        mShader.setUniform("intensity", 0.5f);
}

void Renderer::drawContents() {
        /* Draw the window contents using OpenGL */
        mShader.bind();

        nanogui::Matrix4f mvp;
        mvp.setIdentity();
        mvp.topLeftCorner<3,3>() = nanogui::Matrix3f(Eigen::AngleAxisf((float) glfwGetTime(),  nanogui::Vector3f::UnitZ())) * 0.25f;

        mvp.row(0) *= (float) mSize.y() / (float) mSize.x();

        mShader.setUniform("modelViewProj", mvp);

        /* Draw 2 triangles starting at index 0 */
        mShader.drawIndexed(GL_TRIANGLES, 0, 2);
    }

Renderer::~Renderer()
{
    mShader.free();
}