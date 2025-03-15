#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <cstdlib> // For getenv
#include <iostream>
#include <pwd.h>
#include <string>
#include <unistd.h>

#if defined(_WIN32)
#include <shlobj.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <limits.h>
#endif

namespace PathUtils {

inline std::string getDocumentsFolderPath() {
  std::string path;

#if defined(_WIN32)
  char docPath[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, docPath))) {
    path = docPath;
  } else {
    std::cout << "Warning: Failed to get Documents folder path on Windows. "
                 "Using fallback."
              << std::endl;
    path = "";
  }

#elif defined(__APPLE__) || defined(__linux__)
  const char *homeDir = getenv("HOME");
  if (!homeDir) {
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
      homeDir = pw->pw_dir;
    } else {
      std::cout << "Warning: Failed to determine home directory. Returning "
                   "empty path."
                << std::endl;
      return "";
    }
  }

  path = std::string(homeDir) + "/Documents";

#if defined(__linux__)
  const char *xdgDocsDir = getenv("XDG_DOCUMENTS_DIR");
  if (xdgDocsDir) {
    path = xdgDocsDir;
  }
#endif

#else
  std::cout << "Warning: Unsupported platform. Returning empty path."
            << std::endl;
  return "";
#endif

  return path;
}

inline std::string getApplicationPath() {
  std::string path;

#if defined(_WIN32)
  char programFiles[MAX_PATH];
  if (SUCCEEDED(
          SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILES, NULL, 0, programFiles))) {
    path = programFiles;
  } else {
    std::cout << "Warning: Failed to get Program Files path on Windows. Using "
                 "fallback."
              << std::endl;
    path = "C:\\Program Files";
  }

#elif defined(__APPLE__)
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  if (mainBundle) {
    CFURLRef bundleURL = CFBundleCopyBundleURL(mainBundle);
    if (bundleURL) {
      char bundlePath[PATH_MAX];
      if (CFURLGetFileSystemRepresentation(
              bundleURL, true, reinterpret_cast<UInt8 *>(bundlePath),
              PATH_MAX)) {
        path = bundlePath;
      } else {
        std::cout << "Warning: Failed to convert CFURL to file path on macOS."
                  << std::endl;
      }
      CFRelease(bundleURL);
    } else {
      std::cout << "Warning: Failed to get bundle URL on macOS." << std::endl;
    }
  } else {
    std::cout << "Warning: Failed to get main bundle on macOS. Falling back to "
                 "executable path."
              << std::endl;

    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
      path = buffer;
      size_t pos = path.find_last_of('/');
      if (pos != std::string::npos) {
        path = path.substr(0, pos);
      }
    } else {
      std::cout << "Warning: Failed to determine executable path on macOS."
                << std::endl;
      return "";
    }
  }

#elif defined(__linux__)
  char buffer[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (len != -1) {
    buffer[len] = '\0';
    path = buffer;
    size_t pos = path.find_last_of('/');
    if (pos != std::string::npos) {
      path = path.substr(0, pos);
    }
  } else {
    std::cout
        << "Warning: Failed to get application path on Linux. Using fallback."
        << std::endl;
    path = "/usr/local/share"; // Default install location for many apps
  }

#else
  std::cout << "Warning: Unsupported platform. Returning empty path."
            << std::endl;
  return "";
#endif

  return path;
}

} // namespace PathUtils

#endif // PATH_UTILS_H
