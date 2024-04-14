#include "path.h"

/* Check for unix */
#if defined(unix) || defined(__unix) || defined(__unix__)

const fs::path Path::USER_ROOT = fs::path(getenv("HOME"));
const fs::path Path::CONFIG_FILE = USER_ROOT / ".config/Bragi.ini";
const fs::path Path::LOG_DIR = USER_ROOT / ".local/share/Bragi";

#else

/* Not aviable yet */

#endif