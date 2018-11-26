#pragma once

NS_CC_BEGIN

class View
{
public:
    PROPERTY_READONLY(Size, Size);
    PROPERTY_READONLY(float, StandardDistance);
    PROPERTY_READONLY(float, AspectRatio);
    PROPERTY_READONLY_REF(Mat4, Projection);
    PROPERTY(float, NearPlaneDistance);
    PROPERTY(float, FarPlaneDistance);
    PROPERTY(float, FieldOfView);
    PROPERTY(float, Scale);
    PROPERTY_READONLY(uint8_t, Id);
    PROPERTY_READONLY_REF(std::string, Name);

    void clear();
    void reset();

    template<typename Func>
    void sandwichName(String viewName, const Func& workHere)
    {
        push(viewName);
        workHere();
        pop();
    }
protected:
    View();
    void updateProjection();
    void push(String viewName);
    void pop();
    bool isEmpty();
private:
    int16_t id_;
    std::stack<std::pair<uint8_t, std::string>> views_;
    uint32_t flag_;
    float nearPlaneDistance_;
    float farPlaneDistance_;
    float fieldOfView_; //fov Y axis
    float scale_;
    Size size_;
    Mat4 projection_;
    COCOS_TYPE(View);
    SINGLETON_REF(View, Director);
};

#define SharedView \
    cocos2d::Singleton<cocos2d::View>::shared()

NS_CC_END