#include "path.h"

/* Check for unix */
#if defined(unix) || defined(__unix) || defined(__unix__)

const fs::path Path::user_root = fs::path(getenv("HOME"));
const fs::path Path::config_file = user_root / ".config/bragi.ini";
const fs::path Path::log_dir = user_root / ".local/share/Bragi";

#else

/* Not aviable yet */

#endif