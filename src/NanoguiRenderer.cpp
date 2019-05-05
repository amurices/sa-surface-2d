// #include "GLFW/glfw3.h"
// #define GL_RG                             0x8227
// #define GL_RG_INTEGER                     0x8228
// #define GL_R8                             0x8229
// #define GL_R16                            0x822A
// #define GL_RG8                            0x822B
// #define GL_RG16                           0x822C
// #define GL_R16F                           0x822D
// #define GL_R32F                           0x822E
// #define GL_RG16F                          0x822F
// #define GL_RG32F                          0x8230
// #define GL_R8I                            0x8231
// #define GL_R8UI                           0x8232
// #define GL_R16I                           0x8233
// #define GL_R16UI                          0x8234
// #define GL_R32I                           0x8235
// #define GL_R32UI                          0x8236
// #define GL_RG8I                           0x8237
// #define GL_RG8UI                          0x8238
// #define GL_RG16I                          0x8239
// #define GL_RG16UI                         0x823A
// #define GL_RG32I                          0x823B
// #define GL_RG32UI                         0x823C

// #include <nanogui/opengl.h>
// #include <nanogui/glutil.h>
// #include <nanogui/screen.h>
// #include <nanogui/window.h>
// #include <nanogui/layout.h>
// #include <nanogui/label.h>
// #include <nanogui/checkbox.h>
// #include <nanogui/button.h>
// #include <nanogui/toolbutton.h>
// #include <nanogui/popupbutton.h>
// #include <nanogui/combobox.h>
// #include <nanogui/progressbar.h>
// #include <nanogui/entypo.h>
// #include <nanogui/messagedialog.h>
// #include <nanogui/textbox.h>
// #include <nanogui/slider.h>
// #include <nanogui/imagepanel.h>
// #include <nanogui/imageview.h>
// #include <nanogui/vscrollpanel.h>
// #include <nanogui/colorwheel.h>
// #include <nanogui/colorpicker.h>
// #include <nanogui/graph.h>
// #include <nanogui/tabwidget.h>
// #include <iostream>
// #include <string>

// #if defined(__GNUC__)
// #  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
// #endif
// #if defined(_WIN32)
// #  pragma warning(push)
// #  pragma warning(disable: 4457 4456 4005 4312)
// #endif

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

// #if defined(_WIN32)
// #  pragma warning(pop)
// #endif
// #if defined(_WIN32)
// #  if defined(APIENTRY)
// #    undef APIENTRY
// #  endif
// #  include <windows.h>
// #endif

// class NewRenderer : public nanogui::Screen {
// public:
//     std::vector<nanogui::Window*> windows;
//     std::vector<nanogui::Button*> windowButtons;
//     std::vector<nanogui::Widget*> toolses;
//     std::vector<nanogui::PopupButton*> popupBtns;
//     nanogui::Popup *popup;
//     NewRenderer() : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test") {
//         using namespace nanogui;
//         windows.resize(64);
//         windows[0] = new nanogui::Window(this, "Button demo");
//         windows[0]->setPosition(Vector2i(15, 15));
//         windows[0]->setLayout(new GroupLayout());

//         /* No need to store a pointer, the data structure will be automatically
//            freed when the parent window is deleted */
//         new Label(windows[0], "Push buttons", "sans-bold");
//         windowButtons.resize(64);
//         windowButtons.push_back(new Button (windows[0], "Plain button"));
//         windowButtons[0]->setCallback([] { std::cout << "pushed!" << std::endl; });
//         windowButtons[0]->setTooltip("short tooltip");

//         windowButtons[1] = windows[0]->add<nanogui::Button>("Styled", ENTYPO_ICON_ROCKET);
//         windowButtons[1]->setBackgroundColor(Color(0, 0, 255, 25));
//         windowButtons[1]->setCallback([] { std::cout << "pushed!" << std::endl; });
//         windowButtons[1]->setTooltip("This button has a fairly long tooltip. It is so long, in "
//                 "fact, that the shown text will span several lines.");

//         new Label(windows[0], "Toggle buttons", "sans-bold");
//         windowButtons[2] = new nanogui::Button(windows[0], "Toggle me");
//         windowButtons[2]->setFlags(nanogui::Button::ToggleButton);
//         windowButtons[2]->setChangeCallback([](bool state) { std::cout << "Toggle button state: " << state << std::endl; });

