/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

//
//  jsb_spine_manual.cpp
//  cocos2d_js_bindings
//
//  Created by James Chen on 6/14/17.
//
//

#include "jsb_spine_manual.hpp"
#include "cocos/scripting/js-bindings/jswrapper/SeApi.h"
#include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"
#include "cocos/scripting/js-bindings/manual/jsb_global.h"
#include "cocos/scripting/js-bindings/manual/jsb_helper.hpp"
#include "cocos/scripting/js-bindings/auto/jsb_cocos2dx_spine_auto.hpp"

#include "cocos2d.h"
#include "cocos/editor-support/spine/spine.h"
#include "spine/spine-cocos2dx.h"
#include <spine/extension.h>

#include "spine/SkeletonDataReader.h"

using namespace cocos2d;

// TrackEntry registration

se::Class* __jsb_spine_TrackEntry_class = nullptr;
se::Object* __jsb_spine_TrackEntry_proto = nullptr;

static bool jsb_spine_TrackEntry_finalize(se::State& s)
{
    CCLOGINFO("jsbindings: finalizing JS object %p (spTrackEntry)", s.nativeThisObject());
    return true;
}
SE_BIND_FINALIZE_FUNC(jsb_spine_TrackEntry_finalize)

static bool jsb_spine_TrackEntry_constructor(se::State& s)
{
    assert(false);
    return true;
}
SE_BIND_CTOR(jsb_spine_TrackEntry_constructor, __jsb_spine_TrackEntry_class, jsb_spine_TrackEntry_finalize)

static bool jsb_spine_TrackEntry_get_mixingFrom(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    if (cobj->mixingFrom)
    {
        bool ok = sptrackentry_to_seval(cobj->mixingFrom, &s.rval());
        SE_PRECONDITION2(ok, false, "Converting track entry failed!");
        return true;
    }
    s.rval().setNull();
    return true;
}
SE_BIND_FUNC(jsb_spine_TrackEntry_get_mixingFrom)

static bool jsb_spine_TrackEntry_get_next(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    if (cobj->next)
    {
        bool ok = sptrackentry_to_seval(cobj->next, &s.rval());
        SE_PRECONDITION2(ok, false, "Converting track entry failed!");
        return true;
    }
    s.rval().setNull();
    return true;
}
SE_BIND_FUNC(jsb_spine_TrackEntry_get_next)

static bool jsb_spine_TrackEntry_get_trackIndex(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setInt32(cobj->trackIndex);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_trackIndex)

static bool jsb_spine_TrackEntry_get_loop(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setInt32(cobj->loop);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_loop)

static bool jsb_spine_TrackEntry_get_eventThreshold(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->eventThreshold);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_eventThreshold)

static bool jsb_spine_TrackEntry_get_attachmentThreshold(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->attachmentThreshold);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_attachmentThreshold)

static bool jsb_spine_TrackEntry_get_drawOrderThreshold(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->drawOrderThreshold);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_drawOrderThreshold)

static bool jsb_spine_TrackEntry_get_animationStart(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->animationStart);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_animationStart)

static bool jsb_spine_TrackEntry_get_animationEnd(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->animationEnd);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_animationEnd)

static bool jsb_spine_TrackEntry_get_animationLast(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->animationLast);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_animationLast)

static bool jsb_spine_TrackEntry_get_nextAnimationLast(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->nextAnimationLast);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_nextAnimationLast)

static bool jsb_spine_TrackEntry_get_delay(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->delay);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_delay)

static bool jsb_spine_TrackEntry_get_trackTime(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->trackTime);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_trackTime)

static bool jsb_spine_TrackEntry_get_trackLast(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->trackLast);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_trackLast)

static bool jsb_spine_TrackEntry_get_nextTrackLast(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->nextTrackLast);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_nextTrackLast)

static bool jsb_spine_TrackEntry_get_trackEnd(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->trackEnd);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_trackEnd)

static bool jsb_spine_TrackEntry_get_timeScale(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->timeScale);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_timeScale)

static bool jsb_spine_TrackEntry_get_alpha(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->alpha);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_alpha)

static bool jsb_spine_TrackEntry_get_mixTime(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->mixTime);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_mixTime)

static bool jsb_spine_TrackEntry_get_mixDuration(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setFloat(cobj->mixDuration);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_mixDuration)

//TODO
//static bool jsb_spine_TrackEntry_get_mixAlpha(se::State& s)
//{
//    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
//    s.rval().setFloat(cobj->mixAlpha);
//    return true;
//}
//SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_mixAlpha)

