#ifndef BRAGI_PATH_H
#define BRAGI_PATH_H

#include <filesystem>

namespace fs = std::filesystem;

/* Class for getting the path in the different OS */
class Path final {
public:
	static const fs::path CONFIG_FILE;
	static const fs::path LOG_DIR;

	static void Init();

private:
	static const fs::path USER_ROOT_DIR;

	static void CreateDir(const fs::path dir, const int e_code);
};

#endif