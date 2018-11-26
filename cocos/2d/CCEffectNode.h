#pragma once

#include "2d/CCNode.h"
#include "base/CCData.h"

NS_CC_BEGIN

class EffectManager;
class Effect;
class EffectEmitter;

class CC_DLL EffectNode : public Node
{
public:
	EffectNode();

	~EffectNode();

	static EffectNode* create(Data& data, std::string texPath);

	bool init(Data& data, std::string texPath);

	void setPosition(float x, float y);

	void setIsLooping(bool loop);

	void setScale(float scale);

	virtual void update(float delta) override;

	virtual void visit(IRenderer *renderer, const Mat4& parentTransform, uint32_t parentFlags) override;
private:

	EffectManager * _effectManager;
	Effect* _effect;
	EffectEmitter* _effectEmitter;
};

NS_CC_END