//static bool jsb_spine_TrackEntry_get_timelinesFirstCount(se::State& s)
//{
//    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
//    s.rval().setInt32(cobj->timelinesFirstCount);
//    return true;
//}
//SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_timelinesFirstCount)

static bool jsb_spine_TrackEntry_get_timelinesRotationCount(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();
    s.rval().setInt32(cobj->timelinesRotationCount);
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_timelinesRotationCount)

static bool jsb_spine_TrackEntry_get_animation(se::State& s)
{
    spTrackEntry* cobj = (spTrackEntry*) s.nativeThisObject();

    SE_PRECONDITION2(spanimation_to_seval(cobj->animation, &s.rval()), false, "Converting spAnimation failed!");
    return true;
}
SE_BIND_PROP_GET(jsb_spine_TrackEntry_get_animation)

static bool js_register_spine_TrackEntry(se::Object* obj)
{
    se::Class* cls = se::Class::create("TrackEntry", obj, nullptr, _SE(jsb_spine_TrackEntry_constructor));
    cls->defineFunction("mixingFrom", _SE(jsb_spine_TrackEntry_get_mixingFrom));
    cls->defineFunction("next", _SE(jsb_spine_TrackEntry_get_next));

    cls->defineProperty("delay", _SE(jsb_spine_TrackEntry_get_delay), nullptr);
    cls->defineProperty("trackIndex", _SE(jsb_spine_TrackEntry_get_trackIndex), nullptr);
    cls->defineProperty("loop", _SE(jsb_spine_TrackEntry_get_loop), nullptr);
    cls->defineProperty("eventThreshold", _SE(jsb_spine_TrackEntry_get_eventThreshold), nullptr);
    cls->defineProperty("attachmentThreshold", _SE(jsb_spine_TrackEntry_get_attachmentThreshold), nullptr);
    cls->defineProperty("drawOrderThreshold", _SE(jsb_spine_TrackEntry_get_drawOrderThreshold), nullptr);
    cls->defineProperty("animationStart", _SE(jsb_spine_TrackEntry_get_animationStart), nullptr);
    cls->defineProperty("animationEnd", _SE(jsb_spine_TrackEntry_get_animationEnd), nullptr);
    cls->defineProperty("animationLast", _SE(jsb_spine_TrackEntry_get_animationLast), nullptr);
    cls->defineProperty("nextAnimationLast", _SE(jsb_spine_TrackEntry_get_nextAnimationLast), nullptr);
    cls->defineProperty("trackTime", _SE(jsb_spine_TrackEntry_get_trackTime), nullptr);
    cls->defineProperty("trackLast", _SE(jsb_spine_TrackEntry_get_trackLast), nullptr);
    cls->defineProperty("nextTrackLast", _SE(jsb_spine_TrackEntry_get_nextTrackLast), nullptr);
    cls->defineProperty("trackEnd", _SE(jsb_spine_TrackEntry_get_trackEnd), nullptr);
    cls->defineProperty("timeScale", _SE(jsb_spine_TrackEntry_get_timeScale), nullptr);
    cls->defineProperty("alpha", _SE(jsb_spine_TrackEntry_get_alpha), nullptr);
    cls->defineProperty("mixTime", _SE(jsb_spine_TrackEntry_get_mixTime), nullptr);
    cls->defineProperty("mixDuration", _SE(jsb_spine_TrackEntry_get_mixDuration), nullptr);

	//TODO
    //cls->defineProperty("mixAlpha", _SE(jsb_spine_TrackEntry_get_mixAlpha), nullptr);
    //cls->defineProperty("timelinesFirstCount", _SE(jsb_spine_TrackEntry_get_timelinesFirstCount), nullptr);

    cls->defineProperty("timelinesRotationCount", _SE(jsb_spine_TrackEntry_get_timelinesRotationCount), nullptr);
    cls->defineProperty("animation", _SE(jsb_spine_TrackEntry_get_animation), nullptr);

    cls->defineFinalizeFunction(_SE(jsb_spine_TrackEntry_finalize));
    cls->install();

    JSBClassType::registerClass<spTrackEntry>(cls);
    __jsb_spine_TrackEntry_class = cls;
    __jsb_spine_TrackEntry_proto = cls->getProto();

    spTrackEntry_setDisposeCallback([](spTrackEntry* entry){
        se::Object* seObj = nullptr;

        auto iter = se::NativePtrToObjectMap::find(entry);
        if (iter != se::NativePtrToObjectMap::end())
        {
            // Save se::Object pointer for being used in cleanup method.
            seObj = iter->second;
            // Unmap native and js object since native object was destroyed.
            // Otherwise, it may trigger 'assertion' in se::Object::setPrivateData later
            // since native obj is already released and the new native object may be assigned with
            // the same address.
            se::NativePtrToObjectMap::erase(iter);
        }
        else
        {
            return;
        }

        auto cleanup = [seObj](){

            auto se = se::ScriptEngine::getInstance();
            if (!se->isValid() || se->isInCleanup())
                return;

            se::AutoHandleScope hs;
            se->clearException();

            // The mapping of native object & se::Object was cleared in above code.
            // The private data (native object) may be a different object associated with other se::Object.
            // Therefore, don't clear the mapping again.
			//seObj->clearPrivateData(false);FIXME
			seObj->unroot();
            seObj->decRef();
        };

        if (!se::ScriptEngine::getInstance()->isGarbageCollecting())
        {
            cleanup();
        }
        else
        {
            CleanupTask::pushTaskToAutoReleasePool(cleanup);
        }
    });

    se::ScriptEngine::getInstance()->clearException();
    return true;
}


