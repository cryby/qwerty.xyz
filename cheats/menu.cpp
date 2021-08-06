// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <ShlObj_core.h>
#include <unordered_map>
#include "menu.h"
#include "../ImGui/code_editor.h"
#include "../constchars.h"
#include "../cheats/misc/logs.h"
#include "..\cheats\misc\misc.h"
#include "..\steam_sdk\steam_api.h"
#include "../configs/configs.h"

CHAR my_documents[MAX_PATH];
HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);


#define IMAGE_X 110
#define IMAGE_Y 245

#define IMAGE_OUTLINE 50

#define LINES_X 70
#define LINES_Y 80

#define BOX_POS_X 45
#define BOX_POS_Y 20

#define BOX_SIZE_X 160
#define BOX_SIZE_Y 310

#define MV_HEALTH_TEXT crypt_str("50")
#define MV_ARMOR_TEXT crypt_str("50")
#define MV_NAME_TEXT crypt_str("Name")
#define MV_WEAPON_TEXT crypt_str("Awp")
#define MV_WEAPON_TEXT_ICO crypt_str("S") //M4A4 Icon
#define MV_DISTANCE_TEXT crypt_str("5m")
#define MV_MONEY_TEXT crypt_str("1000$")
#define MV_SCOPE_TEXT crypt_str("SCOPED")
#define MV_FLASHED_TEXT crypt_str("FLASHED")
#define MV_DEFUSING_TEXT crypt_str("DEFUSING")

#define FULL_BACK_SIZE_X 100
#define FULL_BACK_SIZE_Y 100

#define SELECTED_BORDER_SIZE 4

#define VIS_PREW_ZONE_X 300 *c_menu::get().dpi_scale 
#define VIS_PREW_ZONE_Y 300 *c_menu::get().dpi_scale 

bool IsMousPress = false;
bool IsMousClick = false;

int BoxSize_Y = 0;
int BoxSize_X = 0;

bool IsBlockMove = false;


int c_menu::MoveStruct::ActiveIdx = -1;
int c_menu::MoveStruct::LastActive = -1;
bool c_menu::MoveStruct::InitMItems = false;

#define ALPHA (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar| ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)
#define NOALPHA (ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)

std::vector <std::string> files;
std::vector <std::string> scripts;
std::string editing_script;

auto selected_script = 0;
auto loaded_editing_script = false;

static auto menu_setupped = false;
static auto should_update = true;

IDirect3DTexture9* all_skins[36];
void c_menu::Move(MoveStruct& Cont, bool IsPressed, ImVec2 MouseP, ImVec2 StartP, ImVec2 CLines, bool IsText)
{
	ImGuiIO& g = ImGui::GetIO();
	bool ChangeSide = false;
	if (MoveStruct::ActiveIdx == Cont.Id || MoveStruct::ActiveIdx == -1)
	{
		if (IsPressed && Cont.IsHover && Cont.Set)
		{
			MoveStruct::ActiveIdx = Cont.Id;
			MoveStruct::LastActive = Cont.Id;

			Cont.Offset.x = StartP.x - (MouseP.x - Cont.Correct.x);
			Cont.Offset.y = StartP.y - (MouseP.y - Cont.Correct.y);

			if (!IsText)
			{
				Sides OSide = Cont.Side;

				if (Cont.Center.x > CLines.x) { Cont.Side = RIGHT_S; }
				if (Cont.Center.x < CLines.x) { Cont.Side = LEFT_S; }

				if (g.MousePos.y < CLines.y + (BoxSize_Y / 8)) { Cont.Side = TOP_S; }
				if (g.MousePos.y > CLines.y + BoxSize_Y - (BoxSize_Y / 8)) { Cont.Side = DOWN_S; }

				if (OSide != Cont.Side && ((OSide != LEFT_S || Cont.Side != RIGHT_S) && (OSide != RIGHT_S || Cont.Side != LEFT_S)))
					ChangeSide = true;
			}
			else
			{
				if (Cont.Center.x > CLines.x) { Cont.Side = RIGHT_S; }
				if (Cont.Center.x < CLines.x) { Cont.Side = LEFT_S; }

				if (Cont.Center.x > CLines.x && Cont.Center.y > CLines.y + (BoxSize_Y / 3)) { Cont.Side = CENTER_RIGHT_S; }
				if (Cont.Center.x < CLines.x && Cont.Center.y > CLines.y + (BoxSize_Y / 3)) { Cont.Side = CENTER_LEFT_S; }

				if (Cont.Center.x > CLines.x && Cont.Center.y > CLines.y + BoxSize_Y - (BoxSize_Y / 3)) { Cont.Side = DOWN_RIGHT_DOWN_S; }
				if (Cont.Center.x < CLines.x && Cont.Center.y > CLines.y + BoxSize_Y - (BoxSize_Y / 3)) { Cont.Side = DOWN_LEFT_DOWN_S; }

				if (Cont.Center.y < CLines.y) { Cont.Side = TOP_S; }
				if (Cont.Center.y < CLines.y && Cont.Center.x < CLines.x - (BoxSize_X / 4)) { Cont.Side = LEFT_TOP_S; }
				if (Cont.Center.y < CLines.y && Cont.Center.x > CLines.x + (BoxSize_X / 4)) { Cont.Side = RIGHT_TOP_S; }

				if (Cont.Center.y > CLines.y + BoxSize_Y) { Cont.Side = DOWN_S; }
				if (Cont.Center.y > CLines.y + BoxSize_Y && Cont.Center.x < CLines.x - (BoxSize_X / 4)) { Cont.Side = LEFT_DOWN_S; }
				if (Cont.Center.y > CLines.y + BoxSize_Y && Cont.Center.x > CLines.x + (BoxSize_X / 4)) { Cont.Side = RIGHT_DOWN_S; }
			}
		}
		else
			MoveStruct::ActiveIdx = -1;

		Cont.Position = ImVec2(StartP.x - Cont.Offset.x, StartP.y - Cont.Offset.y);

		if ((MouseP.x < (Cont.Position.x + Cont.Size.x) && MouseP.x > Cont.Position.x) && (MouseP.y < (Cont.Position.y + Cont.Size.y) && MouseP.y > Cont.Position.y))
			Cont.IsHover = true;
		else
			Cont.IsHover = false;

		if (IsPressed && Cont.IsHover)
		{
			if (!Cont.Set && !IsBlockMove)
			{
				Cont.Set = true;
				Cont.Correct.x = MouseP.x - Cont.Position.x;
				Cont.Correct.y = MouseP.y - Cont.Position.y;
			}
		}
		else
			Cont.Set = false;

		Cont.Center = ImVec2(Cont.Position.x + Cont.Size.x / 2, Cont.Position.y + Cont.Size.y / 2);
		if (!IsText)
		{
			if (Cont.Side == DOWN_S || Cont.Side == TOP_S)
				Cont.Size = Cont.SizeH;

			if (Cont.Side == LEFT_S || Cont.Side == RIGHT_S)
				Cont.Size = Cont.SizeV;

			if (ChangeSide)
			{
				Cont.Center = MouseP;

				Cont.Offset.x = StartP.x - (MouseP.x - Cont.Size.x / 2);
				Cont.Offset.y = StartP.y - (MouseP.y - Cont.Size.y / 2);

				Cont.Position = ImVec2(StartP.x - Cont.Offset.x, StartP.y - Cont.Offset.y);

				Cont.Correct.x = MouseP.x - Cont.Position.x;
				Cont.Correct.y = MouseP.y - Cont.Position.y;
			}
		}
	}
}
void c_menu::InitVisuals()
{
	auto LInitVisuals = [&]()->void
	{

		static MoveStruct* MV_Hp = &MVItemList[MI_HEALTH_BAR];
		static MoveStruct* MV_Armor = &MVItemList[MI_ARMOR_BAR];

		static MoveStruct* MV_Hp_Text = &MVItemList[MI_HEALTH_TEXT];
		static MoveStruct* MV_Armor_Text = &MVItemList[MI_ARMOR_TEXT];

		static MoveStruct* MV_Name = &MVItemList[MI_NAME_TEXT];
		static MoveStruct* MV_Weapon = &MVItemList[MI_WEAPON_TEXT];
		static MoveStruct* MV_Ammo = &MVItemList[MI_AMMO_TEXT];
		static MoveStruct* MV_Distance = &MVItemList[MI_DISTANCE_TEXT];
		static MoveStruct* MV_Money = &MVItemList[MI_MONEY_TEXT];
		static MoveStruct* MV_Scope = &MVItemList[MI_SCOPE_TEXT];
		static MoveStruct* MV_Flashed = &MVItemList[MI_FLASHED_TEXT];
		static MoveStruct* MV_Defusing = &MVItemList[MI_DEFUSING_TEXT];

		if (!MoveStruct::InitMItems)
		{
			MoveStruct::InitMItems = true;
			MoveStruct::LastActive = MI_MAX;

			/*=====================================================================================*/
			MV_Hp_Text->StartPos = MV_Hp_Text->Position;
			MV_Hp_Text->Id = MI_HEALTH_TEXT;
			MV_Hp_Text->Side = LEFT_S;
			MV_Hp_Text->TextSize = 17;
			MV_Hp_Text->Offset = ImVec2(5, 0);
			MV_Hp_Text->IsText = true;
			MV_Hp_Text->Name = crypt_str("MV_HP_Text");
			MV_Hp_Text->Color1[0] = 1.0f;
			MV_Hp_Text->Color1[1] = 1.0f;
			MV_Hp_Text->Color1[2] = 1.0f;

			/*=====================================================================================*/
			MV_Armor_Text->StartPos = MV_Armor_Text->Position;
			MV_Armor_Text->Id = MI_ARMOR_TEXT;
			MV_Armor_Text->Side = LEFT_S;
			MV_Armor_Text->TextSize = 17;
			MV_Armor_Text->Offset = ImVec2(5, -10);
			MV_Armor_Text->IsText = true;
			MV_Armor_Text->Name = crypt_str("MV_Arm_Text");
			MV_Armor_Text->Color1[0] = 1.0f;
			MV_Armor_Text->Color1[1] = 1.0f;
			MV_Armor_Text->Color1[2] = 1.0f;

			/*=====================================================================================*/
			MV_Hp->StartPos = MV_Hp->Position;
			MV_Hp->Id = MI_HEALTH_BAR;
			MV_Hp->Side = LEFT_S;
			MV_Hp->Offset = ImVec2(5, 0);

			MV_Hp->Color1[0] = 0.1f;
			MV_Hp->Color1[1] = 1.0f;
			MV_Hp->Color1[2] = 0.1f;

			MV_Hp->Color2[0] = 1.0f;
			MV_Hp->Color2[1] = 0.0f;
			MV_Hp->Color2[2] = 0.0f;

			MV_Hp->Color3[0] = 0.0f;
			MV_Hp->Color3[1] = 0.0f;
			MV_Hp->Color3[2] = 0.0f;

			MV_Hp->NumParam1 = 4;
			MV_Hp->NumParam2 = 5;
			MV_Hp->Name = crypt_str("MV_HPBar");
			MV_Hp->boolParam1 = true;
			MV_Hp->boolParam2 = true;
			MV_Hp->boolParam3 = false;

			/*=====================================================================================*/
			MV_Armor->StartPos = MV_Armor->Position;
			MV_Armor->Id = MI_ARMOR_BAR;
			MV_Armor->Side = RIGHT_S;
			MV_Armor->Offset = ImVec2(-5, 0);

			MV_Armor->Color1[0] = 1.0f;
			MV_Armor->Color1[1] = 1.0f;
			MV_Armor->Color1[2] = 1.0f;

			MV_Armor->Color2[0] = 1.0f;
			MV_Armor->Color2[1] = 0.0f;
			MV_Armor->Color2[2] = 0.0f;

			MV_Armor->Color3[0] = 0.0f;
			MV_Armor->Color3[1] = 0.0f;
			MV_Armor->Color3[2] = 0.0f;

			MV_Armor->NumParam1 = 4;
			MV_Armor->NumParam2 = 5;
			MV_Armor->Name = crypt_str("MV_ArmorBar");
			MV_Armor->boolParam1 = true;
			MV_Armor->boolParam2 = true;
			MV_Armor->boolParam3 = false;

			/*=====================================================================================*/
			MV_Name->StartPos = MV_Name->Position;
			MV_Name->Id = MI_NAME_TEXT;
			MV_Name->Side = TOP_S;
			MV_Name->TextSize = 17;
			MV_Name->Offset = ImVec2(0, 4);
			MV_Name->IsText = true;
			MV_Name->Name = crypt_str("MV_Name");
			MV_Name->boolParam1 = false;
			MV_Name->Color2[0] = 0.09f;
			MV_Name->Color2[1] = 0.09f;
			MV_Name->Color2[2] = 0.09f;

			/*=====================================================================================*/
			MV_Weapon->StartPos = MV_Weapon->Position;
			MV_Weapon->Id = MI_WEAPON_TEXT;
			MV_Weapon->Side = DOWN_S;
			MV_Weapon->TextSize = 17;
			MV_Weapon->Offset = ImVec2(0, -1);
			MV_Weapon->IsText = true;
			MV_Weapon->Name = crypt_str("MV_Weapon");

			/*=====================================================================================*/
			MV_Ammo->StartPos = MV_Ammo->Position;
			MV_Ammo->Id = MI_AMMO_TEXT;
			MV_Ammo->Side = DOWN_S;
			MV_Ammo->TextSize = 17;
			MV_Ammo->Offset = ImVec2(0, -10);
			MV_Ammo->IsText = true;
			MV_Ammo->Name = crypt_str("MV_Ammo");
			MV_Ammo->Color1[0] = 1.0f;
			MV_Ammo->Color1[1] = 1.0f;
			MV_Ammo->Color1[2] = 1.0f;

			/*=====================================================================================*/
			MV_Distance->StartPos = MV_Distance->Position;
			MV_Distance->Id = MI_DISTANCE_TEXT;
			MV_Distance->Side = DOWN_RIGHT_DOWN_S;
			MV_Distance->TextSize = 17;
			MV_Distance->Offset = ImVec2(-7, 0);
			MV_Distance->IsText = true;
			MV_Distance->Name = crypt_str("MV_Distance");

			/*=====================================================================================*/
			MV_Money->StartPos = MV_Money->Position;
			MV_Money->Id = MI_MONEY_TEXT;
			MV_Money->Side = RIGHT_S;
			MV_Money->TextSize = 17;
			MV_Money->Color1[0] = 1.0f;
			MV_Money->Color1[1] = 1.0f;
			MV_Money->Color1[2] = 1.0f;
			MV_Money->Offset = ImVec2(-7, 0);
			MV_Money->IsText = true;
			MV_Money->Name = crypt_str("MV_Money");

			/*=====================================================================================*/
			MV_Scope->StartPos = MV_Scope->Position;
			MV_Scope->Id = MI_SCOPE_TEXT;
			MV_Scope->Side = RIGHT_S;
			MV_Scope->TextSize = 17;
			MV_Scope->Color1[0] = 1.0f;
			MV_Scope->Color1[1] = 1.0f;
			MV_Scope->Color1[2] = 1.0f;
			MV_Scope->Offset = ImVec2(-7, -7);
			MV_Scope->IsText = true;
			MV_Scope->Name = crypt_str("MV_Scope");

			/*=====================================================================================*/
			MV_Flashed->StartPos = MV_Flashed->Position;
			MV_Flashed->Id = MI_FLASHED_TEXT;
			MV_Flashed->Side = CENTER_RIGHT_S;
			MV_Flashed->TextSize = 17;
			MV_Flashed->Color1[0] = 1.0f;
			MV_Flashed->Color1[1] = 1.0f;
			MV_Flashed->Color1[2] = 1.0f;
			MV_Flashed->Offset = ImVec2(-7, 0);
			MV_Flashed->IsText = true;
			MV_Flashed->Name = crypt_str("MV_Flashed");

			/*=====================================================================================*/
			MV_Defusing->StartPos = MV_Defusing->Position;
			MV_Defusing->Id = MI_DEFUSING_TEXT;
			MV_Defusing->Side = CENTER_RIGHT_S;
			MV_Defusing->TextSize = 17;
			MV_Defusing->Color1[0] = 1.0f;
			MV_Defusing->Color1[1] = 1.0f;
			MV_Defusing->Color1[2] = 1.0f;
			MV_Defusing->Offset = ImVec2(-7, -7);
			MV_Defusing->IsText = true;
			MV_Defusing->Name = crypt_str("MV_Defusing");
		}
	};
	LInitVisuals();
}

void c_menu::VisualPreview()
{
	static MoveStruct* MV_Hp = &MVItemList[MI_HEALTH_BAR];
	static MoveStruct* MV_Armor = &MVItemList[MI_ARMOR_BAR];
	static MoveStruct* MV_Hp_Text = &MVItemList[MI_HEALTH_TEXT];
	static MoveStruct* MV_Armor_Text = &MVItemList[MI_ARMOR_TEXT];
	static MoveStruct* MV_Name = &MVItemList[MI_NAME_TEXT];
	static MoveStruct* MV_Weapon = &MVItemList[MI_WEAPON_TEXT];
	static MoveStruct* MV_Ammo = &MVItemList[MI_AMMO_TEXT];
	static MoveStruct* MV_Distance = &MVItemList[MI_DISTANCE_TEXT];
	static MoveStruct* MV_Money = &MVItemList[MI_MONEY_TEXT];
	static MoveStruct* MV_Scope = &MVItemList[MI_SCOPE_TEXT];
	static MoveStruct* MV_Flashed = &MVItemList[MI_FLASHED_TEXT];
	static MoveStruct* MV_Defusing = &MVItemList[MI_DEFUSING_TEXT];

	auto cur_window = ImGui::GetCurrentWindow();
	ImVec2 ZoneStartPos = ImGui::GetWindowPos();
	ImGui::SetCursorPos(ZoneStartPos);
	CustomVisuals(ZoneStartPos);
	ImGui::BeginChild("ttt", ImVec2(250 * dpi_scale, 550 * dpi_scale));
	{
		if ((MITypes)MoveStruct::LastActive == MI_AMMO_TEXT)
		{
			ImGui::Text("Selected: Ammo");

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	VectorEx<const char*>itemsGS = { lolc("Type 1") ,lolc("Type 2"), lolc("Type 3") };
			//	DComboBox("Style", AmmoStyle, itemsGS);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	SliderInts("Text size", TextAmmoSize, 9, MAX_FONT_SIZE);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DColorEdit("Color", AmmoColor);
		}
		else if ((MITypes)MoveStruct::LastActive == MI_ARMOR_TEXT)
		{
			ImGui::Text("Selected: Armor");

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	int OldStyle = ArmorStyle;

				//VectorEx<const char*>NumStyle = { lolc("Line") , lolc("Text") };

			//	DComboBox("Style", ArmorStyle, NumStyle);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	SliderInts("Text size", TextArmorSize, 9, MAX_FONT_SIZE);

			//	if (OldStyle != ArmorStyle)
			MoveStruct::LastActive = MI_ARMOR_BAR;

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DColorEdit("Color", ArmorColor);
		}
		else if ((MITypes)MoveStruct::LastActive == MI_ARMOR_BAR)
		{
			ImGui::Text("Selected: Armor");

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	int OldStyle = ArmorStyle;

			//	VectorEx<const char*>NumStyle = { lolc("Line") , lolc("Text") };

			//	DComboBox("Style", ArmorStyle, NumStyle);


			//	if (OldStyle != ArmorStyle)
			MoveStruct::LastActive = MI_ARMOR_TEXT;

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	DColorEdit("Main color", ArmorColor);
			//	DColorEdit("Background color", ArmorColor2);
			//	DColorEdit("Outline color", ArmorColor3);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	SliderInts("Separators", MV_Armor->NumParam1, 0, 10);
			//	SliderInts("Tickness", MV_Armor->NumParam2, 0, 20);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//DCheckBox("Background", MV_Armor->boolParam1);
			ImGui::SameLine();
			//	DCheckBox("Outline", MV_Armor->boolParam2);
			ImGui::SameLine();
			//	DCheckBox("Inverse", MV_Armor->boolParam3);
		}
		else if ((MITypes)MoveStruct::LastActive == MI_DISTANCE_TEXT)
		{
			ImGui::Text("Selected: Selected");
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//SliderInts("Text size", TextDistanceSize, 9, MAX_FONT_SIZE);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DColorEdit("Color", DistanceColor);
		}
		else if ((MITypes)MoveStruct::LastActive == MI_HEALTH_TEXT)
		{
			ImGui::Text("Selected: Health");

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	VectorEx<const char*>NumStyle = { lolc("Line"), lolc("Text") };

			//	int OldStyle = HealthStyle;

			//	DComboBox("Style", HealthStyle, NumStyle);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	SliderInts("Text size", TextHealthSize, 9, MAX_FONT_SIZE);

			//	if (OldStyle != HealthStyle)
			MoveStruct::LastActive = MI_HEALTH_BAR;

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DColorEdit("Color", HealthColor);
		}
		else if ((MITypes)MoveStruct::LastActive == MI_HEALTH_BAR)
		{
			ImGui::Text("Selected: Health");

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	VectorEx<const char*>NumStyle = { lolc("Line"), lolc("Text") };

			//	int OldStyle = HealthStyle;

			//	DComboBox("Style", HealthStyle, NumStyle);

				//if (OldStyle != HealthStyle)
			MoveStruct::LastActive = MI_HEALTH_TEXT;

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	DColorEdit("Main Color", HealthColor);
			//	DColorEdit("Background Color", HealthColor2);
			//	DColorEdit("Outline Color", HealthColor3);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::SliderInt("Separators", &MV_Hp->NumParam1, 0, 10);
			ImGui::SliderInt("Tickness", &MV_Hp->NumParam2, 0, 20);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DCheckBox("Background", MV_Hp->boolParam1);
			ImGui::SameLine();
			//	DCheckBox("Outline", MV_Hp->boolParam2);
			ImGui::SameLine();
			//	DCheckBox("Inverse", MV_Hp->boolParam3);
		}
		else if ((MITypes)MoveStruct::LastActive == MI_NAME_TEXT)
		{
			ImGui::Text("Selected: Name");

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	SliderInts("Text size", TextNameSize, 9, MAX_FONT_SIZE);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DColorEdit("Color", NameColor);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	DCheckBox("Background", NameBackground);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	if (NameBackground)
			//	{
			ImGui::Spacing();
			//	DColorEdit("Background color", NameColorBack);
			//}
		}
		else if ((MITypes)MoveStruct::LastActive == MI_WEAPON_TEXT)
		{
			ImGui::Text("Selected: Weapon");
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	VectorEx<const char*>itemsGS = { lolc("Text"), lolc("Icon") };
			//	DComboBox("Style", WeaponStyle, itemsGS);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	SliderInts("Text size", TextWeaponSize, 9, MAX_FONT_SIZE);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DColorEdit("Color", WeaponColor);
		}
		else if ((MITypes)MoveStruct::LastActive == MI_MONEY_TEXT)
		{
			ImGui::Text("Selected: Money");
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//		SliderInts("Text size", TextMoneySize, 9, MAX_FONT_SIZE);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DColorEdit("Color", MoneyColor);
		}
		else if ((MITypes)MoveStruct::LastActive == MI_SCOPE_TEXT)
		{
			ImGui::Text("Selected: Scoped");
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//		SliderInts("Text size", TextScopedSize, 9, MAX_FONT_SIZE);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DColorEdit("Color", ScopeColor);
		}
		else if ((MITypes)MoveStruct::LastActive == MI_FLASHED_TEXT)
		{
			ImGui::Text("Selected: Flashed");
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	SliderInts("Text size", TextFlashedSize, 9, MAX_FONT_SIZE);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//		DColorEdit("Color", FlashingColor);

		}
		else if ((MITypes)MoveStruct::LastActive == MI_DEFUSING_TEXT)
		{
			ImGui::Text("Selected: Defusing");
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//	SliderInts("Text size", TextDefusingSize, 9, MAX_FONT_SIZE);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			//	DColorEdit("Color", DefusingColor);
		}

		//	float long_item_w = ImGui::GetThis()->Info.Size.x - (ImGui::GetStyle().wndPadding.x * 2);
		ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().WindowPadding.x, 638));
		if (ImGui::Button(crypt_str("Reset"), ImVec2(100, 22)))
			MoveStruct::InitMItems = false;
	}
	ImGui::EndChild();

}


std::string zGetTimeString()
{
	static time_t current_time;
	struct tm* time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}

int zgetfps()
{
	return static_cast<int>(1.f / m_globals()->m_frametime);
}
int zgettickrate()
{
	return static_cast<int>(1.f / m_globals()->m_intervalpertick);
}


void CopyConvert(const uint8_t* rgba, uint8_t* out, const size_t size)
{
	auto in = reinterpret_cast<const uint32_t*>(rgba);
	auto buf = reinterpret_cast<uint32_t*>(out);
	for (auto i = 0u; i < (size / 4); ++i)
	{
		const auto pixel = *in++;
		*buf++ = (pixel & 0xFF00FF00) | ((pixel & 0xFF0000) >> 16) | ((pixel & 0xFF) << 16);
	}
}

LPDIRECT3DTEXTURE9 c_menu::steam_image()
{
	LPDIRECT3DTEXTURE9 asdgsdgadsg;

	int iImage = SteamFriends->GetLargeFriendAvatar(SteamUser->GetSteamID());
	if (iImage == -1)
		return nullptr;
	uint32 uAvatarWidth, uAvatarHeight;
	if (!SteamUtils->GetImageSize(iImage, &uAvatarWidth, &uAvatarHeight))
		return nullptr;
	const int uImageSizeInBytes = uAvatarWidth * uAvatarHeight * 4;
	uint8* pAvatarRGBA = new uint8[uImageSizeInBytes];
	if (!SteamUtils->GetImageRGBA(iImage, pAvatarRGBA, uImageSizeInBytes))
	{
		delete[] pAvatarRGBA;
		return nullptr;
	}
	auto res = c_menu::get().device->CreateTexture(uAvatarWidth,
		uAvatarHeight,
		1,
		D3DUSAGE_DYNAMIC,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&asdgsdgadsg,
		nullptr);
	std::vector<uint8_t> texData;
	texData.resize(uAvatarWidth * uAvatarHeight * 4u);
	CopyConvert(pAvatarRGBA,
		texData.data(),
		uAvatarWidth * uAvatarHeight * 4u);
	D3DLOCKED_RECT rect;
	res = asdgsdgadsg->LockRect(0, &rect, nullptr, D3DLOCK_DISCARD);
	auto src = texData.data();
	auto dst = reinterpret_cast<uint8_t*>(rect.pBits);
	for (auto y = 0u; y < uAvatarHeight; ++y)
	{
		std::copy(src, src + (uAvatarWidth * 4), dst);

		src += uAvatarWidth * 4;
		dst += rect.Pitch;
	}
	res = asdgsdgadsg->UnlockRect(0);
	delete[] pAvatarRGBA;
	return asdgsdgadsg;
}

