#pragma once


struct spAtlas;
struct spSkeletonData;

USING_NS_CC;

class SkeletonDataReader
{
public:
	static spSkeletonData* readSkeletonData(const std::string& skeletonDataFile, spAtlas* atlas, float scale);
	static void readSkeletonDataAsync(const std::string& skeletonDataFile, spAtlas* atlas, float scale, const std::function<void(spSkeletonData*)>& callback);
};

