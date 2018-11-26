#include "ccHeader.h"
#include "Camera.h"
#include "View.h"

NS_CC_BEGIN

Camera::Camera(String name)
    :name_(name)
    , view_{}
    , position_{ 0, 0, 0 }
    , target_{ 0, 0, 1 }
    , up_{ 0,1, 0 }
{

}

const std::string& Camera::getName() const
{
    return name_;
}

const Vec3& Camera::getPosition()
{
    return position_;
}

const Vec3& Camera::getTarget()
{
    return target_;
}

const Vec3& Camera::getUp()
{
    return up_;
}

const float* Camera::getView()
{
    return view_;
}

BasicCamera::BasicCamera(String name)
    :Camera(name)
    ,transformDirty_(true)
    ,rotation_(0.0f)
{

}

void BasicCamera::setRotation(float var)
{
    rotation_ = var;
    transformDirty_ = true;
}

float BasicCamera::getRotation() const
{
    return rotation_;
}

void BasicCamera::setPosition(const Vec3& position)
{
    position_ = position;
    transformDirty_ = true;
}

void BasicCamera::setTarget(const Vec3& target)
{
    target_ = target;
    transformDirty_ = true;
}

const Vec3& BasicCamera::getUp()
{
    updateView();
    return up_;
}

void BasicCamera::updateView()
{
    if (transformDirty_)
    {
        transformDirty_ = false;
        Vec3 dest{};
        bx::vec3Sub(dest, target_, position_);
        float distance = bx::vec3Length(dest);
        if (distance == 0.0f)
        {
            bx::mtxIdentity(view_);
        }
        else
        {
            float rotateX = std::asin(dest.y / distance);
            float rotateY = 0.0f;
            if (dest.x != 0.0f)
            {
                rotateY = -std::atan(dest.z / dest.x);
            }
            Mat4 transform;
            bx::mtxRotateZYX(transform, rotateX, rotateY, -bx::toRad(rotation_));
            bx::vec3MulMtx(up_, Vec3{ 0, 1.0f, 0 }, transform);
            bx::vec3Norm(up_, up_);

            bx::Vec3 pos{ position_.x, position_.y, position_.z };
            bx::Vec3 target{ target_.x, target_.y, target_.z };
            bx::Vec3 up{ up_.x, up_.y, up_.z };
            bx::mtxLookAt(view_, pos, target, up);
        }
        Updated();
    }
}

const float* BasicCamera::getView()
{
    updateView();
    return Camera::getView();
}

Camera2D::Camera2D(String name)
    :Camera(name)
    , transformDirty_(true)
    , rotation_(0.0f)
    , zoom_(1.0f)
{

}

void Camera2D::setPosition(const Vec2& position)
{
    position_.x = target_.x = position.x;
    position_.y = target_.y = position.y;
    transformDirty_ = true;
}

void Camera2D::setRotation(float var)
{
    rotation_ = var;
    transformDirty_ = true;
}

float Camera2D::getRotation() const
{
    return rotation_;
}

void Camera2D::setZoom(float var)
{
    zoom_ = var;
}

float Camera2D::getZoom() const
{
    return zoom_;
}

const Vec3& Camera2D::getUp()
{
    updateView();
    return up_;
}

const float* Camera2D::getView()
{
    updateView();
    return Camera::getView();
}

void Camera2D::updateView()
{
    float z = -SharedView.getStandardDistance() / zoom_;
    if (position_.z != z)
    {
        position_.z = z;
        transformDirty_ = true;
    }
    if (transformDirty_)
    {
        transformDirty_ = false;
        Mat4 rotateZ;
        bx::mtxRotateZ(rotateZ, -bx::toRad(rotation_));
        bx::vec3MulMtx(up_, Vec3{ 0, 1.0f, 0 }, rotateZ);
        bx::vec3Norm(up_, up_);

        bx::Vec3 pos{ position_.x, position_.y, position_.z };
        bx::Vec3 target{ target_.x, target_.y, target_.z };
        bx::Vec3 up{ up_.x, up_.y, up_.z };
        bx::mtxLookAt(view_, pos, target, up);
    }
    Updated();
}


NS_CC_END