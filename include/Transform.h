#if !defined(TRANSFORM_H)
#define TRANSFORM_H

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Transform {
private:
    XMFLOAT3 scale = { 1.f,1.f,1.f };
    XMFLOAT3 pos = {};
    XMFLOAT3 rollPitchYaw = {};
    
public:
    Transform() = default;
    Transform(const XMFLOAT3& Scale, const XMFLOAT3& RollPitchYaw, const XMFLOAT3& position);
    virtual ~Transform() = default;
    // 获取对象缩放比例
    XMFLOAT3 GetScale() const;
    // 获取对象缩放比例
    XMVECTOR GetScaleXM() const;

    // 获取对象欧拉角(弧度制)
    // 对象以Z-X-Y轴顺序旋转
    XMFLOAT3 GetRotation() const;
    // 获取对象欧拉角(弧度制)
    // 对象以Z-X-Y轴顺序旋转
    XMVECTOR GetRotationXM() const;

    // 获取对象位置
    XMFLOAT3 GetPosition() const;
    // 获取对象位置
    XMVECTOR GetPositionXM() const;

    // 获取右方向轴
    XMFLOAT3 GetRightAxis() const;
    // 获取右方向轴
    XMVECTOR GetRightAxisXM() const;

    // 获取上方向轴
    XMFLOAT3 GetUpAxis() const;
    // 获取上方向轴
    XMVECTOR GetUpAxisXM() const;

    // 获取前方向轴
    XMFLOAT3 GetForwardAxis() const;
    // 获取前方向轴
    XMVECTOR GetForwardAxisXM() const;

    // 获取世界变换矩阵
    XMFLOAT4X4 GetLocalToWorldMatrix() const;
    // 获取世界变换矩阵
    XMMATRIX GetLocalToWorldMatrixXM() const;

    // 获取逆世界变换矩阵
    XMFLOAT4X4 GetWorldToLocalMatrix() const;
    // 获取逆世界变换矩阵
    XMMATRIX GetWorldToLocalMatrixXM() const;

    // 设置对象缩放比例
    void SetScale(const XMFLOAT3& scale);
    // 设置对象缩放比例
    void SetScale(float x, float y, float z);

    // 设置对象欧拉角(弧度制)
    // 对象将以Z-X-Y轴顺序旋转
    void SetRotation(const XMFLOAT3& eulerAnglesInRadian);
    // 设置对象欧拉角(弧度制)
    // 对象将以Z-X-Y轴顺序旋转
    void SetRotation(float x, float y, float z);

    // 设置对象位置
    void SetPosition(const XMFLOAT3& position);
    // 设置对象位置
    void SetPosition(float x, float y, float z);

    // 指定欧拉角旋转对象
    void Rotate(const XMFLOAT3& eulerAnglesInRadian);
    // 指定以原点为中心绕轴旋转
    void RotateAxis(const XMFLOAT3& axis, float radian);
    // 指定以point为旋转中心绕轴旋转
    void RotateAround(const XMFLOAT3& point, const XMFLOAT3& axis, float radian);

    // 沿着某一方向平移
    void Translate(const XMFLOAT3& direction, float magnitude);

    // 观察某一点
    void LookAt(const XMFLOAT3& target, const XMFLOAT3& up = { 0.0f, 1.0f, 0.0f });
    // 沿着某一方向观察
    void LookTo(const XMFLOAT3& direction, const XMFLOAT3& up = { 0.0f, 1.0f, 0.0f });
    
private:
    XMFLOAT3 GetEulerAnglesFromRotationMatrix(const XMFLOAT4X4& rotationMatrix);
    
};

#endif // TRANSFORM_H
