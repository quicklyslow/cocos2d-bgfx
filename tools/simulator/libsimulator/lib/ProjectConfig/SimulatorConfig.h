
#ifndef __SIMULATOR_CONFIG_H_
#define __SIMULATOR_CONFIG_H_

#include <vector>
#include <string>
#include "SimulatorExport.h"
#include "math/CCGeometry.h"

#if defined(_WINDOWS)
#define DIRECTORY_SEPARATOR "\\"
#define DIRECTORY_SEPARATOR_CHAR '\\'
#else
#define DIRECTORY_SEPARATOR "/"
#define DIRECTORY_SEPARATOR_CHAR '/'
#endif

typedef struct _SimulatorScreenSize {
    std::string title;
    int width;
    int height;

    _SimulatorScreenSize(const std::string &title_, int width_, int height_)
    {
        title  = title_;
        width  = width_;
        height = height_;
    }
} SimulatorScreenSize;

typedef std::vector<SimulatorScreenSize> ScreenSizeArray;
typedef ScreenSizeArray::iterator ScreenSizeArrayIterator;

class CC_LIBSIM_DLL SimulatorConfig
{
public:
    static SimulatorConfig *getInstance();

    // predefined screen size
    int getScreenSizeCount() const;
    SimulatorScreenSize getScreenSize(int index) const;
    int checkScreenSize(const cocos2d::Size &size) const;

    // helper
    static void makeNormalizePath(std::string *path, const char *directorySeparator = NULL);

private:
    SimulatorConfig();

    static SimulatorConfig *_instance;

    ScreenSizeArray _screenSizeArray;
};

#endif // __SIMULATOR_CONFIG_H_
