#include "EditorContext.h"
#include "imgui/imgui_internal.h"
#include "ImGui/ImGuiPresenter.h"
#include "core/Tools.h"
#include "ui/CocosGUI.h"
#include "sequentity/Sequentity.h"
#include "core/Tools.h"

USING_NS_AX;
USING_NS_AX_EXT;

// 两侧面板范围
static float bothSidesPanelMinWidth = 50.0f;
static float bothSidesPanelMaxWidth = 350.0f;

static float topPanelMinHeight = 50.0f;
static float topPanelMaxHeight = 200.0f;

static float bottomPanelMinHeight = 50.0f;
static float bottomPanelMaxHeight = 400.0f;

static const char* backgroundImageResource = "bg.jpg";



inline ImVec2 operator+(const ImVec2& vec, const ImVec2 value)
{
    return ImVec2{vec.x + value.x, vec.y + value.y};
}

//////////////////////////////////////////////////////////////////////////


static const Sequentity::EventType event_1 = 100;
struct EventData
{
	int number;
};

void EditorContext::testSequentity()
{
	static Sequentity::Context context;
	Sequentity::InitContext(context);

	auto& track = Sequentity::PushTrack(context);

	auto& channel = Sequentity::PushChannel(track, event_1);
	channel.label = "Channel_1";
	channel.color = ImColor::HSV(0.0f, 0.75f, 0.75f);

	static EventData data;
	data.number = 1000;

	auto& event = Sequentity::PushEvent(channel);
	event.time = 0;
	event.length = 5;
	event.type = event_1;
	event.data = static_cast<void*>(&data);


	auto& event2 = Sequentity::PushEvent(channel);
	event2.time = 10;
	event2.length = 5;
	event2.color = ImColor::HSV(0.33f, 0.75f, 0.75f);
	event2.type = event_1;
	event2.data = static_cast<void*>(&data);
	event2.canmove = false;
	event2.click = [](bool isLeft, Sequentity::Event*)
	{
		CCLOG("click event---->> %s", isLeft ? "true" : "false");
	};

	{
		auto& channel = Sequentity::PushChannel(track, event_1 + 1);
		channel.label = "Channel_2";
		channel.color = ImColor::HSV(0.0f, 0.75f, 0.75f);

		auto& event = Sequentity::PushEvent(channel);
		event.time = 3;
		event.length = 3;
		event.type = event_1 + 1;
		event.color = ImColor::HSV(0.0f, 0.75f, 0.75f);
	}
	{
		auto& channel = Sequentity::PushChannel(track, event_1 + 2);
		channel.label = "Channel_1";
		channel.color = ImColor::HSV(0.0f, 0.75f, 0.75f);

		auto& event = Sequentity::PushEvent(channel);
		event.time = 5;
		event.length = 4;
		event.type = event_1 + 2;
		event.color = ImColor::HSV(0.0f, 0.75f, 0.75f);
	}

    ImGuiPresenter::getInstance()->addRenderLoop("drawSequentity", [&]()
    {
        Sequentity::ThemeEditor(NULL);

        if (ImGui::Begin("test"))
        {
            Sequentity::DrawContext(context);

            Sequentity::Intersect(track, context.state.current_time, [&](auto& event) {
                if (event.type == event_1)
                {
                    auto& data = *static_cast<EventData*>(event.data);
                    // CCLOG("in event %d", data.number);
                }
            });
        }
        ImGui::End();
    }, NULL);
}

//////////////////////////////////////////////////////////////////////////

EditorContext::EditorContext()
	: m_backgroundNode(NULL)
	, m_initGUITag(false)
    , m_openDockspace(true)
    , m_redock(false)
{}

EditorContext::~EditorContext()
{
    Director::getInstance()->getTextureCache()->unbindImageAsync(backgroundImageResource);
}


bool EditorContext::init()
{
	if (!Node::init())
	{
		return false;
	}

	Director::getInstance()->getTextureCache()->addImageAsync(backgroundImageResource, [=](Texture2D* texture)
	{
		if (texture == NULL)
		{
			return;
		}
		auto backgroundImage = ui::ImageView::create();
		backgroundImage->loadTexture(backgroundImageResource, ui::Widget::TextureResType::LOCAL);
		//backgroundImage->ignoreContentAdaptWithSize(false);
		//backgroundImage->setOpacity(200);
		backgroundImage->setPosition(Director::getInstance()->getWinSize() * 0.5f);
		backgroundImage->setContentSize(Director::getInstance()->getWinSize());
		m_backgroundNode->addChild(backgroundImage);
	});

	m_backgroundNode = Node::create();
	this->addChild(m_backgroundNode, -2);

    ImGuiPresenter::getInstance()->addRenderLoop("EditorContextLoop", std::bind(&EditorContext::onGUI, this), nullptr);

	//testSequentity();

	return true;
}

