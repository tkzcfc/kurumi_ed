#include "EditorContext.h"
#include "imgui/imgui_internal.h"
#include "core/Tools.h"
#include "ui/CocosGUI.h"
#include "sequentity/Sequentity.h"
#include "core/Tools.h"

// ¡Ω≤‡√Ê∞Â∑∂Œß
static float bothSidesPanelMinWidth = 50.0f;
static float bothSidesPanelMaxWidth = 350.0f;

static float topPanelMinHeight = 50.0f;
static float topPanelMaxHeight = 200.0f;

static float bottomPanelMinHeight = 50.0f;
static float bottomPanelMaxHeight = 400.0f;

static constexpr char* backgroundImageResource = "bg.jpg";


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


	m_iLayer->addImGUI([&]() {
		Sequentity::ThemeEditor(NULL);

		if (ImGui::Begin("test"))
		{
			Sequentity::DrawContext(context);

			Sequentity::Intersect(track, context.state.current_time, [&](auto& event) {
				if (event.type == event_1) {
					auto& data = *static_cast<EventData*>(event.data);
					//CCLOG("in event %d", data.number);
				}
			});
		}
		ImGui::End();

	}, "drawSequentity");
}

//////////////////////////////////////////////////////////////////////////

EditorContext::EditorContext()
	: m_iLayer(NULL)
	, m_backgroundNode(NULL)
	, m_initGUITag(false)
	, m_leftPaneWidth(0.0f)
	, m_centerPaneWidth(0.0f)
	, m_rightPaneWidth(0.0f)
	, m_topPanelHeight(0.0f)
	, m_bottomPanelHeight(0.0f)
	, m_centerPaneHeight(0.0f)
	, m_rightPaneWidthTmp(0.0f)
	, m_bottomPaneWidthTmp(0.0f)
{}

EditorContext::~EditorContext()
{
	TextureCache::getInstance()->unbindImageAsync(backgroundImageResource);
}


bool EditorContext::init()
{
	if (!Node::init())
	{
		return false;
	}

	TextureCache::getInstance()->addImageAsync(backgroundImageResource, [=](Texture2D* texture)
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
	
	m_iLayer = ILayer::create();
	m_iLayer->setDrawDefaultWindow(false);
	m_iLayer->setGUIBeginCall([=]() 
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::Begin("Content", nullptr, ImVec2(0, 0), 0.0f, flags);
	});
	m_iLayer->setGUIEndCall([]() 
	{
		ImGui::End();
	});
	this->addChild(m_iLayer, 1);

	m_iLayer->addImGUI(std::bind(&EditorContext::onGUI, this), "contextGUI");

	//testSequentity();

	return true;
}

ILayer* EditorContext::getIlayer()
{
	return m_iLayer;
}

Node* EditorContext::getBackgroundNode()
{
	return m_backgroundNode;
}

void EditorContext::initGUI()
{
	if(m_initGUITag)
	{
		return;
	}
	m_initGUITag = true;

	auto& io = ImGui::GetIO();

	/// <summary>
	/// ∑∂Œß…Ë÷√
	/// </summary>
	auto winSize = Director::getInstance()->getWinSize();
	bothSidesPanelMaxWidth = winSize.width * 0.3f;
	topPanelMaxHeight = winSize.height * 0.05f;
	bottomPanelMaxHeight = winSize.height * 0.4f;

	float DefaultWidth = winSize.width * 0.2f;

	m_leftPaneWidth = DefaultWidth;
	m_centerPaneWidth = 0;
	m_rightPaneWidth = DefaultWidth;

	m_topPanelHeight = topPanelMinHeight;
	m_bottomPanelHeight = bottomPanelMaxHeight;

	m_rightPaneWidthTmp = std::fabs(bothSidesPanelMaxWidth - m_leftPaneWidth);
	m_bottomPaneWidthTmp = std::fabs(topPanelMaxHeight - m_topPanelHeight);
}

void EditorContext::onGUI()
{
	auto& color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	_director->setClearColor(Color4F(color.x, color.y, color.z, color.w));
	
	initGUI();

	// bar
	if (ImGui::BeginMenuBar())
	{
		callLuaGUI("onGUI_MenuBar");
		ImGui::EndMenuBar();
	}

	splitter("##Splitter_L_R", true, getSplitterThickness(), &m_leftPaneWidth, &m_rightPaneWidthTmp, bothSidesPanelMinWidth, 0.0f);

	if (ImGui::BeginChild("context_left", ImVec2(m_leftPaneWidth - getSplitterThickness(), 0), true))
	{
		callLuaGUI("onGUI_Left");
	}
	ImGui::EndChild();

	//ImGui::SameLine(0.0f, 12.0f);
	ImGui::SameLine();

	if (ImGui::BeginChild("context_right_all", ImVec2(0, 0)))
	{
		m_centerPaneWidth = ImGui::GetContentRegionAvailWidth() - m_rightPaneWidth;
		splitter("##Splitter_C_R", true, getSplitterThickness(), &m_centerPaneWidth, &m_rightPaneWidth, ImGui::GetContentRegionAvailWidth() - bothSidesPanelMaxWidth, bothSidesPanelMinWidth);

		if (ImGui::BeginChild("context_center_all", ImVec2(m_centerPaneWidth, 0)))
		{
			splitter("##Splitter_T_B", false, getSplitterThickness(), &m_topPanelHeight, &m_bottomPaneWidthTmp, topPanelMinHeight, 0);
			if (ImGui::BeginChild("context_top", ImVec2(0, m_topPanelHeight), true))
			{
				callLuaGUI("onGUI_Top");
			}
			ImGui::EndChild();

			auto avali = ImGui::GetContentRegionAvail();
			m_centerPaneHeight = avali.y - m_bottomPanelHeight;
			splitter("##Splitter_C_B", false, getSplitterThickness(), &m_centerPaneHeight, &m_bottomPanelHeight, avali.y - bottomPanelMaxHeight, bottomPanelMinHeight);
			if (ImGui::BeginChild("context_center", ImVec2(0, m_centerPaneHeight), false, ImGuiWindowFlags_NoBackground))
			{
				callLuaGUI("onGUI_Center");
			}
			ImGui::EndChild();

			if (ImGui::BeginChild("context_bottom", ImVec2(0, 0)))
			{
				callLuaGUI("onGUI_Bottom");
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();

		//ImGui::SameLine(0.0f, 12.0f);
		ImGui::SameLine();

		if (ImGui::BeginChild("context_right", ImVec2(0, 0), true))
		{
			callLuaGUI("onGUI_Right");
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();


	//////////////////////////////////////////////////////////////////////////
	// overlay
	const float DISTANCE = 10.0f;
	static int corner = 3;
	static bool open_overlay = true;

	if (open_overlay)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (corner != -1)
		{
			ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
			ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		}
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		if (ImGui::Begin("runtime_information", &open_overlay, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			char buffer[32] = { 0 };
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
				if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
				if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
				if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
				if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
				if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
				if (ImGui::MenuItem("Close")) open_overlay = false;
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}
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


float EditorContext::getSplitterThickness()
{
	return 4.0f * Tools::getImgUIGlobalScale();
}

bool EditorContext::splitter(const char* sp_name, bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size)
{
	ImGuiContext* g = ImGui::GetCurrentContext();
	ImGuiWindow* window = g->CurrentWindow;
	ImGuiID id = window->GetID(sp_name);
	ImRect bb;
	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + ImGui::CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
	return ImGui::SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}