//         new Label(windows[0], "Radio buttons", "sans-bold");
//         windowButtons[3] = new nanogui::Button(windows[0], "Radio button 1");
//         windowButtons[3]->setFlags(Button::RadioButton);

//         windowButtons[4] = new nanogui::Button(windows[0], "Radio button 2");
//         windowButtons[4]->setFlags(Button::RadioButton);

//         new Label(windows[0], "A tool palette", "sans-bold");
//         toolses[0] = new nanogui::Widget(windows[0]);
//         toolses[0]->setLayout(new BoxLayout(Orientation::Horizontal,
//                                        Alignment::Middle, 0, 6));

//         windowButtons[5] = new ToolButton(toolses[0], ENTYPO_ICON_CLOUD);
//         windowButtons[6] = new ToolButton(toolses[0], ENTYPO_ICON_CONTROLLER_FAST_FORWARD);
//         windowButtons[7] = new ToolButton(toolses[0], ENTYPO_ICON_COMPASS);
//         windowButtons[8] = new ToolButton(toolses[0], ENTYPO_ICON_INSTALL);

//         popupBtns.resize(64);
//         new Label(windows[0], "Popup buttons", "sans-bold");
//         popupBtns[0] = new nanogui::PopupButton(windows[0], "Popup", ENTYPO_ICON_EXPORT);
//         popup = popupBtns[0]->popup();
//         popup->setLayout(new GroupLayout());
//         new Label(popup, "Arbitrary widgets can be placed here");
//         new CheckBox(popup, "A check box");
//         // popup right
//         popupBtns[1] = new nanogui::PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
//         nanogui::Popup *popupRight = popupBtns[1]->popup();
//         popupRight->setLayout(new GroupLayout());
//         new CheckBox(popupRight, "Another check box");
//         // popup left
//         popupBtns[2] = new nanogui::PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
//         popupBtns[2]->setSide(nanogui::Popup::Side::Left);
//         nanogui::Popup *popupLeft = popupBtns[2]->popup();
//         popupLeft->setLayout(new GroupLayout()); 
//         new CheckBox(popupLeft, "Another check box");

//         windows[1] = new nanogui::Window(this, "Basic widgets");
//         windows[1]->setPosition(Vector2i(200, 15));
//         windows[1]->setLayout(new GroupLayout());

//         new Label(windows[1], "Message dialog", "sans-bold");
//         toolses[1] = new nanogui::Widget(windows[1]);
//         toolses[1]->setLayout(new BoxLayout(Orientation::Horizontal,
//                                        Alignment::Middle, 0, 6));
//         windowButtons[9] = new nanogui::Button(toolses[1], "Info");
//         windowButtons[9]->setCallback([&] {
//             auto dlg = new MessageDialog(this, MessageDialog::Type::Information, "Title", "This is an information message");
//             dlg->setCallback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
//         });
//         windowButtons[10] = new nanogui::Button(toolses[1], "Warn");
//         windowButtons[10]->setCallback([&] {
//             auto dlg = new MessageDialog(this, MessageDialog::Type::Warning, "Title", "This is a warning message");
//             dlg->setCallback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
//         });
//         windowButtons[11] = new nanogui::Button(toolses[1], "Ask");
//         windowButtons[11]->setCallback([&] {
//             auto dlg = new MessageDialog(this, MessageDialog::Type::Warning, "Title", "This is a question message", "Yes", "No", true);
//             dlg->setCallback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
//         });

//         std::vector<std::pair<int, std::string>>
//             icons = loadImageDirectory(mNVGContext, "icons");
//         #if defined(_WIN32)
//             std::string resourcesFolderPath("../resources/");
//         #else
//             std::string resourcesFolderPath("./");
//         #endif

//         new Label(windows[1], "Image panel & scroll panel", "sans-bold");
//         popupBtns[2] = new nanogui::PopupButton(windows[1], "Image Panel");
//         popupBtns[2]->setIcon(ENTYPO_ICON_FOLDER);
//         popup = popupBtns[2]->popup();
//         VScrollPanel *vscroll = new VScrollPanel(popup);
//         popup->setFixedSize(Vector2i(245, 150));

//         windows[2] = new nanogui::Window(this, "Selected image");
//         windows[2]->setPosition(Vector2i(710, 15));
//         windows[2]->setLayout(new GroupLayout());

