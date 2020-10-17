
#define SDL_MAIN_HANDLED

#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <sdl.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

#include "hsm/schema.h"
#include "hsm/timer.h"
#include "hsm/random.h"
#include "hsm/resource.h"
#include "hsm/state.h"
#include "hsm/state_machine.h"
#include "hsm/state_machine_factory.h"
#include "hsm/state_machine_xml_loader.h"
#include "hsm/timer.h"

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "sdl2.lib")
#pragma comment (lib, "sdl2main.lib")

#define SCREEN_WIDTH            800
#define SCREEN_HEIGHT           800
#define OPENGL_MAJOR_VERSION    2
#define OPENGL_MINOR_VERSION    1
#define OPENGL_PROFILE          SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE

SDL_Window * displayWindow;
SDL_GLContext context;


utils::Timer updateTimer;
hsm::StateMachine* state_machine = nullptr;
hsm::StateMachineFactory* state_machine_factory = nullptr;

namespace ImGui
{
    ImVec2 operator + (const ImVec2& a, const ImVec2& b)
    {
        return ImVec2(a.x + b.x, a.y + b.y);
    }

    ImVec2 operator - (const ImVec2& a, const ImVec2& b)
    {
        return ImVec2(a.x - b.x, a.y - b.y);
    }

    ImVec2 operator * (const ImVec2& a, float k)
    {
        return ImVec2(a.x * k, a.y * k);
    }

    void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(-1.0f, -1.0f))
    {
        ImGui::BeginGroup();

        auto cursorPos = ImGui::GetCursorScreenPos();
        auto itemSpacing = ImGui::GetStyle().ItemSpacing;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();
        ImGui::BeginGroup();

        ImVec2 effectiveSize = size;
        if (size.x < 0.0f)
            effectiveSize.x = ImGui::GetContentRegionAvailWidth();
        else
            effectiveSize.x = size.x;
        ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::TextUnformatted(name);
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
        ImGui::BeginGroup();

        ImGui::PopStyleVar(2);

        //ImDrawList* draw_list = ImGui::GetWindowDrawList();
        //ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
        //ImGui::GetCurrentWindow()->Size.x -= frameHeight;

        ImGui::PushItemWidth(effectiveSize.x - frameHeight);
    }

    void EndGroupPanel()
    {
        ImGui::PopItemWidth();

        auto itemSpacing = ImGui::GetStyle().ItemSpacing;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        auto frameHeight = ImGui::GetFrameHeight();

        ImGui::EndGroup();

        //ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

        ImGui::EndGroup();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
        ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

        ImGui::EndGroup();

        auto itemMin = ImGui::GetItemRectMin();
        auto itemMax = ImGui::GetItemRectMax();
        //ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

        ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
        ImGui::GetWindowDrawList()->AddRect(
            itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f),
            ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
            halfFrame.x);

        ImGui::PopStyleVar(2);

        //ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
        //ImGui::GetCurrentWindow()->Size.x += frameHeight;

        ImGui::Dummy(ImVec2(0.0f, 0.0f));

        ImGui::EndGroup();
    }

    void ToggleButton(const char* str_id, bool* v)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        float height = ImGui::GetFrameHeight();
        float width = height * 1.55f;
        float radius = height * 0.50f;

        if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
            *v = !*v;
        ImU32 col_bg;
        if (ImGui::IsItemHovered())
            col_bg = *v ? IM_COL32(68 + 20, 211, 145 + 20, 255) : IM_COL32(218 - 20, 218 - 20, 218 - 20, 255);
        else
            col_bg = *v ? IM_COL32(68, 211, 145, 255) : IM_COL32(200, 200, 200, 255);

        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
        draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
    }
}

class ImGuiHSMState : public hsm::State
{
public:
    ImGuiHSMState(hsm::StateMachine* parent, const hsm::schema::State& schema)
        : hsm::State(parent, schema)
        , m_running(false)
    {}

    void enter() override;
    void update() override;
    void exit() override;
    void render() const;

    void renderTransitions() const;
    void renderExceptions() const;
    bool m_running;
};

void ImGuiHSMState::enter()
{
    m_running = true;
}

void ImGuiHSMState::update()
{

}

void ImGuiHSMState::exit()
{
    m_running = false;
}

