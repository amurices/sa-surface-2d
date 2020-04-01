#include <fstream>
#include <MathGeometry.hpp>
#include <SurfaceProperties.hpp>
#include "Renderer.hpp"

nanogui::TextBox *Renderer::makeForm(nanogui::Widget *parent,
                                     const std::string &name,
                                     const std::string &initialValue,
                                     const std::string &unit) {
    nanogui::TextBox *textBox;

    /* FP widget */
    new nanogui::Label(parent, name + ": ", "sans-bold");
    textBox = new nanogui::TextBox(parent);
    textBox->setEditable(true);
    textBox->setFixedSize(nanogui::Vector2i(200, 20));
    textBox->setUnits(unit);
    textBox->setFontSize(16);
    textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");
    textBox->setValue(initialValue);
    return textBox;
}

Renderer::Renderer() : nanogui::Screen(Eigen::Vector2i(1600, 900), "NanoGUI Test") {
    windows.resize(1);
    windows[0] = new nanogui::Window(this, "Parameter forms");
    windows[0]->setPosition(Eigen::Vector2i(15, 15));
    windows[0]->setLayout(new nanogui::GroupLayout());
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
}

void Renderer::makeInputForms(nanogui::Window *targetWindow) {
    textBoxes.push_back(
            makeForm(targetWindow, "Smoothness", std::to_string(GlobalState::optimizerParameters.smoothness),
                     "nodes pp"));
    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
        std::cout << "Smoothness went from " << GlobalState::optimizerParameters.smoothness;
        GlobalState::optimizerParameters.smoothness = std::stoi(str);
        if (GlobalState::optimizerParameters.smoothness < 0) GlobalState::optimizerParameters.smoothness = 0;
        std::cout << " to " << GlobalState::optimizerParameters.smoothness << std::endl;
        return true;
    });

    textBoxes.push_back(
            makeForm(targetWindow, "Temperature", std::to_string(GlobalState::optimizerParameters.temperature), ""));
    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
        std::cout << "Temperature went from " << GlobalState::optimizerParameters.temperature;
        GlobalState::optimizerParameters.temperature = std::stod(str);
        if (GlobalState::optimizerParameters.temperature < 0) GlobalState::optimizerParameters.temperature = 0;
        std::cout << " to " << GlobalState::optimizerParameters.temperature << std::endl;
        return true;
    });

//    textBoxes.push_back(
//            makeForm(targetWindow, "DiffMul", std::to_string(GlobalState::optimizerParameters.diffMul), ""));
//    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
//        std::cout << "DiffMul went from " << GlobalState::optimizerParameters.diffMul;
//        GlobalState::optimizerParameters.diffMul = std::stod(str);
//        if (GlobalState::optimizerParameters.diffMul < 0) GlobalState::optimizerParameters.diffMul = 0;
//        std::cout << " to " << GlobalState::optimizerParameters.diffMul << std::endl;
//        return true;
//    });
//
//    textBoxes.push_back(
//            makeForm(targetWindow, "DiffPow", std::to_string(GlobalState::optimizerParameters.diffPow), ""));
//    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
//        std::cout << "DiffPow went from " << GlobalState::optimizerParameters.diffPow;
//        GlobalState::optimizerParameters.diffPow = std::stod(str);
//        if (GlobalState::optimizerParameters.diffPow < 0) GlobalState::optimizerParameters.diffPow = 0;
//        std::cout << " to " << GlobalState::optimizerParameters.diffPow << std::endl;
//        return true;
//    });