// SkeletonData

static cocos2d::Map<std::string, cocos2d::Texture2D*>* _preloadedAtlasTextures = nullptr;
static cocos2d::Texture2D* _getPreloadedAtlasTexture(const char* path)
{
	assert(_preloadedAtlasTextures);
	auto it = _preloadedAtlasTextures->find(path);
	return it != _preloadedAtlasTextures->end() ? it->second : nullptr;
}

se::Class* __jsb_spine_SkeletonData_class = nullptr;
//se::Object* __jsb_spine_SkeletonData_proto = nullptr;

static bool jsb_spine_SkeletonData_finalize(se::State& s)
{
	spSkeletonData_dispose((spSkeletonData*)s.nativeThisObject());
	return true;
}
SE_BIND_FINALIZE_FUNC(jsb_spine_SkeletonData_finalize)

static bool jsb_spine_SkeletonData_constructor(se::State& s)
{
	const auto& args = s.args();
	int argc = (int)args.size();

	bool ok = false;

	std::string filePath;
	ok = seval_to_std_string(args[0], &filePath);
	SE_PRECONDITION2(ok, false, "jsb_spine_SkeletonData_constructor: Invalid json path!");

	std::string atlasText;
	ok = seval_to_std_string(args[1], &atlasText);
	SE_PRECONDITION2(ok, false, "jsb_spine_SkeletonData_constructor: Invalid atlas content!");

	cocos2d::Map<std::string, cocos2d::Texture2D*> textures;
	ok = seval_to_Map_string_key(args[2], &textures);
	SE_PRECONDITION2(ok, false, "jsb_spine_SkeletonData_constructor: Invalid textures!");

	float scale = 1.0f;
	ok = seval_to_float(args[3], &scale);
	SE_PRECONDITION2(ok, false, "jsb_spine_SkeletonData_constructor: Invalid scale!");

	// create atlas from preloaded texture

	_preloadedAtlasTextures = &textures;
	spine::spAtlasPage_setCustomTextureLoader(_getPreloadedAtlasTexture);

	spAtlas* atlas = spAtlas_create(atlasText.c_str(), (int)atlasText.size(), "", nullptr);
	CCASSERT(atlas, "Error creating atlas.");

	_preloadedAtlasTextures = nullptr;
	spine::spAtlasPage_setCustomTextureLoader(nullptr);

	if (argc == 4) {
		spSkeletonData* skeletonData = SkeletonDataReader::readSkeletonData(filePath, atlas, scale);
		s.thisObject()->setPrivateData(skeletonData);
		return true;
	}

	if (argc == 5) {
		se::Value jsThis(s.thisObject(), true);
		se::Value jsFunc(args[4]);
		SkeletonDataReader::readSkeletonDataAsync(filePath, atlas, scale, [=](spSkeletonData* data) -> void {
			se::ScriptEngine::getInstance()->clearException();
			se::AutoHandleScope hs;

            jsThis.toObject()->setPrivateData(data);
			bool succeed = jsFunc.toObject()->call(se::EmptyValueArray, nullptr);
			if (!succeed) {
				se::ScriptEngine::getInstance()->clearException();
			}
		});
		return true;
	}

	SE_REPORT_ERROR("wrong number of arguments: %d", (int)argc);
	return false;
}
SE_BIND_CTOR(jsb_spine_SkeletonData_constructor, __jsb_spine_SkeletonData_class, jsb_spine_SkeletonData_finalize)