void ImGuiHSMState::render() const
{
    if (ImGui::TreeNodeEx(getSchema().m_shortname.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (const auto& it : getSchema().m_transitions)
        {

            ImGui::SameLine();
            if (ImGui::Button(it.m_event.c_str()))
            {
                updateTimer.restart();
                state_machine->setTransition(*this, it.m_event, it.m_attributes);
            }
        }
        //renderTransitions();

        ImGui::TreePop();
    }
}


void ImGuiHSMState::renderTransitions() const
{
    if (m_running)
    {
        if (ImGui::TreeNode("transitions"))
        {
            for (const auto& it : getSchema().m_transitions)
            {

                if (ImGui::Button(it.m_event.c_str()))
                {
                    updateTimer.restart();
                    state_machine->setTransition(*this, it.m_event, it.m_attributes);
                }

                if (!it.m_attributes.empty())
                {
                    char buffer[512];
                    int bufferlen = 0;
                    bufferlen += sprintf_s(buffer + bufferlen, sizeof(buffer) / sizeof(*buffer) - bufferlen, " [");

                    for (std::unordered_map<std::string, std::string>::const_iterator jt = it.m_attributes.begin(); jt != it.m_attributes.end(); ++jt)
                    {
                        if (jt != it.m_attributes.begin())
                            bufferlen += sprintf_s(buffer + bufferlen, sizeof(buffer) / sizeof(*buffer) - bufferlen, ", ");

                        bufferlen += sprintf_s(buffer + bufferlen, sizeof(buffer) / sizeof(*buffer) - bufferlen, "%s=%s", jt->first.c_str(), jt->second.c_str());
                    }
                    
                    bufferlen += sprintf_s(buffer + bufferlen, sizeof(buffer) / sizeof(*buffer) - bufferlen, "]");

                    ImGui::SameLine();
                    ImGui::Text(buffer);
                }
            }
            ImGui::TreePop();
        }
    }
}

class ImGuiHSMResource : public hsm::Resource
{
public:
    ImGuiHSMResource(const hsm::schema::Resource& schema )
        : hsm::Resource(schema)
    {}

    void load() override {}
    void unload() override {}
    void update() override {}
};


class ImGuiHSMFactory : public hsm::StateMachineFactory
{
public:
    ImGuiHSMFactory()
    {}

    hsm::State* createState(hsm::StateMachine* state_machine, const hsm::schema::State& state_schema) override
    {
        return new ImGuiHSMState(state_machine, state_schema);
    }

    hsm::Resource* createResource(const hsm::schema::Resource& resource_schema) override
    {
        return new ImGuiHSMResource(resource_schema);
    }
};

class ImGuiHSMachine : public hsm::StateMachine
{
public:
    hsm::schema::Transition m_transition;

    ImGuiHSMachine(const hsm::schema::StateMachine& schema, hsm::StateMachineFactory* factory)
        : hsm::StateMachine(schema, factory)
    {
    }

    void renderResources() const
    {
        ImGui::BeginGroupPanel("Resources");

        std::list<const hsm::Resource*> resources = getCurrentResources();

        int count = 0;
        for (auto resource : resources)
        {
            if ((++count %6) != 1)
            {
                ImGui::SameLine();
            }

            ImGui::Button(resource->getSchema().m_name.c_str());
        }
        ImGui::EndGroupPanel();
    }

    void renderExceptions(const std::list<const hsm::State*>& states) const
    {
        ImGui::BeginGroupPanel("Exceptions");

        std::list<const hsm::schema::Transition*> thrownExceptions = getCurrentExceptions();

        std::list<const hsm::schema::Transition*> exceptions;

        for (auto state : states)
        {
            for (const auto& exception : state->getSchema().m_exceptions)
            {
                if (exception.m_event != "*")
                {
                    exceptions.push_back(&exception);
                }
            }
        }

        for (auto exception : exceptions)
        {
            auto it = std::find_if(thrownExceptions.begin(), thrownExceptions.end(), [&exception](const hsm::schema::Transition* ex)
            {
                return (ex->m_event == exception->m_event && ex->m_attributes == exception->m_attributes);
            });

            bool was_thrown = it != thrownExceptions.end();
            bool is_thrown = was_thrown;
            ImGui::ToggleButton(exception->m_event.c_str(), &is_thrown);
            if(!was_thrown && is_thrown)
            {
                state_machine->throwException(exception->m_event, exception->m_attributes);
            }

            if (!exception->m_attributes.empty())
            {
                char buffer[512];
                int bufferlen = 0;
                bufferlen += sprintf_s(buffer + bufferlen, sizeof(buffer) / sizeof(*buffer) - bufferlen, " [");

                for (std::unordered_map<std::string, std::string>::const_iterator jt = exception->m_attributes.begin(); jt != exception->m_attributes.end(); ++jt)
                {
                    if (jt != exception->m_attributes.begin())
                        bufferlen += sprintf_s(buffer + bufferlen, sizeof(buffer) / sizeof(*buffer) - bufferlen, ", ");

                    bufferlen += sprintf_s(buffer + bufferlen, sizeof(buffer) / sizeof(*buffer) - bufferlen, "%s=%s", jt->first.c_str(), jt->second.c_str());
                }
                bufferlen += sprintf_s(buffer + bufferlen, sizeof(buffer) / sizeof(*buffer) - bufferlen, "]");

                ImGui::SameLine();
                ImGui::Text(buffer);
            }
        }
        ImGui::EndGroupPanel();
    }

    void render()
    {
        const hsm::schema::Transition* transition = getCurrentTransition();

        if (transition)
        {
            m_transition = *transition;
        }

        bool transitionActive = (transition != nullptr) && (transition->m_state == m_transition.m_state);

        ImGui::Text("Transition :");
        ImGui::SameLine();
        ImGui::TextColored(transitionActive? ImGui::GetStyle().Colors[ImGuiCol_Text] : ImGui::GetStyle().Colors[ImGuiCol_TextDisabled], m_transition.m_state.c_str());

        std::list<const hsm::State*> states = getCurrentStates();

        for (auto state : states)
        {
            ImGui::Indent(16.0f);

            ImGui::BeginGroupPanel(state->getSchema().m_shortname.c_str());

            for (auto it = state->getSchema().m_transitions.begin(); it != state->getSchema().m_transitions.end(); ++it)
            {
                if(it != state->getSchema().m_transitions.begin())
                    ImGui::SameLine();

                if (ImGui::Button((*it).m_event.c_str()))
                {
                    updateTimer.restart();
                    setTransition(*state, (*it).m_event, (*it).m_attributes);
                }
                
            }
        }

        for (auto state : states)
        {
            ImGui::Unindent(16.0f);

            ImGui::EndGroupPanel();
        }

        renderExceptions(states);

        renderResources();
    }
};

void InitSDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        return;
    
    // Set OpenGL profile and version MAJOR.MINOR.
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, OPENGL_PROFILE);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR_VERSION);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR_VERSION);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    // Create SDL display window.
    displayWindow = SDL_CreateWindow("state machine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (displayWindow == nullptr)
        return;

    context = SDL_GL_CreateContext(displayWindow);
    if (context == nullptr)
        return;

    // Must be after the context is assigned.
    const unsigned char *version = glGetString(GL_VERSION);
    if (version == nullptr)
        return;

    SDL_GL_MakeCurrent(displayWindow, context);
 }

