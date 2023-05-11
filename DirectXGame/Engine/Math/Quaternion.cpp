#include "Quaternion.h"
#include <cmath>

Quaternion::Quaternion()
{
}

Quaternion::Quaternion(Vector3 v, float w) :x(v.x), y(v.y), z(v.z), w(w)
{
}

Quaternion::Quaternion(float x, float y, float z, float w) :x(x), y(y), z(z), w(w)
{
}
float Quaternion::dotV(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
float Quaternion::dotQ(const Quaternion& q1, const Quaternion& q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

float Quaternion::dotQW(const Quaternion& q1, const Quaternion& q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

Vector3 Quaternion::cross(const Vector3& q1, const Vector3& q2)
{
	Vector3 Q;
	Q.x = q1.y * q2.z - q1.z * q2.y;
	Q.y = q1.z * q2.x - q1.x * q2.z;
	Q.z = q1.x * q2.y - q1.y * q2.x;

	return Q;
}

Quaternion Quaternion::Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Vector3 lv(lhs.x, lhs.y, lhs.z);
	Vector3 rv(rhs.x, rhs.y, rhs.z);

	Vector3 lrv;
	Quaternion lr;

	lrv = cross(lv, rv) + rhs.w * lv + lhs.w * rv;

	lr.w = lhs.w * rhs.w - dotQ(lhs, rhs);

	lr = { lrv.x,lrv.y,lrv.z,lr.w };

	return lr;
}

Quaternion Quaternion::Identity()
{
	return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion Quaternion::Conjugate(const Quaternion& q)
{
	return Quaternion(-q.x, -q.y, -q.z, q.w);
}

float Quaternion::norm(const Quaternion& q)
{
	return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

Vector3 Quaternion::Normalize(const Vector3& v)
{
	return Vector3();
}

Quaternion Quaternion::Normalize(const Quaternion& q)
{
	float len = norm(q);
	if (len != 0.0f)
	{
		return q / len;
	}
	return *this;
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
	float len = norm(q);
	if (len != 0.0f)
	{
		Quaternion in;
		in.x = 1.0f * Conjugate(q).x / (len * len);
		in.y = 1.0f * Conjugate(q).y / (len * len);
		in.z = 1.0f * Conjugate(q).z / (len * len);
		in.w = 1.0f * Conjugate(q).w / (len * len);
		return in;
	}
	return *this;
}

//任意軸回転を表すQuaternionの生成(axisの正規化は必須)
Quaternion Quaternion::MakeAxisAngle(const Vector3& axis, float angle)
{

	Quaternion result;
	result.x = axis.x * sinf(angle / 2.0f);
	result.y = axis.y * sinf(angle / 2.0f);
	result.z = axis.z * sinf(angle / 2.0f);
	result.w = cosf(angle / 2.0f);

	return result;
}

//ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 Quaternion::RotateVector(const Vector3& v, const Quaternion& q)
{
	Vector3 result;
	Quaternion Q(v, 0.0f);

	Quaternion Q1 = Multiply(q, Q);
	Quaternion Q2 = Conjugate(q);

	Q = Multiply(Q1, Q2);
	
	result = { Q.x,Q.y,Q.z };

	return result;
}

//Quaternionから回転行列を求める
Matrix4 Quaternion::MakeRotateMatrix(const Quaternion& q)
{
	Matrix4 result
	{
		q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,2.0f * (q.x * q.y + q.w * q.z),2.0f * (q.x * q.z - q.w * q.y),0.0f,
		2.0f * (q.x * q.y - q.w * q.z),q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,2.0f * (q.y * q.z + q.w * q.x),0.0f,
		2.0f * (q.x * q.z + q.w * q.y),2.0f * (q.y * q.z - q.w * q.x),q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
}

Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	//極小の値epsilon
	const float epsiron = 0.0005f;
	//q0,q1の内積
	float dot = dotQW(q0, q1);

	Quaternion Q0 = q0;
	
	if (dot < 0.0f)
	{
		Q0 = -Q0;		//もう片方の回転を利用
		dot = -dot;		//内積も回転
	}

	if (dot >= 1.0f - epsiron)
	{
		return (1.0f - t) * q0 + t * q1;
	}

	//なす角を求める
	float thera = std::acos(dot);

	//補間係数
	float scale0 = sinf((1.0f - t) * thera) / sinf(thera);
	float scale1 = sinf(t * thera) / sinf(thera);

	//補間係数を利用して補完したQuaternionを返す
	return scale0 * q0 + scale1 * q1;
}
Quaternion Quaternion::DirectionToDirection(const Vector3& u, const Vector3& v)
{
	Vector3 U = u;
	Vector3 V = v;
	//u,vを正規化して内積を求める
	float dot = dotV(U.normalize(), V.normalize());
	//u,vの外積を取る
	Vector3 c;
	c = cross(U, V);
	//軸は単位ベクトルである必要があるので正規化
	//u,vが単位ベクトルであっても、外積が単位ベクトルとは限らないのでここの正規化は必須
	Vector3 axis = c.normalize();
	//単位ベクトルで内積を取っているのでacosで角度を求める
	float theta = std::acos(dot);

	//axis,thetaで任意軸回転を作って返す
	Quaternion result;

	result = Quaternion
	({ axis.x * sinf(theta / 2.0f),
		axis.y * sinf(theta / 2.0f) ,
		axis.z * sinf(theta / 2.0f),
		cosf(theta / 2.0f) });
	return result;

}

Quaternion Quaternion::operator+()const
{
	return *this;
}

Quaternion Quaternion::operator-()const
{

	return Quaternion(-x, -y, -z, -w);
}

Quaternion& Quaternion::operator+=(const Quaternion& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;

	return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;

	return *this;
}

Quaternion& Quaternion::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

Quaternion& Quaternion::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;

	return *this;
}

//Quaternionクラスに属さない関数群
//二項演算子
const Quaternion operator+(const Quaternion& v1, const Quaternion& v2)
{
	Quaternion temp(v1);
	return temp += v2;
}

const Quaternion operator-(const Quaternion& v1, const Quaternion& v2)
{
	Quaternion temp(v1);
	return temp -= v2;
}

const Quaternion operator*(const Quaternion& v, float s)
{
	Quaternion temp(v);
	return temp *= s;
}

const Quaternion operator*(const Quaternion& q, const Vector3& v)
{
	Quaternion temp(q);
	temp.x *= v.x;
	temp.y *= v.y;
	temp.z *= v.z;

	return temp;
}

const Quaternion operator*(const Quaternion & q1, const Quaternion& q2)
{
	Quaternion temp(q1);
	temp.x *= q2.x;
	temp.y *= q2.y;
	temp.z *= q2.z;
	temp.w *= q2.w;

	return temp;
}

const Quaternion operator*(const Vector3& v, const Quaternion& q)
{
	Quaternion temp(q);
	temp.x *= v.x;
	temp.y *= v.y;
	temp.z *= v.z;
	return temp;
}

const Quaternion operator*(float s, const Quaternion& v)
{
	return v * s;
}

const Quaternion operator/(const Quaternion& v, float s)
{
	Quaternion temp(v);
	return temp /= s;
}