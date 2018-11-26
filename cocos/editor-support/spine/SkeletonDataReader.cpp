#include "ccHeader.h"
#include "SkeletonDataReader.h"
#include "Atlas.h"
#include "SkeletonData.h"
#include "Cocos2dAttachmentLoader.h"
#include <spine/extension.h>
#include "SkeletonJson.h"
#include "SkeletonBinary.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "platform/CCFileUtils.h"

USING_NS_CC;


spSkeletonData * SkeletonDataReader::readSkeletonData(const std::string & skeletonDataFile, spAtlas * atlas, float scale)
{
	spSkeletonData* skeletonData = nullptr;
	spAttachmentLoader* attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(atlas));
	auto ext = skeletonDataFile.substr(skeletonDataFile.length() - 4);
	if (ext == "json")
	{
		spSkeletonJson* json = spSkeletonJson_createWithLoader(attachmentLoader);
		json->scale = scale;
		skeletonData = spSkeletonJson_readSkeletonDataFile(json, skeletonDataFile.c_str());
        skeletonData->atlas = atlas;
        skeletonData->attachmentLoader = attachmentLoader;
		CCASSERT(skeletonData, json->error ? json->error : "Error reading skeleton data file.");
		spSkeletonJson_dispose(json);
	}
	else if (ext == "skel")
	{
		spSkeletonBinary* binary = spSkeletonBinary_createWithLoader(attachmentLoader);
		binary->scale = scale;
		skeletonData = spSkeletonBinary_readSkeletonDataFile(binary, skeletonDataFile.c_str());
        skeletonData->atlas = atlas;
        skeletonData->attachmentLoader = attachmentLoader;
		CCASSERT(skeletonData, binary->error ? binary->error : "Error reading skeleton data file.");
		spSkeletonBinary_dispose(binary);
	}
	//spAttachmentLoader_dispose(attachmentLoader); it will be invoked in spSkeletonData_dispose
	return skeletonData;
}

void SkeletonDataReader::readSkeletonDataAsync(const std::string & skeletonDataFile, spAtlas* atlas, float scale, const std::function<void(spSkeletonData*)>& callback)
{
    std::string ext = skeletonDataFile.substr(skeletonDataFile.length() - 4);
    if (ext == "skel")
    {
        FileUtils::getInstance()->loadFileAsyncUnsafe(skeletonDataFile, [atlas, scale, callback](uint8_t* buffer, ssize_t len)
        {
            spAttachmentLoader* attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(atlas));
            spSkeletonBinary* binary = spSkeletonBinary_createWithLoader(attachmentLoader);
            binary->scale = scale;
            spSkeletonData* skeletonData = spSkeletonBinary_readSkeletonData(binary, buffer, len);
            skeletonData->atlas = atlas;
            skeletonData->attachmentLoader = attachmentLoader;
            CCASSERT(skeletonData, binary->error ? binary->error : "Error reading skeleton data file.");
            spSkeletonBinary_dispose(binary);
            //FREE(buffer);
            callback(skeletonData);
        });
    }
    else if (ext == "json")
    {
        FileUtils::getInstance()->loadFileAsyncUnsafe(skeletonDataFile, [atlas, scale, callback](uint8_t* buffer, ssize_t len)
        {
            spAttachmentLoader* attachmentLoader = SUPER(Cocos2dAttachmentLoader_create(atlas));
            spSkeletonJson* json = spSkeletonJson_createWithLoader(attachmentLoader);
            json->scale = scale;
            spSkeletonData* skeletonData = spSkeletonJson_readSkeletonData(json, (const char*)buffer);
            skeletonData->atlas = atlas;
            skeletonData->attachmentLoader = attachmentLoader;
            CCASSERT(skeletonData, json->error ? json->error : "Error reading skeleton data file.");
            spSkeletonJson_dispose(json);
            //FREE(buffer);
            callback(skeletonData);
        });
    }
}
