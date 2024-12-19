#include <iostream>
#include "path.h"

void Path::Init() {
	/* Create a log directory */
	CreateDir(Path::LOG_DIR, 101);

	/* Create directory for config file */
	CreateDir(CONFIG_FILE.parent_path(), 102);
}

void Path::CreateDir(const fs::path dir, const int e_code) {
	if (!is_directory(dir)) {
		std::cout << "Try to create a directory: " << dir << '\n';

		/* If we cannot create a dir */
		if (!fs::create_directory(dir)) {
			std::cout << "Error: Cannot create a directory";
			exit(e_code);
		} else std::cout << "The directory was successfully created\n";
	}
}

/* UNIX */
#if defined(unix) || defined(__unix) || defined(__unix__)

const fs::path Path::USER_ROOT_DIR = fs::path(getenv("HOME"));
const fs::path Path::CONFIG_FILE = USER_ROOT_DIR / ".config/Bragi.ini";
const fs::path Path::LOG_DIR = USER_ROOT_DIR / ".local/share/Bragi";

/* Windows */
#else

const fs::path Path::USER_ROOT_DIR = fs::path(getenv("USER"));
const fs::path Path::CONFIG_FILE = USER_ROOT_DIR / "Documents/Bragi/Bragi.ini";
const fs::path Path::LOG_DIR = USER_ROOT_DIR / "AppData/Local/Bragi";

#endif