static bool js_register_spine_SkeletonData(se::Object* obj)
{
	se::Class* cls = se::Class::create("SGSkeletonData", obj, nullptr, _SE(jsb_spine_SkeletonData_constructor));

	cls->defineFinalizeFunction(_SE(jsb_spine_SkeletonData_finalize));
	cls->install();

	JSBClassType::registerClass<spSkeletonData>(cls);
	__jsb_spine_SkeletonData_class = cls;
	//__jsb_spine_SkeletonData_proto = cls->getProto();

	se::ScriptEngine::getInstance()->clearException();
	return true;
}


// Bone registration

se::Class* __jsb_spine_Bone_class = nullptr;
//se::Object* __jsb_spine_Bone_proto = nullptr;

static bool jsb_spine_Bone_finalize(se::State& s)
{
	CCLOGINFO("jsbindings: finalizing JS object %p (spBone)", s.nativeThisObject());
	return true;
}
SE_BIND_FINALIZE_FUNC(jsb_spine_Bone_finalize)

static bool jsb_spine_Bone_constructor(se::State& s)
{
	assert(false);
	return true;
}
SE_BIND_CTOR(jsb_spine_Bone_constructor, __jsb_spine_Bone_class, jsb_spine_Bone_finalize)

static bool jsb_spine_Bone_get_x(se::State& s)
{
	spBone* cobj = (spBone*)s.nativeThisObject();
	s.rval().setFloat(cobj->x);
	return true;
}
SE_BIND_PROP_GET(jsb_spine_Bone_get_x)

static bool jsb_spine_Bone_set_x(se::State& s)
{
	spBone* cobj = (spBone*)s.nativeThisObject();
	cobj->x = s.args()[0].toNumber();
	return true;
}
SE_BIND_PROP_SET(jsb_spine_Bone_set_x)

static bool jsb_spine_Bone_get_y(se::State& s)
{
	spBone* cobj = (spBone*)s.nativeThisObject();
	s.rval().setFloat(cobj->y);
	return true;
}
SE_BIND_PROP_GET(jsb_spine_Bone_get_y)

static bool jsb_spine_Bone_set_y(se::State& s)
{
	spBone* cobj = (spBone*)s.nativeThisObject();
	cobj->y = s.args()[0].toNumber();
	return true;
}
SE_BIND_PROP_SET(jsb_spine_Bone_set_y)

static bool jsb_spine_Bone_get_scaleX(se::State& s)
{
	spBone* cobj = (spBone*)s.nativeThisObject();
	s.rval().setFloat(cobj->scaleX);
	return true;
}
SE_BIND_PROP_GET(jsb_spine_Bone_get_scaleX)

static bool jsb_spine_Bone_get_scaleY(se::State& s)
{
	spBone* cobj = (spBone*)s.nativeThisObject();
	s.rval().setFloat(cobj->scaleY);
	return true;
}
SE_BIND_PROP_GET(jsb_spine_Bone_get_scaleY)

static bool jsb_spine_Bone_get_worldX(se::State& s)
{
	spBone* cobj = (spBone*)s.nativeThisObject();
	s.rval().setFloat(cobj->worldX);
	return true;
}
SE_BIND_PROP_GET(jsb_spine_Bone_get_worldX)

static bool jsb_spine_Bone_get_worldY(se::State& s)
{
	spBone* cobj = (spBone*)s.nativeThisObject();
	s.rval().setFloat(cobj->worldY);
	return true;
}
SE_BIND_PROP_GET(jsb_spine_Bone_get_worldY)

