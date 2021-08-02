#include "logger.hpp"


struct notify_t
{
	float life_ime_local;
	float life_ime;
	std::string type;
	std::string message;
	Color c_type;
	Color c_message;
	float x;
	float y = -15;
	float max_x;
};

std::deque<notify_t> notifications;

notify_t find_notify(std::string pre_text, std::string body)
{
	for (size_t i = 0; i < notifications.size(); i++)
		if (notifications[i].type == pre_text && notifications[i].message == body)
			return notifications[i];
	return notify_t();
}
#include "../menu.h"
namespace notify
{

	void render()
	{
		if (notifications.empty())
			return;
		auto csgo = g_ctx;// auto vars = g_cfg;
		static float rainbow = 0.f;
		if (rainbow <= 1.f)
			rainbow += 0.001;
		else
			rainbow = 0.f;
		auto rainbow_col = Color::FromHSB(rainbow, 1, 1);
		float last_y = 0;
		int x, y;
		m_engine()->GetScreenSize(x, y);
		for (size_t i = 0; i < notifications.size(); i++)
		{
			auto& notify = notifications.at(i);

			const auto pre = notify.type.c_str();
			const auto text = notify.message.c_str();
			ImVec2 textSize = c_menu::get().neverpuk->CalcTextSizeA(12.f, FLT_MAX, 0.0f, notify.type.c_str());

			std::string all_text;
			all_text += pre;
			all_text += "";
			all_text += text;

			ImVec2 all_textSize = c_menu::get().neverpuk->CalcTextSizeA(12.f, FLT_MAX, 0.0f, all_text.c_str());

			notify.y = math::lerp(notify.y, (i * 15.f), 0.05f);

			if (notify.y > y + 16) {
				continue;
			}

			if (util::epoch_time() - notify.life_ime_local > notify.life_ime)
			{
				if ((notify.x + all_textSize.x + 16) < 0) {
					notifications.erase(notifications.begin() + i);
					continue;
				}

				notify.max_x = all_textSize.x + 16;

				notify.x = math::lerp(notify.x, (notify.max_x * -1) - 10, 0.05f);

				int procent_x = (100 * (notify.max_x + notify.x)) / notify.max_x;

				auto opacity = int((255 / 100) * procent_x);

				if (procent_x >= 0 && procent_x <= 100)
				{
					notify.c_message = Color(notify.c_message);
					notify.c_message.SetAlpha(opacity);
					notify.c_type = Color(notify.c_type);
					notify.c_type.SetAlpha(opacity);
				}
				else
				{
					notify.c_message = Color(notify.c_message);
					notify.c_message.SetAlpha(255);
					notify.c_type = Color(notify.c_type);
					notify.c_type.SetAlpha(255);
				}
			}

			float box_w = (float)fabs(0 - (all_textSize.x + 16));

			ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(0.f, last_y + notify.y - 1), ImVec2(notify.x + all_textSize.x + 16, last_y + notify.y + all_textSize.y + 2), ImColor(0, 0, 0, 120), ImColor(0, 0, 0, 10), ImColor(0, 0, 0, 10), ImColor(0, 0, 0, 120));
			ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(notify.x + all_textSize.x + 16, last_y + notify.y - 1), ImVec2(notify.x + all_textSize.x + 18, last_y + notify.y + all_textSize.y + 2), ImColor(rainbow_col.r(), rainbow_col.g(), rainbow_col.b(), 150));
			ImGui::GetOverlayDrawList()->AddText(c_menu::get().neverpuk, 12.f, ImVec2(notify.x + 3, last_y + notify.y), ImColor(rainbow_col.r(), rainbow_col.g(), rainbow_col.b()), pre);
			ImGui::GetOverlayDrawList()->AddText(c_menu::get().neverpuk, 12.f, ImVec2(notify.x + 13 + textSize.x, last_y + notify.y), ImColor(notify.c_message.r(), notify.c_message.g(), notify.c_message.b()), text);

		}
	}

	void add_log(std::string pre, std::string text, Color color_pre, Color color_text, int life_time)
	{
		screen::notify(pre, text, color_pre, color_text, life_time);
		console::notify(pre, text, color_pre);
	}

	namespace console {
		void clear() {
			m_engine()->ExecuteClientCmd("clear");
		}

		void notify(std::string pre, std::string msg, Color clr_pre) {
			if (!m_engine() || !m_cvar())
				return;
			static float rainbow = 0.f;
			if (rainbow <= 1.f)
				rainbow += 0.001;
			else
				rainbow = 0.f;
			auto rainbow_col = Color::FromHSB(rainbow, 1, 1);
			m_cvar()->ConsoleColorPrintf(rainbow_col, "[ aiiuur ] ");
			m_cvar()->ConsoleColorPrintf(clr_pre, "[%s]", pre.data());
			m_cvar()->ConsolePrintf(" %s\n", msg.data());
		}
	}

	namespace screen
	{
		void notify(std::string pre, std::string text, Color color_pre, Color color_text, int life_time) {
			std::string type_buf;
			type_buf += "[";
			type_buf += pre;
			type_buf += "]";

			notifications.push_front(notify_t{ static_cast<float>(util::epoch_time()), (float)life_time, type_buf, text, color_pre, color_text });
		}
	}
}