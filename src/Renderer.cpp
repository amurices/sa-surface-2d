#include "Renderer.hpp"

Renderer::Renderer() : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test")
{
    windows.resize(64);
    windows[0] = new nanogui::Window(this, "Button demo");
    windows[0]->setPosition(Eigen::Vector2i(15, 15));
    windows[0]->setLayout(new nanogui::GroupLayout());
    nanogui::TextBox *textBox;

    /* FP widget */ {
        new nanogui::Label(windows[0], "Thickness: ", "sans-bold");
        textBox = new nanogui::TextBox(windows[0]);
        textBox->setEditable(true);
        textBox->setFixedSize(nanogui::Vector2i(100, 20));
        textBox->setValue("1.0");
        textBox->setUnits("idk");
        textBox->setDefaultValue("0.0");
        textBox->setFontSize(16);
        textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");
        textBox->setCallback([this](const std::string &str) {
            std::cout << "Thickness went from " << this->thickSurface->thicknesses[0];
            std::vector<double> newThicknesses(this->thickSurface->outer->nNodes, std::stof(str));
            this->thickSurface->thicknesses = newThicknesses;
            std::cout << " to " << this->thickSurface->thicknesses[0] << std::endl;
            //TODO: Gen thicknesses
            return true;
        });
    }
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
    for (; i < numOuterVertices; i++){
        indices.col(i) << i, (i + 1);
    }
    indices.col(i) << i, 0;

    /* Inner surface indices */
    for (i++ ; i - numOuterVertices <= numInnerVertices; i++){
        indices.col(i) << i, (i + 1);
    }
    indices.col(i) << i, numOuterVertices + 1;

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
    for (; i < numOuterVertices; i++){
        positions.col(i) << (*thickSurface->outer->coords)[thickSurface->outer->graph->nodeFromId(i)].x, (*thickSurface->outer->coords)[thickSurface->outer->graph->nodeFromId(i)].y, 0;        
    }

    /* Inner surface index positions */
    for (; i - numOuterVertices < numInnerVertices; i++){
        positions.col(i) << (*thickSurface->inner->coords)[thickSurface->inner->graph->nodeFromId(i - numOuterVertices)].x, (*thickSurface->inner->coords)[thickSurface->inner->graph->nodeFromId(i - numOuterVertices)].y, 0;        
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