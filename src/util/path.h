#ifndef BRAGI_PATH_H
#define BRAGI_PATH_H

#include <filesystem>

namespace fs = std::filesystem;

/* Class for getting the path in the different OS */
class Path {
public:
	static const fs::path CONFIG_FILE;
	static const fs::path LOG_DIR;
private:
	static const fs::path USER_ROOT;
};

#endif