static bool jsb_spine_Bone_worldToLocal(se::State& s)
{
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		cocos2d::Vec2 arg0;
		ok &= seval_to_Vec2(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "jsb_spine_Bone_worldToLocal : Error processing arguments");
		spBone* cobj = (spBone*)s.nativeThisObject();
		cocos2d::Vec2 result;
		spBone_worldToLocal(cobj, arg0.x, arg0.y, &result.x, &result.y);
		ok &= Vec2_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "jsb_spine_Bone_worldToLocal : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(jsb_spine_Bone_worldToLocal)

static bool js_register_spine_Bone(se::Object* obj)
{
	se::Class* cls = se::Class::create("Bone", obj, nullptr, _SE(jsb_spine_Bone_constructor));
	cls->defineProperty("x", _SE(jsb_spine_Bone_get_x), _SE(jsb_spine_Bone_set_x));
	cls->defineProperty("y", _SE(jsb_spine_Bone_get_y), _SE(jsb_spine_Bone_set_y));
	cls->defineProperty("scaleX", _SE(jsb_spine_Bone_get_scaleX), nullptr);
	cls->defineProperty("scaleY", _SE(jsb_spine_Bone_get_scaleY), nullptr);
	cls->defineProperty("worldX", _SE(jsb_spine_Bone_get_worldX), nullptr);
	cls->defineProperty("worldY", _SE(jsb_spine_Bone_get_worldY), nullptr);
	cls->defineFunction("worldToLocal", _SE(jsb_spine_Bone_worldToLocal));

	cls->defineFinalizeFunction(_SE(jsb_spine_Bone_finalize));
	cls->install();

	JSBClassType::registerClass<spBone>(cls);
	__jsb_spine_Bone_class = cls;
	//__jsb_spine_Bone_proto = cls->getProto();

	spBone_setDisposeCallback([](spBone* entry) {
		se::Object* seObj = nullptr;

		auto iter = se::NativePtrToObjectMap::find(entry);
		if (iter != se::NativePtrToObjectMap::end())
		{
			// Save se::Object pointer for being used in cleanup method.
			seObj = iter->second;
			// Unmap native and js object since native object was destroyed.
			// Otherwise, it may trigger 'assertion' in se::Object::setPrivateData later
			// since native obj is already released and the new native object may be assigned with
			// the same address.
			se::NativePtrToObjectMap::erase(iter);
		}
		else
		{
			return;
		}

		auto cleanup = [seObj]() {

			auto se = se::ScriptEngine::getInstance();
			if (!se->isValid() || se->isInCleanup())
				return;

			se::AutoHandleScope hs;
			se->clearException();

			// The mapping of native object & se::Object was cleared in above code.
			// The private data (native object) may be a different object associated with other se::Object.
			// Therefore, don't clear the mapping again.
			//seObj->clearPrivateData(false);FIXME
			seObj->unroot();
			seObj->decRef();
		};

		if (!se::ScriptEngine::getInstance()->isGarbageCollecting())
		{
			cleanup();
		}
		else
		{
			CleanupTask::pushTaskToAutoReleasePool(cleanup);
		}
	});

	se::ScriptEngine::getInstance()->clearException();
	return true;
}


static bool js_cocos2dx_spine_SkeletonRenderer_setFlipX(se::State& s)
{
    spine::SkeletonRenderer* cobj = (spine::SkeletonRenderer*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_cocos2dx_spine_SkeletonRenderer_setFlipX : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        bool arg0 = false;
        ok &= seval_to_boolean(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_spine_SkeletonRenderer_setFlipX : Error processing arguments");
        int oldFlipX = cobj->getSkeleton()->flipX;
        int newFlipX = arg0 ? 1 : 0;
        if(oldFlipX != newFlipX)
            cobj->getSkeleton()->flipX = newFlipX;
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_spine_SkeletonRenderer_setFlipX)

static bool js_cocos2dx_spine_SkeletonRenderer_setFlipY(se::State& s)
{
    spine::SkeletonRenderer* cobj = (spine::SkeletonRenderer*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_cocos2dx_spine_SkeletonRenderer_setFlipY : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        bool arg0 = false;
        ok &= seval_to_boolean(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_spine_SkeletonRenderer_setFlipY : Error processing arguments");
        int oldFlipY = cobj->getSkeleton()->flipY;
        int newFlipY = arg0 ? 1 : 0;
        if(oldFlipY != newFlipY)
            cobj->getSkeleton()->flipY = newFlipY;
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_spine_SkeletonRenderer_setFlipY)

static bool js_register_cocos2dx_spine_SkeletonRenderer_manual(se::Object* obj)
{
    __jsb_spine_SkeletonRenderer_proto->defineFunction("setFlipX", _SE(js_cocos2dx_spine_SkeletonRenderer_setFlipX));
    __jsb_spine_SkeletonRenderer_proto->defineFunction("setFlipY", _SE(js_cocos2dx_spine_SkeletonRenderer_setFlipY));
    se::ScriptEngine::getInstance()->clearException();
    return true;
}

bool register_all_spine_manual(se::Object* obj)
{
    js_register_spine_TrackEntry(obj);

	// Get the ns
	se::Value nsVal;
	if (!obj->getProperty("sp", &nsVal))
	{
		se::HandleObject jsobj(se::Object::createPlainObject());
		nsVal.setObject(jsobj);
		obj->setProperty("sp", nsVal);
	}
	se::Object* ns = nsVal.toObject();

	js_register_spine_SkeletonData(ns);
	js_register_spine_Bone(ns);
    js_register_cocos2dx_spine_SkeletonRenderer_manual(ns);
    return true;
}
