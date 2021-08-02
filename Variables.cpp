/*#include "Variables.h"

#include <fstream>
#include <ShlObj.h>
#include "../../archivex.h"
#include <filesystem>

void c_config::run(const char* name) noexcept {
	PWSTR pathToDocuments;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
		path = pathToDocuments;
		path /= "Dynamism.space/Configs";
		CoTaskMemFree(pathToDocuments);
	}

	if (!std::filesystem::is_directory(path)) {
		std::filesystem::remove(path);
		std::filesystem::create_directory(path);
	}

	std::transform(std::filesystem::directory_iterator{ path },
		std::filesystem::directory_iterator{ },
		std::back_inserter(configs),
		[](const auto& entry) { return entry.path().filename().string(); });
}

void c_config::refresh() noexcept
{
	PWSTR pathToDocuments;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
		path = pathToDocuments;
		path /= "Dynamism.space/Configs";
		CoTaskMemFree(pathToDocuments);

		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			auto path = entry.path();
			auto filename = path.filename().string();

			configs.emplace_back(filename);

		}
	}
}

#include <streambuf>

#include <iterator>
#include "cheats/menu.h"
void c_config::load(size_t id) noexcept {
	//g_cfg = { };
	if (!std::filesystem::is_directory(path))
		std::filesystem::create_directory(path);

	std::ifstream in{ path / configs[id] };

	if (!in.good())
		return;

	ArchiveX<std::ifstream>{ in } >> g_cfg;

	in.close();
}

void c_config::save(size_t id) const noexcept {
	if (!std::filesystem::is_directory(path))
		std::filesystem::create_directory(path);


	std::ofstream out{ path / configs[id] };

	if (!out.good())
		return;

	ArchiveX<std::ofstream>{ out } << g_cfg;

	out.close();
}

void c_config::add(const char* name) noexcept {
	if (*name && std::find(std::cbegin(configs), std::cend(configs), name) == std::cend(configs))
		configs.emplace_back(name);
}

void c_config::remove(size_t id) noexcept {
	std::filesystem::remove(path / configs[id]);
	configs.erase(configs.cbegin() + id);
}

void c_config::rename(size_t item, const char* newName) noexcept {
	std::filesystem::rename(path / configs[item], path / newName);
	configs[item] = newName;
}

void c_config::reset() noexcept {
	g_cfg = { };
}*/