LPDIRECT3DTEXTURE9 c_menu::steam_image_friends(CSteamID SteamId)
{
	LPDIRECT3DTEXTURE9 asdgsdgadsg;

	int iImage = SteamFriends->GetLargeFriendAvatar(SteamId);
	if (iImage == -1)
		return nullptr;
	uint32 uAvatarWidth, uAvatarHeight;
	if (!SteamUtils->GetImageSize(iImage, &uAvatarWidth, &uAvatarHeight))
		return nullptr;
	const int uImageSizeInBytes = uAvatarWidth * uAvatarHeight * 4;
	uint8* pAvatarRGBA = new uint8[uImageSizeInBytes];
	if (!SteamUtils->GetImageRGBA(iImage, pAvatarRGBA, uImageSizeInBytes))
	{
		delete[] pAvatarRGBA;
		return nullptr;
	}
	auto res = c_menu::get().device->CreateTexture(uAvatarWidth,
		uAvatarHeight,
		1,
		D3DUSAGE_DYNAMIC,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&asdgsdgadsg,
		nullptr);
	std::vector<uint8_t> texData;
	texData.resize(uAvatarWidth * uAvatarHeight * 4u);
	CopyConvert(pAvatarRGBA,
		texData.data(),
		uAvatarWidth * uAvatarHeight * 4u);
	D3DLOCKED_RECT rect;
	res = asdgsdgadsg->LockRect(0, &rect, nullptr, D3DLOCK_DISCARD);
	auto src = texData.data();
	auto dst = reinterpret_cast<uint8_t*>(rect.pBits);
	for (auto y = 0u; y < uAvatarHeight; ++y)
	{
		std::copy(src, src + (uAvatarWidth * 4), dst);

		src += uAvatarWidth * 4;
		dst += rect.Pitch;
	}
	res = asdgsdgadsg->UnlockRect(0);
	delete[] pAvatarRGBA;
	return asdgsdgadsg;
}


std::string get_wep(int id, int custom_index = -1, bool knife = true)
{
	if (custom_index > -1)
	{
		if (knife)
		{
			switch (custom_index)
			{
			case 0: return crypt_str("weapon_knife");
			case 1: return crypt_str("weapon_bayonet");
			case 2: return crypt_str("weapon_knife_css");
			case 3: return crypt_str("weapon_knife_skeleton");
			case 4: return crypt_str("weapon_knife_outdoor");
			case 5: return crypt_str("weapon_knife_cord");
			case 6: return crypt_str("weapon_knife_canis");
			case 7: return crypt_str("weapon_knife_flip");
			case 8: return crypt_str("weapon_knife_gut");
			case 9: return crypt_str("weapon_knife_karambit");
			case 10: return crypt_str("weapon_knife_m9_bayonet");
			case 11: return crypt_str("weapon_knife_tactical");
			case 12: return crypt_str("weapon_knife_falchion");
			case 13: return crypt_str("weapon_knife_survival_bowie");
			case 14: return crypt_str("weapon_knife_butterfly");
			case 15: return crypt_str("weapon_knife_push");
			case 16: return crypt_str("weapon_knife_ursus");
			case 17: return crypt_str("weapon_knife_gypsy_jackknife");
			case 18: return crypt_str("weapon_knife_stiletto");
			case 19: return crypt_str("weapon_knife_widowmaker");
			}
		}
		else
		{
			switch (custom_index)
			{
			case 0: return crypt_str("ct_gloves"); //-V1037
			case 1: return crypt_str("studded_bloodhound_gloves");
			case 2: return crypt_str("t_gloves");
			case 3: return crypt_str("ct_gloves");
			case 4: return crypt_str("sporty_gloves");
			case 5: return crypt_str("slick_gloves");
			case 6: return crypt_str("leather_handwraps");
			case 7: return crypt_str("motorcycle_gloves");
			case 8: return crypt_str("specialist_gloves");
			case 9: return crypt_str("studded_hydra_gloves");
			}
		}
	}
	else
	{
		switch (id)
		{
		case 0: return crypt_str("knife");
		case 1: return crypt_str("gloves");
		case 2: return crypt_str("weapon_ak47");
		case 3: return crypt_str("weapon_aug");
		case 4: return crypt_str("weapon_awp");
		case 5: return crypt_str("weapon_cz75a");
		case 6: return crypt_str("weapon_deagle");
		case 7: return crypt_str("weapon_elite");
		case 8: return crypt_str("weapon_famas");
		case 9: return crypt_str("weapon_fiveseven");
		case 10: return crypt_str("weapon_g3sg1");
		case 11: return crypt_str("weapon_galilar");
		case 12: return crypt_str("weapon_glock");
		case 13: return crypt_str("weapon_m249");
		case 14: return crypt_str("weapon_m4a1_silencer");
		case 15: return crypt_str("weapon_m4a1");
		case 16: return crypt_str("weapon_mac10");
		case 17: return crypt_str("weapon_mag7");
		case 18: return crypt_str("weapon_mp5sd");
		case 19: return crypt_str("weapon_mp7");
		case 20: return crypt_str("weapon_mp9");
		case 21: return crypt_str("weapon_negev");
		case 22: return crypt_str("weapon_nova");
		case 23: return crypt_str("weapon_hkp2000");
		case 24: return crypt_str("weapon_p250");
		case 25: return crypt_str("weapon_p90");
		case 26: return crypt_str("weapon_bizon");
		case 27: return crypt_str("weapon_revolver");
		case 28: return crypt_str("weapon_sawedoff");
		case 29: return crypt_str("weapon_scar20");
		case 30: return crypt_str("weapon_ssg08");
		case 31: return crypt_str("weapon_sg556");
		case 32: return crypt_str("weapon_tec9");
		case 33: return crypt_str("weapon_ump45");
		case 34: return crypt_str("weapon_usp_silencer");
		case 35: return crypt_str("weapon_xm1014");
		default: return crypt_str("unknown");
		}
	}
}

IDirect3DTexture9* get_skin_preview(const char* weapon_name, const std::string& skin_name, IDirect3DDevice9* device)
{
	IDirect3DTexture9* skin_image = nullptr;
	std::string vpk_path;

	if (strcmp(weapon_name, crypt_str("unknown")) && strcmp(weapon_name, crypt_str("knife")) && strcmp(weapon_name, crypt_str("gloves"))) //-V526
	{
		if (skin_name.empty() || skin_name == crypt_str("default"))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");
		else
			vpk_path = crypt_str("resource/flash/econ/default_generated/") + std::string(weapon_name) + crypt_str("_") + std::string(skin_name) + crypt_str("_light_large.png");
	}
	else
	{
		if (!strcmp(weapon_name, crypt_str("knife")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_knife.png");
		else if (!strcmp(weapon_name, crypt_str("gloves")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else if (!strcmp(weapon_name, crypt_str("unknown")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_snowball.png");

	}
	const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));
	if (handle)
	{
		int file_len = m_basefilesys()->Size(handle);
		char* image = new char[file_len]; //-V121

		m_basefilesys()->Read(image, file_len, handle);
		m_basefilesys()->Close(handle);

		D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
		delete[] image;
	}

	if (!skin_image)
	{
		std::string vpk_path;

		if (strstr(weapon_name, crypt_str("bloodhound")) != NULL || strstr(weapon_name, crypt_str("hydra")) != NULL)
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");

		const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));

		if (handle)
		{
			int file_len = m_basefilesys()->Size(handle);
			char* image = new char[file_len]; //-V121

			m_basefilesys()->Read(image, file_len, handle);
			m_basefilesys()->Close(handle);

			D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
			delete[] image;
		}
	}

	return skin_image;
}

// setup some styles and colors, window size and bg alpha
// dpi setup
void c_menu::menu_setup(ImGuiStyle& style) //-V688
{
	ImGui::StyleColorsClassic(); // colors setup
	ImGui::SetNextWindowSize(ImVec2(715, 650), ImGuiCond_Once); // window pos setup
	ImGui::SetNextWindowBgAlpha(0.1f); // window bg alpha setup

	styles.WindowPadding = style.WindowPadding;
	styles.WindowRounding = style.WindowRounding;
	styles.WindowMinSize = style.WindowMinSize;
	styles.ChildRounding = style.ChildRounding;
	styles.PopupRounding = style.PopupRounding;
	styles.FramePadding = style.FramePadding;
	styles.FrameRounding = style.FrameRounding;
	styles.ItemSpacing = style.ItemSpacing;
	styles.ItemInnerSpacing = style.ItemInnerSpacing;
	styles.TouchExtraPadding = style.TouchExtraPadding;
	styles.IndentSpacing = style.IndentSpacing;
	styles.ColumnsMinSpacing = style.ColumnsMinSpacing;
	styles.ScrollbarSize = style.ScrollbarSize;
	styles.ScrollbarRounding = style.ScrollbarRounding;
	styles.GrabMinSize = style.GrabMinSize;
	styles.GrabRounding = style.GrabRounding;
	styles.TabRounding = style.TabRounding;
	styles.TabMinWidthForUnselectedCloseButton = style.TabMinWidthForUnselectedCloseButton;
	styles.DisplayWindowPadding = style.DisplayWindowPadding;
	styles.DisplaySafeAreaPadding = style.DisplaySafeAreaPadding;
	styles.MouseCursorScale = style.MouseCursorScale;


	// setup skins preview
	for (auto i = 0; i < config_system.g_cfg.skins.skinChanger.size(); i++)
		if (!all_skins[i])
			all_skins[i] = get_skin_preview(get_wep(i, (i == 0 || i == 1) ? config_system.g_cfg.skins.skinChanger.at(i).definition_override_vector_index : -1, i == 0).c_str(), config_system.g_cfg.skins.skinChanger.at(i).skin_name, device); //-V810

	menu_setupped = true; // we dont want to setup menu again
}

// resize current style sizes
void c_menu::dpi_resize(float scale_factor, ImGuiStyle& style) //-V688
{
	style.WindowPadding = (styles.WindowPadding * scale_factor);
	style.WindowRounding = (styles.WindowRounding * scale_factor);
	style.WindowMinSize = (styles.WindowMinSize * scale_factor);
	style.ChildRounding = (styles.ChildRounding * scale_factor);
	style.PopupRounding = (styles.PopupRounding * scale_factor);
	style.FramePadding = (styles.FramePadding * scale_factor);
	style.FrameRounding = (styles.FrameRounding * scale_factor);
	style.ItemSpacing = (styles.ItemSpacing * scale_factor);
	style.ItemInnerSpacing = (styles.ItemInnerSpacing * scale_factor);
	style.TouchExtraPadding = (styles.TouchExtraPadding * scale_factor);
	style.IndentSpacing = (styles.IndentSpacing * scale_factor);
	style.ColumnsMinSpacing = (styles.ColumnsMinSpacing * scale_factor);
	style.ScrollbarSize = (styles.ScrollbarSize * scale_factor);
	style.ScrollbarRounding = (styles.ScrollbarRounding * scale_factor);
	style.GrabMinSize = (styles.GrabMinSize * scale_factor);
	style.GrabRounding = (styles.GrabRounding * scale_factor);
	style.TabRounding = (styles.TabRounding * scale_factor);
	if (styles.TabMinWidthForUnselectedCloseButton != FLT_MAX) //-V550
		style.TabMinWidthForUnselectedCloseButton = (styles.TabMinWidthForUnselectedCloseButton * scale_factor);
	style.DisplayWindowPadding = (styles.DisplayWindowPadding * scale_factor);
	style.DisplaySafeAreaPadding = (styles.DisplaySafeAreaPadding * scale_factor);
	style.MouseCursorScale = (styles.MouseCursorScale * scale_factor);
}


std::string get_config_dir()
{
	std::string folder;
	static TCHAR path[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPath(NULL, 0x001a, NULL, NULL, path)))
		folder = std::string(path) + crypt_str("\\Qwerty.xyz\\Configs\\");

	CreateDirectory(folder.c_str(), NULL);
	return folder;
}

void load_config()
{
	if (cfg_manager->files.empty())
		return;

	cfg_manager->load(cfg_manager->files.at(config_system.g_cfg.selected_config), false);
	c_lua::get().unload_all_scripts();

	for (auto& script : config_system.g_cfg.scripts.scripts)
		c_lua::get().load_script(c_lua::get().get_script_id(script));

	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1; //-V103

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}

	for (auto i = 0; i < config_system.g_cfg.skins.skinChanger.size(); ++i)
		all_skins[i] = nullptr;

	config_system.g_cfg.scripts.scripts.clear();

	cfg_manager->load(cfg_manager->files.at(config_system.g_cfg.selected_config), true);
	cfg_manager->config_files();

	eventlogs::get().addnew(crypt_str("Loaded ") + files.at(config_system.g_cfg.selected_config) + crypt_str(" config"), Color::Yellow, false);
}

void save_config()
{
	if (cfg_manager->files.empty())
		return;

	config_system.g_cfg.scripts.scripts.clear();

	for (auto i = 0; i < c_lua::get().scripts.size(); ++i)
	{
		auto script = c_lua::get().scripts.at(i);

		if (c_lua::get().loaded.at(i))
			config_system.g_cfg.scripts.scripts.emplace_back(script);
	}

	cfg_manager->save(cfg_manager->files.at(config_system.g_cfg.selected_config));
	cfg_manager->config_files();

	eventlogs::get().addnew(crypt_str("Saved ") + files.at(config_system.g_cfg.selected_config) + crypt_str(" config"), Color::Yellow, false);
}

void remove_config()
{
	if (cfg_manager->files.empty())
		return;

	eventlogs::get().addnew(crypt_str("Removed ") + files.at(config_system.g_cfg.selected_config) + crypt_str(" config"), Color::Yellow, false);

	cfg_manager->remove(cfg_manager->files.at(config_system.g_cfg.selected_config));
	cfg_manager->config_files();

	files = cfg_manager->files;

	if (config_system.g_cfg.selected_config >= files.size())
		config_system.g_cfg.selected_config = files.size() - 1; //-V103

	for (auto& current : files)
		if (current.size() > 2)
			current.erase(current.size() - 3, 3);
}

void add_config()
{
	auto empty = true;

	for (auto current : config_system.g_cfg.new_config_name)
	{
		if (current != ' ')
		{
			empty = false;
			break;
		}
	}

	if (empty)
		config_system.g_cfg.new_config_name = crypt_str("config");

	eventlogs::get().addnew(crypt_str("Added ") + config_system.g_cfg.new_config_name + crypt_str(" config"), Color::Yellow, false);

	if (config_system.g_cfg.new_config_name.find(crypt_str(".qwerty")) == std::string::npos)
		config_system.g_cfg.new_config_name += crypt_str(".qwerty");

	cfg_manager->save(config_system.g_cfg.new_config_name);
	cfg_manager->config_files();

	config_system.g_cfg.selected_config = cfg_manager->files.size() - 1; //-V103
	files = cfg_manager->files;

	for (auto& current : files)
		if (current.size() > 2)
			current.erase(current.size() - 3, 3);
}

__forceinline void padding(float x, float y)
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x * c_menu::get().dpi_scale);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y * c_menu::get().dpi_scale);
}


// title of content child
void child_title(const char* label)
{
	ImGui::PushFont(c_menu::get().futura_large);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (10 * c_menu::get().dpi_scale));
	ImGui::Text(label);

	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void draw_combo(const char* name, int& variable, const char* labels[], int count)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6 * c_menu::get().dpi_scale);
	ImGui::Text(name);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
	ImGui::Combo(std::string(crypt_str("##COMBO__") + std::string(name)).c_str(), &variable, labels, count);
}

void draw_combo(const char* name, int& variable, bool (*items_getter)(void*, int, const char**), void* data, int count)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6 * c_menu::get().dpi_scale);
	ImGui::Text(name);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
	ImGui::Combo(std::string(crypt_str("##COMBO__") + std::string(name)).c_str(), &variable, items_getter, data, count);
}

void draw_multicombo(std::string name, std::vector<int>& variable, const char* labels[], int count, std::string& preview)
{
	padding(-3, -6);
	ImGui::Text((crypt_str(" ") + name).c_str());
	padding(0, -5);

	auto hashname = crypt_str("##") + name; // we dont want to render name of combo

	for (auto i = 0, j = 0; i < count; i++)
	{
		if (variable[i])
		{
			if (j)
				preview += crypt_str(", ") + (std::string)labels[i];
			else
				preview = labels[i];

			j++;
		}
	}

	if (ImGui::BeginCombo(hashname.c_str(), preview.c_str())) // draw start
	{
		ImGui::Spacing();
		ImGui::BeginGroup();
		{

			for (auto i = 0; i < count; i++)
				ImGui::Selectable(labels[i], (bool*)&variable[i], ImGuiSelectableFlags_DontClosePopups);

		}
		ImGui::EndGroup();
		ImGui::Spacing();

		ImGui::EndCombo();
	}

	preview = crypt_str("None"); // reset preview to use later
}

bool LabelClick(const char* label, bool* v, const char* unique_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
	char Buf[64];
	_snprintf(Buf, 62, crypt_str("%s"), label);

	char getid[128];
	sprintf_s(getid, 128, crypt_str("%s%s"), label, unique_id);


	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(getid);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), ImVec2(window->DC.CursorPos.x + label_size.x, window->DC.CursorPos.y + style.FramePadding.y + label_size.y));

		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
		*v = !(*v);

	if (*v)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 143.f, 255.f, 1.f));
	if (label_size.x > 0.0f)
		ImGui::RenderText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), Buf);
	if (*v)
		ImGui::PopStyleColor();

	return pressed;

}


void draw_keybind(const char* label, key_bind* key_bind, const char* unique_id)
{
	// reset bind if we re pressing esc
	if (key_bind->key == KEY_ESCAPE)
		key_bind->key = KEY_NONE;

	auto clicked = false;
	auto text = (std::string)m_inputsys()->ButtonCodeToString(key_bind->key);

	if (key_bind->key <= KEY_NONE || key_bind->key >= KEY_MAX)
		text = crypt_str("None");

	// if we clicked on keybind
	if (hooks::input_shouldListen && hooks::input_receivedKeyval == &key_bind->key)
	{
		clicked = true;
		text = crypt_str("...");
	}

	auto textsize = ImGui::CalcTextSize(text.c_str()).x + 8 * c_menu::get().dpi_scale;
	auto labelsize = ImGui::CalcTextSize(label);

	ImGui::Text(label);
	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - (ImGui::GetWindowSize().x - ImGui::CalcItemWidth()) - max(50 * c_menu::get().dpi_scale, textsize));
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3 * c_menu::get().dpi_scale);

	if (ImGui::KeybindButton(text.c_str(), unique_id, ImVec2(max(50 * c_menu::get().dpi_scale, textsize), 23 * c_menu::get().dpi_scale), clicked))
		clicked = true;


	if (clicked)
	{
		hooks::input_shouldListen = true;
		hooks::input_receivedKeyval = &key_bind->key;
	}

	static auto hold = false, toggle = false;

	switch (key_bind->mode)
	{
	case HOLD:
		hold = true;
		toggle = false;
		break;
	case TOGGLE:
		toggle = true;
		hold = false;
		break;
	}

	if (ImGui::BeginPopup(unique_id))
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6 * c_menu::get().dpi_scale);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize(crypt_str("Hold")).x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);

		if (LabelClick(crypt_str("Hold"), &hold, unique_id))
		{
			if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else
			{
				toggle = false;
				key_bind->mode = HOLD;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize(crypt_str("Toggle")).x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9 * c_menu::get().dpi_scale);

		if (LabelClick(crypt_str("Toggle"), &toggle, unique_id))
		{
			if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void draw_semitabs(const char* labels[], int count, int& tab, ImGuiStyle& style)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (2 * c_menu::get().dpi_scale));

	// center of main child
	float offset = 343 * c_menu::get().dpi_scale;

	// text size padding + frame padding
	for (int i = 0; i < count; i++)
		offset -= (ImGui::CalcTextSize(labels[i]).x) / 2 + style.FramePadding.x * 2;

	// set new padding
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	ImGui::BeginGroup();

	for (int i = 0; i < count; i++)
	{
		// switch current tab
		if (ImGui::ContentTab(labels[i], tab == i))
			tab = i;

		// continue drawing on same line 
		if (i + 1 != count)
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + style.ItemSpacing.x);
		}
	}

	ImGui::EndGroup();
}

__forceinline void tab_start()
{
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + (20 * c_menu::get().dpi_scale), ImGui::GetCursorPosY() + (5 * c_menu::get().dpi_scale)));
}

__forceinline void tab_end()
{
	ImGui::PopStyleVar();
	ImGui::SetWindowFontScale(c_menu::get().dpi_scale);
}

void lua_edit(std::string window_name)
{
	std::string file_path;

	auto get_dir = [&]() -> void
	{
		static TCHAR path[MAX_PATH];

		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
			file_path = std::string(path) + crypt_str("\\Qwerty.xyz\\Scripts\\");

		CreateDirectory(file_path.c_str(), NULL);
		file_path += window_name + crypt_str(".lua");
	};

	get_dir();
	const char* child_name = (window_name + window_name).c_str();

	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_Once);
	ImGui::Begin(window_name.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 5.f);

	static TextEditor editor;

	if (!loaded_editing_script)
	{
		static auto lang = TextEditor::LanguageDefinition::Lua();

		editor.SetLanguageDefinition(lang);
		editor.SetReadOnly(false);

		std::ifstream t(file_path);

		if (t.good()) // does while exist?
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			editor.SetText(str); // setup script content
		}

		loaded_editing_script = true;
	}

	// dpi scale for font
	// we dont need to resize it for full scale
	ImGui::SetWindowFontScale(1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f));

	// new size depending on dpi scale
	ImGui::SetWindowSize(ImVec2(ImFloor(800 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f))), ImFloor(700 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f)))));
	editor.Render(child_name, ImGui::GetWindowSize() - ImVec2(0, 66 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f))));

	// seperate code with buttons
	ImGui::Separator();

	// set cursor pos to right edge of window
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowSize().x - (16.f * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f))) - (250.f * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f))));
	ImGui::BeginGroup();

	if (ImGui::CustomButton(crypt_str("Save"), (crypt_str("Save") + window_name).c_str(), ImVec2(125 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f)), 0), true, c_menu::get().ico_bottom, crypt_str("S")))
	{
		std::ofstream out;

		out.open(file_path);
		out << editor.GetText() << std::endl;
		out.close();
	}

	ImGui::SameLine();

	// TOOD: close button will close window (return in start of function)
	if (ImGui::CustomButton(crypt_str("Close"), (crypt_str("Close") + window_name).c_str(), ImVec2(125 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f)), 0)))
	{
		g_ctx.globals.focused_on_input = false;
		loaded_editing_script = false;
		editing_script.clear();
	}

	ImGui::EndGroup();

	ImGui::PopStyleVar();
	ImGui::End();
}

