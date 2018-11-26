#include "ccHeader.h"
#include "View.h"
#include "base/CCDirector.h"

NS_CC_BEGIN

View::View()
    :id_(-1)
    , nearPlaneDistance_(10.0f)
    , farPlaneDistance_(10000.0f)
    , fieldOfView_(60.0f)
    , flag_(BGFX_RESET_VSYNC | BGFX_RESET_HIDPI)
    , size_(SharedDirector.getWinSize())
    , scale_(1.0f)
{

}

uint8_t View::getId() const
{
    CCAssertIf(views_.empty(), "invalid view id");
    return views_.top().first;
}

const std::string& View::getName() const
{
    CCAssertIf(views_.empty(), "invalid view id");
    return views_.top().second;
}

void View::clear()
{
    id_ = -1;
    if (!isEmpty())
    {
        decltype(views_) dummy;
        views_.swap(dummy);
    }
}

void View::push(String viewName)
{
    CCAssertIf(id_ == 255, "running views exceeds 256.");
    uint8_t viewId = static_cast<uint8_t>(++id_);
    std::string name = viewName.toString();
    bgfx::resetView(viewId);
    if (!viewName.empty())
    {
        bgfx::setViewName(viewId, name.c_str());
    }
    bgfx::setViewRect(viewId, 0, -SharedDirector.getOpenGLView()->getViewPortRect().origin.y, bgfx::BackbufferRatio::Equal);
    bgfx::setViewMode(viewId, bgfx::ViewMode::Sequential);
    bgfx::touch(viewId);
    views_.push(std::make_pair(viewId, name));
}

void View::pop()
{
    CCAssertIf(views_.empty(), "Already pop to the last view, no more views to pop.");
    views_.pop();
}

bool View::isEmpty()
{
    return views_.empty();
}

Size View::getSize() const
{
    return size_;
}

void View::setScale(float scale)
{
    scale_ = scale;
    Size winSize = SharedDirector.getOpenGLView()->getFrameSize();
    size_ = Size{ winSize.width / scale_, winSize.height / scale_ };
}

float View::getScale() const
{
    return scale_;
}

float View::getStandardDistance() const
{
    return size_.height * 0.5f / std::tan(bx::toRad(fieldOfView_) * 0.5f);
}

float View::getAspectRatio() const
{
    return size_.width / size_.height;
}

void View::setNearPlaneDistance(float var)
{
    nearPlaneDistance_ = var;
    updateProjection();
}

float View::getNearPlaneDistance() const
{
    return nearPlaneDistance_;
}

void View::setFarPlaneDistance(float var)
{
    farPlaneDistance_ = var;
    updateProjection();
}

float View::getFarPlaneDistance() const
{
    return farPlaneDistance_;
}

void View::setFieldOfView(float var)
{
    fieldOfView_ = var;
    updateProjection();
}

float View::getFieldOfView() const
{
    return fieldOfView_;
}

void View::updateProjection()
{
    bx::mtxProj(projection_, fieldOfView_, getAspectRatio(), nearPlaneDistance_, farPlaneDistance_, bgfx::getCaps()->homogeneousDepth);
    //todo:mark dirty 
}

const Mat4& View::getProjection() const
{
    return projection_;
}

void View::reset()
{
    GLView* glView = SharedDirector.getOpenGLView();
    Size size = glView->getViewPortRect().size;
    scale_ = glView->getScaleX();
    size_ = Size{ size.width / scale_, size.height / scale_ };
    farPlaneDistance_ = getStandardDistance() + size_.height / 2;
    bgfx::reset(static_cast<uint32_t>(round(size.width * glView->getFrameZoomFactor())),
        static_cast<uint32_t>(round(size.height * glView->getFrameZoomFactor())),
        flag_);
    updateProjection();
}


NS_CC_END