//         new Label(windows[2], "File dialog", "sans-bold");
//         toolses[2] = new nanogui::Widget(windows[2]);
//         toolses[2]->setLayout(new BoxLayout(Orientation::Horizontal,
//                                        Alignment::Middle, 0, 6));
//         windowButtons[12] = new nanogui::Button(toolses[2], "Open");
//         windowButtons[12]->setCallback([&] {
//             std::cout << "File dialog result: " << file_dialog(
//                     { {"png", "Portable Network Graphics"}, {"txt", "Text file"} }, false) << std::endl;
//         });
//         windowButtons[13] = new nanogui::Button(toolses[2], "Save");
//         windowButtons[13]->setCallback([&] {
//             std::cout << "File dialog result: " << file_dialog(
//                     { {"png", "Portable Network Graphics"}, {"txt", "Text file"} }, true) << std::endl;
//         });

//         new Label(windows[1], "Combo box", "sans-bold");
//         new ComboBox(windows[1], { "Combo box item 1", "Combo box item 2", "Combo box item 3"});
//         new Label(windows[1], "Check box", "sans-bold");
//         CheckBox *cb = new CheckBox(windows[1], "Flag 1",
//             [](bool state) { std::cout << "Check box 1 state: " << state << std::endl; }
//         );
//         cb->setChecked(true);
//         cb = new CheckBox(windows[1], "Flag 2",
//             [](bool state) { std::cout << "Check box 2 state: " << state << std::endl; }
//         );
//         new Label(windows[1], "Progress bar", "sans-bold");
//         mProgress = new ProgressBar(windows[1]);

//         new Label(windows[1], "Slider and text box", "sans-bold");

//         nanogui::Widget *panel = new nanogui::Widget(windows[1]);
//         panel->setLayout(new BoxLayout(Orientation::Horizontal,
//                                        Alignment::Middle, 0, 20));

//         Slider *slider = new Slider(panel);
//         slider->setValue(0.5f);
//         slider->setFixedWidth(80);

//         TextBox *textBox = new TextBox(panel);
//         textBox->setFixedSize(Vector2i(60, 25));
//         textBox->setValue("50");
//         textBox->setUnits("%");
//         slider->setCallback([textBox](float value) {
//             textBox->setValue(std::to_string((int) (value * 100)));
//         });
//         slider->setFinalCallback([&](float value) {
//             std::cout << "Final slider value: " << (int) (value * 100) << std::endl;
//         });
//         textBox->setFixedSize(Vector2i(60,25));
//         textBox->setFontSize(20);
//         textBox->setAlignment(TextBox::Alignment::Right);

//         windows[3] = new nanogui::Window(this, "Misc. widgets");
//         windows[3]->setPosition(Vector2i(425,15));
//         windows[3]->setLayout(new GroupLayout());

//         nanogui::TabWidget* tabWidget = windows[3]->add<nanogui::TabWidget>();

//         nanogui::Widget* layer = tabWidget->createTab("Color Wheel");
//         layer->setLayout(new GroupLayout());

//         // Use overloaded variadic add to fill the tab widget with Different tabs.
//         layer->add<Label>("Color wheel widget", "sans-bold");
//         layer->add<ColorWheel>();

//         layer = tabWidget->createTab("Function Graph");
//         layer->setLayout(new GroupLayout());

//         layer->add<Label>("Function graph widget", "sans-bold");

//         Graph *graph = layer->add<Graph>("Some Function");

//         graph->setHeader("E = 2.35e-3");
//         graph->setFooter("Iteration 89");
//         VectorXf &func = graph->values();
//         func.resize(100);
//         for (int i = 0; i < 100; ++i)
//             func[i] = 0.5f * (0.5f * std::sin(i / 10.f) +
//                               0.5f * std::cos(i / 23.f) + 1);

//         // Dummy tab used to represent the last tab button.
//         tabWidget->createTab("+");

//         // A simple counter.
//         int counter = 1;
//         tabWidget->setCallback([tabWidget, this, counter] (int index) mutable {
//             if (index == (tabWidget->tabCount()-1)) {
//                 // When the "+" tab has been clicked, simply add a new tab.
//                 std::string tabName = "Dynamic " + std::to_string(counter);
//                 nanogui::Widget* layerDyn = tabWidget->createTab(index, tabName);
//                 layerDyn->setLayout(new GroupLayout());
//                 layerDyn->add<Label>("Function graph widget", "sans-bold");
//                 Graph *graphDyn = layerDyn->add<Graph>("Dynamic function");