void c_menu::CustomVisuals(ImVec2 Start)
{
	auto player = c_menu::get().players_section;
	auto Draw = ImGui::GetWindowDrawList();
	//CGui* Draw = &X1Gui();
//	ImDrawCmd* Draw = ImGui::GetWindowDrawList();
	ImGuiIO& g = ImGui::GetIO();

	std::string MV_AMMO_TEXT = "";

	/*if (AmmoStyle == 0)
	{
		MV_AMMO_TEXT = crypt_str("[30]");
	}
	else if (AmmoStyle == 1)
	{
		MV_AMMO_TEXT = crypt_str("[30\\90]");
	}
	else if (AmmoStyle == 2)
	{
		MV_AMMO_TEXT = crypt_str("[120]");
	}*/
	static MoveStruct* MV_Hp = &MVItemList[MI_HEALTH_BAR];
	static MoveStruct* MV_Armor = &MVItemList[MI_ARMOR_BAR];

	static MoveStruct* MV_Hp_Text = &MVItemList[MI_HEALTH_TEXT];
	static MoveStruct* MV_Armor_Text = &MVItemList[MI_ARMOR_TEXT];

	static MoveStruct* MV_Name = &MVItemList[MI_NAME_TEXT];
	static MoveStruct* MV_Weapon = &MVItemList[MI_WEAPON_TEXT];
	static MoveStruct* MV_Ammo = &MVItemList[MI_AMMO_TEXT];
	static MoveStruct* MV_Distance = &MVItemList[MI_DISTANCE_TEXT];
	static MoveStruct* MV_Money = &MVItemList[MI_MONEY_TEXT];
	static MoveStruct* MV_Scope = &MVItemList[MI_SCOPE_TEXT];
	static MoveStruct* MV_Flashed = &MVItemList[MI_FLASHED_TEXT];
	static MoveStruct* MV_Defusing = &MVItemList[MI_DEFUSING_TEXT];


	MV_Name->Size = ImGui::CalcTextSize(MV_NAME_TEXT, NULL, true);
	MV_Weapon->Size = ImGui::CalcTextSize(MV_WEAPON_TEXT, NULL, true);
	/*MV_Hp_Text->Size = GP_Render->CalcTextSize(MV_HEALTH_TEXT, GP_Render->SzFonts[TextHealthSize]);
	MV_Armor_Text->Size = GP_Render->CalcTextSize(MV_ARMOR_TEXT, GP_Render->SzFonts[TextArmorSize]);
	MV_Name->Size = GP_Render->CalcTextSize(MV_NAME_TEXT, GP_Render->SzFonts[TextNameSize]);
	MV_Ammo->Size = GP_Render->CalcTextSize(MV_AMMO_TEXT.c_str(), GP_Render->SzFonts[TextAmmoSize]);
	MV_Distance->Size = GP_Render->CalcTextSize(MV_DISTANCE_TEXT, GP_Render->SzFonts[TextDistanceSize]);
	MV_Money->Size = GP_Render->CalcTextSize(MV_MONEY_TEXT, GP_Render->SzFonts[TextMoneySize]);
	MV_Flashed->Size = GP_Render->CalcTextSize(MV_FLASHED_TEXT, GP_Render->SzFonts[TextFlashedSize]);
	MV_Scope->Size = GP_Render->CalcTextSize(MV_SCOPE_TEXT, GP_Render->SzFonts[TextScopedSize]);
	MV_Defusing->Size = GP_Render->CalcTextSize(MV_DEFUSING_TEXT, GP_Render->SzFonts[TextDefusingSize]);*/

	//	if (WeaponStyle == 0)
		//	MV_Weapon->Size = GP_Render->CalcTextSize(MV_WEAPON_TEXT, GP_Render->SzFonts[TextWeaponSize]);
	//	else if (WeaponStyle == 1)
		//	MV_Weapon->Size = GP_Render->CalcTextSize(MV_WEAPON_TEXT_ICO, GP_Render->SzFontsIcon[TextWeaponSize]);

	MV_Hp->IsEnabled = config_system.g_cfg.player.type[player].health;
	//	MV_Armor->IsEnabled = Armor && ArmorStyle == 0;

		////MV_Hp_Text->IsEnabled = Health && HealthStyle == 1;
		//MV_Armor_Text->IsEnabled = Armor && ArmorStyle == 1;

	MV_Name->IsEnabled = config_system.g_cfg.player.type[player].name;
	MV_Weapon->IsEnabled = &config_system.g_cfg.player.type[player].weapon;
	//MV_Ammo->IsEnabled = Ammo;
//	MV_Distance->IsEnabled = Distance;
//	MV_Money->IsEnabled = Money;
//	MV_Scope->IsEnabled = Scope;
//	MV_Flashed->IsEnabled = Flashing;
//	MV_Defusing->IsEnabled = Defusing;

	//ImU32 LinesColor = Color(ImVec4(1.f * 255.f, 1.f * 255.f, 1.f * 255.f, 0.1f * 255.f));
	//ImU32 OutLineColor = ImVec4(1.f * 255.f, 1.f * 255.f, 1.f * 255.f, 0.3f * 255.f);
	//ImU32 BackColor = ImVec4(0.5f * 255.f, 0.5f * 255.f, 0.5f * 255.f, 0.5f * 255.f);

	ImU32 LinesColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
	ImU32 OutLineColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.3f));
	ImU32 BackColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

	//auto LinesColor = Color(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.1f / 255.f);
	//auto OutLineColor = Color(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.3f / 255.f);
	//auto BackColor = Color(255/ 255.f, 255 / 255.f, 255 / 255.f, 0.5f / 255.f);

	IsMousPress = g.MouseDownDuration[0] > 0.0f;
	IsMousClick = g.MouseClicked[0];

	ImVec2 MousePos = g.MousePos;

	auto FixPos = [](ImVec2 Max, ImVec2 Min, ImVec2& Cur)
	{
		if (Cur.x > Max.x - 5) { Cur.x = Max.x - 5; }
		if (Cur.y > Max.y - 5) { Cur.y = Max.y - 5; }
		if (Cur.x < Min.x + 5) { Cur.x = Min.x + 5; }
		if (Cur.y < Min.y + 5) { Cur.y = Min.y + 5; }
	};

	auto DrawVBox = [Draw](ImVec2 Pos, ImVec2 Size, ImU32 Color)
	{
		Pos = ImVec2(int(Pos.x), int(Pos.y));
		Draw->AddLine(ImVec2(Pos.x - 0.5f, Pos.y), ImVec2(Pos.x + Size.x, Pos.y), Color);
		Draw->AddLine(ImVec2(Pos.x, Pos.y - 0.5f), ImVec2(Pos.x, Pos.y + Size.y), Color);
		Draw->AddLine(ImVec2(Pos.x, Pos.y + Size.y), ImVec2(Pos.x + Size.x + 0.5f, Pos.y + Size.y), Color);
		Draw->AddLine(ImVec2(Pos.x + Size.x, Pos.y), ImVec2(Pos.x + Size.x, Pos.y + Size.y + 0.5f), Color);
	};

	auto DrawVBoxCorner = [Draw](ImVec2 Pos, ImVec2 Size, ImU32 Color)
	{
		Pos = ImVec2(int(Pos.x), int(Pos.y));
		/*

		01-----------11
		2             2
		|             |
		|             |
		1             1
		22-----------23
		*/

		int LSize_X = Size.x / 4;
		int LSize_Y = Size.y / 4;

		/*01*/
		Draw->AddLine(
			ImVec2(Pos.x, Pos.y),
			ImVec2(Pos.x + LSize_X, Pos.y),
			Color);

		/*02*/
		Draw->AddLine(
			ImVec2(Pos.x, Pos.y),
			ImVec2(Pos.x, Pos.y + LSize_Y),
			Color);

		/*11*/
		Draw->AddLine(
			ImVec2(Pos.x + Size.x - LSize_X, Pos.y),
			ImVec2(Pos.x + Size.x, Pos.y),
			Color);

		/*12*/
		Draw->AddLine(
			ImVec2(Pos.x + Size.x, Pos.y),
			ImVec2(Pos.x + Size.x, Pos.y + LSize_Y),
			Color);


		/*21*/
		Draw->AddLine(
			ImVec2(Pos.x, Pos.y + Size.y - LSize_Y),
			ImVec2(Pos.x, Pos.y + Size.y),
			Color);

		/*22*/
		Draw->AddLine(
			ImVec2(Pos.x, Pos.y + Size.y),
			ImVec2(Pos.x + LSize_X, Pos.y + Size.y),
			Color);

		/*31*/
		Draw->AddLine(
			ImVec2(Pos.x + Size.x, Pos.y + Size.y - LSize_Y),
			ImVec2(Pos.x + Size.x, Pos.y + Size.y),
			Color);

		/*32*/
		Draw->AddLine(
			ImVec2(Pos.x + Size.x - LSize_X, Pos.y + Size.y),
			ImVec2(Pos.x + Size.x, Pos.y + Size.y),
			Color);
	};

	auto Background = [Draw, DrawVBox, LinesColor, OutLineColor, BackColor](ImVec2 Min, ImVec2 Max, ImVec2 MenuPos)
	{

		//Draw->AddRectFilled(ImVec2(Min.x, Min.y + (VIS_PREW_ZONE_Y / LINES_Y)), ImVec2(VIS_PREW_ZONE_X, VIS_PREW_ZONE_Y), BackColor);

	//	for (int i(1); i <= LINES_Y; i++)
	//		Draw->AddLine(ImVec2(Min.x, Min.y + (VIS_PREW_ZONE_Y / LINES_Y) * i), ImVec2(Min.x + VIS_PREW_ZONE_X, Min.y + (VIS_PREW_ZONE_Y / LINES_Y) * i), LinesColor);
	//	for (int i(1); i <= LINES_X; i++)
	//		Draw->AddLine(ImVec2(Min.x + (VIS_PREW_ZONE_X / LINES_X) * i, Min.y), ImVec2(Min.x + (VIS_PREW_ZONE_X / LINES_X) * i, Min.y + VIS_PREW_ZONE_Y), LinesColor);

		DrawVBox(ImVec2(Min.x, Min.y + (VIS_PREW_ZONE_Y / LINES_Y)), ImVec2(VIS_PREW_ZONE_X, VIS_PREW_ZONE_Y), OutLineColor);
	};

	auto DrawBarVertical = [Draw, DrawVBox](ImVec2 Pos, ImVec2 Size, ImU32 MainColor, ImU32 BackColor, ImU32 OutLineColor, int Lines, MoveStruct* Curt)
	{

		//	Pos = ImVec2(int(Pos.x), int(Pos.y));

			/*if (Curt->boolParam1)
				Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
					ImVec2(Size.x, Size.y),
					BackColor);

			if (!Curt->boolParam3)
				Draw->AddRectFilled(ImVec2(Pos.x, Pos.y + (Size.y / 10)),
					ImVec2(Size.x, Size.y - (Size.y / 10)),
					MainColor);*/


		if (Lines != 0)
		{
			//	Lines += 1;
			//	int DistL = Size.y / Lines;

			//	if (Lines == 1)
			//		DistL = Size.y / 2;

			//	for (int i(1); i < Lines; i++)
			//	{
			//		Draw->AddLine(ImVec2(Pos.x, Pos.y + DistL * i), ImVec2(Pos.x + Size.x, Pos.y + DistL * i), OutLineColor);
			//	}
		}
		Draw->AddRectFilled(ImVec2(Pos.x, Pos.y + Size.y / 2),
			ImVec2(Pos.x + Size.x, Pos.y + Size.y),
			MainColor);

		if (Curt->boolParam2)
			DrawVBox(Pos, Size, OutLineColor);
	};

	auto DrawBarHorizontal = [Draw, DrawVBox](ImVec2 Pos, ImVec2 Size, ImU32 MainColor, ImU32 BackColor, ImU32 OutLineColor, int Lines, MoveStruct* Curt)
	{
		//Pos = ImVec2(int(Pos.x), int(Pos.y));
	//	if (Curt->boolParam1)
		//	Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
		//		ImVec2(Size.x, Size.y),
		//		BackColor);

	//	if (Curt->boolParam3)
	////		Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
	//			ImVec2(Size.x - (Size.x / 10), Size.y),
	//			MainColor);
	//	else
	//		Draw->AddRectFilled(ImVec2(Pos.x + (Size.x / 10), Pos.y),
		//		ImVec2(Size.x - (Size.x / 10), Size.y),
		//		MainColor);

	//	if (Lines != 0)
	//	{
		//	Lines += 1;
		//	int DistL = Size.x / Lines;
		//
		//	if (Lines == 1)
		//		DistL = Size.x / 2;
		//
		//	for (int i(1); i < Lines; i++)
		//		Draw->AddLine(ImVec2(Pos.x + DistL * i, Pos.y), ImVec2(Pos.x + DistL * i, Pos.y + Size.y), OutLineColor);
		//}
		Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
			ImVec2(Pos.x + Size.x / 2, Pos.y + Size.y),
			MainColor);
		if (Curt->boolParam2)
			DrawVBox(Pos, Size, OutLineColor);

	};
	//ImGui::SetCursorPos(ImVec2(ImGui::GetWindowPos()));
	ImVec2 StartPos = ImGui::GetCursorPos();

	ImVec2 StartPosScreen = ImGui::GetWindowPos();

	ImVec2 TextOffset = ImVec2(StartPosScreen.x - StartPos.x, StartPosScreen.y - StartPos.y);
	auto _DrawText = [Draw, TextOffset](std::string text, MoveStruct* Curt/*, CFont* font*/)
	{
		ImVec2 Pos = ImVec2(Curt->Position.x - TextOffset.x, Curt->Position.y - TextOffset.y);
		ImU32 background = ImGui::GetColorU32(ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 20 / 255.f));
		ImU32 texts = ImGui::GetColorU32(ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f));
		if (Curt->boolParam3)
		{
			ImVec2 NSize = Curt->Size;
			ImVec2 NPos = Curt->Position;

			NSize.x += 30;
			NPos.x -= 15;
			NPos.y += 1.5f;

			float max = NSize.x / 3.f;

			for (int i(0); i < max; i++)
				Draw->AddRectFilled(ImVec2(NPos.x + i, NPos.y),
					ImVec2(NPos.x + i + 1, NPos.y + NSize.y),
					background);

			Draw->AddRectFilled(ImVec2(NPos.x + max, NPos.y),
				ImVec2(NPos.x + (max * 2), NPos.y + NSize.y),
				background);

			for (int i(0); i < max; i++)
				Draw->AddRectFilled(ImVec2(NPos.x + i + (max * 2), NPos.y),
					ImVec2(NPos.x + i + 1 + (max * 2), NPos.y + NSize.y),
					background);


		}
		if (true)
		{
			Draw->AddText(ImVec2(Pos.x + 1, Pos.y + 1), texts, text.c_str());
			//	Draw->AddText(ImVec2(Pos.x + 1, Pos.y + 1), texts, text.c_str());
			//	RenderTextClipped(frame_bb.Min + style.FramePadding, ImVec2(value_x2, frame_bb.Max.y), preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));
			//	ImGui::RenderTextClipped(ImVec2(Pos.x,Pos.y),ImVec2(Pos.x, Pos.y), text.c_str(),NULL, NULL, ImVec2(0.0f, 0.0f));

			//	Draw->Text
		}

		Draw->AddText(ImVec2(Pos.x + 1, Pos.y + 1), texts, text.c_str());

	};

	ImVec2 EndPosScreen = ImVec2(StartPosScreen.x + VIS_PREW_ZONE_X, StartPosScreen.y + VIS_PREW_ZONE_Y);

	if ((MousePos.x > StartPosScreen.x && MousePos.x < EndPosScreen.x) &&
		(MousePos.y > StartPosScreen.y && MousePos.y < EndPosScreen.y))
	{

	}

	int FixImgPos_X = ((VIS_PREW_ZONE_X - IMAGE_X) / 2);
	int FixImgPos_Y = ((VIS_PREW_ZONE_Y - IMAGE_Y) / 2);

	ImVec2 ImgePos = ImVec2(StartPos.x + FixImgPos_X, StartPos.y + FixImgPos_Y);

	ImVec2 ScreenImgePos = ImgePos;

	Background(StartPosScreen, EndPosScreen, StartPos);

	//	if (!VisualBodyImage)
		//	D3DXCreateTextureFromFileEx(GP_Render->m_pDevice, (CGlobal::SystemDisk + XorStr("X1N3\\Resources\\Images\\Body.png")).c_str(), 222, 579,
		//		D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &VisualBodyImage);

	//	if (VisualBodyImage)
		//	Draw->DrawImage(VisualBodyImage, ImgePos, ImgePos + ImVec2(IMAGE_X, IMAGE_Y));

	Draw->AddLine(ImVec2(StartPosScreen.x + (VIS_PREW_ZONE_X / 2), StartPosScreen.y), ImVec2(StartPosScreen.x + (VIS_PREW_ZONE_X / 2), StartPosScreen.y + VIS_PREW_ZONE_Y), OutLineColor);
	Draw->AddLine(ImVec2(StartPosScreen.x, StartPosScreen.y + (VIS_PREW_ZONE_Y / 2)), ImVec2(StartPosScreen.x + VIS_PREW_ZONE_X, StartPosScreen.y + (VIS_PREW_ZONE_Y / 2)), OutLineColor);
	ImU32 skeleton = ImGui::GetColorU32(ImVec4(config_system.g_cfg.player.type[player].skeleton_color.r(), config_system.g_cfg.player.type[player].skeleton_color.g(), config_system.g_cfg.player.type[player].skeleton_color.b(), config_system.g_cfg.player.type[player].skeleton_color.a()));
	auto SkeletonColor = skeleton;

	if (config_system.g_cfg.player.type[player].skeleton)
	{
		Draw->AddLine(ImVec2(StartPosScreen.x + 152 + 20, StartPosScreen.y + 122), ImVec2(StartPosScreen.x + 150 + 20, StartPosScreen.y + 103), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 152 + 20, StartPosScreen.y + 122), ImVec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 129), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 129), ImVec2(StartPosScreen.x + 158 + 20, StartPosScreen.y + 163), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 158 + 20, StartPosScreen.y + 163), ImVec2(StartPosScreen.x + 159 + 20, StartPosScreen.y + 194), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 159 + 20, StartPosScreen.y + 194), ImVec2(StartPosScreen.x + 180 + 20, StartPosScreen.y + 219), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 180 + 20, StartPosScreen.y + 219), ImVec2(StartPosScreen.x + 195 + 20, StartPosScreen.y + 273), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 195 + 20, StartPosScreen.y + 273), ImVec2(StartPosScreen.x + 196 + 20, StartPosScreen.y + 306), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 159 + 20, StartPosScreen.y + 194), ImVec2(StartPosScreen.x + 139 + 20, StartPosScreen.y + 218), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 139 + 20, StartPosScreen.y + 216), ImVec2(StartPosScreen.x + 131 + 20, StartPosScreen.y + 267), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 131 + 20, StartPosScreen.y + 267), ImVec2(StartPosScreen.x + 134 + 20, StartPosScreen.y + 286), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 132), ImVec2(StartPosScreen.x + 121 + 20, StartPosScreen.y + 142), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 121 + 20, StartPosScreen.y + 142), ImVec2(StartPosScreen.x + 126 + 20, StartPosScreen.y + 146), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 125 + 20, StartPosScreen.y + 145), ImVec2(StartPosScreen.x + 141 + 20, StartPosScreen.y + 134), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 132), ImVec2(StartPosScreen.x + 183 + 20, StartPosScreen.y + 135), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 181 + 20, StartPosScreen.y + 135), ImVec2(StartPosScreen.x + 174 + 20, StartPosScreen.y + 141), SkeletonColor);
		Draw->AddLine(ImVec2(StartPosScreen.x + 161 + 20, StartPosScreen.y + 122), ImVec2(StartPosScreen.x + 174 + 20, StartPosScreen.y + 141), SkeletonColor);
	}

	ImVec2 StartBoxPos = ImVec2(int(ScreenImgePos.x + IMAGE_X / 2), int(ScreenImgePos.y + (IMAGE_Y / 13) - ((int)10 * 2.9)));

	ImVec2 CLines = ImVec2(StartPosScreen.x + (VIS_PREW_ZONE_X / 2), StartBoxPos.y);

	BoxSize_Y = int(IMAGE_Y - (IMAGE_Y / 13) + ((int)10 * 4));
	BoxSize_X = int(BoxSize_Y / 2);
	StartBoxPos.x -= int(BoxSize_X / 2.f);

	int HPTol = MV_Hp->NumParam2;
	int ATol = MV_Armor->NumParam2;

	for (int i(0); (MITypes)i < MI_MAX; i++)
		MVItemList[i].Position = ImVec2(int(StartPosScreen.x + MVItemList[i].ZonePos.x), int(StartPosScreen.y + MVItemList[i].ZonePos.y));

	if (!MoveStruct::InitMItems)
	{
		MoveStruct::InitMItems = true;
		MoveStruct::LastActive = MI_MAX;

		/*=====================================================================================*/
		MV_Hp_Text->Position = ImVec2(StartBoxPos.x - MV_Hp_Text->Size.x, StartBoxPos.y);
		MV_Hp_Text->StartPos = MV_Hp_Text->Position;
		MV_Hp_Text->Id = MI_HEALTH_TEXT;
		MV_Hp_Text->Side = LEFT_S;
		MV_Hp_Text->TextSize = 17;
		MV_Hp_Text->Offset = ImVec2(5, 0);
		MV_Hp_Text->IsText = true;
		MV_Hp_Text->Name = crypt_str("MV_HP_Text");
		MV_Hp_Text->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Armor_Text->Position = ImVec2(StartBoxPos.x - MV_Armor_Text->Size.x, StartBoxPos.y);
		MV_Armor_Text->StartPos = MV_Armor_Text->Position;
		MV_Armor_Text->Id = MI_ARMOR_TEXT;
		MV_Armor_Text->Side = LEFT_S;
		MV_Armor_Text->TextSize = 17;
		MV_Armor_Text->Offset = ImVec2(5, -10);
		MV_Armor_Text->IsText = true;
		MV_Armor_Text->Name = crypt_str("MV_Arm_Text");
		MV_Hp_Text->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Hp->Position = ImVec2(StartBoxPos.x - HPTol, StartBoxPos.y);
		MV_Hp->StartPos = MV_Hp->Position;
		MV_Hp->Id = MI_HEALTH_BAR;
		MV_Hp->Side = LEFT_S;
		MV_Hp->Size = ImVec2(HPTol, BoxSize_Y);
		MV_Hp->Offset = ImVec2(5, 0);

		MV_Hp->Color1 = Color(10, 255, 10, 255);
		MV_Hp->Color2 = Color(255, 0, 0, 255);
		MV_Hp->Color3 = Color(0, 0, 0, 255);

		MV_Hp->NumParam1 = 4;
		MV_Hp->NumParam2 = 5;
		MV_Hp->Name = crypt_str("MV_HPBar");
		MV_Hp->boolParam1 = true;
		MV_Hp->boolParam2 = true;
		MV_Hp->boolParam3 = false;

		/*=====================================================================================*/
		MV_Armor->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
		MV_Armor->StartPos = MV_Armor->Position;
		MV_Armor->Id = MI_ARMOR_BAR;
		MV_Armor->Side = RIGHT_S;
		MV_Armor->Size = ImVec2(ATol, BoxSize_Y);
		MV_Armor->Offset = ImVec2(-5, 0);

		MV_Armor->Color1 = Color(255, 255, 255, 255);
		MV_Armor->Color2 = Color(255, 0, 0, 255);
		MV_Armor->Color3 = Color(0, 0, 0, 255);

		MV_Armor->NumParam1 = 4;
		MV_Armor->NumParam2 = 5;
		MV_Armor->Name = crypt_str("MV_ArmorBar");
		MV_Armor->boolParam1 = true;
		MV_Armor->boolParam2 = true;
		MV_Armor->boolParam3 = false;

		/*=====================================================================================*/
		MV_Name->Position = ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Name->Size.x / 2), StartBoxPos.y - MV_Name->Size.y);
		MV_Name->StartPos = MV_Name->Position;
		MV_Name->Id = MI_NAME_TEXT;
		MV_Name->Side = TOP_S;
		MV_Name->TextSize = 17;
		MV_Name->Offset = ImVec2(0, 4);
		MV_Name->IsText = true;
		MV_Name->Name = crypt_str("MV_Name");
		MV_Name->boolParam1 = false;

		MV_Name->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Weapon->Position = ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Weapon->Size.x / 2), StartBoxPos.y + BoxSize_Y);
		MV_Weapon->StartPos = MV_Weapon->Position;
		MV_Weapon->Id = MI_WEAPON_TEXT;
		MV_Weapon->Side = DOWN_S;
		MV_Weapon->TextSize = 17;
		MV_Weapon->Offset = ImVec2(0, -1);
		MV_Weapon->IsText = true;
		MV_Weapon->Name = crypt_str("MV_Weapon");
		MV_Weapon->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Ammo->Position = ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Ammo->Size.x / 2), StartBoxPos.y + BoxSize_Y);
		MV_Ammo->StartPos = MV_Ammo->Position;
		MV_Ammo->Id = MI_AMMO_TEXT;
		MV_Ammo->Side = DOWN_S;
		MV_Ammo->TextSize = 17;
		MV_Ammo->Offset = ImVec2(0, -10);
		MV_Ammo->IsText = true;
		MV_Ammo->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Distance->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y - MV_Distance->Size.y);
		MV_Distance->StartPos = MV_Distance->Position;
		MV_Distance->Id = MI_DISTANCE_TEXT;
		MV_Distance->Side = DOWN_RIGHT_DOWN_S;
		MV_Distance->TextSize = 17;
		MV_Distance->Offset = ImVec2(-7, 0);
		MV_Distance->IsText = true;
		MV_Distance->Name = crypt_str("MV_Distance");
		MV_Distance->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Money->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
		MV_Money->StartPos = MV_Money->Position;
		MV_Money->Id = MI_MONEY_TEXT;
		MV_Money->Side = RIGHT_S;
		MV_Money->TextSize = 17;
		MV_Money->Offset = ImVec2(-7, 0);
		MV_Money->IsText = true;
		MV_Money->Name = crypt_str("MV_Money");
		MV_Money->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Scope->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
		MV_Scope->StartPos = MV_Scope->Position;
		MV_Scope->Id = MI_SCOPE_TEXT;
		MV_Scope->Side = RIGHT_S;
		MV_Scope->TextSize = 17;
		MV_Scope->Offset = ImVec2(-7, -7);
		MV_Scope->IsText = true;
		MV_Scope->Name = crypt_str("MV_Scope");
		MV_Scope->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Flashed->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y / 2);
		MV_Flashed->StartPos = MV_Flashed->Position;
		MV_Flashed->Id = MI_FLASHED_TEXT;
		MV_Flashed->Side = CENTER_RIGHT_S;
		MV_Flashed->TextSize = 17;
		MV_Flashed->Color1 = Color(255, 255, 255, 255);
		MV_Flashed->Offset = ImVec2(-7, 0);
		MV_Flashed->IsText = true;
		MV_Flashed->Name = crypt_str("MV_Flashed");

		/*=====================================================================================*/
		MV_Defusing->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y / 2);
		MV_Defusing->StartPos = MV_Defusing->Position;
		MV_Defusing->Id = MI_DEFUSING_TEXT;
		MV_Defusing->Side = CENTER_RIGHT_S;
		MV_Defusing->TextSize = 17;
		MV_Defusing->Color1 = Color(255, 255, 255, 255);
		MV_Defusing->Offset = ImVec2(-7, -7);
		MV_Defusing->IsText = true;
		MV_Defusing->Name = crypt_str("MV_Defusing");
	}

	//if (FillBox)
	//	Draw->DrawFilledBox(StartBoxPos, ImVec2(BoxSize_X, BoxSize_Y), FillBoxColor, BoxStyle == 3 ? BoxSize_X / 5.f : 0.0f);

	if (config_system.g_cfg.player.type[player].box)
	{
		ImU32 box = ImGui::GetColorU32(ImVec4(config_system.g_cfg.player.type[player].box_color.r(), config_system.g_cfg.player.type[player].box_color.g(), config_system.g_cfg.player.type[player].box_color.b(), config_system.g_cfg.player.type[player].box_color.a()));
		auto BoxOutLineColor = box;

		//color_t BoxOutLineColor = color_t(0.f, 0.f, 0.f, 255.f);

		DrawVBoxCorner(StartBoxPos, ImVec2(BoxSize_X, BoxSize_Y), BoxOutLineColor);
		//	if (BoxOutline)
		//	{
		DrawVBoxCorner(ImVec2(StartBoxPos.x + 1, StartBoxPos.y + 1), ImVec2(BoxSize_X - 2, BoxSize_Y - 2), BoxOutLineColor);
		DrawVBoxCorner(ImVec2(StartBoxPos.x - 1, StartBoxPos.y - 1), ImVec2(BoxSize_X + 2, BoxSize_Y + 2), BoxOutLineColor);

		int LSize_X = BoxSize_X / 4;
		int LSize_Y = BoxSize_Y / 4;

		Draw->AddLine(ImVec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y - 1), ImVec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y + 1), BoxOutLineColor);
		Draw->AddLine(ImVec2(StartBoxPos.x - 1, StartBoxPos.y + LSize_Y + 0.5f), ImVec2(StartBoxPos.x + 1, StartBoxPos.y + LSize_Y + 0.5f), BoxOutLineColor);

		Draw->AddLine(ImVec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y - 1), ImVec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y + 1), BoxOutLineColor);
		Draw->AddLine(ImVec2(StartBoxPos.x + BoxSize_X - 1, StartBoxPos.y + LSize_Y + 0.5f), ImVec2(StartBoxPos.x + BoxSize_X + 1, StartBoxPos.y + LSize_Y + 0.5f), BoxOutLineColor);

		Draw->AddLine(ImVec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y + BoxSize_Y - 1), ImVec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y + BoxSize_Y + 1), BoxOutLineColor);
		Draw->AddLine(ImVec2(StartBoxPos.x - 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), ImVec2(StartBoxPos.x + 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), BoxOutLineColor);

		Draw->AddLine(ImVec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y + BoxSize_Y - 1), ImVec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y + BoxSize_Y + 1), BoxOutLineColor);
		Draw->AddLine(ImVec2(StartBoxPos.x + BoxSize_X - 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), ImVec2(StartBoxPos.x + BoxSize_X + 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), BoxOutLineColor);

	}

	//	if (HeadEsp)
		//{
		//	ImVec2 HeadCenterPos = ImVec2(ScreenImgePos.x + 51, ScreenImgePos.y + 20);
	//
		//	Draw->DrawRing(HeadCenterPos.x, HeadCenterPos.y, 20, 64, HeadEspColor, 1);
	//	}

	auto ChangeSidePos = [StartBoxPos](MoveStruct* Curt, int fix) -> ImVec2
	{
		switch (Curt->Side)
		{
		case LEFT_S:  return ImVec2(StartBoxPos.x - fix - Curt->CorrectPadding.x, StartBoxPos.y);
		case RIGHT_S: return ImVec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y);
		case TOP_S:   return ImVec2(StartBoxPos.x, StartBoxPos.y - fix - Curt->CorrectPadding.y);
		case DOWN_S:  return ImVec2(StartBoxPos.x, StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		default: return ImVec2(0, 0);
		}
	};

	auto ChangeSidePosText = [StartBoxPos](MoveStruct* Curt, ImVec2 Size) -> ImVec2
	{
		switch (Curt->Side)
		{
		case LEFT_S: return ImVec2(StartBoxPos.x - Size.x - Curt->CorrectPadding.x, StartBoxPos.y + Curt->CorrectPadding.y);
		case RIGHT_S:return ImVec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y + Curt->CorrectPadding.y);
		case TOP_S:  return ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (Size.x / 2), StartBoxPos.y - Size.y - Curt->CorrectPadding.y);
		case DOWN_S: return ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (Size.x / 2), StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		case LEFT_TOP_S:  return ImVec2(StartBoxPos.x, StartBoxPos.y - Size.y - Curt->CorrectPadding.y);
		case RIGHT_TOP_S: return ImVec2(StartBoxPos.x + BoxSize_X - Size.x, StartBoxPos.y - Size.y - Curt->CorrectPadding.y);
		case LEFT_DOWN_S: return ImVec2(StartBoxPos.x, StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		case RIGHT_DOWN_S:return ImVec2(StartBoxPos.x + BoxSize_X - Size.x, StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		case CENTER_LEFT_S:    return ImVec2(StartBoxPos.x - Size.x - Curt->CorrectPadding.x, StartBoxPos.y + (BoxSize_Y / 2) + Curt->CorrectPadding.y);
		case CENTER_RIGHT_S:   return ImVec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y + (BoxSize_Y / 2) + Curt->CorrectPadding.y);
		case DOWN_LEFT_DOWN_S: return ImVec2(StartBoxPos.x - Size.x - Curt->CorrectPadding.x, StartBoxPos.y + BoxSize_Y - Size.y + Curt->CorrectPadding.y);
		case DOWN_RIGHT_DOWN_S:return ImVec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y + BoxSize_Y - Size.y + Curt->CorrectPadding.y);
		default: return ImVec2(0, 0);
		}
	};

	ImVec2 HPStartPos = ChangeSidePos(MV_Hp, HPTol);
	ImVec2 ArmorStartPos = ChangeSidePos(MV_Armor, ATol);

	ImVec2 HPTextStartPos = ChangeSidePosText(MV_Hp_Text, MV_Hp_Text->Size);
	ImVec2 ArmorTextStartPos = ChangeSidePosText(MV_Armor_Text, MV_Armor_Text->Size);

	ImVec2 NameStartPos = ChangeSidePosText(MV_Name, MV_Name->Size);
	ImVec2 WeaponStartPos = ChangeSidePosText(MV_Weapon, MV_Weapon->Size);
	ImVec2 AmmoStartPos = ChangeSidePosText(MV_Ammo, MV_Ammo->Size);
	ImVec2 DistanceStartPos = ChangeSidePosText(MV_Distance, MV_Distance->Size);
	ImVec2 MoneyStartPos = ChangeSidePosText(MV_Money, MV_Money->Size);
	ImVec2 DefusingStartPos = ChangeSidePosText(MV_Defusing, MV_Defusing->Size);
	ImVec2 ScopeStartPos = ChangeSidePosText(MV_Scope, MV_Scope->Size);
	ImVec2 FlashedStartPos = ChangeSidePosText(MV_Flashed, MV_Flashed->Size);

	MV_Hp->SizeV = ImVec2(HPTol, BoxSize_Y);
	MV_Armor->SizeV = ImVec2(ATol, BoxSize_Y);

	MV_Hp->SizeH = ImVec2(BoxSize_X, HPTol);
	MV_Armor->SizeH = ImVec2(BoxSize_X, ATol);

	auto IsPosTouch = [](MoveStruct& Curt, ImVec2 Pos) -> bool
	{
		if ((Pos.x > Curt.Position.x && Pos.x < Curt.Position.x + Curt.Size.x) &&
			(Pos.y > Curt.Position.y && Pos.y < Curt.Position.y + Curt.Size.y))
		{
			return true;
		}
		return false;
	};

	auto CalcPadding = [&](MoveStruct& Curt) -> void
	{
		bool CheckVert = true;
		bool CheckHor = true;

		Curt.CorrectPadding = ImVec2(0, 0);

		int TraceLenX = 25;
		int TraceLenY = 30;

		if (Curt.Side == RIGHT_S || Curt.Side == CENTER_RIGHT_S || Curt.Side == DOWN_RIGHT_DOWN_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && (CheckHor || CheckVert) &&
					(MoveStruct::LastActive == Curt.Id || MoveStruct::LastActive == MV.Id ? !IsPosTouch(MV, Curt.Center) : true) &&
					(MV.Side != TOP_S && MV.Side != LEFT_TOP_S && MV.Side != RIGHT_TOP_S))
				{
					if (CheckVert)
					{
						for (int i(-3); i < TraceLenY; i++)
						{
							if (IsPosTouch(MV, ImVec2(Curt.Center.x, Curt.Center.y - (Curt.Size.y / 2) - i)))
							{
								Curt.CorrectPadding.y = MV.CorrectPadding.y + MV.Size.y;
								CheckVert = false;
							}
						}
					}

					if (CheckHor && (!MV.IsText ? true : Curt.Center.y > MV.Center.y))
					{
						for (int i(CheckVert ? -3 : 1); i < TraceLenX; i++)
						{
							if (IsPosTouch(MV, ImVec2(Curt.Center.x - (Curt.Size.x / 2) - i, Curt.Center.y)))
							{
								Curt.CorrectPadding.x = MV.CorrectPadding.x + MV.Size.x;
								CheckHor = false;
							}
						}
					}
				}
			}
		}

		if (Curt.Side == LEFT_S || Curt.Side == CENTER_LEFT_S || Curt.Side == DOWN_LEFT_DOWN_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && (CheckHor || CheckVert) &&
					(MoveStruct::LastActive == Curt.Id || MoveStruct::LastActive == MV.Id ? !IsPosTouch(MV, Curt.Center) : true) &&
					(MV.Side != TOP_S && MV.Side != LEFT_TOP_S && MV.Side != RIGHT_TOP_S))
				{
					if (CheckVert)
					{
						for (int i(-3); i < TraceLenY; i++)
						{
							if (IsPosTouch(MV, ImVec2(Curt.Center.x, Curt.Center.y - (Curt.Size.y / 2) - i)))
							{
								Curt.CorrectPadding.y = MV.CorrectPadding.y + MV.Size.y;
								CheckVert = false;
							}
						}
					}

					if (CheckHor && (!MV.IsText ? true : Curt.Center.y > MV.Center.y))
					{
						for (int i(CheckVert ? -3 : 1); i < TraceLenX; i++)
						{
							if (IsPosTouch(MV, ImVec2(Curt.Center.x + (Curt.Size.x / 2) + i, Curt.Center.y)))
							{
								Curt.CorrectPadding.x = MV.CorrectPadding.x + MV.Size.x;
								CheckHor = false;
							}
						}
					}
				}
			}
		}

		if (Curt.Side == TOP_S || Curt.Side == LEFT_TOP_S || Curt.Side == RIGHT_TOP_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && !IsPosTouch(MV, Curt.Center) &&
					(MV.Side != LEFT_S && MV.Side != CENTER_LEFT_S && MV.Side != DOWN_LEFT_DOWN_S && MV.Side != RIGHT_S && MV.Side != CENTER_RIGHT_S && MV.Side != DOWN_RIGHT_DOWN_S))
				{
					for (int i(-3); i < TraceLenY; i++)
					{
						if (IsPosTouch(MV, ImVec2(Curt.Center.x, Curt.Center.y + (Curt.Size.y / 2) + i)))
						{
							Curt.CorrectPadding.y = MV.CorrectPadding.y + MV.Size.y;
							return;
						}
					}
				}
			}
		}

		if (Curt.Side == DOWN_S || Curt.Side == LEFT_DOWN_S || Curt.Side == RIGHT_DOWN_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && !IsPosTouch(MV, Curt.Center) &&
					(MV.Side != LEFT_S && MV.Side != CENTER_LEFT_S && MV.Side != DOWN_LEFT_DOWN_S && MV.Side != RIGHT_S && MV.Side != CENTER_RIGHT_S && MV.Side != DOWN_RIGHT_DOWN_S))
				{
					for (int i(-3); i < TraceLenY; i++)
					{
						if (IsPosTouch(MV, ImVec2(Curt.Center.x, Curt.Center.y - (Curt.Size.y / 2) - i)))
						{
							Curt.CorrectPadding.y = MV.Size.y;
							return;
						}
					}
				}
			}
		}
	};

	if (config_system.g_cfg.player.type[player].health)
	{
		ImU32 color1 = ImGui::GetColorU32(ImVec4(config_system.g_cfg.player.type[player].health_color.r() / 255.f, config_system.g_cfg.player.type[player].health_color.g() / 255.f, config_system.g_cfg.player.type[player].health_color.b() / 255.f, config_system.g_cfg.player.type[player].health_color.a() / 255.f));
		ImU32 color2 = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.3f));
		ImU32 color3 = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.5f));

		//		MV_Hp->Color1 = HealthColor;
			//	MV_Hp_Text->Color1 = HealthColor;
			//	MV_Hp->Color2 = HealthColor2;
			//	MV_Hp->Color3 = HealthColor3;
				//if (HealthStyle == 0)
			//	{
		Move(*MV_Hp, IsMousPress, MousePos, HPStartPos, CLines);
		if (MV_Hp->Side == LEFT_S || MV_Hp->Side == RIGHT_S)
			DrawBarVertical(MV_Hp->Position, MV_Hp->Size, color1, color2, color3, MV_Hp->NumParam1, MV_Hp);
		else
			DrawBarHorizontal(MV_Hp->Position, MV_Hp->Size, color1, color2, color3, MV_Hp->NumParam1, MV_Hp);
		//	}
		//	else if (HealthStyle == 1)
		//	{
		//		Move(*MV_Hp_Text, IsMousPress, MousePos, HPTextStartPos, CLines, true);
		//		_DrawText(MV_HEALTH_TEXT, MV_Hp_Text, GP_Render->SzFonts[TextHealthSize]);
		//	}
	}

	/*	if (Armor)
		{
			MV_Armor->Color1 = ArmorColor;
			MV_Armor_Text->Color1 = ArmorColor;
			MV_Armor->Color2 = ArmorColor2;
			MV_Armor->Color3 = ArmorColor3;

			if (ArmorStyle == 0)
			{
				Move(*MV_Armor, IsMousPress, MousePos, ArmorStartPos, CLines);
				if (MV_Armor->Side == LEFT_S || MV_Armor->Side == RIGHT_S)
					DrawBarVertical(MV_Armor->Position, MV_Armor->Size, MV_Armor->Color1, MV_Armor->Color2, MV_Armor->Color3, MV_Armor->NumParam1, MV_Armor);
				else
					DrawBarHorizontal(MV_Armor->Position, MV_Armor->Size, MV_Armor->Color1, MV_Armor->Color2, MV_Armor->Color3, MV_Armor->NumParam1, MV_Armor);
			}
			else if (ArmorStyle == 1)
			{
				Move(*MV_Armor_Text, IsMousPress, MousePos, ArmorTextStartPos, CLines, true);
				_DrawText(MV_ARMOR_TEXT, MV_Armor_Text, GP_Render->SzFonts[TextArmorSize]);
			}
		}
	*/
	if (config_system.g_cfg.player.type[player].name)
	{
		Move(*MV_Name, IsMousPress, MousePos, NameStartPos, CLines, true);

		/*	if (NameBackground)
			{
				ImVec2 NSize = MV_Name->Size;
				ImVec2 NPos = MV_Name->Position;

				NSize.x += 30;
				NPos.x -= 15;
				NPos.y += 1.5f;

				float max = NSize.x / 3.f;

				for (int i(0); i < max; i++)
					Draw->DrawFilledBoxEx(ImVec2(NPos.x + i, NPos.y),
						ImVec2(NPos.x + i + 1, NPos.y + NSize.y),
						color_t(NameColorBack.r(), NameColorBack.g(), NameColorBack.b(), (((float)i / max) * 255)));

				Draw->DrawFilledBoxEx(ImVec2(NPos.x + max, NPos.y),
					ImVec2(NPos.x + (max * 2), NPos.y + NSize.y),
					color_t(NameColorBack.r(), NameColorBack.g(), NameColorBack.b(), 255));

				for (int i(0); i < max; i++)
					Draw->DrawFilledBoxEx(ImVec2(NPos.x + i + (max * 2), NPos.y),
						ImVec2(NPos.x + i + 1 + (max * 2), NPos.y + NSize.y),
						color_t(NameColorBack.r(), NameColorBack.g(), NameColorBack.b(), (255 - (((float)i / max) * 255))));
	/		}*/
	//	MV_Name->Color1 = NameColor;

		_DrawText(MV_NAME_TEXT, MV_Name);
	}

	if (config_system.g_cfg.player.type[player].snap_lines)
	{
		//	CFont* wep_fnt = nullptr;

		//	if (WeaponStyle == 0)
			//	wep_fnt = GP_Render->SzFonts[TextWeaponSize];
		//	else if (WeaponStyle == 1)
			//	wep_fnt = GP_Render->SzFontsIcon[TextWeaponSize];

		Move(*MV_Weapon, IsMousPress, MousePos, WeaponStartPos, CLines, true);

		//	MV_Weapon->Color1 = WeaponColor;

			//if (WeaponStyle == 0)
		_DrawText(MV_WEAPON_TEXT, MV_Weapon);
		//	else if (WeaponStyle == 1)
			//	_DrawText(MV_WEAPON_TEXT_ICO, MV_Weapon);
	}

	/*	if (Ammo)
		{
			Move(*MV_Ammo, IsMousPress, MousePos, AmmoStartPos, CLines, true);
			MV_Ammo->Color1 = AmmoColor;
			_DrawText(MV_AMMO_TEXT, MV_Ammo, GP_Render->SzFonts[TextAmmoSize]);
		}

		if (Distance)
		{
			Move(*MV_Distance, IsMousPress, MousePos, DistanceStartPos, CLines, true);
			MV_Distance->Color1 = DistanceColor;
			_DrawText(MV_DISTANCE_TEXT, MV_Distance, GP_Render->SzFonts[TextDistanceSize]);
		}

		if (Money)
		{
			Move(*MV_Money, IsMousPress, MousePos, MoneyStartPos, CLines, true);
			MV_Money->Color1 = MoneyColor;
			_DrawText(MV_MONEY_TEXT, MV_Money, GP_Render->SzFonts[TextMoneySize]);
		}

		if (Defusing)
		{
			Move(*MV_Defusing, IsMousPress, MousePos, DefusingStartPos, CLines, true);
			MV_Defusing->Color1 = DefusingColor;
			_DrawText(MV_DEFUSING_TEXT, MV_Defusing, GP_Render->SzFonts[TextDefusingSize]);
		}

		if (Scope)
		{
			Move(*MV_Scope, IsMousPress, MousePos, ScopeStartPos, CLines, true);
			MV_Scope->Color1 = ScopeColor;
			_DrawText(MV_SCOPE_TEXT, MV_Scope, GP_Render->SzFonts[TextScopedSize]);
		}

		if (Flashing)
		{
			Move(*MV_Flashed, IsMousPress, MousePos, FlashedStartPos, CLines, true);
			MV_Flashed->Color1 = FlashingColor;
			_DrawText(MV_FLASHED_TEXT, MV_Flashed, GP_Render->SzFonts[TextFlashedSize]);
		}
		*/
	if (MoveStruct::LastActive != -1 && (MITypes)MoveStruct::LastActive <= MI_MAX && MoveStruct::ActiveIdx != -1)
	{
		MoveStruct* Selected = &MVItemList[MoveStruct::LastActive];
		ImU32 SelColor = ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 1.0f, 0.5f));
		//	color_t SelColor = color_t(0, 255, 255, 102);
		ImVec2 SP = Selected->Position;

		int Correct = 0;

		if (!Selected->IsText)
			Correct = 1;

		Draw->AddLine(ImVec2(int(StartPosScreen.x), int(SP.y - Correct)), ImVec2(int(StartPosScreen.x + VIS_PREW_ZONE_X), int(SP.y - Correct)), SelColor);
		Draw->AddLine(ImVec2(int(SP.x - Correct), int(StartPosScreen.y)), ImVec2(int(SP.x - Correct), int(StartPosScreen.y + VIS_PREW_ZONE_Y)), SelColor);
		Draw->AddLine(ImVec2(int(StartPosScreen.x), int(SP.y + Selected->Size.y + Correct)), ImVec2(int(StartPosScreen.x + VIS_PREW_ZONE_X), SP.y + int(Selected->Size.y + Correct)), SelColor);
		Draw->AddLine(ImVec2(int(SP.x + Selected->Size.x + Correct), int(StartPosScreen.y)), ImVec2(int(SP.x + Selected->Size.x + Correct), int(StartPosScreen.y + VIS_PREW_ZONE_Y)), SelColor);
	}

	auto CalcVisOffset = [StartBoxPos](MoveStruct& Curt, bool IsText = false)
	{
		float DisttX = 0.000000001;
		float DisttY = 0.000000001;

		if (!IsText)
		{
			if (Curt.Side == LEFT_S)
			{
				DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x) - (Curt.Position.x + Curt.Size.x));
				DisttY = (StartBoxPos.y - Curt.Position.y);
			}
			else if (Curt.Side == RIGHT_S)
			{
				DisttX = ((StartBoxPos.x + Curt.CorrectPadding.x) - Curt.Position.x);
				DisttY = (StartBoxPos.y - Curt.Position.y);
			}
			else if (Curt.Side == TOP_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x);
				DisttY = ((StartBoxPos.y - Curt.CorrectPadding.y) - Curt.Position.y - Curt.Size.y);
			}
			else if (Curt.Side == DOWN_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x);
				DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
		}
		else
		{
			if (Curt.Side == LEFT_S)
			{
				DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x - Curt.Size.x) - (Curt.Position.x));
				DisttY = ((StartBoxPos.y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
			else if (Curt.Side == RIGHT_S)
			{
				DisttX = ((StartBoxPos.x + BoxSize_X + Curt.CorrectPadding.x) - Curt.Position.x);
				DisttY = ((StartBoxPos.y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
			else if (Curt.Side == TOP_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x + (BoxSize_X / 2) - (Curt.Size.x / 2));
				DisttY = ((StartBoxPos.y - Curt.CorrectPadding.y) - Curt.Position.y - Curt.Size.y);
			}
			else if (Curt.Side == DOWN_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x + (BoxSize_X / 2) - (Curt.Size.x / 2));
				DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
		}

		if (Curt.Side == LEFT_TOP_S)
		{
			DisttX = (StartBoxPos.x - Curt.Position.x);
			DisttY = ((StartBoxPos.y - Curt.Size.y - Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == RIGHT_TOP_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X) - (Curt.Position.x + Curt.Size.x));
			DisttY = ((StartBoxPos.y - Curt.Size.y - Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == LEFT_DOWN_S)
		{
			DisttX = (StartBoxPos.x - Curt.Position.x);
			DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == RIGHT_DOWN_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X - Curt.Size.x) - Curt.Position.x);
			DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == CENTER_LEFT_S)
		{
			DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x - Curt.Size.x) - (Curt.Position.x));
			DisttY = ((StartBoxPos.y + (BoxSize_Y / 2) + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == CENTER_RIGHT_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X + Curt.CorrectPadding.y) - Curt.Position.x);
			DisttY = ((StartBoxPos.y + (BoxSize_Y / 2) + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == DOWN_LEFT_DOWN_S)
		{
			DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x - Curt.Size.x) - (Curt.Position.x));
			DisttY = ((StartBoxPos.y + BoxSize_Y - Curt.Size.y + Curt.CorrectPadding.y) - (Curt.Position.y));
		}
		else if (Curt.Side == DOWN_RIGHT_DOWN_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X + Curt.CorrectPadding.x) - Curt.Position.x);
			DisttY = ((StartBoxPos.y + BoxSize_Y - Curt.Size.y + Curt.CorrectPadding.y) - (Curt.Position.y));
		}

		if (DisttX == 0)
			DisttX = 0.000000001;

		if (DisttY == 0)
			DisttY = 0.000000001;

		Curt.CorrectOfst.x = (BoxSize_X / DisttX);
		Curt.CorrectOfst.y = (BoxSize_Y / DisttY);
	};

	CalcPadding(*MV_Hp);
	CalcPadding(*MV_Armor);

	CalcPadding(*MV_Hp_Text);
	CalcPadding(*MV_Armor_Text);

	CalcPadding(*MV_Name);
	CalcPadding(*MV_Weapon);
	CalcPadding(*MV_Ammo);
	CalcPadding(*MV_Distance);
	CalcPadding(*MV_Money);
	CalcPadding(*MV_Scope);
	CalcPadding(*MV_Flashed);
	CalcPadding(*MV_Defusing);

	CalcVisOffset(*MV_Hp);
	CalcVisOffset(*MV_Armor);

	CalcVisOffset(*MV_Hp_Text, true);
	CalcVisOffset(*MV_Armor_Text, true);

	CalcVisOffset(*MV_Name, true);
	CalcVisOffset(*MV_Weapon, true);
	CalcVisOffset(*MV_Ammo, true);
	CalcVisOffset(*MV_Distance, true);
	CalcVisOffset(*MV_Money, true);
	CalcVisOffset(*MV_Scope, true);
	CalcVisOffset(*MV_Flashed, true);
	CalcVisOffset(*MV_Defusing, true);

	for (int i(0); (MITypes)i < MI_MAX; i++)
		MVItemList[i].ZonePos = ImVec2(MVItemList[i].Position.x - StartPosScreen.x, MVItemList[i].Position.y - StartPosScreen.y);

	ImGui::SetCursorPos(ImVec2(StartPos.x, StartPos.y + VIS_PREW_ZONE_Y + 3));
}





