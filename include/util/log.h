#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#ifdef DEBUG
#define LOG_DEBUG(msg) std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << "() : " << msg << endl
#else
#define LOG_DEBUG(msg)
#endif

#define LOG_INFO(msg) std::cout << "[INFO] " << msg << endl

#define LOG_WARN(msg) std::cout << "[WARN] " << msg << endl

#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << endl

#endif // LOG_H_INCLUDED