//    textBoxes.push_back(
//            makeForm(targetWindow, "AreaMul", std::to_string(GlobalState::optimizerParameters.areaMul), ""));
//    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
//        std::cout << "AreaMul went from " << GlobalState::optimizerParameters.areaMul;
//        GlobalState::optimizerParameters.areaMul = std::stod(str);
//        if (GlobalState::optimizerParameters.areaMul < 0) GlobalState::optimizerParameters.areaMul = 0;
//        std::cout << " to " << GlobalState::optimizerParameters.areaMul << std::endl;
//        return true;
//    });
//
//    textBoxes.push_back(
//            makeForm(targetWindow, "AreaPow", std::to_string(GlobalState::optimizerParameters.areaPow), ""));
//    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
//        std::cout << "AreaPow went from " << GlobalState::optimizerParameters.areaPow;
//        GlobalState::optimizerParameters.areaPow = std::stod(str);
//        if (GlobalState::optimizerParameters.areaPow < 0) GlobalState::optimizerParameters.areaPow = 0;
//        std::cout << " to " << GlobalState::optimizerParameters.areaPow << std::endl;
//        return true;
//    });

    textBoxes.push_back(
            makeForm(targetWindow, "Compression", std::to_string(GlobalState::optimizerParameters.compression), ""));
    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
        std::cout << "Compression went from " << GlobalState::optimizerParameters.compression;
        GlobalState::optimizerParameters.compression = std::stod(str);
        if (GlobalState::optimizerParameters.compression < 0) GlobalState::optimizerParameters.compression = 0;
        std::cout << " to " << GlobalState::optimizerParameters.compression << std::endl;
        return true;
    });

    textBoxes.push_back(makeForm(targetWindow, "ForceOffsetRange",
                                 std::to_string(GlobalState::optimizerParameters.forceOffsetRange), ""));
    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
        std::cout << "ForceOffsetRange went from " << GlobalState::optimizerParameters.forceOffsetRange;
        GlobalState::optimizerParameters.forceOffsetRange = std::stod(str);
        if (GlobalState::optimizerParameters.forceOffsetRange < 0)
            GlobalState::optimizerParameters.forceOffsetRange = 0;
        std::cout << " to " << GlobalState::optimizerParameters.forceOffsetRange << std::endl;
        return true;
    });

    textBoxes.push_back(
            makeForm(targetWindow, "MultiProb", std::to_string(GlobalState::optimizerParameters.multiProb), ""));
    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
        std::cout << "MultiProb went from " << GlobalState::optimizerParameters.multiProb;
        GlobalState::optimizerParameters.multiProb = std::stod(str);
        if (GlobalState::optimizerParameters.multiProb < 0) GlobalState::optimizerParameters.multiProb = 0;
        std::cout << " to " << GlobalState::optimizerParameters.multiProb << std::endl;
        return true;
    });

    textBoxes.push_back(
            makeForm(targetWindow, "TempProb", std::to_string(GlobalState::optimizerParameters.tempProb), ""));
    textBoxes[textBoxes.size() - 1]->setCallback([this](const std::string &str) {
        std::cout << "TempProb went from " << GlobalState::optimizerParameters.tempProb;
        GlobalState::optimizerParameters.tempProb = std::stod(str);
        if (GlobalState::optimizerParameters.tempProb < 0) GlobalState::optimizerParameters.tempProb = 0;
        std::cout << " to " << GlobalState::optimizerParameters.tempProb << std::endl;
        return true;
    });

    nanogui::Button *b0 = new nanogui::Button(targetWindow, "Single step");
    b0->setCallback([this] { GlobalState::singleStep = true; });

    nanogui::Button *b1 = new nanogui::Button(targetWindow, "Start/Pause");
    b1->setCallback([this] { GlobalState::shouldStep = !GlobalState::shouldStep; });

    nanogui::Button *b2 = new nanogui::Button(targetWindow, "Start recording");
    b2->setCallback([this, b2] {
        if (!GlobalState::recording) {
            IO::openOutputFileAndWriteHeader(GlobalState::recordedAttributes);
            b2->setCaption("Stop recording");
        } else {
            IO::closeOutputFile();
            b2->setCaption("Start recording");
        }
        GlobalState::recording = !GlobalState::recording;
    });

    nanogui::CheckBox *cb = new nanogui::CheckBox(targetWindow, "show correspondences");
    cb->setCallback([](bool checked) {
        if (checked)
            GlobalState::showCorrespondences = true;
        else
            GlobalState::showCorrespondences = false;
    });
    nanogui::CheckBox *adjustCb = new nanogui::CheckBox(targetWindow, "adjust node resolution");
    adjustCb->setCallback([](bool checked) {
        if (checked)
            GlobalState::adjustNodeResolution = true;
        else
            GlobalState::adjustNodeResolution = false;
    });

    // Recorded surface properties setting

    for (auto it = SurfaceProperties::fns.begin(); it != SurfaceProperties::fns.end(); it++) {
        nanogui::CheckBox *cb0 = new nanogui::CheckBox(targetWindow, it->first);
        cb0->setCallback([it](bool checked) {
            if (checked)
                GlobalState::recordedAttributes.insert(it->first);
            else
                GlobalState::recordedAttributes.erase(it->first);
        });
    }


    nanogui::Button *b3 = new nanogui::Button(targetWindow, "Reset");
    b3->setCallback([this] {
        GlobalState::shouldStep = false;
        GlobalState::singleStep = false;
        std::unordered_map<std::string, std::string> inputMap;
        IO::sillyMapReader("../input.txt", inputMap);
        IO::InitSaParams theseParams;
        IO::parseInputToParams(inputMap, &theseParams);
        GlobalState::setSurfaceParameters(theseParams.radius, theseParams.thickness, 0.0, 0.0, theseParams.points,
                                          theseParams.bothCorrsDist, 0);
        GlobalState::deliberatelyDeleteBecauseDestructorIsCalledWheneverItWants();
        GlobalState::initThickSurface();
        double initialGrayMatter =
                Graph::surfaceArea(GlobalState::thickSurface.layers[Graph::OUTER]) -
                Graph::surfaceArea(GlobalState::thickSurface.layers[Graph::INNER]);
        GlobalState::setOptimizerParameters(initialGrayMatter, theseParams.smooth, theseParams.diffMul,
                                            theseParams.diffPow,
                                            theseParams.areaMul, theseParams.areaPow, theseParams.multiProb,
                                            theseParams.tempProb, theseParams.forceOffsetRange, theseParams.compression,
                                            MathGeometry::linearSmooth, theseParams.temperature, theseParams.craniumRadius);
    });
    performLayout();
}