namespace ImGui
{

	bool ColorEdit4(const char* label, Color* v, bool show_alpha = true, bool is_solo = false)
	{
		auto clr = ImVec4{
			v->r() / 255.f,
			v->g() / 255.f,
			v->b() / 255.f,
			v->a() / 255.f

		};

		if (ImGui::ColorEdit4(label, &clr.x, show_alpha, is_solo)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}

	bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit4(label, v, true);
	}


	bool Tab(const char* icon, const char* label, const char* desc, const ImVec2& size_arg, const bool selected)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		static float sizeplus = 0.f;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 4.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		//if (selected)
			//window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(30, 30, 30));

		//if (selected)
			//window->DrawList->AddRectFilled({ bb.Max.x,bb.Max.y }, { bb.Max.x - 3,bb.Min.y }, ImColor(0.f, 143.f, 255.f));

		//ImGui::PushFont(c_menu::get().bigxd);
		//window->DrawList->AddText(ImVec2(bb.Min.x + 35, bb.Min.y + 13), ImColor(32.f / 255.f, 32.f / 255.f, 32.f / 255.f, 255.f / 255.f), label);
		//ImGui::PopFont();

		//window->DrawList->AddText(ImVec2(bb.Min.x + 35, bb.Min.y + 20), ImColor(255.f, 255.f, 255.f), desc);

		ImGui::PushFont(c_menu::get().icons);
		window->DrawList->AddText(ImVec2(bb.Min.x + 5, bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(icon).y / 2), ImColor(83, 86, 88), icon);
		ImGui::PopFont();

		return pressed;
	}


	bool SubTab(const char* label, const ImVec2& size_arg, const bool selected)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		static float sizeplus = 0.f;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 4.0f, label_size.y + style.FramePadding.y * 4.0f);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		if (selected)
			window->DrawList->AddRectFilled({ bb.Min.x - 15,bb.Min.y}, { bb.Max.x - 15,bb.Max.y}, ImColor(33, 33, 33), 15.f);

		//ImGui::PushFont(c_menu::get().bigxd);
		window->DrawList->AddText(ImVec2(bb.Min.x + 5, bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2), ImColor(83, 86, 88), label);
		//ImGui::PopFont();

		if (selected)
			window->DrawList->AddText(ImVec2(bb.Min.x + 5, bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2), ImColor(255, 255, 255), label);


		return pressed;
	}

	/*bool ColorPicker(const char* label, Color* v)
	{
		auto clr = ImVec4{
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};

		if (ImGui::ColorEdit4(label, &clr.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoSidePreview)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}*/
}

void c_menu::render2(bool is_open) {

	static auto w = 0, h = 0, current_h = 0;
	m_engine()->GetScreenSize(w, current_h);

	if (h != current_h)
	{
		if (h)
			update_scripts = true;

		h = current_h;
		update_dpi = true;
	}

	// animation related code
	static float m_alpha = 0.0002f;
	m_alpha = math::clamp(m_alpha + (3.f * ImGui::GetIO().DeltaTime * (is_open ? 1.f : -1.f)), 0.0001f, 1.f);

	// set alpha in class to use later in widgets
	public_alpha = m_alpha;

	if (m_alpha <= 0.0001f)
		return;

	// set new alpha
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);

	// setup colors and some styles
	if (!menu_setupped)
		menu_setup(ImGui::GetStyle());

	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].x, ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].y, ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].z, m_alpha));

	// default menu size
	const int x = 250, y = 120;

	ImGuiStyle* Style = &ImGui::GetStyle();
	Style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
	Style->Colors[ImGuiCol_WindowBg] = ImColor(30, 30, 30);
	Style->Colors[ImGuiCol_ChildBg] = ImColor(30, 30, 30);

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);

	Style->WindowBorderSize = 0;
	Style->WindowRounding = 0;
	Style->ChildRounding = 0;

	//auto Name = "Qwerty.xyz";
	ImGuiWindowFlags Flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
	ImGuiWindowFlags NoMove = ImGuiWindowFlags_NoMove;
	ImGuiWindowFlags NoScroll = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	static auto Active_Tab = 1;
	static auto Active_Rage_Tab = 1;
	static auto Active_Legit_Tab = 1;
	static auto Active_Visuals_Tab = 1;
	static auto Active_Changer_Tab = 1;
	static auto Active_Misc_Tab = 1;
	static auto Active_Movement_Tab = 1;

	if (Active_Tab != 1) Active_Rage_Tab = 1;
	if (Active_Tab != 2) Active_Legit_Tab = 1;
	if (Active_Tab != 3) Active_Visuals_Tab = 1;
	if (Active_Tab != 4) Active_Changer_Tab = 1;
	if (Active_Tab != 5) Active_Misc_Tab = 1;
	if (Active_Tab != 6) Active_Movement_Tab = 1;

	// last active tab to switch effect & reverse alpha & preview alpha
