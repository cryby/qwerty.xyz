#include <iostream>
#include <vector>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#define DIRECTINPUT_VERSION 0x0800
#define LOADERW 800
#define LOADERH 600
#include <dinput.h>
#include <tchar.h>
#include "Icons.h"
#include "Font.h"
#include "SecondFont.h"
#include "xorstr.h"
#include "LightFont.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

static int curtab;
static bool checkbox;
static float slider;
static float anti;
static float aim;
static int slider1;
const char* themes[]{ "Dark Purple", "Dark Blue", "Dark Red", "Dark Green", "Light Green" };
static int curTheme;
static const char* w[]{ "Ak-47", "Awp", "Scout" };
static const char* item[]{ "Head", "Body", "Arms", "Legs" };
static int combo;
static int selectedItem = 0;
static bool selected[4];
static std::string previewValue = "";

float r1;
float g1;
float b2;
float a1 = 0.5;
static const char* type[]{ "Fake-lag", "Desync" };
static int combo2;

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Loader"), NULL };
    ::RegisterClassEx(&wc);
    int x = GetSystemMetrics(SM_CXSCREEN) / 2 - LOADERW / 2; // cetner screen x
    int y = GetSystemMetrics(SM_CYSCREEN) / 2 - LOADERH / 2; // center screen y
    HWND hwnd = ::CreateWindowExW(WS_EX_LAYERED, wc.lpszClassName, _T("Loader"), WS_POPUP, x, y, LOADERW, LOADERH, NULL, NULL, wc.hInstance, NULL);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryTTF(&LogoFont, sizeof LogoFont, 15);
    ImFont* Logo = io.Fonts->AddFontFromMemoryTTF(&LogoFont, sizeof LogoFont, 30);
    ImFont* yep = io.Fonts->AddFontFromMemoryTTF(&OtherFont, sizeof OtherFont, 18);
    ImFont* yep2 = io.Fonts->AddFontFromMemoryTTF(&OtherFont, sizeof OtherFont, 24);
    ImFont* light = io.Fonts->AddFontFromMemoryTTF(&LightFont, sizeof LightFont, 14);
    ImFont* test = io.Fonts->AddFontFromMemoryTTF(&IconFont, sizeof IconFont, 24);

    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui::GetStyle().WindowPadding = ImVec2(0.0f, 0.0f);
    ImGui::GetStyle().ChildRounding = 0.0f;
    ImGui::GetStyle().WindowBorderSize = 0.0f;
    ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5, 0.5);
    ImGui::GetStyle().PopupBorderSize = 0.0f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.73f, 0.75f, 0.74f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.39f, 0.00f, 0.63f, 0.11f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.39f, 0.00f, 0.63f, 0.15f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.94f, 0.02f, 1.28f, 0.30f);
    colors[ImGuiCol_Border] = ImVec4(2.45f, 0.00f, 1.01f, 0.30f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.94f, 0.02f, 1.28f, 0.15f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.02f, 1.28f, 0.25f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.94f, 0.02f, 1.28f, 0.07f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.75f, 0.01f, 0.94f, 1.2f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.39f, 0.00f, 0.63f, 0.15f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.39f, 0.00f, 0.63f, 0.15f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.34f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.94f, 0.02f, 1.28f, 0.30f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.94f, 0.02f, 1.28f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.94f, 0.02f, 1.28f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.94f, 0.02f, 1.28f, 0.20f);
    colors[ImGuiCol_CheckMark] = ImVec4(2.45f, 0.00f, 0.50f, 0.25f);
    colors[ImGuiCol_SliderGrab] = ImVec4(2.45f, 0.00f, 0.50f, 0.25f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(2.45f, 0.00f, 0.50f, 0.15f);
    colors[ImGuiCol_Button] = ImVec4(0.39f, 0.00f, 0.63f, 0.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.39f, 0.00f, 0.63f, 0.15f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.00f, 0.63f, 0.05f);
    colors[ImGuiCol_Header] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.65f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
    colors[ImGuiCol_Tab] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
    colors[ImGuiCol_TabActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(2.45f, 0.00f, 1.01f, 0.30f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    static bool opened = true;

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(LOADERW, LOADERH));
            ImGui::Begin(xorstr("Qwerty.xyz"), &opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar); // Create a window called "Hello, world!" and append into it.
            ImGui::SetCursorPos(ImVec2(0, 0));
            ImGui::BeginChild(xorstr("LeftTabs"), ImVec2(230, 600));
            ImGui::SetCursorPos(ImVec2(25, 55));
            ImGui::PushFont(Logo);
            ImGui::Text(xorstr(" QWERTY"));
            ImGui::SetCursorPos(ImVec2(180, 57));
            ImGui::PushFont(test);
            ImGui::Text(xorstr("1"));
            ImGui::PushFont(yep);
            ImGui::SetCursorPos(ImVec2(0, 110));
            ImGui::Text(xorstr("   Features"));
            if (ImGui::Button(xorstr("Aimbot"), ImVec2(230, 61)))
            {
                curtab = 2;
            }
            if (ImGui::Button(xorstr("Visuals"), ImVec2(230, 61)))
            {
                curtab = 3;
            }
            if (ImGui::Button(xorstr("Anti-aim"), ImVec2(230, 61)))
            {
                curtab = 4;
            }
            if (ImGui::Button(xorstr("Changers"), ImVec2(230, 61)))
            {
                curtab = 5;
            }
            if (ImGui::Button(xorstr("Misc"), ImVec2(230, 61)))
            {
                curtab = 6;
            }
            ImGui::PushFont(yep);
            ImGui::Text(xorstr("   Information"));
            if (ImGui::Button(xorstr("Dashboard"), ImVec2(230, 61)))
            {
                curtab = 1;
            }
            ImGui::PushFont(yep);
            ImGui::SetCursorPos(ImVec2(50, 560));
            ImGui::Text(xorstr("Your Build: Test"));
            ImGui::SetCursorPos(ImVec2(42, 150));
            ImGui::PushFont(test);
            ImGui::Text(xorstr("6"));
            ImGui::SetCursorPos(ImVec2(42, 215));
            ImGui::Text(xorstr("2"));
            ImGui::SetCursorPos(ImVec2(42, 280));
            ImGui::Text(xorstr("5"));
            ImGui::SetCursorPos(ImVec2(42, 345));
            ImGui::Text(xorstr("7"));
            ImGui::SetCursorPos(ImVec2(42, 410));
            ImGui::Text(xorstr("3"));
            ImGui::SetCursorPos(ImVec2(42, 498));
            ImGui::Text(xorstr("4"));
            ImGui::PushFont(light);
            ImGui::SetCursorPos(ImVec2(65, 168));
            ImGui::Text(xorstr(" Assist your aiming."));
            ImGui::SetCursorPos(ImVec2(65, 233));
            ImGui::Text(xorstr("    Show enemies."));
            ImGui::SetCursorPos(ImVec2(65, 298));
            ImGui::Text(xorstr("   Make you spin."));
            ImGui::SetCursorPos(ImVec2(65, 363));
            ImGui::Text(xorstr("     Just beatiful."));
            ImGui::SetCursorPos(ImVec2(65, 428));
            ImGui::Text(xorstr("  Other functions."));
            ImGui::SetCursorPos(ImVec2(65, 516));
            ImGui::Text(xorstr("     Update log."));
            ImGui::PushFont(yep);
            ImGui::EndChild();
            if (curtab == 1)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, 478), ImVec2(230, 539), ImGui::ColorConvertFloat4ToU32(ImVec4(2.55f, 0.00f, 0.00f, 0.1f)));
                ImGui::SetCursorPos(ImVec2(246, 43));
                ImGui::BeginChild(xorstr("Updatelog"), ImVec2(540, 530));
                ImGui::PushFont(yep2);
                ImGui::SetCursorPos(ImVec2(165, 10));
                ImGui::Text(xorstr("Welcome xLamantine"));
                ImGui::PushFont(yep);
                ImGui::SetCursorPos(ImVec2(220, 35));
                ImGui::Text(xorstr("Update Log:"));
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(320, 100), ImVec2(730, 200), ImGui::ColorConvertFloat4ToU32(ImVec4(2.55f, 0.00f, 0.00f, 0.1f)));
                ImGui::Text(xorstr("                            -              Added Neverlose resolver"));
                ImGui::Text(xorstr("                            -              Added Nemesis Fake-lag"));
                ImGui::Text(xorstr("                            -              Added 1 tick Double Tap"));
                ImGui::PushFont(light);
                ImGui::PushFont(yep);
                ImGui::SetCursorPos(ImVec2(220, 160));
                ImGui::Text(xorstr("  Version: 1.0"));
                ImGui::EndChild();
            }
            else if (curtab == 2)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, 133), ImVec2(230, 194), ImGui::ColorConvertFloat4ToU32(ImVec4(2.55f, 0.00f, 0.00f, 0.1f)));
                ImGui::SetCursorPos(ImVec2(246, 45));
                ImGui::BeginChild(xorstr("AimParam"), ImVec2(540, 530));
                ImGui::PushFont(yep);
                ImGui::SetCursorPos(ImVec2(10, 10));
                ImGui::Text(xorstr("Aimbot"));
                ImGui::SameLine();
                ImGui::Checkbox(xorstr("##Aimbot"), &checkbox);
                ImGui::SliderFloat(xorstr("Aimbot FOV"), &slider, 0, 100);
                ImGui::SliderInt(xorstr("Aimbot Distance"), &slider1, 0, 100);
                ImGui::Combo(xorstr("Weapon"), &combo, w, ARRAYSIZE(w));
                if (ImGui::BeginCombo(xorstr("Hitbox"), previewValue.c_str()))
                {
                    previewValue = "";
                    std::vector<std::string> vec;
                    for (size_t i = 0; i < IM_ARRAYSIZE(item); i++)
                    {
                        ImGui::Selectable(item[i], &selected[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
                        if (selected[i])
                            vec.push_back(item[i]);
                    }
                    for (size_t i = 0; i < vec.size(); i++)
                    {
                        if (vec.size() == 1)
                            previewValue += vec.at(i);
                        else if (!(i == vec.size() - 1))
                            previewValue += vec.at(i) + ", ";
                        else
                            previewValue += vec.at(i);

                    }
                }
            }
            else if (curtab == 3)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, 198), ImVec2(230, 259), ImGui::ColorConvertFloat4ToU32(ImVec4(r1, b2, g1, a1)));
                ImGui::SetCursorPos(ImVec2(246, 45));
                ImGui::BeginChild(xorstr("VisualParam"), ImVec2(540, 530));
                ImGui::SetCursorPos(ImVec2(10, 10));
                ImGui::PushFont(yep);;
            }
            else if (curtab == 4)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, 263), ImVec2(230, 324), ImGui::ColorConvertFloat4ToU32(ImVec4(2.55f, 0.00f, 0.00f, 0.1f)));
                ImGui::SetCursorPos(ImVec2(246, 45));
                ImGui::BeginChild(xorstr("AntiParam"), ImVec2(540, 530));
                ImGui::SetCursorPos(ImVec2(10, 10));
                ImGui::PushFont(yep);;
            }
            else if (curtab == 5)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, 328), ImVec2(230, 389), ImGui::ColorConvertFloat4ToU32(ImVec4(2.55f, 0.00f, 0.00f, 0.1f)));
                ImGui::SetCursorPos(ImVec2(246, 45));
                ImGui::BeginChild(xorstr("ChangerParam"), ImVec2(540, 530));
                ImGui::SetCursorPos(ImVec2(10, 10));
                ImGui::PushFont(yep);
                ImGui::Combo(xorstr("Menu Theme"), &curTheme, themes, ARRAYSIZE(themes));
            }
            else if (curtab == 6)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, 393), ImVec2(230, 454), ImGui::ColorConvertFloat4ToU32(ImVec4(2.55f, 0.00f, 0.00f, 0.1f)));
                ImGui::SetCursorPos(ImVec2(246, 45));
                ImGui::BeginChild(xorstr("MiscParam"), ImVec2(540, 530));
            }
            if (curTheme == 0)
            {
                r1 = 2.55f;
                b2 = 0.00f;
                g1 = 0.00f;
                a1 = 0.1f;
                colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
                colors[ImGuiCol_TextDisabled] = ImVec4(0.73f, 0.75f, 0.74f, 1.00f);
                colors[ImGuiCol_WindowBg] = ImVec4(0.39f, 0.00f, 0.63f, 0.11f);
                colors[ImGuiCol_ChildBg] = ImVec4(0.39f, 0.00f, 0.63f, 0.15f);
                colors[ImGuiCol_PopupBg] = ImVec4(0.94f, 0.02f, 1.28f, 0.30f);
                colors[ImGuiCol_Border] = ImVec4(2.45f, 0.00f, 1.01f, 0.30f);
                colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                colors[ImGuiCol_FrameBg] = ImVec4(0.94f, 0.02f, 1.28f, 0.15f);
                colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.02f, 1.28f, 0.25f);
                colors[ImGuiCol_FrameBgActive] = ImVec4(0.94f, 0.02f, 1.28f, 0.07f);
                colors[ImGuiCol_TitleBg] = ImVec4(0.75f, 0.01f, 0.94f, 1.2f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.39f, 0.00f, 0.63f, 0.15f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.39f, 0.00f, 0.63f, 0.15f);
                colors[ImGuiCol_MenuBarBg] = ImVec4(0.34f, 0.16f, 0.16f, 1.00f);
                colors[ImGuiCol_ScrollbarBg] = ImVec4(0.94f, 0.02f, 1.28f, 0.30f);
                colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.94f, 0.02f, 1.28f, 0.30f);
                colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.94f, 0.02f, 1.28f, 0.40f);
                colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.94f, 0.02f, 1.28f, 0.20f);
                colors[ImGuiCol_CheckMark] = ImVec4(2.45f, 0.00f, 0.50f, 0.25f);
                colors[ImGuiCol_SliderGrab] = ImVec4(2.45f, 0.00f, 0.50f, 0.25f);
                colors[ImGuiCol_SliderGrabActive] = ImVec4(2.45f, 0.00f, 0.50f, 0.15f);
                colors[ImGuiCol_Button] = ImVec4(0.39f, 0.00f, 0.63f, 0.0f);
                colors[ImGuiCol_ButtonHovered] = ImVec4(0.39f, 0.00f, 0.63f, 0.15f);
                colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.00f, 0.63f, 0.05f);
                colors[ImGuiCol_Header] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
                colors[ImGuiCol_HeaderHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.65f);
                colors[ImGuiCol_HeaderActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.00f);
                colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
                colors[ImGuiCol_SeparatorHovered] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
                colors[ImGuiCol_SeparatorActive] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
                colors[ImGuiCol_ResizeGrip] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
                colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
                colors[ImGuiCol_ResizeGripActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
                colors[ImGuiCol_Tab] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
                colors[ImGuiCol_TabHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
                colors[ImGuiCol_TabActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
                colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
                colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
                colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
                colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
                colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
                colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
                colors[ImGuiCol_TextSelectedBg] = ImVec4(2.45f, 0.00f, 1.01f, 0.30f);
                colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
                colors[ImGuiCol_NavHighlight] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
                colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
                colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
                colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
            }
            else if (curTheme == 1)
            {
                r1 = 0.016f;
                b2 = 0.984f;
                g1 = 0.984f;
                a1 = 0.1f;
                colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
                colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
                colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
                colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
                colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
                colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
                colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
                colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
                colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
                colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
                colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
                colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
                colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
                colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
                colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
                colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
                colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
                colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
                colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 0.00f);
                colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
                colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
                colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
                colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
                colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
                colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
                colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
                colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
                colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
                colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
                colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
                colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
                colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
                colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
                colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
                colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
                colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
                colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
                colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
                colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
                colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
                colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
                colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
                colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
            }
            else if (curTheme == 2)
            {
                colors[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
                colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
                colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
                colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 0.19f, 0.19f, 0.10f);
                colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
                colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
                colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
                colors[ImGuiCol_FrameBgHovered] = ImVec4(0.37f, 0.14f, 0.14f, 0.67f);
                colors[ImGuiCol_FrameBgActive] = ImVec4(0.39f, 0.20f, 0.20f, 0.67f);
                colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.48f, 0.16f, 0.16f, 1.00f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.48f, 0.16f, 0.16f, 1.00f);
                colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
                colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
                colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
                colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
                colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
                colors[ImGuiCol_CheckMark] = ImVec4(0.56f, 0.10f, 0.10f, 1.00f);
                colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.19f, 0.19f, 0.40f);
                colors[ImGuiCol_SliderGrabActive] = ImVec4(0.89f, 0.00f, 0.19f, 1.00f);
                colors[ImGuiCol_Button] = ImVec4(1.00f, 0.19f, 0.19f, 0.00f);
                colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.17f, 0.00f, 1.00f);
                colors[ImGuiCol_ButtonActive] = ImVec4(0.89f, 0.00f, 0.19f, 1.00f);
                colors[ImGuiCol_Header] = ImVec4(0.33f, 0.35f, 0.36f, 0.53f);
                colors[ImGuiCol_HeaderHovered] = ImVec4(0.76f, 0.28f, 0.44f, 0.67f);
                colors[ImGuiCol_HeaderActive] = ImVec4(0.47f, 0.47f, 0.47f, 0.67f);
                colors[ImGuiCol_Separator] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
                colors[ImGuiCol_SeparatorHovered] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
                colors[ImGuiCol_SeparatorActive] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
                colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
                colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
                colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
                colors[ImGuiCol_Tab] = ImVec4(0.07f, 0.07f, 0.07f, 0.51f);
                colors[ImGuiCol_TabHovered] = ImVec4(0.86f, 0.23f, 0.43f, 0.67f);
                colors[ImGuiCol_TabActive] = ImVec4(0.19f, 0.19f, 0.19f, 0.57f);
                colors[ImGuiCol_TabUnfocused] = ImVec4(0.05f, 0.05f, 0.05f, 0.90f);
                colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.13f, 0.13f, 0.74f);
                colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
                colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
                colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
                colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
                colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
                colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
                colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
                colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
                colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
            }
            else if (curTheme == 3)
            {
                colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
                colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
                colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.75f, 0.55f, 0.10f);
                colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
                colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
                colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.44f, 0.44f, 0.60f);
                colors[ImGuiCol_FrameBgHovered] = ImVec4(0.57f, 0.57f, 0.57f, 0.70f);
                colors[ImGuiCol_FrameBgActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.80f);
                colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
                colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
                colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
                colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
                colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
                colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
                colors[ImGuiCol_CheckMark] = ImVec4(0.13f, 0.75f, 0.55f, 0.80f);
                colors[ImGuiCol_SliderGrab] = ImVec4(0.13f, 0.75f, 0.75f, 0.80f);
                colors[ImGuiCol_SliderGrabActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
                colors[ImGuiCol_Button] = ImVec4(0.13f, 0.75f, 0.55f, 0.00f);
                colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
                colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
                colors[ImGuiCol_Header] = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
                colors[ImGuiCol_HeaderHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
                colors[ImGuiCol_HeaderActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
                colors[ImGuiCol_Separator] = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
                colors[ImGuiCol_SeparatorHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
                colors[ImGuiCol_SeparatorActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
                colors[ImGuiCol_ResizeGrip] = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
                colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
                colors[ImGuiCol_ResizeGripActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
                colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.75f, 0.55f, 0.80f);
                colors[ImGuiCol_TabHovered] = ImVec4(0.13f, 0.75f, 0.75f, 0.80f);
                colors[ImGuiCol_TabActive] = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
                colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
                colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.36f, 0.36f, 0.36f, 0.54f);
                colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
                colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
                colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
                colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
                colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
                colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
                colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
                colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
                colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
            }
            else if (curTheme == 4)
            {
                colors[ImGuiCol_Text] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
                colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
                colors[ImGuiCol_WindowBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
                colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
                colors[ImGuiCol_PopupBg] = ImVec4(0.93f, 0.93f, 0.93f, 0.98f);
                colors[ImGuiCol_Border] = ImVec4(0.71f, 0.71f, 0.71f, 0.08f);
                colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
                colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.55f);
                colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.94f, 0.94f, 0.55f);
                colors[ImGuiCol_FrameBgActive] = ImVec4(0.71f, 0.78f, 0.69f, 0.98f);
                colors[ImGuiCol_TitleBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.82f, 0.78f, 0.78f, 0.51f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
                colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
                colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.61f);
                colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
                colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.92f, 0.92f, 0.78f);
                colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                colors[ImGuiCol_CheckMark] = ImVec4(0.184f, 0.407f, 0.193f, 1.00f);
                colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
                colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                colors[ImGuiCol_Button] = ImVec4(0.71f, 0.78f, 0.69f, 0.00f);
                colors[ImGuiCol_ButtonHovered] = ImVec4(0.725f, 0.805f, 0.702f, 1.00f);
                colors[ImGuiCol_ButtonActive] = ImVec4(0.793f, 0.900f, 0.836f, 1.00f);
                colors[ImGuiCol_Header] = ImVec4(0.71f, 0.78f, 0.69f, 0.31f);
                colors[ImGuiCol_HeaderHovered] = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
                colors[ImGuiCol_HeaderActive] = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
                colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
                colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
                colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
                colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
                colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.45f);
                colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
                colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
                colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
                colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
                colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
                colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
                colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
                colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
                colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
                colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
            }
            ImGui::End();
        }


        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, NULL, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();

        if (!opened) msg.message = WM_QUIT;
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

POINTS m_Pos; // store user click pos

// Win32 message handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        m_Pos = MAKEPOINTS(lParam); // set click points
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        if (wParam == MK_LBUTTON)
        {
            POINTS p = MAKEPOINTS(lParam); // get cur mousemove click points
            RECT rect;
            GetWindowRect(hWnd, &rect);
            rect.left += p.x - m_Pos.x; // get xDelta
            rect.top += p.y - m_Pos.y;  // get yDelta
            if (m_Pos.x >= 0 && m_Pos.x <= LOADERW - 20 /* cuz 20px - close btn */ && m_Pos.y >= 0 && m_Pos.y <= ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f)
                SetWindowPos(hWnd, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
        }
        return 0;
    }
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
