#include "Matrix4.h"

Matrix4::Matrix4(void)	{
	ToIdentity();
}

Matrix4::Matrix4( float elements[16] )	{
	memcpy(this->values,elements,16*sizeof(float));
}

Matrix4::~Matrix4(void)	{
	ToIdentity();
}

void Matrix4::ToIdentity() {
	ToZero();
	values[0]  = 1.0f;
	values[5]  = 1.0f;
	values[10] = 1.0f;
	values[15] = 1.0f;
}

void Matrix4::ToZero()	{
	for(int i = 0; i < 16; i++)	{
		values[i] = 0.0f;
	}
}

Vector3 Matrix4::GetPositionVector() const{
	return Vector3(values[12],values[13],values[14]);
}

void	Matrix4::SetPositionVector(const Vector3 in) {
	values[12] = in.x;
	values[13] = in.y;
	values[14] = in.z;		
}

Vector3 Matrix4::GetScalingVector() const{
	return Vector3(values[0],values[5],values[10]);
}

void	Matrix4::SetScalingVector(const Vector3 &in) {
	values[0]  = in.x;
	values[5]  = in.y;
	values[10] = in.z;		
}

Matrix4 Matrix4::Perspective(float znear, float zfar, float aspect, float fov) {
	Matrix4 m;

	const float h = 1.0f / tan(fov*PI_OVER_360);
	float neg_depth = znear-zfar;

	m.values[0]		= h / aspect;
	m.values[5]		= h;
	m.values[10]	= (zfar + znear)/neg_depth;
	m.values[11]	= -1.0f;
	m.values[14]	= 2.0f*(znear*zfar)/neg_depth;
	m.values[15]	= 0.0f;

	return m;
}

//http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
Matrix4 Matrix4::Orthographic(float znear, float zfar,float right, float left, float top, float bottom)	{
	Matrix4 m;

	m.values[0]	= 2.0f / (right-left);
	m.values[5]	= 2.0f / (top-bottom);
	m.values[10]	= -2.0f / (zfar-znear);

	m.values[12]  = -(right+left)/(right-left);
	m.values[13]  = -(top+bottom)/(top-bottom);
	m.values[14]  = -(zfar+znear)/(zfar-znear);
	m.values[15]  = 1.0f;

	return m;
}

Matrix4 Matrix4::BuildViewMatrix(const Vector3 &from, const Vector3 &lookingAt, const Vector3 up /*= Vector3(1,0,0)*/ )	{
	Matrix4 r;
	r.SetPositionVector(Vector3(-from.x,-from.y,-from.z));

	Matrix4 m;

	Vector3 f = (lookingAt - from);
	f.Normalise();

	Vector3 s = Vector3::Cross(f,up);
	Vector3 u = Vector3::Cross(s,f);

	m.values[0] = s.x;
	m.values[4] = s.y;
	m.values[8] = s.z;

	m.values[1] = u.x;
	m.values[5] = u.y;
	m.values[9] = u.z;

	m.values[2]  = -f.x;
	m.values[6]  = -f.y;
	m.values[10] = -f.z;

	return m*r;
}

Matrix4 Matrix4::Rotation(float degrees, const Vector3 &inaxis)	 {
	Matrix4 m;

	Vector3 axis = inaxis;

	axis.Normalise();

	float c = cos((float)DegToRad(degrees));
	float s = sin((float)DegToRad(degrees));

	m.values[0]  = (axis.x * axis.x) * (1.0f - c) + c;
	m.values[1]  = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	m.values[2]  = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

	m.values[4]  = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	m.values[5]  = (axis.y * axis.y) * (1.0f - c) + c;
	m.values[6]  = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

	m.values[8]  = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	m.values[9]  = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	m.values[10] = (axis.z * axis.z) * (1.0f - c) + c;

	return m;
}

Matrix4 Matrix4::Scale( const Vector3 &scale )	{
	Matrix4 m;

	m.values[0]  = scale.x;
	m.values[5]  = scale.y;
	m.values[10] = scale.z;	

	return m;
}

Matrix4 Matrix4::Translation( const Vector3 &translation )	{
	Matrix4 m;

	m.values[12] = translation.x;
	m.values[13] = translation.y;
	m.values[14] = translation.z;	

	return m;
}

Matrix4 Matrix4::GetTransposedRotation() {
	Matrix4 temp;
	temp.values[0] = values[0];
	temp.values[5] = values[5];
	temp.values[10] = values[10];
	temp.values[1] = values[4];
	temp.values[4] = values[1];
	temp.values[2] = values[8];
	temp.values[8] = values[2];
	temp.values[6] = values[9];
	temp.values[9] = values[6];
	return temp;
}

Matrix4 Matrix4::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	Vector3 z((eye  - target).GetNormal());
	Vector3 x((Vector3::Cross(z, up).GetNormal()));
	Vector3 y(Vector3::Cross(x, z));

	Matrix4 result;

	result.values[0] = x.x;
	result.values[4] = x.y;
	result.values[8] = x.z;
	result.values[12] = -Vector3::Dot(x, eye);

	result.values[1] = y.x;
	result.values[5] = y.y;
	result.values[9] = y.z;
	result.values[13] = -Vector3::Dot(y, eye);

	result.values[2] = z.x;
	result.values[6] = z.y;
	result.values[10] = z.z;
	result.values[14] = -Vector3::Dot(z, eye);

	result.values[3] = result.values[7] = result.values[11] = 0.0f;
	result.values[15] = 1.0f;
	return result;
}

Matrix4 Matrix4::GetInversedMatrix()
{
	double det = (this->values[0] * (this->values[5] * this->values[10] - this->values[6] * this->values[9]) -
		this->values[1] * (this->values[4] * this->values[10] - this->values[6] * this->values[8]) +
		this->values[2] * (this->values[4] * this->values[9]- this->values[5]* this->values[8]));

	Matrix4 out;

	// equal to 0
	if (abs(det) < 0.00000001)
		return out;

	double det_inv = 1.0 / det;

	out.values[0]  = det_inv * (this->values[5] * this->values[10] - this->values[6] * this->values[9]);
	out.values[1]  = -det_inv * (this->values[1] * this->values[10] - this->values[2] * this->values[9]);
	out.values[2]  = det_inv * (this->values[1] * this->values[6]- this->values[2] * this->values[5]);
	out.values[3]  = 0.0;
	   
	out.values[4]  = -det_inv * (this->values[4] * this->values[10] - this->values[6] * this->values[8]);
	out.values[5]  = det_inv * (this->values[0] * this->values[10] - this->values[2] * this->values[8]);
	out.values[6]  = -det_inv * (this->values[0]* this->values[6] - this->values[2]* this->values[4]);
	out.values[7]  = 0.0;
	   
	out.values[8]  = det_inv * (this->values[4] * this->values[9] - this->values[5] * this->values[8]);
	out.values[9]  = -det_inv * (this->values[0] * this->values[9] - this->values[1] * this->values[8]);
	out.values[10] = det_inv * (this->values[0] * this->values[5] - this->values[1] * this->values[4]);
	out.values[11] = 0.0;
	
	out.values[12] = -(this->values[12] * out.values[0] + this->values[13] * out.values[4] + this->values[14] * out.values[8]);
	out.values[13] = -(this->values[12] * out.values[1] + this->values[13] * out.values[5] + this->values[14] * out.values[9]);
	out.values[14] = -(this->values[12] * out.values[2] + this->values[13] * out.values[6] + this->values[14] * out.values[10]);
	out.values[15] = 1.0;

	return out;
}