// IMPORTANT: DO TAB SWITCHING BY LAST_TAB!!!!!
	static int last_tab = Active_Tab;
	static bool preview_reverse = false;

	if (ImGui::Begin(crypt_str("Qwerty.xyz"), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground))
	{

	auto p = ImGui::GetWindowPos();

	// main dpi update logic
	// KEYWORD : DPI_FIND


	int child;
	auto player = config_system.g_cfg.player.teams;

	if (last_tab != active_tab || preview_reverse)
	{
		if (!preview_reverse)
		{
			if (preview_alpha == 1.f) //-V550
				preview_reverse = true;

			preview_alpha = math::clamp(preview_alpha + (4.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
		}
		else
		{
			last_tab = active_tab;
			if (preview_alpha == 0.01f) //-V550
			{
				preview_reverse = false;
			}

			preview_alpha = math::clamp(preview_alpha - (4.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
		}
	}
	else
		preview_alpha = math::clamp(preview_alpha - (4.f * ImGui::GetIO().DeltaTime), 0.0f, 1.f);


	Style->ChildRounding = 6;
	Style->WindowRounding = 6;

	ImGui::BeginChild("##Full", ImVec2{ 715, 640 }, false);
	{


		//ImGui::SetCursorPos(ImVec2{ -10, -15 });

		Style->Colors[ImGuiCol_ChildBg] = ImColor(78, 189, 253);

		ImGui::SetNextWindowSize(ImVec2{ 110, 650 });
		Style->WindowRounding = 10;
		Style->ChildRounding = 10;

		ImGui::Begin(crypt_str("Test"), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
		{
			ImGui::BeginChild("##Side", ImVec2{ 100, 640 }, false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{

				ImGui::SetCursorPos(ImVec2{ 5,	5 });
				//ImGui::Image(texture, ImVec2(900, 681));
				ImGui::Image(logotype, ImVec2(90.f, 90.f)); //Когда сделать картинку в байты тогда раскомменти, а то так краш будет
				//ImGui::Image(TextureImage, ImVec2(0.f, 0.f));
				ImGui::SetCursorPos(ImVec2{ 5, 30 });
				ImGui::SetCursorPos(ImVec2{ 50, 50 });
				//ImGui::PushFont(c_menu::get().futura_large); ImGui::TextColored(ImColor(255, 255, 255), "Qwerty.xyz"); ImGui::PopFont();

				//#if BETA
				//ImGui::SetCursorPos(ImVec2{ -200, 430 });
				//ImGui::PushFont(c_menu::get().futura); ImGui::TextColored(ImColor(255, 255, 255), "BETA"); ImGui::PopFont();
				//#endif

				ImGui::SetCursorPos(ImVec2{ 30, 80 });
				if (ImGui::Tab("1", "Rage", "Rage", ImVec2{ 85, 50 }, Active_Tab == 1 ? true : false))
					Active_Tab = 1;

				ImGui::SetCursorPos(ImVec2{ 30, 140 });
				if (ImGui::Tab("2", "Legit", "Legit", ImVec2{ 85, 50 }, Active_Tab == 2 ? true : false))
					Active_Tab = 2;

				ImGui::SetCursorPos(ImVec2{ 30, 200 });
				if (ImGui::Tab("3", "Visuals", "Visuals", ImVec2{ 85, 50 }, Active_Tab == 3 ? true : false))
					Active_Tab = 3;

				ImGui::SetCursorPos(ImVec2{ 30, 260 });
				if (ImGui::Tab("4", "Skins", "Skins", ImVec2{ 85, 50 }, Active_Tab == 4 ? true : false))
					Active_Tab = 4;

				ImGui::SetCursorPos(ImVec2{ 30, 320 });
				if (ImGui::Tab("5", "Misc", "Misc", ImVec2{ 85, 50 }, Active_Tab == 5 ? true : false))
					Active_Tab = 5;
				ImGui::SetCursorPos(ImVec2{ 30, 380 });
				if (ImGui::Tab("6", "Movement", "Beta", ImVec2{ 85, 50 }, Active_Tab == 6 ? true : false))
					Active_Tab = 6;
				ImGui::SetCursorPos(ImVec2{ 31, 500 });
				if (ImGui::Tab("7", "Settings", "Settings", ImVec2{ 85, 50 }, Active_Tab == 6 ? true : false))
					Active_Tab = 6;
				ImGui::SetCursorPos(ImVec2{ 30, 560 });
				if (ImGui::Tab("8", "User", "User", ImVec2{ 85, 50 }, Active_Tab == 6 ? true : false))
					Active_Tab = 6;
			}
			ImGui::EndChild();
		}
		ImGui::End();
		


		Style->ChildRounding = 5;

		Style->Colors[ImGuiCol_ChildBg] = ImColor(83, 86, 88);

		ImGui::SetCursorPos(ImVec2{ 25, 15 });

			if (Active_Tab == 1) // Rage | Đĺéäć
			{
				if (config_system.g_cfg.ragebot.enable)
				{


					auto cur_window = ImGui::GetCurrentWindow();
					ImVec2 w_pos = cur_window->Pos;
					ImGuiStyle* Style = &ImGui::GetStyle();

					Style->WindowRounding = 0;
					Style->WindowBorderSize = 1;
					Style->WindowMinSize = { 1,1 };
					bool theme = true;
					Style->Colors[ImGuiCol_WindowBg] = ImColor(19, 19, 19, 255);
					Style->Colors[ImGuiCol_Button] = ImColor(0, 0, 0, 0);
					Style->Colors[ImGuiCol_ButtonHovered] = ImColor(29, 125, 229, 0);
					Style->Colors[ImGuiCol_ButtonActive] = ImColor(29, 125, 229, 0);


					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.f);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

					ImGui::SetNextWindowSize({ 340, 570 });

					ImGui::Begin("##test", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
					{

						ImGui::Spacing();
						ImGui::PushFont(futura_large);
						ImGui::SetCursorPos({ 140, 5 });
						ImGui::TextColored(ImColor(14, 176, 255), "Hitboxes");
						ImGui::PopFont();
						ImGui::SetCursorPos({ 0, 10 });
						ImGui::Image(CTmd1, { 340,555 });

						//head
						{
							ImGui::SetCursorPos({ 164, 50 });
							if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(0))
							{
								if (ImGui::ImageButton((void*)head, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(0) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(0);
							}
							else
							{
								if (ImGui::ImageButton((void*)head1, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(0) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(0);
							}
						}
						//body
						{
							ImGui::SetCursorPos({ 164, 110 });
							if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(1))
							{
								if (ImGui::ImageButton((void*)upchest, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(1) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(1);
								}
							}
							else
							{
								if (ImGui::ImageButton((void*)upchest1, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(1) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(1);

								}
							}

							ImGui::SetCursorPos({ 164, 140 });
							if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(2))
							{
								if (ImGui::ImageButton((void*)chest, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(2) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(2);

								}
							}
							else
							{
								if (ImGui::ImageButton((void*)chest1, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(2) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(2);

								}
							}
							ImGui::SetCursorPos({ 164, 180 });
							if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(3))
							{
								if (ImGui::ImageButton((void*)lowchest, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{

									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(3) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(3);

								}
							}
							else
							{
								if (ImGui::ImageButton((void*)lowchest1, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{

									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(3) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(3);

								}
							}
							ImGui::SetCursorPos({ 164, 215 });
							if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(4))
							{
								if (ImGui::ImageButton((void*)neck, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{

									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(4) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(4);

								}
							}
							else
							{
								if (ImGui::ImageButton((void*)neck1, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{

									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(4) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(4);

								}
							}
							ImGui::SetCursorPos({ 164, 280 });
							if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(5))
							{
								if (ImGui::ImageButton((void*)pel, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{

									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(5) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(5);
								}
							}
							else
							{
								if (ImGui::ImageButton((void*)pel1, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{

									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(5) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(5);
								}
							}
						}

						//arms
						{
							ImGui::SetCursorPos({ 65, 200 });
							if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(6))
							{
								//left
								if (ImGui::ImageButton((void*)arms, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(6) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(6);

								ImGui::SetCursorPos({ 265, 200 });
								if (ImGui::ImageButton((void*)arms2, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(6) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(6);
							}
							else
							{
								//right
								if (ImGui::ImageButton((void*)arms1, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(6) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(6);
								ImGui::SetCursorPos({ 265, 200 });
								if (ImGui::ImageButton((void*)arms3, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(6) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(6);
							}
						}
						//legs
						{
							ImGui::SetCursorPos({ 135, 330 });
							if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(7))
							{
								if (ImGui::ImageButton((void*)legs, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(7) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(7);

								}
								ImGui::SetCursorPos({ 195, 330 });
								if (ImGui::ImageButton((void*)legs2, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(7) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(7);

								}
							}
							else
							{
								if (ImGui::ImageButton((void*)legs1, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(7) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(7);

								}
								ImGui::SetCursorPos({ 195, 330 });
								if (ImGui::ImageButton((void*)legs3, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{
									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(7) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(7);

								}
							}

							//feet
							ImGui::SetCursorPos({ 135, 480 });
							if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(8))
							{
								if (ImGui::ImageButton((void*)feet, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{

									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(8) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(8);
								}
								ImGui::SetCursorPos({ 195, 480 });
								if (ImGui::ImageButton((void*)feet2, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{

									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(8) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(8);
								}
							}
							else
							{
								if (ImGui::ImageButton((void*)feet1, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{


									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(8) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(8);
								}
								ImGui::SetCursorPos({ 195, 480 });
								if (ImGui::ImageButton((void*)feet3, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0)))
								{

									config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(8) = !config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes.at(8);
								}
							}
						}

					}
					ImGui::End();
					ImGui::PopStyleVar(3);
				}
				Style->ChildRounding = 5;
				ImGui::SetCursorPos(ImVec2{ 60, 25 });
				ImGui::BeginChild("##Main", ImVec2{ 655, 45 }, false, NoMove | ImGuiWindowFlags_NoBackground);
				{
					ImGui::SetCursorPos(ImVec2{ 15, 10 });
					if (ImGui::SubTab("	General", ImVec2(180, 25), Active_Rage_Tab == 1 ? true : false))
						Active_Rage_Tab = 1;

					ImGui::SetCursorPos(ImVec2{ 230, 10 });
					if (ImGui::SubTab("	Weapons", ImVec2(180, 25), Active_Rage_Tab == 2 ? true : false))
						Active_Rage_Tab = 2;

					ImGui::SetCursorPos(ImVec2{ 450, 10 });
					if (ImGui::SubTab("	Player list", ImVec2(180, 25), Active_Rage_Tab == 3 ? true : false))
						Active_Rage_Tab = 3;
				}
				ImGui::EndChild();
			}
			else if (Active_Tab == 2) // Legit | Ëĺăčň
			{
				
				auto cur_window = ImGui::GetCurrentWindow();
				ImVec2 w_pos = cur_window->Pos;
				ImGuiStyle* Style = &ImGui::GetStyle();

				Style->WindowRounding = 0;
				Style->WindowBorderSize = 1;
				Style->WindowMinSize = { 1,1 };
				bool theme = true;
				Style->Colors[ImGuiCol_WindowBg] = ImColor(19, 19, 19, 255);
				Style->Colors[ImGuiCol_Button] = ImColor(0, 0, 0, 0);
				Style->Colors[ImGuiCol_ButtonHovered] = ImColor(29, 125, 229, 0);
				Style->Colors[ImGuiCol_ButtonActive] = ImColor(29, 125, 229, 0);


				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

				ImGui::SetNextWindowSize({ 340, 570 });

				ImGui::Begin("##test", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
				{

					ImGui::Spacing();
					ImGui::PushFont(futura_large);
					ImGui::SetCursorPos({ 140, 5 });
					ImGui::TextColored(ImColor(14, 176, 255), "Hitboxes");
					ImGui::PopFont();
					ImGui::SetCursorPos({ 0, 10 });
					ImGui::Image(CTmd1, { 340,555 });
				};

				ImGui::End();
				ImGui::PopStyleVar(3);

				Style->ChildRounding = 5;
				ImGui::SetCursorPos(ImVec2{ 60, 25 });
				ImGui::BeginChild("##Main", ImVec2{ 655, 45 }, false, NoMove | ImGuiWindowFlags_NoBackground );
				{
					ImGui::SetCursorPos(ImVec2{ 15, 10 });
					if (ImGui::SubTab("	General", ImVec2(140, 25), Active_Legit_Tab == 1 ? true : false))
						Active_Legit_Tab = 1;

					ImGui::SetCursorPos(ImVec2{ 175, 10 });
					if (ImGui::SubTab("	RSC", ImVec2(140, 25), Active_Legit_Tab == 2 ? true : false))
						Active_Legit_Tab = 2;

					ImGui::SetCursorPos(ImVec2{ 335, 10 });
					if (ImGui::SubTab("	Trigger", ImVec2(140, 25), Active_Legit_Tab == 3 ? true : false))
						Active_Legit_Tab = 3;

					ImGui::SetCursorPos(ImVec2{ 495, 10 });
					if (ImGui::SubTab("	Others", ImVec2(140, 25), Active_Legit_Tab == 4 ? true : false))
						Active_Legit_Tab = 4;
				}
				ImGui::EndChild();
			}
			else if (Active_Tab == 3) // Visuals | Âčçóŕëű
			{
				auto cur_window = ImGui::GetCurrentWindow();
				ImVec2 w_pos = cur_window->Pos;
				ImGuiStyle* Style = &ImGui::GetStyle();

				Style->WindowRounding = 0;
				Style->WindowBorderSize = 1;
				Style->WindowMinSize = { 1,1 };
				bool theme = true;
				Style->Colors[ImGuiCol_WindowBg] = ImColor(19, 19, 19, 255);
				Style->Colors[ImGuiCol_Button] = ImColor(0, 0, 0, 0);
				Style->Colors[ImGuiCol_ButtonHovered] = ImColor(29, 125, 229, 0);
				Style->Colors[ImGuiCol_ButtonActive] = ImColor(29, 125, 229, 0);


				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

				ImGui::SetNextWindowSize({ 340, 570 });

				ImGui::Begin("##test", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
				{

					ImGui::Spacing();
					ImGui::PushFont(futura_large);
					ImGui::SetCursorPos({ 140, 5 });
					ImGui::TextColored(ImColor(14, 176, 255), "ESP Preview");
					ImGui::PopFont();
					ImGui::SetCursorPos({ 0, 10 });
					
					ImGui::SetCursorPos(ImVec2{ 5, 50 });
					VisualPreview();
				}
				ImGui::End();
				ImGui::PopStyleVar(3);

				

				enum esp_info_position
				{
					RIGHT,
					CENTER_DOWN,
					CENTER_UP
				};

				struct esp_info_s
				{
					esp_info_s(const std::string name, const ImColor color, const int position = RIGHT)
					{
						f_name = name; f_color = color; f_position = position;
					}
					std::string f_name;
					int f_position;
					ImColor f_color;
				};


				static std::vector<esp_info_s> info;

				static bool enabled = true;
				ImGui::SetNextWindowSize(ImVec2(280, 430));
				//ffs i dont find showborder here maybe u can find it on old imgui or i dont know where it is
				/*ImGui::Begin("ESP Preview", &enabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				{
					ImGui::SetCursorPos(ImVec2{ 5, 50 });
					ImGui::Image(esptype, ImVec2(280.f, 330.f));

					auto cur_window = ImGui::GetCurrentWindow();
					ImVec2 w_pos = cur_window->Pos;
					if (config_system.g_cfg.player.type->box)
					{
						//clear shit box
						cur_window->DrawList->AddRect(ImVec2(w_pos.x + 40, w_pos.y + 60), ImVec2(w_pos.x + 200, w_pos.y + 360), ImColor(config_system.g_cfg.player.type->box_color[0] * 255, config_system.g_cfg.player.type->box_color[1] * 255, config_system.g_cfg.player.type->box_color[2] * 255));//getrawcolor???
					}
					if (config_system.g_cfg.player.type->health)
					{
						cur_window->DrawList->AddRectFilled(ImVec2(w_pos.x + 34, w_pos.y + 60), ImVec2(w_pos.x + 37, w_pos.y + 360), ImColor(255, 255, 255, 255));//change this to green value im lazy to look hex color
					}
					if (config_system.g_cfg.player.type->name)
						info.emplace_back(esp_info_s("name", ImColor(255, 255, 255, 255), CENTER_UP));

					for (auto render : info)
					{
						auto text_size = ImGui::CalcTextSize(render.f_name.c_str());
						auto pos = ImVec2(w_pos.x + 205, w_pos.y + 60);
						if (render.f_position == CENTER_DOWN)
						{
							pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y + 315 - text_size.y);
						}
						else if (render.f_position == CENTER_UP)
						{
							pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y - 5 - text_size.y);
						}
						cur_window->DrawList->AddText(pos, render.f_color, render.f_name.c_str());//still no u32 ;D
					}


				}
				ImGui::End();
				info.clear();*/

				Style->ChildRounding = 5;
				ImGui::SetCursorPos(ImVec2{ 60, 25 });
				ImGui::BeginChild("##Main", ImVec2{ 655, 45 }, false, NoMove | ImGuiWindowFlags_NoBackground );
				{
					ImGui::SetCursorPos(ImVec2{ 15, 10 });
					if (ImGui::SubTab("ESP", ImVec2(90, 25), Active_Visuals_Tab == 1 ? true : false))
						Active_Visuals_Tab = 1;

					ImGui::SetCursorPos(ImVec2{ 115, 10 });
					if (ImGui::SubTab("Glow", ImVec2(90, 25), Active_Visuals_Tab == 2 ? true : false))
						Active_Visuals_Tab = 2;

					ImGui::SetCursorPos(ImVec2{ 215, 10 });
					if (ImGui::SubTab("Chams", ImVec2(90, 25), Active_Visuals_Tab == 3 ? true : false))
						Active_Visuals_Tab = 3;

					ImGui::SetCursorPos(ImVec2{ 315, 10 });
					if (ImGui::SubTab("Radar", ImVec2(90, 25), Active_Visuals_Tab == 4 ? true : false))
						Active_Visuals_Tab = 4;

					ImGui::SetCursorPos(ImVec2{ 415, 10 });
					if (ImGui::SubTab("World", ImVec2(90, 25), Active_Visuals_Tab == 6 ? true : false))
						Active_Visuals_Tab = 6;

					ImGui::SetCursorPos(ImVec2{ 515, 10 });
					if (ImGui::SubTab("Others", ImVec2(90, 25), Active_Visuals_Tab == 5 ? true : false))
						Active_Visuals_Tab = 5;
				}
				ImGui::EndChild();
			}
			else if (Active_Tab == 4) // Changers | ×ĺíäćĺđű (Číâĺíňŕđü, ďđîôčëü)
			{
				Style->ChildRounding = 5;
				ImGui::SetCursorPos(ImVec2{ 60, 25 });
				ImGui::BeginChild("##Main", ImVec2{ 655, 45 }, false, NoMove | ImGuiWindowFlags_NoBackground );
				{
					ImGui::SetCursorPos(ImVec2{ 15, 10 });
					if (ImGui::SubTab(" Inventory", ImVec2(180, 25), Active_Changer_Tab == 1 ? true : false))
						Active_Changer_Tab = 1;

					ImGui::SetCursorPos(ImVec2{ 230, 10 });
					if (ImGui::SubTab(" Profile", ImVec2(180, 25), Active_Changer_Tab == 2 ? true : false))
						Active_Changer_Tab = 2;

					ImGui::SetCursorPos(ImVec2{ 450, 10 });
					if (ImGui::SubTab(" Character", ImVec2(180, 25), Active_Changer_Tab == 3 ? true : false))
						Active_Changer_Tab = 3;
				}
				ImGui::EndChild();
			}
			else if (Active_Tab == 5) // Misc | Ěčńę
			{
				Style->ChildRounding = 5;
				ImGui::SetCursorPos(ImVec2{ 60, 25 });
				ImGui::BeginChild("##Main", ImVec2{ 655, 45 }, false, NoMove | ImGuiWindowFlags_NoBackground );
				{
					ImGui::SetCursorPos(ImVec2{ 15, 10 });
					if (ImGui::SubTab(" Main", ImVec2(140, 25), Active_Misc_Tab == 1 ? true : false))
						Active_Misc_Tab = 1;

					ImGui::SetCursorPos(ImVec2{ 175, 10 });
					if (ImGui::SubTab(" Misc", ImVec2(140, 25), Active_Misc_Tab == 2 ? true : false))
						Active_Misc_Tab = 2;

					ImGui::SetCursorPos(ImVec2{ 335, 10 });
					if (ImGui::SubTab("	Configs", ImVec2(140, 25), Active_Misc_Tab == 3 ? true : false))
						Active_Misc_Tab = 3;

					ImGui::SetCursorPos(ImVec2{ 495, 10 });
					if (ImGui::SubTab(" Lua", ImVec2(140, 25), Active_Misc_Tab == 4 ? true : false))
						Active_Misc_Tab = 4;
				}
				ImGui::EndChild();
			}
			else if (Active_Tab == 6)
			{
				Style->ChildRounding = 5;
				ImGui::SetCursorPos(ImVec2{ 60, 25 });
				ImGui::BeginChild("##Main", ImVec2{ 655, 45 }, false, NoMove | ImGuiWindowFlags_NoBackground );
				{
					ImGui::SetCursorPos(ImVec2{ 15, 10 });
					if (ImGui::SubTab("	Movement Recorder", ImVec2(180, 25), Active_Movement_Tab == 1 ? true : false))
						Active_Movement_Tab = 1;

					ImGui::SetCursorPos(ImVec2{ 230, 10 });
					if (ImGui::SubTab("	Grenade Helper", ImVec2(180, 25), Active_Movement_Tab == 2 ? true : false))
						Active_Movement_Tab = 2;

					ImGui::SetCursorPos(ImVec2{ 450, 10 });
					if (ImGui::SubTab("	Information", ImVec2(180, 25), Active_Movement_Tab == 3 ? true : false))
						Active_Movement_Tab = 3;
				}
				ImGui::EndChild();
			}
			ImGui::SetCursorPos(ImVec2{ 75, 100 });
			Style->ChildRounding = 10;
			
			Style->Colors[ImGuiCol_ChildBg] = ImColor(30, 30, 30);
			ImGui::BeginChild("##Draw Function", ImVec2{650, 540}, false, NoScroll);
			{
				ImGui::SetCursorPos(ImVec2{ 0, 0 });
				ImGui::BeginChild("##Function", ImVec2{ 650, 530 }, false), NoScroll;
				{
					if (Active_Tab == 1 && Active_Rage_Tab == 1) //Rage | General
					{
						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("it3ems", { 300, 530 });
						{

							ImGui::SetCursorPos({ 2,2 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Ragebot"));

								ImGui::Checkbox(crypt_str("Enabled"), &config_system.g_cfg.ragebot.enable);

								if (config_system.g_cfg.ragebot.enable)
									config_system.g_cfg.legitbot.enabled = false;

								ImGui::SliderInt(crypt_str("Maximum FOV"), &config_system.g_cfg.ragebot.field_of_view, 1, 180, false, crypt_str("%d°"));
								ImGui::Checkbox(crypt_str("Silent aim"), &config_system.g_cfg.ragebot.silent_aim);
								ImGui::Checkbox(crypt_str("Automatic penetration"), &config_system.g_cfg.ragebot.autowall);
								ImGui::Checkbox(crypt_str("Automatic zeus"), &config_system.g_cfg.ragebot.zeus_bot);
								ImGui::Checkbox(crypt_str("Automatic knife"), &config_system.g_cfg.ragebot.knife_bot);
								ImGui::Checkbox(crypt_str("Automatic fire"), &config_system.g_cfg.ragebot.autoshoot);
								ImGui::Checkbox(crypt_str("Automatic scope"), &config_system.g_cfg.ragebot.autoscope);
								ImGui::Checkbox(crypt_str("Pitch desync correction"), &config_system.g_cfg.ragebot.pitch_antiaim_correction);

								ImGui::Spacing();
								ImGui::Checkbox(crypt_str("Double tap"), &config_system.g_cfg.ragebot.double_tap);

								if (config_system.g_cfg.ragebot.double_tap)
								{
									ImGui::SameLine();
									draw_keybind(crypt_str(""), &config_system.g_cfg.ragebot.double_tap_key, crypt_str("##HOTKEY_DT"));
									ImGui::Checkbox(crypt_str("Slow teleport"), &config_system.g_cfg.ragebot.slow_teleport);
								}

								ImGui::Checkbox(crypt_str("Hide shots"), &config_system.g_cfg.antiaim.hide_shots);

								if (config_system.g_cfg.antiaim.hide_shots)
								{
									ImGui::SameLine();
									draw_keybind(crypt_str(""), &config_system.g_cfg.antiaim.hide_shots_key, crypt_str("##HOTKEY_HIDESHOTS"));
								}
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::NextColumn();
						ImGui::BeginChild("item6s2", { 300, 530 });
						{

							ImGui::SetCursorPos({ 0,0 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Anti-aim"));

								static auto type = 0;

								ImGui::Checkbox(crypt_str("Enabled"), &config_system.g_cfg.antiaim.enable);
								draw_combo(crypt_str("Anti-aim type"), config_system.g_cfg.antiaim.antiaim_type, antiaim_type, ARRAYSIZE(antiaim_type));

								if (config_system.g_cfg.antiaim.antiaim_type)
								{
									padding(0, 3);
									draw_combo(crypt_str("Desync"), config_system.g_cfg.antiaim.desync, desync, ARRAYSIZE(desync));

									if (config_system.g_cfg.antiaim.desync)
									{
										padding(0, 3);
										draw_combo(crypt_str("LBY type"), config_system.g_cfg.antiaim.legit_lby_type, lby_type, ARRAYSIZE(lby_type));

										if (config_system.g_cfg.antiaim.desync == 1)
										{
											draw_keybind(crypt_str("Invert desync"), &config_system.g_cfg.antiaim.flip_desync, crypt_str("##HOTKEY_INVERT_DESYNC"));
											ImGui::Checkbox(crypt_str("Side Arrow"), &config_system.g_cfg.antiaim.sidearrow);
											ImGui::SameLine();
											ImGui::ColorEdit(crypt_str("##invd"), &config_system.g_cfg.antiaim.invert_indicator_color, ALPHA);
										}
									}
								}
								else
								{
									draw_combo(crypt_str("Movement type"), type, movement_type, ARRAYSIZE(movement_type));
									padding(0, 3);
									draw_combo(crypt_str("Pitch"), config_system.g_cfg.antiaim.type[type].pitch, pitch, ARRAYSIZE(pitch));
									padding(0, 3);
									draw_combo(crypt_str("Yaw"), config_system.g_cfg.antiaim.type[type].yaw, yaw, ARRAYSIZE(yaw));
									padding(0, 3);
									draw_combo(crypt_str("Base angle"), config_system.g_cfg.antiaim.type[type].base_angle, baseangle, ARRAYSIZE(baseangle));

									if (config_system.g_cfg.antiaim.type[type].yaw)
									{
										ImGui::SliderInt(config_system.g_cfg.antiaim.type[type].yaw == 1 ? crypt_str("Jitter range") : crypt_str("Spin range"), &config_system.g_cfg.antiaim.type[type].range, 1, 180);

										if (config_system.g_cfg.antiaim.type[type].yaw == 2)
											ImGui::SliderInt(crypt_str("Spin speed"), &config_system.g_cfg.antiaim.type[type].speed, 1, 15);

										padding(0, 3);
									}

									draw_combo(crypt_str("Desync"), config_system.g_cfg.antiaim.type[type].desync, desync, ARRAYSIZE(desync));

									if (config_system.g_cfg.antiaim.type[type].desync)
									{
										if (type == ANTIAIM_STAND)
										{
											padding(0, 3);
											draw_combo(crypt_str("LBY type"), config_system.g_cfg.antiaim.lby_type, lby_type, ARRAYSIZE(lby_type));
										}

										if (type != ANTIAIM_STAND || !config_system.g_cfg.antiaim.lby_type)
										{
											ImGui::SliderInt(crypt_str("Fake amount"), &config_system.g_cfg.antiaim.type[type].desync_range, 1, 60);
											ImGui::SliderInt(crypt_str("Inverted fake amount"), &config_system.g_cfg.antiaim.type[type].inverted_desync_range, 1, 60);
											ImGui::SliderInt(crypt_str("Body lean"), &config_system.g_cfg.antiaim.type[type].body_lean, 0, 100);
											ImGui::SliderInt(crypt_str("Inverted body lean"), &config_system.g_cfg.antiaim.type[type].inverted_body_lean, 0, 100);
										}

										if (config_system.g_cfg.antiaim.type[type].desync == 1)
										{
											draw_keybind(crypt_str("Invert desync"), &config_system.g_cfg.antiaim.flip_desync, crypt_str("##HOTKEY_INVERT_DESYNC"));
										}
									}

									draw_keybind(crypt_str("Manual back"), &config_system.g_cfg.antiaim.manual_back, crypt_str("##HOTKEY_INVERT_BACK"));

									draw_keybind(crypt_str("Manual left"), &config_system.g_cfg.antiaim.manual_left, crypt_str("##HOTKEY_INVERT_LEFT"));

									draw_keybind(crypt_str("Manual right"), &config_system.g_cfg.antiaim.manual_right, crypt_str("##HOTKEY_INVERT_RIGHT"));

									if (config_system.g_cfg.antiaim.manual_back.key > KEY_NONE && config_system.g_cfg.antiaim.manual_back.key < KEY_MAX || config_system.g_cfg.antiaim.manual_left.key > KEY_NONE && config_system.g_cfg.antiaim.manual_left.key < KEY_MAX || config_system.g_cfg.antiaim.manual_right.key > KEY_NONE && config_system.g_cfg.antiaim.manual_right.key < KEY_MAX)
									{
										ImGui::Checkbox(crypt_str("Manual indicator"), &config_system.g_cfg.antiaim.flip_indicator);
										ImGui::SameLine();
										ImGui::ColorEdit(crypt_str("##invc"), &config_system.g_cfg.antiaim.flip_indicator_color, ALPHA);
									}
								}



							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::Columns(1, nullptr, false);
					}
					else if (Active_Tab == 1 && Active_Rage_Tab == 2) //Rage | Weapons
					{
						const char* rage_weapons[8] = { crypt_str("Revolver / Deagle"), crypt_str("Pistols"), crypt_str("SMGs"), crypt_str("Rifles"), crypt_str("Auto"), crypt_str("Scout"), crypt_str("AWP"), crypt_str("Heavy") };


						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("it3ems", { 300, 530 });
						{

							ImGui::SetCursorPos({ 2,2 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("General"));


								ImGui::Spacing();

								draw_combo(crypt_str("Current weapon"), hooks::rage_weapon, rage_weapons, ARRAYSIZE(rage_weapons));
								ImGui::Spacing();

								draw_combo(crypt_str("Target selection"), config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].selection_type, selection, ARRAYSIZE(selection));
								padding(0, 3);
								draw_multicombo(crypt_str("Hitboxes"), config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes, hitboxes, ARRAYSIZE(hitboxes), preview);

								ImGui::Checkbox(crypt_str("Static point scale"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale);

								if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale)
								{
									ImGui::SliderFloat(crypt_str("Head scale"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale, 0.0f, 1.0f, config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale ? crypt_str("%.2f") : crypt_str("None"));
									ImGui::SliderFloat(crypt_str("Body scale"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale, 0.0f, 1.0f, config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale ? crypt_str("%.2f") : crypt_str("None"));
								}


								ImGui::Checkbox(crypt_str("Enable max misses"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses);

								if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses)
									ImGui::SliderInt(crypt_str("Max misses"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses_amount, 0, 6);

								ImGui::Checkbox(crypt_str("Prefer safe points"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].prefer_safe_points);
								ImGui::Checkbox(crypt_str("Prefer body aim"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].prefer_body_aim);

								draw_keybind(crypt_str("Force safe points"), &config_system.g_cfg.ragebot.safe_point_key, crypt_str("##HOKEY_FORCE_SAFE_POINTS"));
								draw_keybind(crypt_str("Force body aim"), &config_system.g_cfg.ragebot.body_aim_key, crypt_str("##HOKEY_FORCE_BODY_AIM"));

								ImGui::SliderInt(crypt_str("Minimum visible damage"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_visible_damage, 1, 120, true);

								if (config_system.g_cfg.ragebot.autowall)
									ImGui::SliderInt(crypt_str("Minimum penetration damage"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_damage, 1, 120, true);

								draw_keybind(crypt_str("Damage override"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key, crypt_str("##HOTKEY__DAMAGE_OVERRIDE"));

								if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key > KEY_NONE && config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key < KEY_MAX)
									ImGui::SliderInt(crypt_str("Minimum override damage"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_override_damage, 1, 120, true);
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::NextColumn();
						ImGui::BeginChild("item6s2", { 300, 530 });
						{

							ImGui::SetCursorPos({ 0,0 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Accuracy"));


								ImGui::Checkbox(crypt_str("Automatic stop"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].autostop);

								if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].autostop)
									draw_multicombo(crypt_str("Modifiers"), config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].autostop_modifiers, autostop_modifiers, ARRAYSIZE(autostop_modifiers), preview);

								ImGui::Checkbox(crypt_str("Hit chance"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance);

								if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance)
									ImGui::SliderInt(crypt_str("Minimum hit chance"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance_amount, 1, 100);

								if (config_system.g_cfg.ragebot.double_tap && hooks::rage_weapon <= 4)
								{
									ImGui::Checkbox(crypt_str("Double tap hit chance"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance);

									if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance)
										ImGui::SliderInt(crypt_str("Double tap hit chance"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance_amount, 1, 100);
								}
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::Columns(1, nullptr, false);
					}
					else if (Active_Tab == 1 && Active_Rage_Tab == 3) //Rage | Anti-Aims
					{
						auto player = players_section;

						static std::vector <Player_list_data> players;

						if (!config_system.g_cfg.player_list.refreshing)
						{
							players.clear();

							for (auto player : config_system.g_cfg.player_list.players)
								players.emplace_back(player);
						}

						static auto current_player = 0;
						ImGui::Columns(2, nullptr, false);
						child_title(crypt_str("Players"));
						ImGui::BeginChild("it3ems", { 300, 530 });
						{

							ImGui::SetCursorPos({ 1,1 });
							ImGui::BeginGroup();
							{
								if (!players.empty())
								{
									std::vector <std::string> player_names;

									for (auto player : players)
										player_names.emplace_back(player.name);

									ImGui::PushItemWidth(291 * dpi_scale);
									ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
									ImGui::ListBoxConfigArray(crypt_str("##PLAYERLIST"), &current_player, player_names, 14);
									ImGui::PopStyleVar();
									ImGui::PopItemWidth();
								}
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::NextColumn();
						child_title(crypt_str("Settings"));
						ImGui::BeginChild("item6s2", { 300, 530 });
						{

							ImGui::SetCursorPos({ 10,10 });
							ImGui::BeginGroup();
							{

								if (!players.empty())
								{
									if (current_player >= players.size())
										current_player = players.size() - 1; //-V103

									ImGui::Checkbox(crypt_str("White list"), &config_system.g_cfg.player_list.white_list[players.at(current_player).i]); //-V106 //-V807

									if (!config_system.g_cfg.player_list.white_list[players.at(current_player).i]) //-V106
									{
										ImGui::Checkbox(crypt_str("High priority"), &config_system.g_cfg.player_list.high_priority[players.at(current_player).i]); //-V106
										ImGui::Checkbox(crypt_str("Force safe points"), &config_system.g_cfg.player_list.force_safe_points[players.at(current_player).i]); //-V106
										ImGui::Checkbox(crypt_str("Force body aim"), &config_system.g_cfg.player_list.force_body_aim[players.at(current_player).i]); //-V106
										ImGui::Checkbox(crypt_str("Low delta"), &config_system.g_cfg.player_list.low_delta[players.at(current_player).i]); //-V106
									}
								}



							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::Columns(1, nullptr, false);
					}
					if (Active_Tab == 2 && Active_Legit_Tab == 1) //Legit | General
					{
						const char* legit_weapons[6] = { crypt_str("Deagle"), crypt_str("Pistols"), crypt_str("Rifles"), crypt_str("SMGs"), crypt_str("Snipers"), crypt_str("Heavy") };
						const char* hitbox_legit[3] = { crypt_str("Nearest"), crypt_str("Head"), crypt_str("Body") };

						ImGui::Columns(2, nullptr, false);
						child_title(crypt_str("Legit"));
						ImGui::BeginChild("it3emsl", { 300, 530 });
						{

							ImGui::SetCursorPos({ 1,1 });
							ImGui::BeginGroup();
							{
								padding(0, 6);
								ImGui::Checkbox(crypt_str("Enabled"), &config_system.g_cfg.legitbot.enabled);
								ImGui::SameLine();
								draw_keybind(crypt_str(""), &config_system.g_cfg.legitbot.key, crypt_str("##HOTKEY_LGBT_KEY"));

								if (config_system.g_cfg.legitbot.enabled)
									config_system.g_cfg.ragebot.enable = false;

								ImGui::Checkbox(crypt_str("Friendly fire"), &config_system.g_cfg.legitbot.friendly_fire);
								ImGui::Checkbox(crypt_str("Automatic pistols"), &config_system.g_cfg.legitbot.autopistol);

								ImGui::Checkbox(crypt_str("Automatic scope"), &config_system.g_cfg.legitbot.autoscope);

								if (config_system.g_cfg.legitbot.autoscope)
									ImGui::Checkbox(crypt_str("Unscope after shot"), &config_system.g_cfg.legitbot.unscope);

								ImGui::Checkbox(crypt_str("Snipers in zoom only"), &config_system.g_cfg.legitbot.sniper_in_zoom_only);

								ImGui::Checkbox(crypt_str("Aim if in air"), &config_system.g_cfg.legitbot.do_if_local_in_air);
								ImGui::Checkbox(crypt_str("Aim if flashed"), &config_system.g_cfg.legitbot.do_if_local_flashed);
								ImGui::Checkbox(crypt_str("Aim thru smoke"), &config_system.g_cfg.legitbot.do_if_enemy_in_smoke);
								ImGui::Checkbox(crypt_str("Backtracking"), &config_system.g_cfg.legitbot.backtrackl);
								ImGui::Checkbox(crypt_str("Legit Resolver"), &config_system.g_cfg.legitbot.legit_resolver);
								draw_keybind(crypt_str("Automatic fire key"), &config_system.g_cfg.legitbot.autofire_key, crypt_str("##HOTKEY_AUTOFIRE_LGBT_KEY"));
								ImGui::SliderInt(crypt_str("Automatic fire delay"), &config_system.g_cfg.legitbot.autofire_delay, 0, 12, false, (!config_system.g_cfg.legitbot.autofire_delay ? crypt_str("None") : (config_system.g_cfg.legitbot.autofire_delay == 1 ? crypt_str("%d tick") : crypt_str("%d ticks"))));
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::NextColumn();
						child_title(crypt_str("Settings"));
						ImGui::BeginChild("item6s2l2", { 300, 530 });
						{

							ImGui::SetCursorPos({ 10,10 });
							ImGui::BeginGroup();
							{
								//	ImGui::Spacing();
								draw_combo(crypt_str("Current weapon"), hooks::legit_weapon, legit_weapons, ARRAYSIZE(legit_weapons));
								ImGui::Spacing();
								draw_combo(crypt_str("Hitbox"), config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].priority, hitbox_legit, ARRAYSIZE(hitbox_legit));

								ImGui::Checkbox(crypt_str("Automatic stop"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].auto_stop);

								draw_combo(crypt_str("Maximum FOV type"), config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].fov_type, LegitFov, ARRAYSIZE(LegitFov));
								ImGui::SliderFloat(crypt_str("Maximum FOV amount"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].fov, 0.f, 30.f, crypt_str("%.2f"));

								ImGui::Spacing();

								ImGui::SliderFloat(crypt_str("Silent FOV"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov, 0.f, 30.f, (!config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov ? crypt_str("None") : crypt_str("%.2f"))); //-V550

								ImGui::Spacing();

								draw_combo(crypt_str("Smooth type"), config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].smooth_type, LegitSmooth, ARRAYSIZE(LegitSmooth));
								ImGui::SliderFloat(crypt_str("Smooth amount"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].smooth, 1.f, 12.f, crypt_str("%.1f"));

								ImGui::Spacing();

								ImGui::SliderInt(crypt_str("Automatic wall damage"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg, 0, 100, false, (!config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg ? crypt_str("None") : crypt_str("%d")));
								ImGui::SliderInt(crypt_str("Automatic fire hitchance"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance, 0, 100, false, (!config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance ? crypt_str("None") : crypt_str("%d")));
								ImGui::SliderFloat(crypt_str("Target switch delay"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].target_switch_delay, 0.f, 1.f);
								ImGui::SliderFloat(crypt_str("Backtrack ticks"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].backtrack_ticks, 0, 12);



							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::Columns(1, nullptr, false);
					}
					else if (Active_Tab == 2 && Active_Legit_Tab == 2) //Legit | RSC
					{
						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("it3ems", { 300, 530 });
						{
							const char* legit_weapons[6] = { crypt_str("Deagle"), crypt_str("Pistols"), crypt_str("Rifles"), crypt_str("SMGs"), crypt_str("Snipers"), crypt_str("Heavy") };
							const char* hitbox_legit[3] = { crypt_str("Nearest"), crypt_str("Head"), crypt_str("Body") };

							ImGui::SetCursorPos({ 2,2 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Recoil Control System"));

								draw_combo(crypt_str("Current weapon"), hooks::legit_weapon, legit_weapons, ARRAYSIZE(legit_weapons));

								draw_combo(crypt_str("RCS type"), config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].rcs_type, RCSType, ARRAYSIZE(RCSType));
								ImGui::SliderFloat(crypt_str("RCS amount"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].rcs, 0.f, 100.f, crypt_str("%.0f%%"), 1.f);

								if (config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].rcs > 0)
								{
									ImGui::SliderFloat(crypt_str("RCS custom FOV"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov, 0.f, 30.f, (!config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov ? crypt_str("None") : crypt_str("%.2f"))); //-V550
									ImGui::SliderFloat(crypt_str("RCS Custom smooth"), &config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth, 0.f, 12.f, (!config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth ? crypt_str("None") : crypt_str("%.1f"))); //-V550
								}

							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::NextColumn();
						ImGui::BeginChild("item6s2", { 300, 530 });
						{

							ImGui::SetCursorPos({ 0,0 });
							ImGui::BeginGroup();
							{

							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::Columns(1, nullptr, false);
					
					}
					else if (Active_Tab == 2 && Active_Legit_Tab == 3) //Legit | Trigger
					{
					ImGui::Columns(2, nullptr, false);
					child_title(crypt_str("Recorder"));
					ImGui::BeginChild("it3ems", { 300, 530 });
					{
						padding(8, 8);

					}
					ImGui::EndChild();

					}
					else if (Active_Tab == 2 && Active_Legit_Tab == 4) //Legit | Others
					{
					static int map = 0;
					ImGui::Checkbox(crypt_str("Blockbot"), &config_system.g_cfg.misc.blockbot_enabled);

					if (config_system.g_cfg.misc.blockbot_enabled == 1)
					{
						draw_keybind(crypt_str("Blockbot key"), &config_system.g_cfg.misc.block_bot, crypt_str("##BLOCKBOT__HOTKEY"));
						padding(10, 0);
						draw_combo(crypt_str("Blockbot type"), config_system.g_cfg.misc.blockbot_type, blockbot_type_cb, ARRAYSIZE(blockbot_type_cb));
					}

					ImGui::Checkbox(crypt_str("Force Crosshair"), &config_system.g_cfg.misc.forcecrosshair);
					

					ImGui::Checkbox("Grenade helper", &config_system.g_cfg.grenadehelper.enable); padding(8, 2);

					ImGui::Combo("Choose the map", &map, "de_mirage\0de_inferno\0de_overpass\0de_dust2\0de_cache\0de_nuke\0de_cbble\0de_train"); padding(8, 2);
					if (map == 0)
					{
						static char pos_name[256] = "";
						ImGui::InputText("Put name here", pos_name, 256);
						std::string opt_pos_name = pos_name;

						padding(8, 2); if (ImGui::CustomButton("Add new pos", "##1"))
						{
							if (!m_engine()->IsInGame())
								return;

							mirage new_str;
							new_str.info = opt_pos_name;
							new_str.pos = g_ctx.local()->GetAbsOrigin();
							new_str.ang = g_ctx.local()->get_shoot_position();
							m_engine()->GetViewAngles(new_str.ang);

							config_system.g_cfg.grenadehelper.MirageGrenade.push_back(new_str);
						}
					}
					if (map == 1)
					{
						static char pos_name[256] = "";
						ImGui::InputText("Put name here", pos_name, 256);
						std::string opt_pos_name = pos_name;

						padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
						{
							if (!m_engine()->IsInGame())
								return;

							inferno new_str;
							new_str.info = opt_pos_name;
							new_str.pos = g_ctx.local()->GetAbsOrigin();
							new_str.ang = g_ctx.local()->get_shoot_position();
							m_engine()->GetViewAngles(new_str.ang);

							config_system.g_cfg.grenadehelper.InfernoGrenade.push_back(new_str);
						}
					}

					if (map == 7)
					{
						static char pos_name[256] = "";
						ImGui::InputText("Put name here", pos_name, 256);
						std::string opt_pos_name = pos_name;

						padding(8, 2); if (ImGui::CustomButton("Add new pos", "##1"))
						{
							if (!m_engine()->IsInGame())
								return;

							train new_str;
							new_str.info = opt_pos_name;
							new_str.pos = g_ctx.local()->GetAbsOrigin();
							new_str.ang = g_ctx.local()->get_shoot_position();
							m_engine()->GetViewAngles(new_str.ang);

							config_system.g_cfg.grenadehelper.trainGrenade.push_back(new_str);
						}
					}

					if (map == 3)
					{
						static char pos_name[256] = "";
						ImGui::InputText("Put name here", pos_name, 256);
						std::string opt_pos_name = pos_name;

						padding(8, 2); if (ImGui::CustomButton("Add new pos", "##1"))
						{
							if (!m_engine()->IsInGame())
								return;

							dust2 new_str;
							new_str.info = opt_pos_name;
							new_str.pos = g_ctx.local()->GetAbsOrigin();
							new_str.ang = g_ctx.local()->get_shoot_position();
							m_engine()->GetViewAngles(new_str.ang);

							config_system.g_cfg.grenadehelper.dust2Grenade.push_back(new_str);
						}
					}
					if (map == 2)
					{
						static char pos_name[256] = "";
						ImGui::InputText("Put name here", pos_name, 256);
						std::string opt_pos_name = pos_name;

						padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
						{
							if (!m_engine()->IsInGame())
								return;

							overpass new_str;
							new_str.info = opt_pos_name;
							new_str.pos = g_ctx.local()->GetAbsOrigin();
							new_str.ang = g_ctx.local()->get_shoot_position();
							m_engine()->GetViewAngles(new_str.ang);

							config_system.g_cfg.grenadehelper.overpassGrenade.push_back(new_str);
						}
					}
					if (map == 5)
					{
						static char pos_name[256] = "";
						ImGui::InputText("Put name here", pos_name, 256);
						std::string opt_pos_name = pos_name;

						padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
						{
							if (!m_engine()->IsInGame())
								return;

							nuke new_str;
							new_str.info = opt_pos_name;
							new_str.pos = g_ctx.local()->GetAbsOrigin();
							new_str.ang = g_ctx.local()->get_shoot_position();
							m_engine()->GetViewAngles(new_str.ang);

							config_system.g_cfg.grenadehelper.nukeGrenade.push_back(new_str);
						}
					}
					if (map == 4)
					{
						static char pos_name[256] = "";
						ImGui::InputText("Put name here", pos_name, 256);
						std::string opt_pos_name = pos_name;

						padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
						{
							if (!m_engine()->IsInGame())
								return;

							cache new_str;
							new_str.info = opt_pos_name;
							new_str.pos = g_ctx.local()->GetAbsOrigin();
							new_str.ang = g_ctx.local()->get_shoot_position();
							m_engine()->GetViewAngles(new_str.ang);

							config_system.g_cfg.grenadehelper.cacheGrenade.push_back(new_str);
						}
					}
					if (map == 6)
					{
						static char pos_name[256] = "";
						ImGui::InputText("Put name here", pos_name, 256);
						std::string opt_pos_name = pos_name;

						padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
						{
							if (!m_engine()->IsInGame())
								return;

							cbble new_str;
							new_str.info = opt_pos_name;
							new_str.pos = g_ctx.local()->GetAbsOrigin();
							new_str.ang = g_ctx.local()->get_shoot_position();
							m_engine()->GetViewAngles(new_str.ang);

							config_system.g_cfg.grenadehelper.cbbleGrenade.push_back(new_str);
						}
					}
					
					}
					if (Active_Tab == 3 && Active_Visuals_Tab == 1) //Visuals | ESP
					{

						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("it3ems", { 300, 530 });
						{

							ImGui::SetCursorPos({ 2,2 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Players"));

								//tab_start();

								ImGui::Checkbox(crypt_str("Enabled"), &config_system.g_cfg.player.enable);
								draw_combo(crypt_str("Set team"), config_system.g_cfg.player.teams, player_teams, ARRAYSIZE(player_teams));

								if (player == 0 || ENEMY)
								{
									ImGui::Checkbox(crypt_str("OOF arrows"), &config_system.g_cfg.player.arrows);
									ImGui::SameLine(100); ImGui::ColorEdit4("##fov_arrow", &config_system.g_cfg.player.arrows_color); padding(8, 2);

									if (config_system.g_cfg.player.arrows)
									{
										ImGui::SliderInt(crypt_str("Arrows distance"), &config_system.g_cfg.player.distance, 1, 100);
										ImGui::SliderInt(crypt_str("Arrows size"), &config_system.g_cfg.player.size, 1, 100);
									}
								}

								ImGui::Checkbox(crypt_str("Bounding box"), &config_system.g_cfg.player.type[player].box);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##boxcolor"), &config_system.g_cfg.player.type[player].box_color, ALPHA);

								ImGui::Checkbox(crypt_str("Name"), &config_system.g_cfg.player.type[player].name);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##namecolor"), &config_system.g_cfg.player.type[player].name_color, ALPHA);

								ImGui::Checkbox(crypt_str("Health bar"), &config_system.g_cfg.player.type[player].health);
								ImGui::Checkbox(crypt_str("Override health color"), &config_system.g_cfg.player.type[player].custom_health_color);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##healthcolor"), &config_system.g_cfg.player.type[player].health_color, ALPHA);

								for (auto i = 0, j = 0; i < ARRAYSIZE(flags); i++)
								{
									if (config_system.g_cfg.player.type[player].flags[i])
									{
										if (j)
											preview += crypt_str(", ") + (std::string)flags[i];
										else
											preview = flags[i];

										j++;
									}
								}

								draw_multicombo(crypt_str("Flags"), config_system.g_cfg.player.type[player].flags, flags, ARRAYSIZE(flags), preview);
								draw_multicombo(crypt_str("Weapon"), config_system.g_cfg.player.type[player].weapon, weaponplayer, ARRAYSIZE(weaponplayer), preview);


								if (config_system.g_cfg.player.type[player].weapon[WEAPON_ICON] || config_system.g_cfg.player.type[player].weapon[WEAPON_TEXT])
								{
									ImGui::Text(crypt_str("Color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##weapcolor"), &config_system.g_cfg.player.type[player].weapon_color, ALPHA);
								}

								ImGui::Checkbox(crypt_str("Skeleton"), &config_system.g_cfg.player.type[player].skeleton);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##skeletoncolor"), &config_system.g_cfg.player.type[player].skeleton_color, ALPHA);

								ImGui::Checkbox(crypt_str("Ammo bar"), &config_system.g_cfg.player.type[player].ammo);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##ammocolor"), &config_system.g_cfg.player.type[player].ammobar_color, ALPHA);

								ImGui::Checkbox(crypt_str("Foot steps"), &config_system.g_cfg.player.type[player].footsteps);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##footstepscolor"), &config_system.g_cfg.player.type[player].footsteps_color, ALPHA);

								if (config_system.g_cfg.player.type[player].footsteps)
								{
									ImGui::SliderInt(crypt_str("Thickness"), &config_system.g_cfg.player.type[player].thickness, 1, 10);
									ImGui::SliderInt(crypt_str("Radius"), &config_system.g_cfg.player.type[player].radius, 50, 500);
								}

								if (player == 0 || ENEMY || player == 1 || TEAM)
								{
									ImGui::Checkbox(crypt_str("Snap lines"), &config_system.g_cfg.player.type[player].snap_lines);
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##snapcolor"), &config_system.g_cfg.player.type[player].snap_lines_color, ALPHA);

									if (player == 0)
									{
										if (config_system.g_cfg.ragebot.enable)
										{
											ImGui::Checkbox(crypt_str("Aimbot points"), &config_system.g_cfg.player.show_multi_points);
											ImGui::SameLine();
											ImGui::ColorEdit(crypt_str("##showmultipointscolor"), &config_system.g_cfg.player.show_multi_points_color, ALPHA);
										}

										ImGui::Checkbox(crypt_str("Aimbot hitboxes"), &config_system.g_cfg.player.lag_hitbox);
										ImGui::SameLine();
										ImGui::ColorEdit(crypt_str("##lagcompcolor"), &config_system.g_cfg.player.lag_hitbox_color, ALPHA);
									}
								}
								else
								{
									draw_combo(crypt_str("Player model T"), config_system.g_cfg.player.player_model_t, player_model_t, ARRAYSIZE(player_model_t));
									padding(0, 3);
									draw_combo(crypt_str("Player model CT"), config_system.g_cfg.player.player_model_ct, player_model_ct, ARRAYSIZE(player_model_ct));
								}
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::NextColumn();
						ImGui::BeginChild("item6s2", { 300, 530 });
						{

							ImGui::SetCursorPos({ 0,0 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Other"));

								ImGui::Checkbox(crypt_str("Grenade prediction"), &config_system.g_cfg.esp.grenade_prediction);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##grenpredcolor"), &config_system.g_cfg.esp.grenade_prediction_color, ALPHA);

								if (config_system.g_cfg.esp.grenade_prediction)
								{
									ImGui::Checkbox(crypt_str("On click"), &config_system.g_cfg.esp.on_click);
									ImGui::Text(crypt_str("Tracer color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##tracergrenpredcolor"), &config_system.g_cfg.esp.grenade_prediction_tracer_color, ALPHA);
								}

								ImGui::Checkbox(crypt_str("Grenade warning "), &config_system.g_cfg.esp.grenade_warning);
								if (config_system.g_cfg.esp.grenade_warning) {
									ImGui::BeginGroup(); {//line
										ImGui::Checkbox("Enable tracer", &config_system.g_cfg.warning.trace.enable);
										draw_combo(crypt_str("Tracer type"), config_system.g_cfg.warning.trace.type, warning_mode, ARRAYSIZE(warning_mode));
										if (config_system.g_cfg.warning.trace.type != 1) {
											ImGui::Text(crypt_str("Tracer color "));
											ImGui::SameLine();
											ImGui::ColorEdit(crypt_str("##grenade_tr_color"), &config_system.g_cfg.warning.trace.color, ALPHA);
										}
										ImGui::Checkbox("Visible only##trace", &config_system.g_cfg.warning.trace.visible_only);
										ImGui::SliderFloat(crypt_str("Width"), &config_system.g_cfg.warning.trace.width, 1, 5);
									}
									ImGui::EndGroup();

									ImGui::Spacing();

									ImGui::BeginGroup(); {//icon и другая хуйня
										ImGui::Checkbox("Enable warning", &config_system.g_cfg.warning.main.enable);
										if (ImGui::BeginCombo("Designers", "Select any element"))
										{

											ImGui::Checkbox("Show grenade icon", &config_system.g_cfg.warning.main.show_icon); if (!config_system.g_cfg.warning.main.color_by_time) {
												ImGui::SameLine();
												ImGui::ColorEdit(crypt_str("##grenade_ic_color"), &config_system.g_cfg.warning.main.icon_col, ALPHA);
											}

											ImGui::Checkbox("Show background", &config_system.g_cfg.warning.main.show_bg); ImGui::SameLine();
											ImGui::ColorEdit(crypt_str("##grenade_bg_color"), &config_system.g_cfg.warning.main.bg_col, ALPHA);


											ImGui::Checkbox("Show timer", &config_system.g_cfg.warning.main.show_timer); if (!config_system.g_cfg.warning.main.color_by_time) {
												ImGui::SameLine();
												ImGui::ColorEdit(crypt_str("##grenade_tim_color"), &config_system.g_cfg.warning.main.timer_col, ALPHA);
											}

											ImGui::Checkbox("Show damage/distance", &config_system.g_cfg.warning.main.show_damage_dist);

											ImGui::Checkbox("Set color based on remaining time", &config_system.g_cfg.warning.main.color_by_time);

											ImGui::EndCombo();
										}

										draw_combo(crypt_str("Damage warning type"), config_system.g_cfg.warning.main.d_warn_type, d_warning_mode, ARRAYSIZE(d_warning_mode));
										if (config_system.g_cfg.warning.main.d_warn_type) {
											ImGui::Text(crypt_str("Damage warning color "));
											ImGui::SameLine();
											ImGui::ColorEdit(crypt_str("##grenadewa_tr_color"), &config_system.g_cfg.warning.main.warn_col, ALPHA);
										}
										ImGui::Checkbox("Visible only##main", &config_system.g_cfg.warning.main.visible_only);
									}
									ImGui::EndGroup();
								}

								ImGui::Checkbox(crypt_str("Grenade projectiles"), &config_system.g_cfg.esp.projectiles);

								if (config_system.g_cfg.esp.projectiles)
									draw_multicombo(crypt_str("Grenade ESP"), config_system.g_cfg.esp.grenade_esp, proj_combo, ARRAYSIZE(proj_combo), preview);

								if (config_system.g_cfg.esp.grenade_esp[GRENADE_ICON] || config_system.g_cfg.esp.grenade_esp[GRENADE_TEXT])
								{
									ImGui::Text(crypt_str("Color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##projectcolor"), &config_system.g_cfg.esp.projectiles_color, ALPHA);
								}

								if (config_system.g_cfg.esp.grenade_esp[GRENADE_BOX])
								{
									ImGui::Text(crypt_str("Box color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##grenade_box_color"), &config_system.g_cfg.esp.grenade_box_color, ALPHA);
								}

								if (config_system.g_cfg.esp.grenade_esp[GRENADE_GLOW])
								{
									ImGui::Text(crypt_str("Glow color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##grenade_glow_color"), &config_system.g_cfg.esp.grenade_glow_color, ALPHA);
								}

								if (config_system.g_cfg.esp.grenade_esp[GRENADE_WARNING])
								{
									ImGui::Text(crypt_str("Warning color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##grenade_warning_color"), &config_system.g_cfg.esp.grenade_warning_color, ALPHA);
								}

								ImGui::Checkbox(crypt_str("Fire timer"), &config_system.g_cfg.esp.molotov_timer);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##molotovcolor"), &config_system.g_cfg.esp.molotov_timer_color, ALPHA);

								ImGui::Checkbox(crypt_str("Smoke timer"), &config_system.g_cfg.esp.smoke_timer);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##smokecolor"), &config_system.g_cfg.esp.smoke_timer_color, ALPHA);

								ImGui::Checkbox(crypt_str("Bomb indicator"), &config_system.g_cfg.esp.bomb_timer);
								draw_multicombo(crypt_str("Weapon ESP"), config_system.g_cfg.esp.weapon, weaponesp, ARRAYSIZE(weaponesp), preview);

								if (config_system.g_cfg.esp.weapon[WEAPON_ICON] || config_system.g_cfg.esp.weapon[WEAPON_TEXT] || config_system.g_cfg.esp.weapon[WEAPON_DISTANCE])
								{
									ImGui::Text(crypt_str("Color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##weaponcolor"), &config_system.g_cfg.esp.weapon_color, ALPHA);
								}

								if (config_system.g_cfg.esp.weapon[WEAPON_BOX])
								{
									ImGui::Text(crypt_str("Box color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##weaponboxcolor"), &config_system.g_cfg.esp.box_color, ALPHA);
								}

								if (config_system.g_cfg.esp.weapon[WEAPON_GLOW])
								{
									ImGui::Text(crypt_str("Glow color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##weaponglowcolor"), &config_system.g_cfg.esp.weapon_glow_color, ALPHA);
								}

								if (config_system.g_cfg.esp.weapon[WEAPON_AMMO])
								{
									ImGui::Text(crypt_str("Ammo bar color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##weaponammocolor"), &config_system.g_cfg.esp.weapon_ammo_color, ALPHA);
								}
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::Columns(1, nullptr, false);
					}
					else if (Active_Tab == 3 && Active_Visuals_Tab == 2) //Visuals | Glow 
					{
						child_title(crypt_str("Glow"));

						draw_combo(crypt_str("Set team"), config_system.g_cfg.player.teams, player_teams, ARRAYSIZE(player_teams));

						ImGui::Checkbox(crypt_str("Glow"), &config_system.g_cfg.player.type[player].glow);

						if (config_system.g_cfg.player.type[player].glow)
						{
							draw_combo(crypt_str("Glow type"), config_system.g_cfg.player.type[player].glow_type, glowtype, ARRAYSIZE(glowtype));
							ImGui::Text(crypt_str("Color "));
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##glowcolor"), &config_system.g_cfg.player.type[player].glow_color, ALPHA);
						}
					}
					else if (Active_Tab == 3 && Active_Visuals_Tab == 3)  //Visuals | Chams
					{

					
				


						//tab_end();
						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("it3ems", { 300, 530 });
						{

							ImGui::SetCursorPos({ 2,2 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Player models"));
								draw_combo(crypt_str("Set team"), config_system.g_cfg.player.teams, player_teams, ARRAYSIZE(player_teams));
								//tab_start();
									//ImGui::Spacing();
								if (player == 2 || LOCAL)
									draw_combo(crypt_str("Type"), config_system.g_cfg.player.local_chams_type, local_chams_type, ARRAYSIZE(local_chams_type));

								if (player != 2 || LOCAL || !config_system.g_cfg.player.local_chams_type)
									draw_multicombo(crypt_str("Chams"), config_system.g_cfg.player.type[player].chams, config_system.g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] ? chamsvisact : chamsvis, config_system.g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] ? ARRAYSIZE(chamsvisact) : ARRAYSIZE(chamsvis), preview);

								if (config_system.g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] || player == 2 || LOCAL && config_system.g_cfg.player.local_chams_type) //-V648
								{
									if (player == LOCAL && config_system.g_cfg.player.local_chams_type)
									{
										ImGui::Checkbox(crypt_str("Enable desync chams"), &config_system.g_cfg.player.fake_chams_enable);
										ImGui::Checkbox(crypt_str("Visualize fake-lag"), &config_system.g_cfg.player.visualize_lag);
										ImGui::Checkbox(crypt_str("Layered"), &config_system.g_cfg.player.layered);

										draw_combo(crypt_str("Player chams material"), config_system.g_cfg.player.fake_chams_type, chamstype, ARRAYSIZE(chamstype));

										ImGui::Text(crypt_str("Color "));
										ImGui::SameLine();
										ImGui::ColorEdit(crypt_str("##fakechamscolor"), &config_system.g_cfg.player.fake_chams_color, ALPHA);

										if (config_system.g_cfg.player.fake_chams_type != 6)
										{
											ImGui::Checkbox(crypt_str("Fake Double material "), &config_system.g_cfg.player.fake_double_material);
											ImGui::SameLine();
											ImGui::ColorEdit(crypt_str("##doublematerialcolor"), &config_system.g_cfg.player.fake_double_material_color, ALPHA);
										}

										ImGui::Checkbox(crypt_str("Animated material"), &config_system.g_cfg.player.fake_animated_material);
										ImGui::SameLine();
										ImGui::ColorEdit(crypt_str("##animcolormat"), &config_system.g_cfg.player.fake_animated_material_color, ALPHA);
									}
									else
									{
										draw_combo(crypt_str("Player chams material"), config_system.g_cfg.player.type[player].chams_type, chamstype, ARRAYSIZE(chamstype));

										if (config_system.g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE])
										{
											ImGui::Text(crypt_str("Visible "));
											ImGui::SameLine();
											ImGui::ColorEdit(crypt_str("##chamsvisible"), &config_system.g_cfg.player.type[player].chams_color, ALPHA);
										}

										if (config_system.g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] && config_system.g_cfg.player.type[player].chams[PLAYER_CHAMS_INVISIBLE])
										{
											ImGui::Text(crypt_str("Invisible "));
											ImGui::SameLine();
											ImGui::ColorEdit(crypt_str("##chamsinvisible"), &config_system.g_cfg.player.type[player].xqz_color, ALPHA);
										}

										if (config_system.g_cfg.player.type[player].chams_type != 6)
										{
											ImGui::Checkbox(crypt_str("Player Double material "), &config_system.g_cfg.player.type[player].double_material);
											ImGui::SameLine();
											ImGui::ColorEdit(crypt_str("##doublematerialcolor"), &config_system.g_cfg.player.type[player].double_material_color, ALPHA);
										}

										ImGui::Checkbox(crypt_str("Animated material"), &config_system.g_cfg.player.type[player].animated_material);
										ImGui::SameLine();
										ImGui::ColorEdit(crypt_str("##animcolormat"), &config_system.g_cfg.player.type[player].animated_material_color, ALPHA);

										if (player == 0 || ENEMY)
										{
											ImGui::Checkbox(crypt_str("Backtrack chams"), &config_system.g_cfg.player.backtrack_chams);

											if (config_system.g_cfg.player.backtrack_chams)
											{
												draw_combo(crypt_str("Backtrack chams material"), config_system.g_cfg.player.backtrack_chams_material, chamstype, ARRAYSIZE(chamstype));

												ImGui::Text(crypt_str("Color "));
												ImGui::SameLine();
												ImGui::ColorEdit(crypt_str("##backtrackcolor"), &config_system.g_cfg.player.backtrack_chams_color, ALPHA);
											}
										}
										if (player == 0 || ENEMY || player == 1 || TEAM)
										{
											ImGui::Checkbox(crypt_str("Ragdoll chams"), &config_system.g_cfg.player.type[player].ragdoll_chams);

											if (config_system.g_cfg.player.type[player].ragdoll_chams)
											{
												draw_combo(crypt_str("Ragdoll chams material"), config_system.g_cfg.player.type[player].ragdoll_chams_material, chamstype, ARRAYSIZE(chamstype));

												ImGui::Text(crypt_str("Color "));
												ImGui::SameLine();
												ImGui::ColorEdit(crypt_str("##ragdollcolor"), &config_system.g_cfg.player.type[player].ragdoll_chams_color, ALPHA);
											}
										}
										else if (!config_system.g_cfg.player.local_chams_type)
										{
											ImGui::Checkbox(crypt_str("Transparency in scope"), &config_system.g_cfg.player.transparency_in_scope);

											if (config_system.g_cfg.player.transparency_in_scope)
												ImGui::SliderFloat(crypt_str("Alpha"), &config_system.g_cfg.player.transparency_in_scope_amount, 0.0f, 1.0f);
										}
									}
								}

							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::NextColumn();
						ImGui::BeginChild("item6s2", { 300, 530 });
						{

							ImGui::SetCursorPos({ 0,0 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Other models"));

								ImGui::Checkbox(crypt_str("Arms chams"), &config_system.g_cfg.esp.arms_chams);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##armscolor"), &config_system.g_cfg.esp.arms_chams_color, ALPHA);
								
								if (config_system.g_cfg.esp.arms_chams)
								{

									draw_combo(crypt_str("Arms chams material"), config_system.g_cfg.esp.arms_chams_type, chamstype, ARRAYSIZE(chamstype));

									if (config_system.g_cfg.esp.arms_chams_type != 6)
									{
										ImGui::Checkbox(crypt_str("Arms double material "), &config_system.g_cfg.esp.arms_double_material);
										ImGui::SameLine();
										ImGui::ColorEdit(crypt_str("##armsdoublematerial"), &config_system.g_cfg.esp.arms_double_material_color, ALPHA);
									}

									ImGui::Checkbox(crypt_str("Arms animated material "), &config_system.g_cfg.esp.arms_animated_material);
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##armsanimatedmaterial"), &config_system.g_cfg.esp.arms_animated_material_color, ALPHA);
								}

								ImGui::Checkbox(crypt_str("Weapon chams"), &config_system.g_cfg.esp.weapon_chams);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##weaponchamscolors"), &config_system.g_cfg.esp.weapon_chams_color, ALPHA);

								if (config_system.g_cfg.esp.weapon_chams)
								{
									draw_combo(crypt_str("Weapon chams material"), config_system.g_cfg.esp.weapon_chams_type, chamstype, ARRAYSIZE(chamstype));

									if (config_system.g_cfg.esp.weapon_chams_type != 6)
									{
										ImGui::Checkbox(crypt_str("Weapon Double material "), &config_system.g_cfg.esp.weapon_double_material);
										ImGui::SameLine();
										ImGui::ColorEdit(crypt_str("##weapondoublematerial"), &config_system.g_cfg.esp.weapon_double_material_color, ALPHA);
									}

									ImGui::Checkbox(crypt_str("Animated material "), &config_system.g_cfg.esp.weapon_animated_material);
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##weaponanimatedmaterial"), &config_system.g_cfg.esp.weapon_animated_material_color, ALPHA);
									ImGui::Spacing();
								}
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::Columns(1, nullptr, false);
					

					}
					else if (Active_Tab == 3 && Active_Visuals_Tab == 4) //Visuals | Radar
					{
						child_title(crypt_str("Radar"));
						//tab_start();

						ImGui::Spacing();

						ImGui::Checkbox(crypt_str("Enabled"), &config_system.g_cfg.misc.ingame_radar);
						ImGui::Checkbox(crypt_str("Draw local"), &config_system.g_cfg.radar.render_local);
						ImGui::Checkbox(crypt_str("Draw enemies"), &config_system.g_cfg.radar.render_enemy);
						ImGui::Checkbox(crypt_str("Draw teammates"), &config_system.g_cfg.radar.render_team);
						ImGui::Checkbox(crypt_str("Draw planted c4"), &config_system.g_cfg.radar.render_planted_c4);
						ImGui::Checkbox(crypt_str("Draw dropped c4"), &config_system.g_cfg.radar.render_dropped_c4);
						ImGui::Checkbox(crypt_str("Draw health"), &config_system.g_cfg.radar.render_health);

						//tab_end();
					}
					else if (Active_Tab == 3 && Active_Visuals_Tab == 5) //Visuals | Others
					{
						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("it3ems", { 300, 530 });
						{

							ImGui::SetCursorPos({ 2,2 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("General"));

								ImGui::Checkbox(crypt_str("Enabled"), &config_system.g_cfg.player.enable);

								draw_multicombo(crypt_str("Indicators"), config_system.g_cfg.esp.indicators, indicators, ARRAYSIZE(indicators), preview);
								padding(0, 3);

								draw_multicombo(crypt_str("Removals"), config_system.g_cfg.esp.removals, removals, ARRAYSIZE(removals), preview);

								if (config_system.g_cfg.esp.removals[REMOVALS_ZOOM])
									ImGui::Checkbox(crypt_str("Fix zoom sensivity"), &config_system.g_cfg.esp.fix_zoom_sensivity);



								ImGui::Checkbox(crypt_str("Client bullet impacts"), &config_system.g_cfg.esp.client_bullet_impacts);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##clientbulletimpacts"), &config_system.g_cfg.esp.client_bullet_impacts_color, ALPHA);

								ImGui::Checkbox(crypt_str("Server bullet impacts"), &config_system.g_cfg.esp.server_bullet_impacts);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##serverbulletimpacts"), &config_system.g_cfg.esp.server_bullet_impacts_color, ALPHA);

								ImGui::Checkbox(crypt_str("Local bullet tracers"), &config_system.g_cfg.esp.bullet_tracer);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##bulltracecolor"), &config_system.g_cfg.esp.bullet_tracer_color, ALPHA);

								ImGui::Checkbox(crypt_str("Enemy bullet tracers"), &config_system.g_cfg.esp.enemy_bullet_tracer);
								ImGui::SameLine();

								ImGui::ColorEdit(crypt_str("##enemybulltracecolor"), &config_system.g_cfg.esp.enemy_bullet_tracer_color, ALPHA);
								draw_multicombo(crypt_str("Hit marker"), config_system.g_cfg.esp.hitmarker, hitmarkers, ARRAYSIZE(hitmarkers), preview);
								ImGui::Checkbox(crypt_str("Damage marker"), &config_system.g_cfg.esp.damage_marker);
								ImGui::Checkbox(crypt_str("Kill effect"), &config_system.g_cfg.esp.kill_effect);

								if (config_system.g_cfg.esp.kill_effect)
									ImGui::SliderFloat(crypt_str("Duration"), &config_system.g_cfg.esp.kill_effect_duration, 0.01f, 3.0f);

								ImGui::Checkbox(crypt_str("Velocity graph"), &config_system.g_cfg.esp.velocity_graph);

							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::NextColumn();
						ImGui::BeginChild("item6s2", { 300, 530 });
						{

							ImGui::SetCursorPos({ 0,0 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Other"));

								draw_keybind(crypt_str("Third person"), &config_system.g_cfg.misc.thirdperson_toggle, crypt_str("##TPKEY__HOTKEY"));

								ImGui::Checkbox(crypt_str("Third person when dead"), &config_system.g_cfg.misc.thirdperson_when_spectating);

								if (config_system.g_cfg.misc.thirdperson_toggle.key > KEY_NONE && config_system.g_cfg.misc.thirdperson_toggle.key < KEY_MAX)
									ImGui::SliderInt(crypt_str("Third person distance"), &config_system.g_cfg.misc.thirdperson_distance, 100, 300);

								ImGui::SliderInt(crypt_str("Field of view"), &config_system.g_cfg.esp.fov, 0, 89);
								ImGui::Checkbox(crypt_str("Taser range"), &config_system.g_cfg.esp.taser_range);
								ImGui::Checkbox(crypt_str("Show spread"), &config_system.g_cfg.esp.show_spread);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##spredcolor"), &config_system.g_cfg.esp.show_spread_color, ALPHA);
								ImGui::Checkbox(crypt_str("Penetration crosshair"), &config_system.g_cfg.esp.penetration_reticle);

								child_title(crypt_str("Viewmodel"));

								ImGui::SliderInt(crypt_str("Viewmodel field of view"), &config_system.g_cfg.esp.viewmodel_fov, 0, 89);
								ImGui::SliderInt(crypt_str("Viewmodel X"), &config_system.g_cfg.esp.viewmodel_x, -50, 50);
								ImGui::SliderInt(crypt_str("Viewmodel Y"), &config_system.g_cfg.esp.viewmodel_y, -50, 50);
								ImGui::SliderInt(crypt_str("Viewmodel Z"), &config_system.g_cfg.esp.viewmodel_z, -50, 50);
								ImGui::SliderInt(crypt_str("Viewmodel roll"), &config_system.g_cfg.esp.viewmodel_roll, -180, 180);
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::Columns(1, nullptr, false);
					}


					else if (Active_Tab == 3 && Active_Visuals_Tab == 6) //Visuals | Others
					{

						child_title(crypt_str("World"));

						ImGui::Checkbox(crypt_str("Enabled"), &config_system.g_cfg.player.enable);


						ImGui::Checkbox(crypt_str("Rain"), &config_system.g_cfg.esp.rain);
						ImGui::Checkbox(crypt_str("Full bright"), &config_system.g_cfg.esp.bright);

						draw_combo(crypt_str("Skybox"), config_system.g_cfg.esp.skybox, skybox, ARRAYSIZE(skybox));

						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##skyboxcolor"), &config_system.g_cfg.esp.skybox_color, NOALPHA);

						if (config_system.g_cfg.esp.skybox == 21)
						{
							static char sky_custom[64] = "\0";

							if (!config_system.g_cfg.esp.custom_skybox.empty())
								strcpy_s(sky_custom, sizeof(sky_custom), config_system.g_cfg.esp.custom_skybox.c_str());

							ImGui::Text(crypt_str("Name"));
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

							if (ImGui::InputText(crypt_str("##customsky"), sky_custom, sizeof(sky_custom)))
								config_system.g_cfg.esp.custom_skybox = sky_custom;

							ImGui::PopStyleVar();
						}

						ImGui::Checkbox(crypt_str("Color modulation"), &config_system.g_cfg.esp.nightmode);

						if (config_system.g_cfg.esp.nightmode)
						{
							ImGui::Text(crypt_str("World color "));
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##worldcolor"), &config_system.g_cfg.esp.world_color, ALPHA);

							ImGui::Text(crypt_str("Props color "));
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##propscolor"), &config_system.g_cfg.esp.props_color, ALPHA);
						}

						ImGui::Checkbox(crypt_str("World modulation"), &config_system.g_cfg.esp.world_modulation);

						if (config_system.g_cfg.esp.world_modulation)
						{
							ImGui::SliderFloat(crypt_str("Bloom"), &config_system.g_cfg.esp.bloom, 0.0f, 750.0f);
							ImGui::SliderFloat(crypt_str("Exposure"), &config_system.g_cfg.esp.exposure, 0.0f, 2000.0f);
							ImGui::SliderFloat(crypt_str("Ambient"), &config_system.g_cfg.esp.ambient, 0.0f, 1500.0f);
						}

						ImGui::Checkbox(crypt_str("Fog modulation"), &config_system.g_cfg.esp.fog);

						if (config_system.g_cfg.esp.fog)
						{
							ImGui::SliderInt(crypt_str("Distance"), &config_system.g_cfg.esp.fog_distance, 0, 2500);
							ImGui::SliderInt(crypt_str("Density"), &config_system.g_cfg.esp.fog_density, 0, 100);

							ImGui::Text(crypt_str("Color "));
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##fogcolor"), &config_system.g_cfg.esp.fog_color, NOALPHA);
						}
					}

				
					if (Active_Tab == 4 && Active_Changer_Tab == 1) //Changers | Inventory
					{
						// hey stewen, what r u doing there? he, hm heee, DRUGS
						static bool drugs = false;

						// some animation logic(switch)
						static bool active_animation = false;
						static bool preview_reverse = false;
						static float switch_alpha = 1.f;
						static int next_id = -1;
						if (active_animation)
						{
							if (preview_reverse)
							{
								if (switch_alpha == 1.f) //-V550
								{
									preview_reverse = false;
									active_animation = false;
								}

								switch_alpha = math::clamp(switch_alpha + (4.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
							}
							else
							{
								if (switch_alpha == 0.01f) //-V550
								{
									preview_reverse = true;
								}

								switch_alpha = math::clamp(switch_alpha - (4.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
							}
						}
						else
							switch_alpha = math::clamp(switch_alpha + (4.f * ImGui::GetIO().DeltaTime), 0.0f, 1.f);

							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (1.f - preview_alpha) * public_alpha * switch_alpha);
							child_title(current_profile == -1 ? crypt_str("Skinchanger") : game_data::weapon_names[current_profile].name);
							//tab_start();
							ImGui::Spacing();
							ImGui::BeginChild(crypt_str("##SKINCHANGER__CHILD"), ImVec2(586 * dpi_scale, (child_height - 35)* dpi_scale));
							{
								// we need to count our items in 1 line
								auto same_line_counter = 0;

								// if we didnt choose any weapon
								if (current_profile == -1)
								{
									for (auto i = 0; i < config_system.g_cfg.skins.skinChanger.size(); i++)
									{
										// do we need update our preview for some reasons?
										if (!all_skins[i])
										{
											config_system.g_cfg.skins.skinChanger.at(i).update();
											all_skins[i] = get_skin_preview(get_wep(i, (i == 0 || i == 1) ? config_system.g_cfg.skins.skinChanger.at(i).definition_override_vector_index : -1, i == 0).c_str(), config_system.g_cfg.skins.skinChanger.at(i).skin_name, device); //-V810
										}

										// we licked on weapon
										if (ImGui::ImageButton(all_skins[i], ImVec2(87, 76)))
										{
											next_id = i;
											active_animation = true;
										}

										// if our animation step is half from all - switch profile
										if (active_animation && preview_reverse)
										{
											ImGui::SetScrollY(0);
											current_profile = next_id;
										}

										if (same_line_counter < 4) { // continue push same-line
											ImGui::SameLine();
											same_line_counter++;
										}
										else { // we have maximum elements in 1 line
											same_line_counter = 0;
										}
									}
								}
								else
								{
									// update skin preview bool
									static bool need_update[36];

									// we pressed crypt_str("Save & Close") button
									static bool leave;

									// update if we have nullptr texture or if we push force update
									if (!all_skins[current_profile] || need_update[current_profile])
									{
										all_skins[current_profile] = get_skin_preview(get_wep(current_profile, (current_profile == 0 || current_profile == 1) ? config_system.g_cfg.skins.skinChanger.at(current_profile).definition_override_vector_index : -1, current_profile == 0).c_str(), config_system.g_cfg.skins.skinChanger.at(current_profile).skin_name, device); //-V810
										need_update[current_profile] = false;
									}

									// get settings for selected weapon
									auto& selected_entry = config_system.g_cfg.skins.skinChanger[current_profile];
									selected_entry.itemIdIndex = current_profile;

									ImGui::BeginGroup();
									ImGui::PushItemWidth(260 * dpi_scale);

									// search input later
									static char search_skins[64] = "\0";
									static auto item_index = selected_entry.paint_kit_vector_index;

									if (!current_profile)
									{
										ImGui::Text(crypt_str("Knife"));
										ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
										if (ImGui::Combo(crypt_str("##Knife_combo"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
											{
												*out_text = game_data::knife_names[idx].name;
												return true;
											}, nullptr, IM_ARRAYSIZE(game_data::knife_names)))
											need_update[current_profile] = true; // push force update
									}
									else if (current_profile == 1)
									{
										ImGui::Text(crypt_str("Gloves"));
										ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
										if (ImGui::Combo(crypt_str("##Glove_combo"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
											{
												*out_text = game_data::glove_names[idx].name;
												return true;
											}, nullptr, IM_ARRAYSIZE(game_data::glove_names)))
										{
											item_index = 0; // set new generated paintkits element to 0;
											need_update[current_profile] = true; // push force update
										}
									}
									else
										selected_entry.definition_override_vector_index = 0;

									if (current_profile != 1)
									{
										ImGui::Text(crypt_str("Search"));
										ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

										if (ImGui::InputText(crypt_str("##search"), search_skins, sizeof(search_skins)))
											item_index = -1;

										ImGui::PopStyleVar();
									}

									auto main_kits = current_profile == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits;
									auto display_index = 0;

									SkinChanger::displayKits = main_kits;

									// we dont need custom gloves
									if (current_profile == 1)
									{
										for (auto i = 0; i < main_kits.size(); i++)
										{
											auto main_name = main_kits.at(i).name;

											for (auto i = 0; i < main_name.size(); i++)
												if (iswalpha((main_name.at(i))))
													main_name.at(i) = towlower(main_name.at(i));

											char search_name[64];

											if (!strcmp(game_data::glove_names[selected_entry.definition_override_vector_index].name, crypt_str("Hydra")))
												strcpy_s(search_name, sizeof(search_name), crypt_str("Bloodhound"));
											else
												strcpy_s(search_name, sizeof(search_name), game_data::glove_names[selected_entry.definition_override_vector_index].name);

											for (auto i = 0; i < sizeof(search_name); i++)
												if (iswalpha(search_name[i]))
													search_name[i] = towlower(search_name[i]);

											if (main_name.find(search_name) != std::string::npos)
											{
												SkinChanger::displayKits.at(display_index) = main_kits.at(i);
												display_index++;
											}
										}

										SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
									}
									else
									{
										if (strcmp(search_skins, crypt_str(""))) //-V526
										{
											for (auto i = 0; i < main_kits.size(); i++)
											{
												auto main_name = main_kits.at(i).name;

												for (auto i = 0; i < main_name.size(); i++)
													if (iswalpha(main_name.at(i)))
														main_name.at(i) = towlower(main_name.at(i));

												char search_name[64];
												strcpy_s(search_name, sizeof(search_name), search_skins);

												for (auto i = 0; i < sizeof(search_name); i++)
													if (iswalpha(search_name[i]))
														search_name[i] = towlower(search_name[i]);

												if (main_name.find(search_name) != std::string::npos)
												{
													SkinChanger::displayKits.at(display_index) = main_kits.at(i);
													display_index++;
												}
											}

											SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
										}
										else
											item_index = selected_entry.paint_kit_vector_index;
									}

									ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
									if (!SkinChanger::displayKits.empty())
									{
										if (ImGui::ListBox(crypt_str("##PAINTKITS"), &item_index, [](void* data, int idx, const char** out_text) //-V107
											{
												while (SkinChanger::displayKits.at(idx).name.find(crypt_str("С‘")) != std::string::npos) //-V807
													SkinChanger::displayKits.at(idx).name.replace(SkinChanger::displayKits.at(idx).name.find(crypt_str("С‘")), 2, crypt_str("Рµ"));

												*out_text = SkinChanger::displayKits.at(idx).name.c_str();
												return true;
											}, nullptr, SkinChanger::displayKits.size(), SkinChanger::displayKits.size() > 9 ? 9 : SkinChanger::displayKits.size()) || !all_skins[current_profile])
										{
											SkinChanger::scheduleHudUpdate();
											need_update[current_profile] = true;

											auto i = 0;

											while (i < main_kits.size())
											{
												if (main_kits.at(i).id == SkinChanger::displayKits.at(item_index).id)
												{
													selected_entry.paint_kit_vector_index = i;
													break;
												}

												i++;
											}

										}
									}
									ImGui::PopStyleVar();

									if (ImGui::InputInt(crypt_str("Seed"), &selected_entry.seed, 1, 100))
										SkinChanger::scheduleHudUpdate();

									if (ImGui::InputInt(crypt_str("StatTrak"), &selected_entry.stat_trak, 1, 15))
										SkinChanger::scheduleHudUpdate();

									if (ImGui::SliderFloat(crypt_str("Wear"), &selected_entry.wear, 0.0f, 1.0f))
										drugs = true;
									else if (drugs)
									{
										SkinChanger::scheduleHudUpdate();
										drugs = false;
									}

									ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6 * c_menu::get().dpi_scale);
									ImGui::Text(crypt_str("Quality"));
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
									if (ImGui::Combo(crypt_str("##Quality_combo"), &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
										{
											*out_text = game_data::quality_names[idx].name;
											return true;
										}, nullptr, IM_ARRAYSIZE(game_data::quality_names)))
										SkinChanger::scheduleHudUpdate();

										if (current_profile != 1)
										{
											if (!config_system.g_cfg.skins.custom_name_tag[current_profile].empty())
												strcpy_s(selected_entry.custom_name, sizeof(selected_entry.custom_name), config_system.g_cfg.skins.custom_name_tag[current_profile].c_str());

											ImGui::Text(crypt_str("Name Tag"));
											ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

											if (ImGui::InputText(crypt_str("##nametag"), selected_entry.custom_name, sizeof(selected_entry.custom_name)))
											{
												config_system.g_cfg.skins.custom_name_tag[current_profile] = selected_entry.custom_name;
												SkinChanger::scheduleHudUpdate();
											}

											ImGui::PopStyleVar();
										}

										ImGui::PopItemWidth();

										ImGui::EndGroup();

										ImGui::SameLine();
										ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30);

										ImGui::BeginGroup();
										if (ImGui::ImageButton(all_skins[current_profile], ImVec2(190, 155)))
										{
											// maybe i will do smth later where, who knows :/
										}

										if (ImGui::CustomButton(crypt_str("Close"), crypt_str("##CLOSE__SKING"), ImVec2(198 * dpi_scale, 26 * dpi_scale)))
										{
											// start animation
											active_animation = true;
											next_id = -1;
											leave = true;
										}
										ImGui::Checkbox(crypt_str("Force rare animations"), &config_system.g_cfg.skins.rare_animations);

										ImGui::EndGroup();

										// update element
										selected_entry.update();

										// we need to reset profile in the end to prevent render images with massive's index == -1
										if (leave && (preview_reverse || !active_animation))
										{
											ImGui::SetScrollY(0);
											current_profile = next_id;
											leave = false;
										}

								}
							}
							ImGui::EndChild();
							//tab_end();
					}
					else if (Active_Tab == 4 && Active_Changer_Tab == 2) //Changers | Profile
					{
					ImGui::Checkbox(crypt_str("Fake Prime"), &config_system.g_cfg.misc.prime);
					}
					else if (Active_Tab == 4 && Active_Changer_Tab == 3) //Changers | Character
					{

					}
					if (Active_Tab == 5 && Active_Misc_Tab == 1) //Misc | Main
					{
							ImGui::Columns(2, nullptr, false);
							ImGui::BeginChild("it3ems", { 300, 530 });
							{

								ImGui::SetCursorPos({ 2,2 });
								ImGui::BeginGroup();
								{
									child_title(crypt_str("General"));

									//tab_start();
									ImGui::Checkbox(crypt_str("Anti-untrusted"), &config_system.g_cfg.misc.anti_untrusted);
									ImGui::Checkbox(crypt_str("Rank reveal"), &config_system.g_cfg.misc.rank_reveal);
									ImGui::Checkbox(crypt_str("Unlock inventory access"), &config_system.g_cfg.misc.inventory_access);
									ImGui::Checkbox(crypt_str("Gravity ragdolls"), &config_system.g_cfg.misc.ragdolls);
									ImGui::Checkbox(crypt_str("Preserve killfeed"), &config_system.g_cfg.esp.preserve_killfeed);
									ImGui::Checkbox(crypt_str("Aspect ratio"), &config_system.g_cfg.misc.aspect_ratio);

								

									if (config_system.g_cfg.misc.aspect_ratio)
									{
										padding(0, -5);
										ImGui::SliderFloat(crypt_str("Amount"), &config_system.g_cfg.misc.aspect_ratio_amount, 1.0f, 2.0f);
									}

									ImGui::Checkbox(crypt_str("Fake-lag"), &config_system.g_cfg.antiaim.fakelag);
									if (config_system.g_cfg.antiaim.fakelag)
									{
										draw_combo(crypt_str("Fake-lag type"), config_system.g_cfg.antiaim.fakelag_type, fakelags, ARRAYSIZE(fakelags));
										ImGui::SliderInt(crypt_str("Limit"), &config_system.g_cfg.antiaim.fakelag_amount, 1, 16);

										draw_multicombo(crypt_str("Fake-lag triggers"), config_system.g_cfg.antiaim.fakelag_enablers, lagstrigger, ARRAYSIZE(lagstrigger), preview);

										auto enabled_fakelag_triggers = false;

										for (auto i = 0; i < ARRAYSIZE(lagstrigger); i++)
											if (config_system.g_cfg.antiaim.fakelag_enablers[i])
												enabled_fakelag_triggers = true;

										if (enabled_fakelag_triggers)
											ImGui::SliderInt(crypt_str("Triggers limit"), &config_system.g_cfg.antiaim.triggers_fakelag_amount, 1, 16);
									}
									draw_combo(crypt_str("Force mm region"), config_system.g_cfg.misc.region_changer, mmregions, ARRAYSIZE(mmregions));
									if (ImGui::CustomButton(crypt_str("Apply region changes"), crypt_str("##CONFIG__CREATE"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
									{
										misc::get().ChangeRegion();
									}
								}
								ImGui::EndGroup();
							}
							ImGui::EndChild();
							ImGui::NextColumn();
							ImGui::BeginChild("item6s2", { 300, 530 });
							{

								ImGui::SetCursorPos({ 0,0 });
								ImGui::BeginGroup();
								{
									child_title(crypt_str("Information"));


									ImGui::Checkbox(crypt_str("Watermark"), &config_system.g_cfg.menu.watermark);
									ImGui::Checkbox(crypt_str("Spectators list"), &config_system.g_cfg.misc.spectators_list);
									draw_combo(crypt_str("Hitsound"), config_system.g_cfg.esp.hitsound, sounds, ARRAYSIZE(sounds));
									ImGui::Checkbox(crypt_str("Killsound"), &config_system.g_cfg.esp.killsound);
									draw_multicombo(crypt_str("Logs"), config_system.g_cfg.misc.events_to_log, events, ARRAYSIZE(events), preview);
									padding(0, 3);
									draw_multicombo(crypt_str("Logs output"), config_system.g_cfg.misc.log_output, events_output, ARRAYSIZE(events_output), preview);

									if (config_system.g_cfg.misc.events_to_log[EVENTLOG_HIT] || config_system.g_cfg.misc.events_to_log[EVENTLOG_ITEM_PURCHASES] || config_system.g_cfg.misc.events_to_log[EVENTLOG_BOMB])
									{
										ImGui::Text(crypt_str("Color "));
										ImGui::SameLine();
										ImGui::ColorEdit(crypt_str("##logcolor"), &config_system.g_cfg.misc.log_color, ALPHA);
									}

									ImGui::Checkbox(crypt_str("Show CS:GO logs"), &config_system.g_cfg.misc.show_default_log);
								}
								ImGui::EndGroup();
							}
							ImGui::EndChild();
							ImGui::Columns(1, nullptr, false);
					


					}
					else if (Active_Tab == 5 && Active_Misc_Tab == 2) //Misc | Additives
					{


						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("it3ems", { 300, 530 });
						{

							ImGui::SetCursorPos({ 2,2 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Movement"));

								//tab_start();

								ImGui::Checkbox(crypt_str("Automatic jump"), &config_system.g_cfg.misc.bunnyhop);
								draw_combo(crypt_str("Automatic strafes"), config_system.g_cfg.misc.airstrafe, strafes, ARRAYSIZE(strafes));
								ImGui::Checkbox(crypt_str("Crouch in air"), &config_system.g_cfg.misc.crouch_in_air);
								ImGui::Checkbox(crypt_str("Fast stop"), &config_system.g_cfg.misc.fast_stop);
								ImGui::Checkbox(crypt_str("Slide walk"), &config_system.g_cfg.misc.slidewalk);
								ImGui::Checkbox(crypt_str("No duck cooldown"), &config_system.g_cfg.misc.noduck);

								if (config_system.g_cfg.misc.noduck)
									draw_keybind(crypt_str("Fake duck"), &config_system.g_cfg.misc.fakeduck_key, crypt_str("##FAKEDUCK__HOTKEY"));

								draw_keybind(crypt_str("Slow walk"), &config_system.g_cfg.misc.slowwalk_key, crypt_str("##SLOWWALK__HOTKEY"));
								draw_keybind(crypt_str("Auto peek"), &config_system.g_cfg.misc.automatic_peek, crypt_str("##AUTOPEEK__HOTKEY"));
								draw_keybind(crypt_str("Edge jump"), &config_system.g_cfg.misc.edge_jump, crypt_str("##EDGEJUMP__HOTKEY"));
								draw_keybind(crypt_str("Edge bug"), &config_system.g_cfg.misc.edgebug, crypt_str("##EDGEBUG__HOTKEY"));
								draw_keybind(crypt_str("Jump bug"), &config_system.g_cfg.misc.jumpbug, crypt_str("##JUMPBUG__HOTKEY"));

							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::NextColumn();
						ImGui::BeginChild("item6s2", { 300, 530 });
						{

							ImGui::SetCursorPos({ 0,0 });
							ImGui::BeginGroup();
							{
								child_title(crypt_str("Extra"));

								ImGui::Checkbox(crypt_str("Anti-screenshot"), &config_system.g_cfg.misc.anti_screenshot);
								ImGui::Checkbox(crypt_str("Clantag"), &config_system.g_cfg.misc.clantag_spammer);
								ImGui::Checkbox(crypt_str("Chat spam"), &config_system.g_cfg.misc.chat);
								ImGui::Checkbox(crypt_str("Enable buybot"), &config_system.g_cfg.misc.buybot_enable);

								if (config_system.g_cfg.misc.buybot_enable)
								{
									draw_combo(crypt_str("Snipers"), config_system.g_cfg.misc.buybot1, mainwep, ARRAYSIZE(mainwep));
									padding(0, 3);
									draw_combo(crypt_str("Pistols"), config_system.g_cfg.misc.buybot2, secwep, ARRAYSIZE(secwep));
									padding(0, 3);
									draw_multicombo(crypt_str("Other"), config_system.g_cfg.misc.buybot3, grenades, ARRAYSIZE(grenades), preview);
								}
							}
							ImGui::EndGroup();
						}
						ImGui::EndChild();
						ImGui::Columns(1, nullptr, false);
					}
					else if (Active_Tab == 5 && Active_Misc_Tab == 3) //Misc | Configs
					{
						child_title(crypt_str("Configs"));
						ImGui::Spacing();


						ImGui::BeginChild("item6s2", { 300, 530 });
						{

							ImGui::PushItemWidth(220);

							static auto should_update = true;

							if (should_update)
							{
								should_update = false;

								cfg_manager->config_files();
								files = cfg_manager->files;

								for (auto& current : files)
									if (current.size() > 2)
										current.erase(current.size() - 3, 3);
							}

							if (ImGui::CustomButton(crypt_str("Open configs folder"), crypt_str("##CONFIG__FOLDER"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								std::string folder;

								auto get_dir = [&folder]() -> void
								{
									static TCHAR path[MAX_PATH];

									if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
										folder = std::string(path) + crypt_str("\\Qwerty.xyz\\Configs\\");

									CreateDirectory(folder.c_str(), NULL);
								};

								get_dir();
								ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
							}

							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
							ImGui::ListBoxConfigArray(crypt_str("##CONFIGS"), &config_system.g_cfg.selected_config, files, 7);
							ImGui::PopStyleVar();

							if (ImGui::CustomButton(crypt_str("Refresh configs"), crypt_str("##CONFIG__REFRESH"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								cfg_manager->config_files();
								files = cfg_manager->files;

								for (auto& current : files)
									if (current.size() > 2)
										current.erase(current.size() - 3, 3);
							}

							static char config_name[64] = "\0";

							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
							ImGui::InputText(crypt_str("##configname"), config_name, sizeof(config_name));
							ImGui::PopStyleVar();

							if (ImGui::CustomButton(crypt_str("Create config"), crypt_str("##CONFIG__CREATE"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								config_system.g_cfg.new_config_name = config_name;
								add_config();
							}

							static auto next_save = false;
							static auto prenext_save = false;
							static auto clicked_sure = false;
							static auto save_time = m_globals()->m_realtime;
							static auto save_alpha = 1.0f;

							save_alpha = math::clamp(save_alpha + (4.f * ImGui::GetIO().DeltaTime * (!prenext_save ? 1.f : -1.f)), 0.01f, 1.f);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, save_alpha * public_alpha * (1.f - preview_alpha));

							if (!next_save)
							{
								clicked_sure = false;

								if (prenext_save && save_alpha <= 0.01f)
									next_save = true;

								if (ImGui::CustomButton(crypt_str("Save config"), crypt_str("##CONFIG__SAVE"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
								{
									save_time = m_globals()->m_realtime;
									prenext_save = true;
								}
							}
							else
							{
								if (prenext_save && save_alpha <= 0.01f)
								{
									prenext_save = false;
									next_save = !clicked_sure;
								}

								if (ImGui::CustomButton(crypt_str("Are you sure?"), crypt_str("##AREYOUSURE__SAVE"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
								{
									save_config();
									prenext_save = true;
									clicked_sure = true;
								}

								if (!clicked_sure && m_globals()->m_realtime > save_time + 1.5f)
								{
									prenext_save = true;
									clicked_sure = true;
								}
							}

							ImGui::PopStyleVar();

							if (ImGui::CustomButton(crypt_str("Load config"), crypt_str("##CONFIG__LOAD"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
								load_config();

							static auto next_delete = false;
							static auto prenext_delete = false;
							static auto clicked_sure_del = false;
							static auto delete_time = m_globals()->m_realtime;
							static auto delete_alpha = 1.0f;

							delete_alpha = math::clamp(delete_alpha + (4.f * ImGui::GetIO().DeltaTime * (!prenext_delete ? 1.f : -1.f)), 0.01f, 1.f);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, delete_alpha * public_alpha * (1.f - preview_alpha));

							if (!next_delete)
							{
								clicked_sure_del = false;

								if (prenext_delete && delete_alpha <= 0.01f)
									next_delete = true;

								if (ImGui::CustomButton(crypt_str("Remove config"), crypt_str("##CONFIG__delete"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
								{
									delete_time = m_globals()->m_realtime;
									prenext_delete = true;
								}
							}
							else
							{
								if (prenext_delete && delete_alpha <= 0.01f)
								{
									prenext_delete = false;
									next_delete = !clicked_sure_del;
								}

								if (ImGui::CustomButton(crypt_str("Are you sure?"), crypt_str("##AREYOUSURE__delete"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
								{
									remove_config();
									prenext_delete = true;
									clicked_sure_del = true;
								}

								if (!clicked_sure_del && m_globals()->m_realtime > delete_time + 1.5f)
								{
									prenext_delete = true;
									clicked_sure_del = true;
								}
							}

							ImGui::PopStyleVar();
							ImGui::PopItemWidth();

							//ImGui::EndChild();
							//ImGui::SetWindowFontScale(c_menu::get().dpi_scale);
						}

						ImGui::EndChild();
					}
					else if (Active_Tab == 5 && Active_Misc_Tab == 4) //Misc | Lua
					{
					ImGui::Columns(2, nullptr, false);
					child_title(crypt_str("Scripts"));
					ImGui::BeginChild("it3ems", { 300, 530 });
					{

						//	ImGui::SetCursorPos({ 1,1 });
						ImGui::BeginGroup();
						{

							ImGui::PushItemWidth(220);
							static auto should_update = true;

							if (should_update)
							{
								should_update = false;
								scripts = c_lua::get().scripts;

								for (auto& current : scripts)
								{
									if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
										current.erase(current.size() - 5, 5);
									else if (current.size() >= 4)
										current.erase(current.size() - 4, 4);
								}
							}

							if (ImGui::CustomButton(crypt_str("Open scripts folder"), crypt_str("##LUAS__FOLDER"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								std::string folder;

								auto get_dir = [&folder]() -> void
								{
									static TCHAR path[MAX_PATH];

									if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
										folder = std::string(path) + crypt_str("\\Qwerty.xyz\\Scripts\\");

									CreateDirectory(folder.c_str(), NULL);
								};

								get_dir();
								ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
							}

							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

							if (scripts.empty())
								ImGui::ListBoxConfigArray(crypt_str("##LUAS"), &selected_script, scripts, 7);

							else
							{
								auto backup_scripts = scripts;

								for (auto& script : scripts)
								{
									auto script_id = c_lua::get().get_script_id(script + crypt_str(".lua"));

									if (script_id == -1)
										continue;

									if (c_lua::get().loaded.at(script_id))
										scripts.at(script_id) += crypt_str(" [loaded]");
								}

								ImGui::ListBoxConfigArray(crypt_str("##LUAS"), &selected_script, scripts, 7);
								scripts = std::move(backup_scripts);
							}

							ImGui::PopStyleVar();

							if (ImGui::CustomButton(crypt_str("Refresh scripts"), crypt_str("##LUA__REFRESH"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								c_lua::get().refresh_scripts();
								scripts = c_lua::get().scripts;

								if (selected_script >= scripts.size())
									selected_script = scripts.size() - 1; //-V103

								for (auto& current : scripts)
								{
									if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
										current.erase(current.size() - 5, 5);
									else if (current.size() >= 4)
										current.erase(current.size() - 4, 4);
								}
							}

							if (ImGui::CustomButton(crypt_str("Edit script"), crypt_str("##LUA__EDIT"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								loaded_editing_script = false;
								editing_script = scripts.at(selected_script);
							}

							if (ImGui::CustomButton(crypt_str("Load script"), crypt_str("##SCRIPTS__LOAD"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								c_lua::get().load_script(selected_script);
								c_lua::get().refresh_scripts();

								scripts = c_lua::get().scripts;

								if (selected_script >= scripts.size())
									selected_script = scripts.size() - 1; //-V103

								for (auto& current : scripts)
								{
									if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
										current.erase(current.size() - 5, 5);
									else if (current.size() >= 4)
										current.erase(current.size() - 4, 4);
								}

								eventlogs::get().addnew(crypt_str("Loaded ") + scripts.at(selected_script) + crypt_str(" script"), Color::Yellow, false); //-V106
							}

							if (ImGui::CustomButton(crypt_str("Unload script"), crypt_str("##SCRIPTS__UNLOAD"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								c_lua::get().unload_script(selected_script);
								c_lua::get().refresh_scripts();

								scripts = c_lua::get().scripts;

								if (selected_script >= scripts.size())
									selected_script = scripts.size() - 1; //-V103

								for (auto& current : scripts)
								{
									if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
										current.erase(current.size() - 5, 5);
									else if (current.size() >= 4)
										current.erase(current.size() - 4, 4);
								}

								eventlogs::get().addnew(crypt_str("Unloaded ") + scripts.at(selected_script) + crypt_str(" script"), Color::Yellow, false); //-V106
							}

							if (ImGui::CustomButton(crypt_str("Reload all scripts"), crypt_str("##SCRIPTS__RELOAD"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								c_lua::get().reload_all_scripts();
								c_lua::get().refresh_scripts();

								scripts = c_lua::get().scripts;

								if (selected_script >= scripts.size())
									selected_script = scripts.size() - 1; //-V103

								for (auto& current : scripts)
								{
									if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
										current.erase(current.size() - 5, 5);
									else if (current.size() >= 4)
										current.erase(current.size() - 4, 4);
								}
							}

							if (ImGui::CustomButton(crypt_str("Unload all scripts"), crypt_str("##SCRIPTS__UNLOADALL"), ImVec2(220 * dpi_scale, 26 * dpi_scale)))
							{
								c_lua::get().unload_all_scripts();
								c_lua::get().refresh_scripts();

								scripts = c_lua::get().scripts;

								if (selected_script >= scripts.size())
									selected_script = scripts.size() - 1; //-V103

								for (auto& current : scripts)
								{
									if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
										current.erase(current.size() - 5, 5);
									else if (current.size() >= 4)
										current.erase(current.size() - 4, 4);
								}
							}

							//ImGui::PopItemWidth();
							//tab_end();
						}
						if (Active_Tab == 6 && Active_Movement_Tab == 1) //Changers | Inventory
						{
						}
						else if (Active_Tab == 6 && Active_Movement_Tab == 2) //Changers | Profile
						{

						}
						else if (Active_Tab == 6 && Active_Movement_Tab == 3) //Changers | Character
						{
							ImGui::SetCursorPos(ImVec2{370, 60});
							ImGui::BeginChild("##Maein", ImVec2{ 300, 240 }, false, NoMove);
							{

								IDirect3DTexture9* image;
								image = steam_image();

								ImGui::Image(image, ImVec2(75, 75));
								ImGui::Text("Current user: %s\nTime ingame: %f\nTime in real: %s\nTickrate: %d\nFps: %d\nServer time: %f\nSteam level: %d\nPrealpha build", SteamFriends->GetPersonaName(), m_globals()->m_realtime, zGetTimeString().c_str(), zgettickrate(), zgetfps(), m_globals()->m_curtime, SteamUser->GetPlayerSteamLevel());

							}
							ImGui::EndChild();
							ImGui::SetCursorPos(ImVec2{ 370, 285 });
							ImGui::BeginChild("##eMain", ImVec2{ 300, 260 }, false, NoMove);
							{
								enum k_EFriendFlags
								{
									k_EFriendFlagNone = 0x00,
									k_EFriendFlagBlocked = 0x01,
									k_EFriendFlagFriendshipRequested = 0x02,
									k_EFriendFlagImmediate = 0x04,			// "regular" friend
									k_EFriendFlagClanMember = 0x08,
									k_EFriendFlagOnGameServer = 0x10,
									//	k_EFriendFlagHasPlayedWith	= 0x20,
									//	k_EFriendFlagFriendOfFriend	= 0x40,
									k_EFriendFlagRequestingFriendship = 0x80,
									k_EFriendFlagRequestingInfo = 0x100,
									k_EFriendFlagAll = 0xFFFF,
								};
								static auto friends = SteamFriends->GetFriendCount(k_EFriendFlagAll);


								for (static auto i = 0; i < 1; friends_images = new IDirect3DTexture9 * [friends], i++); //creates


								for (static auto i = 0; i < friends; friends_images[i] = steam_image_friends(SteamFriends->GetFriendByIndex(i, k_EFriendFlagAll)), i++);

								for (auto i = 0; i < friends; i++)
								{
									ImGui::Image(friends_images[i], ImVec2(48, 48));
									ImGui::SameLine();
									ImGui::Text("%s Steam level %d", SteamFriends->GetFriendPersonaName(SteamFriends->GetFriendByIndex(i, k_EFriendFlagAll)),
										SteamFriends->GetFriendSteamLevel(SteamFriends->GetFriendByIndex(i, k_EFriendFlagAll)));
								}
							}
							ImGui::EndChild();
						}

						ImGui::EndGroup();
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					child_title(crypt_str("Script elements"));
					ImGui::Spacing();
					ImGui::BeginChild("item6s2", { 300, 530 });
					{

						ImGui::SetCursorPos({ 10,10 });
						ImGui::BeginGroup();
						{

								ImGui::Checkbox(crypt_str("Developer mode"), &config_system.g_cfg.scripts.developer_mode);
								ImGui::Checkbox(crypt_str("Allow HTTP requests"), &config_system.g_cfg.scripts.allow_http);
								ImGui::Checkbox(crypt_str("Allow files read or write"), &config_system.g_cfg.scripts.allow_file);
								ImGui::Spacing();

								auto previous_check_box = false;

								for (auto& current : c_lua::get().scripts)
								{
									auto& items = c_lua::get().items.at(c_lua::get().get_script_id(current));

									for (auto& item : items)
									{
										std::string item_name;

										auto first_point = false;
										auto item_str = false;

										for (auto& c : item.first)
										{
											if (c == '.')
											{
												if (first_point)
												{
													item_str = true;
													continue;
												}
												else
													first_point = true;
											}

											if (item_str)
												item_name.push_back(c);
										}

										switch (item.second.type)
										{
										case NEXT_LINE:
											previous_check_box = false;
											break;
										case CHECK_BOX:
											previous_check_box = true;
											ImGui::Checkbox(item_name.c_str(), &item.second.check_box_value);
											break;
										case COMBO_BOX:
											previous_check_box = false;
											draw_combo(item_name.c_str(), item.second.combo_box_value, [](void* data, int idx, const char** out_text)
												{
													auto labels = (std::vector <std::string>*)data;
													*out_text = labels->at(idx).c_str(); //-V106
													return true;
												}, &item.second.combo_box_labels, item.second.combo_box_labels.size());
											break;
										case SLIDER_INT:
											previous_check_box = false;
											ImGui::SliderInt(item_name.c_str(), &item.second.slider_int_value, item.second.slider_int_min, item.second.slider_int_max);
											break;
										case SLIDER_FLOAT:
											previous_check_box = false;
											ImGui::SliderFloat(item_name.c_str(), &item.second.slider_float_value, item.second.slider_float_min, item.second.slider_float_max);
											break;
										case COLOR_PICKER:
											if (previous_check_box)
												previous_check_box = false;
											else
												ImGui::Text((item_name + ' ').c_str());

											ImGui::SameLine();
											ImGui::ColorEdit((crypt_str("##") + item_name).c_str(), &item.second.color_picker_value, ALPHA, true);
											break;
										}
									}
								}

								

						}
						ImGui::EndGroup();
					}
					ImGui::EndChild();
					ImGui::Columns(1, nullptr, false);
					}
		        }
				ImGui::EndChild();
				if (!editing_script.empty())
					lua_edit(editing_script);
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
}
ImGui::End();
}

void c_menu::create_style() 
{
	#define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
    #define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
    #define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
    // backgrounds (@todo: complete with BG_MED, BG_LOW)
    #define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
    // text
    #define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

	ImGui::StyleColorsDark();
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoInputs);
	_style = ImGui::GetStyle();

	/*edit Style if needed*/
	_style.ScrollbarRounding = 0.f;
	_style.TabRounding = 5.f;
	_style.WindowRounding = 0;
	_style.FrameRounding = 0;

	_style.Colors[ImGuiCol_::ImGuiCol_Header] = TEXT(0.78f);;
	_style.Colors[ImGuiCol_::ImGuiCol_HeaderHovered] = TEXT(0.28f);
	_style.Colors[ImGuiCol_::ImGuiCol_HeaderActive] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
	_style.Colors[ImGuiCol_::ImGuiCol_TitleBg] = BG(0.58f);
	_style.Colors[ImGuiCol_::ImGuiCol_TitleBgActive] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	_style.Colors[ImGuiCol_::ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	_style.Colors[ImGuiCol_::ImGuiCol_WindowBg] = BG(1.00f);
	_style.Colors[ImGuiCol_::ImGuiCol_ChildWindowBg] = MED(0.78f);
	_style.Colors[ImGuiCol_::ImGuiCol_ChildBg] = MED(1.00f);
	_style.Colors[ImGuiCol_::ImGuiCol_Border] = LOW(1.00f);
	_style.Colors[ImGuiCol_FrameBg] = HI(1.00f);
	_style.Colors[ImGuiCol_FrameBgActive] = BG(0.75f);
	_style.Colors[ImGuiCol_FrameBgHovered] = BG(0.47f);
	_style.ScrollbarSize = 5.f;

	ImGui::GetStyle() = _style;
}