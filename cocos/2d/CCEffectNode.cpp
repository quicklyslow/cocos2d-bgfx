#include "ccHeader.h"
#include "CCEffectNode.h"
#include "extensions/effekseer/Effekseer.h"


NS_CC_BEGIN

EffectNode::EffectNode()
	:_effectManager(nullptr)
	,_effect(nullptr)
	,_effectEmitter(nullptr)
{

}

EffectNode::~EffectNode()
{
	if (_effectManager)
	{
		_effectManager->release();
		_effect = nullptr;
	}
}


EffectNode* EffectNode::create(Data& data, std::string texPath)
{
	EffectNode* effectNode = new (std::nothrow) EffectNode();
	if (effectNode && effectNode->init(data, texPath))
	{
		effectNode->autorelease();
		return effectNode;
	}
	CC_SAFE_DELETE(effectNode);
	return nullptr;
}

bool EffectNode::init(Data& data, std::string texPath)
{
	_effectManager = EffectManager::create(_director->getVisibleSize());
	uint8_t* buffer = new uint8_t[data.getSize()];
	memcpy(buffer, data.getBytes(), data.getSize());
	_effect = Effect::create(buffer, data.getSize(), texPath);
	if (_effect == nullptr)
		return false;

	_effectEmitter = EffectEmitter::create(_effectManager);
	_effectEmitter->setEffect(_effect);
	_effectEmitter->setPlayOnEnter(true);

	//_effectEmitter->setPosition(Vec2(0, 0));
	_effectEmitter->setScale(20);
	this->addChild(_effectEmitter, 0);

    auto emitter = EffectEmitter::create(_effectManager);
    emitter->setEffect(_effect);
    emitter->setPlayOnEnter(true);

    emitter->setScale(20);
    emitter->setPosition(300 + 100, 375);
    this->addChild(emitter, 0);
    emitter->setIsLooping(true);

	_effect->release();

	this->scheduleUpdate();
	return true;
}

void EffectNode::setIsLooping(bool loop)
{
	if (_effectEmitter)
	{
		_effectEmitter->setIsLooping(loop);
	}
}

void EffectNode::setPosition(float x, float y)
{
	if (_effectEmitter)
	{
		_effectEmitter->setPosition(x, y);
	}
}

void EffectNode::setScale(float scale)
{
	if (_effectEmitter)
	{
		_effectEmitter->setScale(20.0f * scale);
	}
}

void EffectNode::update(float delta)
{
	if (_effectManager)
	{
		_effectManager->update(delta);
	}
}

void EffectNode::visit(IRenderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (_effectManager)
	{
		_effectManager->begin(renderer, Node::_globalZOrder);
		Node::visit(renderer, parentTransform, parentFlags);
		_effectManager->end(renderer, Node::_globalZOrder);
	}
}

NS_CC_END