int Renderer::countNumberOfLines() {
    int counter = 0;
    auto outerIt = GlobalState::thickSurface.layers[Graph::OUTER].nodes[0];
    do {
        if (GlobalState::showCorrespondences)
            counter += outerIt->correspondents.size();
        outerIt = outerIt->to;
        counter++;
    } while (outerIt != GlobalState::thickSurface.layers[Graph::OUTER].nodes[0]);

    auto innerIt = GlobalState::thickSurface.layers[Graph::INNER].nodes[0];
    do {
        innerIt = innerIt->to;
        counter++;
    } while (innerIt != GlobalState::thickSurface.layers[Graph::INNER].nodes[0]);
    return counter;
}

void Renderer::uploadIndices() {
    int indicesNumber = countNumberOfLines();
    int outerSurfaceIndicesNumber = indicesNumber - GlobalState::thickSurface.layers[Graph::INNER].nodes.size();

    nanogui::MatrixXu indices(2, indicesNumber);
    size_t counter = 0;
    /* Outer surface indices << */
    int eachNodeCounter = 0;
    int eachCorrespondentCounter = 1;
    auto outerIt = GlobalState::thickSurface.layers[Graph::OUTER].nodes[0];
    do {
        if (GlobalState::showCorrespondences) {
            for (auto corrsIt = outerIt->correspondents.begin(); corrsIt != outerIt->correspondents.end(); corrsIt++) {
                indices.col(counter) << eachNodeCounter, eachNodeCounter + eachCorrespondentCounter;
                counter++;
                eachCorrespondentCounter++;
            }
        }
        outerIt = outerIt->to;
        indices.col(counter) << eachNodeCounter, (eachNodeCounter + eachCorrespondentCounter) %
                                                 outerSurfaceIndicesNumber; // Next in same layer
        eachNodeCounter += eachCorrespondentCounter;
        eachCorrespondentCounter = 1;
        counter++;
    } while (outerIt != GlobalState::thickSurface.layers[Graph::OUTER].nodes[0]);

    /* Inner surface indices  */
    auto innerIt = GlobalState::thickSurface.layers[Graph::INNER].nodes[0];
    int innerStartsAt = counter;
    do {
        innerIt = innerIt->to;
        indices.col(counter) << counter, ((counter + 1) - innerStartsAt) %
                                         GlobalState::thickSurface.layers[Graph::INNER].nodes.size() ? (counter + 1)
                                                                                                     : innerStartsAt; // Next in same layer
        counter++;
    } while (innerIt != GlobalState::thickSurface.layers[Graph::INNER].nodes[0]);

    /* Shader binding */
    mShader.bind();
    mShader.uploadIndices(indices);
}

void Renderer::uploadSurface() {
    int indicesNumber = countNumberOfLines();
    nanogui::MatrixXf positions(3, indicesNumber);
    size_t counter = 0;
    /* Outer surface indices << */
    auto outerIt = GlobalState::thickSurface.layers[Graph::OUTER].nodes[0];
    do {
        positions.col(counter) << outerIt->coords[Graph::X], outerIt->coords[Graph::Y], 0;

        if (GlobalState::showCorrespondences){
            for (auto corrsIt = outerIt->correspondents.begin(); corrsIt != outerIt->correspondents.end(); corrsIt++) {
                counter++;
                positions.col(counter) << (*corrsIt)->coords[Graph::X], (*corrsIt)->coords[Graph::Y], 0;
            }
        }
        outerIt = outerIt->to;
        counter++;
    } while (outerIt != GlobalState::thickSurface.layers[Graph::OUTER].nodes[0]);

    /* Inner surface indices  */
    auto innerIt = GlobalState::thickSurface.layers[Graph::INNER].nodes[0];
    do {
        positions.col(counter) << innerIt->coords[Graph::X], innerIt->coords[Graph::Y], 0;
        innerIt = innerIt->to;
        counter++;
    } while (innerIt != GlobalState::thickSurface.layers[Graph::INNER].nodes[0]);
    /* Shader binding */
    mShader.bind();
    mShader.uploadAttrib("position", positions);
    mShader.setUniform("intensity", 0.8f);
}

void Renderer::drawContents() {
    /* Draw the window contents using OpenGL */
    mShader.bind();

    nanogui::Matrix4f mvp;
    mvp.setIdentity();

    mvp.row(0) *= (float) mSize.y() / (float) mSize.x();

    mShader.setUniform("modelViewProj", mvp);

    /* Draw lines starting at index 0 */
    mShader.drawIndexed(GL_LINES, 0, countNumberOfLines());
}

Renderer::~Renderer() {
    mShader.free();
}