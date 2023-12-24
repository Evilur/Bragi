#ifndef BRAGI_PATH_H
#define BRAGI_PATH_H

#include <filesystem>

namespace fs = std::filesystem;

/* Class for getting the path in the different OS */
class Path {
public:
	static const fs::path config_file;
	static const fs::path log_dir;
private:
	static const fs::path user_root;
};

#endif
