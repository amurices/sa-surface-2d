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
    
    return textBox;
}

Renderer::Renderer() : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test")
{
    windows.resize(1);
    windows[0] = new nanogui::Window(this, "Parameter forms");
    windows[0]->setPosition(Eigen::Vector2i(15, 15));
    windows[0]->setLayout(new nanogui::GroupLayout());
    nanogui::TextBox *textBox;
    textBox = makeForm(windows[0], "Thickness", "dunno", "* outer radius");
    textBox->setCallback([this](const std::string &str) {
        std::cout << "Thickness went from " << this->thickSurface->thicknesses[0];
        std::vector<double> newThicknesses(this->thickSurface->outer->nNodes, std::stof(str));
        this->thickSurface->thicknesses = newThicknesses;
        this->thickSurface->updateEntireInnerSurface();
        std::cout << " to " << this->thickSurface->thicknesses[0] << std::endl;
        //TODO: Gen thicknesses
        return true;
    });

    nanogui::TextBox *textBox1;
    textBox1 = makeForm(windows[0], "Smoothness", "dunno", "# of nodes pushed");
    textBox1->setCallback([this](const std::string &str) {
        std::cout << "Smoothness went from " << this->optimizer->params->smooth;
        this->optimizer->params->smooth = std::stoi(str); if (this->optimizer->params->smooth < 0) this->optimizer->params->smooth = 0;
        std::cout << " to " << this->optimizer->params->smooth << std::endl;
        return true;
    });

    nanogui::Button *b = new nanogui::Button(windows[0], "Start/Pause");
    b->setCallback([this] { this->shouldStep = !this->shouldStep; });
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

Renderer::~Renderer()
{
    mShader.free();
}