#pragma once
#include "../includes.hpp"
#include "../steam_sdk/steam_api.h"

class c_menu : public singleton<c_menu> {
public:
	LPDIRECT3DTEXTURE9 CTmd1 = nullptr;
	LPDIRECT3DTEXTURE9 head = nullptr;
	LPDIRECT3DTEXTURE9 head1 = nullptr;

	LPDIRECT3DTEXTURE9 neck = nullptr;
	LPDIRECT3DTEXTURE9 neck1 = nullptr;
	LPDIRECT3DTEXTURE9 upchest = nullptr;
	LPDIRECT3DTEXTURE9 upchest1 = nullptr;
	LPDIRECT3DTEXTURE9 chest = nullptr;
	LPDIRECT3DTEXTURE9 chest1 = nullptr;
	LPDIRECT3DTEXTURE9 lowchest = nullptr;
	LPDIRECT3DTEXTURE9 lowchest1 = nullptr;
	LPDIRECT3DTEXTURE9 pel = nullptr;
	LPDIRECT3DTEXTURE9 pel1 = nullptr;

	LPDIRECT3DTEXTURE9 arms = nullptr;
	LPDIRECT3DTEXTURE9 arms1 = nullptr;
	LPDIRECT3DTEXTURE9 arms2 = nullptr;
	LPDIRECT3DTEXTURE9 arms3 = nullptr;

	LPDIRECT3DTEXTURE9 legs = nullptr;
	LPDIRECT3DTEXTURE9 legs1 = nullptr;
	LPDIRECT3DTEXTURE9 legs2 = nullptr;
	LPDIRECT3DTEXTURE9 legs3 = nullptr;

	LPDIRECT3DTEXTURE9 feet = nullptr;
	LPDIRECT3DTEXTURE9 feet1 = nullptr;
	LPDIRECT3DTEXTURE9 feet2 = nullptr;
	LPDIRECT3DTEXTURE9 feet3 = nullptr;
	void draw( bool is_open );
	void menu_setup(ImGuiStyle &style);
	void render2(bool is_open);

	float dpi_scale = 1.f;


	ImFont* neverpuk;
	ImFont* neverpuk_small;
	ImFont* neverpuk_large;

	ImFont* futura;
	ImFont* futura_large;
	ImFont* icons;
	ImFont* bigxd;
	ImFont* futura_small;

	IDirect3DTexture9** friends_images;
	LPDIRECT3DTEXTURE9 steam_image();
	LPDIRECT3DTEXTURE9 steam_image_friends(CSteamID SteamId);

	ImFont* gotham;

	ImFont* ico_menu;
	ImFont* ico_bottom;

	float public_alpha;
	IDirect3DDevice9* device;
	float color_buffer[4] = { 1.f, 1.f, 1.f, 1.f };
	int players_section;
	void InitVisuals();//переписать
	enum Sides
	{
		LEFT_S,
		RIGHT_S,
		TOP_S,
		DOWN_S,
		LEFT_TOP_S,
		RIGHT_TOP_S,
		LEFT_DOWN_S,
		RIGHT_DOWN_S,
		DOWN_LEFT_DOWN_S,
		DOWN_RIGHT_DOWN_S,
		CENTER_LEFT_S,
		CENTER_RIGHT_S,
	};

	struct MoveStruct
	{
		ImVec2 ZonePos;

		ImVec2 StartPos;
		ImVec2 Position;
		ImVec2 Offset;
		ImVec2 Correct;
		ImVec2 Size;
		ImVec2 SizeV;
		ImVec2 SizeH;
		ImVec2 Center;
		ImVec2 CorrectPadding;

		std::string Name;

		Color Color1;
		Color Color2;
		Color Color3;


		int NumParam1 = 4;
		int NumParam2 = 5;

		int TextSize = 17;

		bool boolParam1 = true;
		bool boolParam2 = true;
		bool boolParam3 = false;

		bool IsHover = false;
		bool Set = false;
		bool IsText = false;
		bool IsEnabled = false;

		int Id = -2;

		ImVec2 CorrectOfst;

		Sides Side = LEFT_S;

		static int ActiveIdx;
		static int LastActive;
		static bool InitMItems;
	};
	enum MITypes : int
	{
		MI_HEALTH_BAR = 0,
		MI_ARMOR_BAR = 1,
		MI_NAME_TEXT = 2,
		MI_WEAPON_TEXT = 3,
		MI_AMMO_TEXT = 4,
		MI_DISTANCE_TEXT = 5,
		MI_MONEY_TEXT = 6,
		MI_SCOPE_TEXT = 7,
		MI_FLASHED_TEXT = 8,
		MI_DEFUSING_TEXT = 9,
		MI_HEALTH_TEXT = 10,
		MI_ARMOR_TEXT = 11,
		MI_MAX
	};//переписать

	std::vector<MoveStruct> MVItemList;
	void Move(MoveStruct& Cont, bool IsPressed, ImVec2 MouseP, ImVec2 StartP, ImVec2 CLines, bool IsText = false); //переписать
	enum Orent
	{
		HORIZONTAL,
		VERTICAL
	};
	void VisualPreview(); //переписать
	void CustomVisuals(ImVec2 Start); //переписать
	c_menu()
	{
		for (int i(0); (MITypes)i < MI_MAX; i++)
			MVItemList.push_back(MoveStruct());
	}

private:
	struct {
		ImVec2 WindowPadding;
		float  WindowRounding;
		ImVec2 WindowMinSize;
		float  ChildRounding;
		float  PopupRounding;
		ImVec2 FramePadding;
		float  FrameRounding;
		ImVec2 ItemSpacing;
		ImVec2 ItemInnerSpacing;
		ImVec2 TouchExtraPadding;
		float  IndentSpacing;
		float  ColumnsMinSpacing;
		float  ScrollbarSize;
		float  ScrollbarRounding;
		float  GrabMinSize;
		float  GrabRounding;
		float  TabRounding;
		float  TabMinWidthForUnselectedCloseButton;
		ImVec2 DisplayWindowPadding;
		ImVec2 DisplaySafeAreaPadding;
		float  MouseCursorScale;
	} styles;

	bool update_dpi = false;
	bool update_scripts = false;
	void dpi_resize(float scale_factor, ImGuiStyle &style);

	int active_tab_index;
	ImGuiStyle style;
	int width = 850, height = 560;
	float child_height;

	float preview_alpha = 1.f;

	int active_tab;

	int rage_section;
	int legit_section;
	int visuals_section;
	//int players_section;
	int misc_section;
	int settings_section;

	// we need to use 'int child' to seperate content in 2 childs
	void draw_ragebot(int child);
	void draw_tabs_ragebot();

	void draw_legit(int child);

	void create_style();

	ImGuiStyle        _style;
	 
	void draw_visuals(int child);
	void draw_tabs_visuals();
	int current_profile = -1;

	void draw_players(int child);
	void draw_tabs_players();

	void draw_misc(int child);
	void draw_tabs_misc();

	void draw_settings(int child);

	void draw_lua(int child);
	void draw_radar(int child);
	void draw_player_list(int child);

	std::string preview = crypt_str("None");
};