void InitOpenGL()
{

    glShadeModel    (GL_SMOOTH);
    glClearColor    (0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth    (1.0f);
    glEnable        (GL_DEPTH_TEST);
    glDepthFunc     (GL_LEQUAL);
    glHint          (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glBlendFunc	    (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable       (GL_DEPTH_TEST);
    glDisable       (GL_LIGHTING);
    glDisable       (GL_CULL_FACE);
    glEnable        (GL_BLEND);
    glEnable        (GL_TEXTURE_2D);


    glViewport(0, 0, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1.0, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    // window is the SDL_Window*
    // contex is the SDL_GLContext
    ImGui_ImplSDL2_InitForOpenGL(displayWindow, context);
    ImGui_ImplOpenGL2_Init();
}

void Shutdown()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_Quit();
}

void Start()
{
    std::string filename = "xmls\\state_machine_ovr_hierarchical.xml";

    hsm::StateMachineXmlLoader state_machine_loader;

    if (!state_machine_loader.load(filename))
    {
        debug::logFatal("schema '%s' failed to load.", filename.c_str());

        exit(-1);
    }

    // copy the loaded schema.
    hsm::schema::StateMachine state_machine_schema = state_machine_loader.getSchema();

    state_machine_factory = new ImGuiHSMFactory();

    // the state machine.
    state_machine = new ImGuiHSMachine(state_machine_schema, state_machine_factory);
}

void Update()
{
    if (!updateTimer.isStarted())
    {
        updateTimer.start();
    }
    else if (updateTimer.getElapsedTimeSecs() > 1.0f)
    {
        state_machine->update();

        updateTimer.restart();
    }
}

void Render()
{
    if (ImGui::Begin("StateMachine", nullptr, ImGuiWindowFlags_NoDecoration))
    {
        ImGui::SetWindowSize(ImVec2(SCREEN_WIDTH-4, SCREEN_HEIGHT-4));
        ImGui::SetWindowPos(ImVec2(2, 2));

        if (state_machine->isStopped())
        {
            if (ImGui::Button("Start"))
            {
                updateTimer.restart();
                state_machine->start();
            }
        }

        if (state_machine->isStarted())
        {
            if (ImGui::Button("Stop"))
            {
                updateTimer.restart();
                state_machine->stop();
            }
        }

        ImGui::Text("State :");
        ImGui::SameLine();
        ImGui::Text(state_machine->getStatusString().c_str());

        ((ImGuiHSMachine*)state_machine)->render();

        ImGui::End();
    }
}

int main (int ArgCount, char **Args)
{
    InitSDL();
    InitOpenGL();
    InitImGui();

    Start();

    bool Running = true;
    while (Running)
    {
        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            ImGui_ImplSDL2_ProcessEvent(&Event);

            if (Event.type == SDL_QUIT)
                Running = false;

            if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE)
                Running = false;

            if(Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_CLOSE && Event.window.windowID == SDL_GetWindowID(displayWindow))
                Running = false;
        }

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(displayWindow);
        ImGui::NewFrame();

        glViewport(0, 0, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1.0, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Update();
        Render();

        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(displayWindow);
    }
    
    Shutdown();

    return 0;
}