//                 graphDyn->setHeader("E = 2.35e-3");
//                 graphDyn->setFooter("Iteration " + std::to_string(index*counter));
//                 VectorXf &funcDyn = graphDyn->values();
//                 funcDyn.resize(100);
//                 for (int i = 0; i < 100; ++i)
//                     funcDyn[i] = 0.5f *
//                         std::abs((0.5f * std::sin(i / 10.f + counter) +
//                                   0.5f * std::cos(i / 23.f + 1 + counter)));
//                 ++counter;
//                 // We must invoke perform layout from the screen instance to keep everything in order.
//                 // This is essential when creating tabs dynamically.
//                 performLayout();
//                 // Ensure that the newly added header is visible on screen
//                 tabWidget->ensureTabVisible(index);

//             }
//         });
//         tabWidget->setActiveTab(0);

//         // A button to go back to the first tab and scroll the window.
//         panel = windows[3]->add<nanogui::Widget>();
//         panel->add<Label>("Jump to tab: ");
//         panel->setLayout(new BoxLayout(Orientation::Horizontal,
//                                        Alignment::Middle, 0, 6));

//         auto ib = panel->add<IntBox<int>>();
//         ib->setEditable(true);

//         windowButtons[14] = panel->add<Button>("", ENTYPO_ICON_FORWARD);
//         windowButtons[14]->setFixedSize(Vector2i(22, 22));
//         ib->setFixedHeight(22);
//         windowButtons[14]->setCallback([tabWidget, ib] {
//             int value = ib->value();
//             if (value >= 0 && value < tabWidget->tabCount()) {
//                 tabWidget->setActiveTab(value);
//                 tabWidget->ensureTabVisible(value);
//             }
//         });

//         windows[4] = new nanogui::Window(this, "Grid of small widgets");
//         windows[4]->setPosition(Vector2i(425, 300));
//         GridLayout *layout =
//             new GridLayout(Orientation::Horizontal, 2,
//                            Alignment::Middle, 15, 5);
//         layout->setColAlignment(
//             { Alignment::Maximum, Alignment::Fill });
//         layout->setSpacing(0, 10);
//         windows[4]->setLayout(layout);

//         /* FP widget */ {
//             new Label(windows[4], "Floating point :", "sans-bold");
//             textBox = new TextBox(windows[4]);
//             textBox->setEditable(true);
//             textBox->setFixedSize(Vector2i(100, 20));
//             textBox->setValue("50");
//             textBox->setUnits("GiB");
//             textBox->setDefaultValue("0.0");
//             textBox->setFontSize(16);
//             textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");
//         }

//         /* Positive integer widget */ {
//             new Label(windows[4], "Positive integer :", "sans-bold");
//             auto intBox = new IntBox<int>(windows[4]);
//             intBox->setEditable(true);
//             intBox->setFixedSize(Vector2i(100, 20));
//             intBox->setValue(50);
//             intBox->setUnits("Mhz");
//             intBox->setDefaultValue("0");
//             intBox->setFontSize(16);
//             intBox->setFormat("[1-9][0-9]*");
//             intBox->setSpinnable(true);
//             intBox->setMinValue(1);
//             intBox->setValueIncrement(2);
//         }

//         /* Checkbox widget */ {
//             new Label(windows[4], "Checkbox :", "sans-bold");

//             cb = new CheckBox(windows[4], "Check me");
//             cb->setFontSize(16);
//             cb->setChecked(true);
//         }

//         new Label(windows[4], "Combo box :", "sans-bold");
//         ComboBox *cobo =
//             new ComboBox(windows[4], { "Item 1", "Item 2", "Item 3" });
//         cobo->setFontSize(16);
//         cobo->setFixedSize(Vector2i(100,20));

