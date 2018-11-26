#ifndef __CONFIG_PARSER_H__
#define __CONFIG_PARSER_H__

#include <string>
#include <vector>
#include "json/document.h"
#include "ProjectConfig/SimulatorConfig.h"
#include "ProjectConfig/ProjectConfig.h"
#include "SimulatorExport.h"


#define CONFIG_FILE "config.json"

typedef std::vector<SimulatorScreenSize> ScreenSizeArray;
class CC_LIBSIM_DLL ConfigParser
{
public:
    static ConfigParser *getInstance(void);
    static void purge();

    void readConfig(const std::string &filepath = "");
	int checkScreenSize(const cocos2d::Size &size) const;

    // predefined screen size
    int getScreenSizeCount(void);
    cocos2d::Size getInitViewSize();
    std::string getInitViewName();
    std::string getEntryFile();
    rapidjson::Document& getConfigJsonRoot();
    const SimulatorScreenSize getScreenSize(int index);
    void setConsolePort(int port);
    void setUploadPort(int port);
    int getConsolePort();
    int getUploadPort();
    int getDebugPort();
    bool isLanscape();
    bool isWindowTop();
    
    void setEntryFile(const std::string &file);
    void setInitViewSize(const cocos2d::Size &size);
    void setBindAddress(const std::string &address);
    const std::string &getBindAddress();
    
private:
    ConfigParser(void);
    void setDebugPort(int port);
    static ConfigParser *s_sharedConfigParserInstance;
    ScreenSizeArray _screenSizeArray;
    cocos2d::Size _initViewSize;
    std::string _viewName;
    std::string _entryfile;
    bool _isLandscape;
    bool _isWindowTop;
    int _consolePort;
    int _uploadPort;
    int _debugPort;
    std::string _bindAddress;
    
    rapidjson::Document _docRootjson;
};

#endif  // __CONFIG_PARSER_H__

