#include <fstream>
#include "Renderer.hpp"

nanogui::TextBox* Renderer::makeForm(nanogui::Widget *parent,
              const std::string &name,
              const std::string &initialValue,
              const std::string &unit)
{
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

Renderer::Renderer() : nanogui::Screen(Eigen::Vector2i(1600, 900), "NanoGUI Test")
{
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

void Renderer::makeInputForms(nanogui::Window *targetWindow)
{
    textBoxes.resize(32);
    textBoxes[0] = makeForm(targetWindow, "Thickness", std::to_string(this->thickSurface->thicknesses[0]), "* outer radius");
    textBoxes[0]->setCallback([this](const std::string &str) {
        std::cout << "Thickness went from " << this->thickSurface->thicknesses[0];
        std::vector<double> newThicknesses(this->thickSurface->outer->nNodes, std::stod(str));
        this->thickSurface->thicknesses = newThicknesses;
        this->thickSurface->updateEntireInnerSurface();
        std::cout << " to " << this->thickSurface->thicknesses[0] << std::endl;
        //TODO: Gen thicknesses
        return true;
    });

    textBoxes[1] = makeForm(targetWindow, "Smoothness", std::to_string(this->optimizer->params->smooth), "nodes pp");
    textBoxes[1]->setCallback([this](const std::string &str) {
        std::cout << "Smoothness went from " << this->optimizer->params->smooth;
        this->optimizer->params->smooth = std::stoi(str); if (this->optimizer->params->smooth < 0) this->optimizer->params->smooth = 0;
        std::cout << " to " << this->optimizer->params->smooth << std::endl;
        return true;
    });

    textBoxes[1] = makeForm(targetWindow, "DiffMul", std::to_string(this->optimizer->params->diffMul), "");
    textBoxes[1]->setCallback([this](const std::string &str) {
        std::cout << "DiffMul went from " << this->optimizer->params->diffMul;
        this->optimizer->params->diffMul = std::stod(str); if (this->optimizer->params->diffMul < 0) this->optimizer->params->diffMul = 0;
        std::cout << " to " << this->optimizer->params->diffMul << std::endl;
        return true;
    });

    textBoxes[2] = makeForm(targetWindow, "DiffPow", std::to_string(this->optimizer->params->diffPow), "");
    textBoxes[2]->setCallback([this](const std::string &str) {
        std::cout << "DiffPow went from " << this->optimizer->params->diffPow;
        this->optimizer->params->diffPow = std::stod(str); if (this->optimizer->params->diffPow < 0) this->optimizer->params->diffPow = 0;
        std::cout << " to " << this->optimizer->params->diffPow << std::endl;
        return true;
    });

    textBoxes[3] = makeForm(targetWindow, "AreaMul", std::to_string(this->optimizer->params->areaMul), "");
    textBoxes[3]->setCallback([this](const std::string &str) {
        std::cout << "AreaMul went from " << this->optimizer->params->areaMul;
        this->optimizer->params->areaMul = std::stod(str); if (this->optimizer->params->areaMul < 0) this->optimizer->params->areaMul = 0;
        std::cout << " to " << this->optimizer->params->areaMul << std::endl;
        return true;
    });

    textBoxes[4] = makeForm(targetWindow, "AreaPow", std::to_string(this->optimizer->params->areaPow), "");
    textBoxes[4]->setCallback([this](const std::string &str) {
        std::cout << "AreaPow went from " << this->optimizer->params->areaPow;
        this->optimizer->params->areaPow = std::stod(str); if (this->optimizer->params->areaPow < 0) this->optimizer->params->areaPow = 0;
        std::cout << " to " << this->optimizer->params->areaPow << std::endl;
        return true;
    });

    textBoxes[4] = makeForm(targetWindow, "Compression", std::to_string(this->optimizer->params->compression), "");
    textBoxes[4]->setCallback([this](const std::string &str) {
        std::cout << "Compression went from " << this->optimizer->params->compression;
        this->optimizer->params->compression = std::stod(str); if (this->optimizer->params->compression < 0) this->optimizer->params->compression = 0;
        std::cout << " to " << this->optimizer->params->compression << std::endl;
        return true;
    });

    textBoxes[5] = makeForm(targetWindow, "ForceOffsetRange", std::to_string(this->optimizer->params->forceOffsetRange), "");
    textBoxes[5]->setCallback([this](const std::string &str) {
        std::cout << "ForceOffsetRange went from " << this->optimizer->params->forceOffsetRange;
        this->optimizer->params->forceOffsetRange = std::stod(str); if (this->optimizer->params->forceOffsetRange < 0) this->optimizer->params->forceOffsetRange = 0;
        std::cout << " to " << this->optimizer->params->forceOffsetRange << std::endl;
        return true;
    });

    textBoxes[6] = makeForm(targetWindow, "MultiProb", std::to_string(this->optimizer->params->multiProb), "");
    textBoxes[6]->setCallback([this](const std::string &str) {
        std::cout << "MultiProb went from " << this->optimizer->params->multiProb;
        this->optimizer->params->multiProb = std::stod(str); if (this->optimizer->params->multiProb < 0) this->optimizer->params->multiProb = 0;
        std::cout << " to " << this->optimizer->params->multiProb << std::endl;
        return true;
    });

    textBoxes[7] = makeForm(targetWindow, "TempProb", std::to_string(this->optimizer->params->tempProb), "");
    textBoxes[7]->setCallback([this](const std::string &str) {
        std::cout << "TempProb went from " << this->optimizer->params->tempProb;
        this->optimizer->params->tempProb = std::stod(str); if (this->optimizer->params->tempProb < 0) this->optimizer->params->tempProb = 0;
        std::cout << " to " << this->optimizer->params->tempProb << std::endl;
        return true;
    });

    nanogui::Button *b0 = new nanogui::Button(targetWindow, "Single step");
    b0->setCallback([this] { this->optimizer->singleStep = true; });

    nanogui::Button *b1 = new nanogui::Button(targetWindow, "Start/Pause");
    b1->setCallback([this] { this->optimizer->shouldStep = !this->optimizer->shouldStep; });

    nanogui::Button *b2 = new nanogui::Button(targetWindow, "Start recording");
    b2->setCallback([this, b2] {
        if (!this->optimizer->recording){
            this->optimizer->outputFile.open(OUTPUT_FILE_NAME);
            if (this->optimizer->outputFile.fail()){
                printf("Error opening output file %s.\n", OUTPUT_FILE_NAME);
                exit(0);
            }
            b2->setCaption("Stop recording");
        } else {
            this->optimizer->outputFile.close();
            b2->setCaption("Start recording");
        }
        this->optimizer->recording = !this->optimizer->recording;

    });

    nanogui::Button *b3 = new nanogui::Button(targetWindow, "Reset");
    b3->setCallback([this] {
        this->optimizer->reset();
        std::unordered_map <std::string, std::string> inputMap;
        IO::sillyMapReader("../input.txt", inputMap);
        InitSaParams theseParams;
        IO::parseInputToParams(inputMap, &theseParams);
        this->thickSurface->generateCircularThickSurface(theseParams.radius, theseParams.points, true, theseParams.thickness, theseParams.thickness, point_t(0.0, 0.0));
    });
    performLayout();
}

static bool indexUploaded = false;

void Renderer::uploadIndices(){
    size_t numOuterVertices = thickSurface->outer->nNodes;
    size_t numInnerVertices = thickSurface->inner->nNodes;
    nanogui::MatrixXu indices(2, numOuterVertices + 1 + numInnerVertices + 1);
    size_t i = 0;
    /* Outer surface indices */
    for (; i < numOuterVertices - 1; i++){
        indices.col(i) << i, (i + 1);
    }
    indices.col(i) << i, 0;

    /* Inner surface indices */
    for (i++ ; i - numOuterVertices < numInnerVertices - 1; i++){
        indices.col(i) << i, (i + 1);

    }
    indices.col(i) << i, numOuterVertices;

    /* Shader binding */
    mShader.bind();
    mShader.uploadIndices(indices);
}

void Renderer::uploadSurface(){
    size_t numOuterVertices = thickSurface->outer->nNodes;
    size_t numInnerVertices = thickSurface->inner->nNodes;
    nanogui::MatrixXf positions(3, numOuterVertices + numInnerVertices);
    size_t i = 0;
    /* Outer surface index positions */
    for (auto it = thickSurface->outer->graph->nodeFromId(0); true;){
        positions.col(i) << (*thickSurface->outer->coords)[it].x, (*thickSurface->outer->coords)[it].y, 0;
        ListDigraph::OutArcIt next(*thickSurface->outer->graph, it);
        it = thickSurface->outer->graph->target(next);
        i++;
        if (it == thickSurface->outer->graph->nodeFromId(0))
            break;
    }
    /* Inner surface index positions */
    for (auto it = thickSurface->inner->graph->nodeFromId(0); true;){
        positions.col(i) << (*thickSurface->inner->coords)[it].x, (*thickSurface->inner->coords)[it].y, 0;
        ListDigraph::OutArcIt next(*thickSurface->inner->graph, it);
        it = thickSurface->inner->graph->target(next);
        i++;
        if (it == thickSurface->inner->graph->nodeFromId(0))
            break;
    }

    /* Shader binding */
    mShader.bind();
    mShader.uploadAttrib("position", positions);
    mShader.setUniform("intensity", 0.8f);
}

void Renderer::uploadIndices2(){
    size_t numOuterVertices = thickSurface2->layers[Graph::OUTER].nodes.size();
    size_t numInnerVertices = thickSurface2->layers[Graph::INNER].nodes.size();
    nanogui::MatrixXu indices(2, numOuterVertices + 1 + numInnerVertices + 1);
    size_t i = 0;
    /* Outer surface indices */
    for (; i < numOuterVertices - 1; i++){
        indices.col(i) << i, (i + 1);
    }
    indices.col(i) << i, 0;

    /* Inner surface indices */
    for (i++ ; i - numOuterVertices < numInnerVertices - 1; i++){
        indices.col(i) << i, (i + 1);

    }
    indices.col(i) << i, numOuterVertices;

    /* Shader binding */
    mShader.bind();
    mShader.uploadIndices(indices);
}

void Renderer::uploadSurface2(){
    size_t numOuterVertices = thickSurface2->layers[Graph::OUTER].nodes.size();
    size_t numInnerVertices = thickSurface2->layers[Graph::INNER].nodes.size();
    nanogui::MatrixXf positions(3, numOuterVertices + numInnerVertices);
    size_t i = 0;
    /* Outer surface index positions */
    for (auto it = thickSurface2->layers[Graph::OUTER].nodes[0]; true;){
        positions.col(i) << it->coords[Graph::X], it->coords[Graph::Y], 0;
        it = it->to;
        i++;
        if (it == thickSurface2->layers[Graph::OUTER].nodes[0])
            break;
    }
    /* Inner surface index positions */
    for (auto it = thickSurface2->layers[Graph::INNER].nodes[0]; true;){
        positions.col(i) << it->coords[Graph::X], it->coords[Graph::Y], 0;
        it = it->to;
        i++;
        if (it == thickSurface2->layers[Graph::INNER].nodes[0])
            break;
    }

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
    mShader.drawIndexed(GL_LINES, 0, thickSurface->outer->nNodes * 2);
}

void Renderer::drawContents2() {
    /* Draw the window contents using OpenGL */
    mShader.bind();

    nanogui::Matrix4f mvp;
    mvp.setIdentity();

    mvp.row(0) *= (float) mSize.y() / (float) mSize.x();

    mShader.setUniform("modelViewProj", mvp);

    /* Draw lines starting at index 0 */
    mShader.drawIndexed(GL_LINES, 0, thickSurface2->layers[Graph::OUTER].nodes.size() + thickSurface2->layers[Graph::INNER].nodes.size());
}

Renderer::~Renderer()
{
    mShader.free();
}