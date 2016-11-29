#include "mat4.hpp"

class Camera{
	mat4 c_transform;
	mat4 c_projection;
public:
	void setTransformation(mat4 transfrom){
		c_transform = transfrom;
	}

	void setProjection(mat4 projection){
		c_projection = projection;
	}

	void lookAt(vec4 eye , vec4 at , vec4 up){
		vec4 n = vec4::normalize(eye - at);
		vec4 u = vec4::normalize(vec4::cross(up, n));
		vec4 v = vec4::normalize(vec4::cross(n, u));
		vec4 t = vec4(0, 0, 0, 1);
		mat4 c = mat4(u, v, n, t);
		c_transform = c * mat4::translate(-eye);
	}

	mat4& transformation(){
		return c_transform;
	}

	mat4 projection(){
		return c_projection;
	}

};