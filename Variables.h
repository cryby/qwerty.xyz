/*#pragma once
#include <array>
#include <algorithm>
#include <vector>
#include <filesystem>

struct Pos
{
	Vector			viewAngles[200];
	float			forwardmove[200];
	float			sidemove[200];
	float			upmove[200];
	int				buttons[200];
	std::string		name[200];
	std::string		map[200];
};



struct MovementRecorder
{
	int movement_int = 1;


	struct Position
	{
		Vector position[200];
	};

	std::vector<Pos> CmdFinal;
	std::vector<Position> CmdFinal2;

	Position tempCmd2;
	Pos tempCmd;
	Vector startVec[200];


};

inline MovementRecorder GetMrecorder;

class c_config
{
public:
	void run(const char*) noexcept;
	void refresh() noexcept;
	void load(size_t) noexcept;
	void save(size_t) const noexcept;
	void add(const char*) noexcept;
	void remove(size_t) noexcept;
	void rename(size_t, const char*) noexcept;
	void reset() noexcept;
	std::vector<std::string> configs;
private:
	std::filesystem::path path;
};*/