void EditorContext::onGUI()
{
    if (!m_initGUITag)
    {
        m_initGUITag = true;
        callLuaGUI("onGUI_Init");
    }

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", &m_openDockspace, window_flags);
    ImGui::PopStyleVar(3);

    // bar
    if (ImGui::BeginMenuBar())
    {
        callLuaGUI("onGUI_MenuBar");
        ImGui::EndMenuBar();
    }

    if (ImGui::DockBuilderGetNode(ImGui::GetID("MyDockspace")) == nullptr || m_redock)
    {
        m_redock             = false;
        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGui::DockBuilderRemoveNode(dockspace_id);                             // Clear out existing layout
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);  // Add empty node
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);
        
        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_id_left_top =
            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);
        ImGuiID dock_id_left_bottom =
            ImGui::DockBuilderSplitNode(dock_id_left_top, ImGuiDir_Down, 0.3f, nullptr, &dock_id_left_top);

        // 主窗口占百分之60
        ImGuiID docker_id_center_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.60f, nullptr, &dock_main_id);
        ImGuiID docker_id_center =
            ImGui::DockBuilderSplitNode(docker_id_center_bottom, ImGuiDir_Up, 0.70f, nullptr,
                                                               &docker_id_center_bottom);

        ImGuiID docker_id_right_bottom = 0;
        ImGuiID docker_id_right_top =
            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &docker_id_right_bottom);


        auto handle = this->getLuaHandle("onDockBuilder");
        if (handle && handle->isvalid())
        {
            handle->ppush();

            auto L = LuaFunction::G_L;
            lua_newtable(L);

            lua_pushstring(L, "dock_id_left_top");
            lua_pushinteger(L, (lua_Integer)dock_id_left_top);
            lua_rawset(L, -3);

            lua_pushstring(L, "dock_id_left_bottom");
            lua_pushinteger(L, (lua_Integer)dock_id_left_bottom);
            lua_rawset(L, -3);

            lua_pushstring(L, "docker_id_center");
            lua_pushinteger(L, (lua_Integer)docker_id_center);
            lua_rawset(L, -3);

            lua_pushstring(L, "docker_id_center_bottom");
            lua_pushinteger(L, (lua_Integer)docker_id_center_bottom);
            lua_rawset(L, -3);

            lua_pushstring(L, "docker_id_right_top");
            lua_pushinteger(L, (lua_Integer)docker_id_right_top);
            lua_rawset(L, -3);

            lua_pushstring(L, "docker_id_right_bottom");
            lua_pushinteger(L, (lua_Integer)docker_id_right_bottom);
            lua_rawset(L, -3);

            handle->pcall();
        }

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGuiID dockspace_id               = ImGui::GetID("MyDockspace");
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    //////////////////////////////////////////////////////////////////////////
    // overlay
    const float DISTANCE     = 10.0f;
    static int corner        = 3;
    static bool open_overlay = true;

    if (open_overlay)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (corner != -1)
        {
            ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE,
                                       (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
            ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        }
        ImGui::SetNextWindowBgAlpha(0.35f);  // Transparent background
        if (ImGui::Begin("runtime_information", &open_overlay,
                         (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            char buffer[32] = {0};
            sprintf(buffer, "FPS:%.1f / %.3f", _director->getFrameRate(), _director->getSecondsPerFrame());
            ImGui::Text(buffer);

            auto currentCalls = (unsigned long)_director->getRenderer()->getDrawnBatches();
            auto currentVerts = (unsigned long)_director->getRenderer()->getDrawnVertices();
            sprintf(buffer, "GL calls:%6lu", currentCalls);
            ImGui::Text(buffer);

            sprintf(buffer, "GL verts:%6lu", currentVerts);
            ImGui::Text(buffer);

            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Custom", NULL, corner == -1))
                    corner = -1;
                if (ImGui::MenuItem("Top-left", NULL, corner == 0))
                    corner = 0;
                if (ImGui::MenuItem("Top-right", NULL, corner == 1))
                    corner = 1;
                if (ImGui::MenuItem("Bottom-left", NULL, corner == 2))
                    corner = 2;
                if (ImGui::MenuItem("Bottom-right", NULL, corner == 3))
                    corner = 3;
                if (ImGui::MenuItem("Close"))
                    open_overlay = false;
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    ImGui::End();
}

void EditorContext::callLuaGUI(const char* name)
{
	auto handle = this->getLuaHandle(name);
	if (handle && handle->isvalid())
	{
		handle->ppush();
		handle->pcall();
	}
}
