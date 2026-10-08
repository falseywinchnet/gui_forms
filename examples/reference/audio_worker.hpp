#pragma once
#include <filesystem>

// The adapter depends on Audio, which supplies the shared threading target.
[[nodiscard]] bool cancel_ogg_load(const std::filesystem::path& path);
