#pragma once

NS_CC_BEGIN

class Camera : public Ref
{
public:
    PROPERTY_READONLY_REF(std::string, Name);
    virtual const Vec3& getPosition();
    virtual const Vec3& getTarget();
    virtual const Vec3& getUp();
    virtual const float* getView();
    std::function<void()> Updated;
protected:
    Camera(String name);
protected:
    std::string name_;
    Vec3 position_;
    Vec3 target_;
    Vec3 up_;
    Mat4 view_;
    COCOS_TYPE_OVERRIDE(Camera);
};

class BasicCamera : public Camera
{
public:
    PROPERTY(float, Rotation);
    void setPosition(const Vec3& position);
    void setTarget(const Vec3& target);
    virtual const Vec3& getUp() override;
    virtual const float* getView() override;
    CREATE_FUNC(BasicCamera);
protected:
    BasicCamera(String name);
    void updateView();
private:
    bool transformDirty_;
    float rotation_;
    COCOS_TYPE_OVERRIDE(BasicCamera);
};

class Camera2D : public Camera
{
public:
    PROPERTY(float, Rotation);
    PROPERTY(float, Zoom);
    void setPosition(const Vec2& position);
    virtual const Vec3& getUp() override;
    virtual const float* getView() override;
    CREATE_FUNC(Camera2D);
protected:
    Camera2D(String name);
    void updateView();
private:
    bool transformDirty_;
    float rotation_;
    float zoom_;
    COCOS_TYPE_OVERRIDE(Camera2D);
};

NS_CC_END