//         new Label(windows[4], "Color picker :", "sans-bold");
//         auto cp = new ColorPicker(windows[4], {255, 120, 0, 255});
//         cp->setFixedSize({100, 20});
//         cp->setFinalCallback([](const Color &c) {
//             std::cout << "ColorPicker Final Callback: ["
//                       << c.r() << ", "
//                       << c.g() << ", "
//                       << c.b() << ", "
//                       << c.w() << "]" << std::endl;
//         });
//         // setup a fast callback for the color picker widget on a new window
//         // for demonstrative purposes
//         windows[5] = new nanogui::Window(this, "Color Picker Fast Callback");
//         layout =
//             new GridLayout(Orientation::Horizontal, 2,
//                            Alignment::Middle, 15, 5);
//         layout->setColAlignment(
//             { Alignment::Maximum, Alignment::Fill });
//         layout->setSpacing(0, 10);
//         windows[5]->setLayout(layout);
//         windows[5]->setPosition(Vector2i(425, 500));
//         new Label(windows[5], "Combined: ");
//         windowButtons[15] = new nanogui::Button(windows[5], "ColorWheel", ENTYPO_ICON_500PX);
//         new Label(windows[5], "Red: ");
//         auto redIntBox = new IntBox<int>(windows[5]);
//         redIntBox->setEditable(false);
//         new Label(windows[5], "Green: ");
//         auto greenIntBox = new IntBox<int>(windows[5]);
//         greenIntBox->setEditable(false);
//         new Label(windows[5], "Blue: ");
//         auto blueIntBox = new IntBox<int>(windows[5]);
//         blueIntBox->setEditable(false);
//         new Label(windows[5], "Alpha: ");
//         auto alphaIntBox = new IntBox<int>(windows[5]);
//         nanogui::Button* b = windowButtons[14];
//         cp->setCallback([b,redIntBox,blueIntBox,greenIntBox,alphaIntBox](const Color &c) {
//             b->setBackgroundColor(c);
//             b->setTextColor(c.contrastingColor());
//             int red = (int) (c.r() * 255.0f);
//             redIntBox->setValue(red);
//             int green = (int) (c.g() * 255.0f);
//             greenIntBox->setValue(green);
//             int blue = (int) (c.b() * 255.0f);
//             blueIntBox->setValue(blue);
//             int alpha = (int) (c.w() * 255.0f);
//             alphaIntBox->setValue(alpha);

//         });

//         performLayout();

//         /* All NanoGUI widgets are initialized at this point. Now
//            create an OpenGL shader to draw the main window contents.

//            NanoGUI comes with a simple Eigen-based wrapper around OpenGL 3,
//            which eliminates most of the tedious and error-prone shader and
//            buffer object management.
//         */

//         mShader.init(
//             /* An identifying name */
//             "a_simple_shader",

//             /* Vertex shader */
//             "#version 330\n"
//             "uniform mat4 modelViewProj;\n"
//             "in vec3 position;\n"
//             "void main() {\n"
//             "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
//             "}",

//             /* Fragment shader */
//             "#version 330\n"
//             "out vec4 color;\n"
//             "uniform float intensity;\n"
//             "void main() {\n"
//             "    color = vec4(vec3(intensity), 1.0);\n"
//             "}"
//         );

//         MatrixXu indices(2, 50); /* Draw 50 lines */
//         for (size_t i = 0; i < 50; i++){
//             indices.col(i) << i, i + 1;
//         }

//         MatrixXf positions(3, 51);
//         for (size_t i = 0; i < 51; i++){
//             positions.col(i) << ((float)i - 25.0f) / 50.0f, (i % 2), 0;
//         }
        
//         mShader.bind();
//         mShader.uploadIndices(indices);
//         mShader.uploadAttrib("position", positions);
//         mShader.setUniform("intensity", 0.5f);
//     }

//     ~NewRenderer() {
//         mShader.free();
//     }

//     virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
//         if (Screen::keyboardEvent(key, scancode, action, modifiers))
//             return true;
//         if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
//             setVisible(false);
//             return true;
//         }
//         return false;
//     }

//     virtual void draw(NVGcontext *ctx) {
//         /* Animate the scrollbar */
//         mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));

//         /* Draw the user interface */
//         Screen::draw(ctx);
//     }

//     virtual void drawContents() {
//         using namespace nanogui;

//         /* Draw the window contents using OpenGL */
//         mShader.bind();

//         Matrix4f mvp;
//         mvp.setIdentity();
//     //    mvp.topLeftCorner<3,3>() = Matrix3f(Eigen::AngleAxisf((float) glfwGetTime(),  Vector3f::UnitZ())) * 0.45f;

//         mvp.row(0) *= (float) mSize.y() / (float) mSize.x();

//         mShader.setUniform("modelViewProj", mvp);
//         mShader.drawIndexed(GL_LINES, 0, 50);
//     }
// private:
//     nanogui::ProgressBar *mProgress;
//     nanogui::GLShader mShader;

// };
