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
    // ��ȡ�������ű���
    XMFLOAT3 GetScale() const;
    // ��ȡ�������ű���
    XMVECTOR GetScaleXM() const;

    // ��ȡ����ŷ����(������)
    // ������Z-X-Y��˳����ת
    XMFLOAT3 GetRotation() const;
    // ��ȡ����ŷ����(������)
    // ������Z-X-Y��˳����ת
    XMVECTOR GetRotationXM() const;

    // ��ȡ����λ��
    XMFLOAT3 GetPosition() const;
    // ��ȡ����λ��
    XMVECTOR GetPositionXM() const;

    // ��ȡ�ҷ�����
    XMFLOAT3 GetRightAxis() const;
    // ��ȡ�ҷ�����
    XMVECTOR GetRightAxisXM() const;

    // ��ȡ�Ϸ�����
    XMFLOAT3 GetUpAxis() const;
    // ��ȡ�Ϸ�����
    XMVECTOR GetUpAxisXM() const;

    // ��ȡǰ������
    XMFLOAT3 GetForwardAxis() const;
    // ��ȡǰ������
    XMVECTOR GetForwardAxisXM() const;

    // ��ȡ����任����
    XMFLOAT4X4 GetLocalToWorldMatrix() const;
    // ��ȡ����任����
    XMMATRIX GetLocalToWorldMatrixXM() const;

    // ��ȡ������任����
    XMFLOAT4X4 GetWorldToLocalMatrix() const;
    // ��ȡ������任����
    XMMATRIX GetWorldToLocalMatrixXM() const;

    // ���ö������ű���
    void SetScale(const XMFLOAT3& scale);
    // ���ö������ű���
    void SetScale(float x, float y, float z);

    // ���ö���ŷ����(������)
    // ������Z-X-Y��˳����ת
    void SetRotation(const XMFLOAT3& eulerAnglesInRadian);
    // ���ö���ŷ����(������)
    // ������Z-X-Y��˳����ת
    void SetRotation(float x, float y, float z);

    // ���ö���λ��
    void SetPosition(const XMFLOAT3& position);
    // ���ö���λ��
    void SetPosition(float x, float y, float z);

    // ָ��ŷ������ת����
    void Rotate(const XMFLOAT3& eulerAnglesInRadian);
    // ָ����ԭ��Ϊ����������ת
    void RotateAxis(const XMFLOAT3& axis, float radian);
    // ָ����pointΪ��ת����������ת
    void RotateAround(const XMFLOAT3& point, const XMFLOAT3& axis, float radian);

    // ����ĳһ����ƽ��
    void Translate(const XMFLOAT3& direction, float magnitude);

    // �۲�ĳһ��
    void LookAt(const XMFLOAT3& target, const XMFLOAT3& up = { 0.0f, 1.0f, 0.0f });
    // ����ĳһ����۲�
    void LookTo(const XMFLOAT3& direction, const XMFLOAT3& up = { 0.0f, 1.0f, 0.0f });
    
private:
    XMFLOAT3 GetEulerAnglesFromRotationMatrix(const XMFLOAT4X4& rotationMatrix);
    
};

#endif // TRANSFORM_H
