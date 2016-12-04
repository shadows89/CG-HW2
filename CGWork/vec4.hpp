//
//  vec4.hpp
//  LList
//
//  Created by Efi Shtain on 24/11/2016.
//  Copyright © 2016 Efi Shtain. All rights reserved.
//

#ifndef vec4_hpp
#define vec4_hpp

#define EPSILON 0.0001

#include <stdio.h>

class vec4{
protected:
	double coords[4];
public:
	vec4(){ coords[0] = 0; coords[1] = 0; coords[2] = 0; coords[3] = 0; }
	vec4(double x, double y, double z){ coords[0] = x; coords[1] = y; coords[2] = z; coords[3] = 0; }
	vec4(double x, double y, double z, double w){ coords[0] = x; coords[1] = y; coords[2] = z; coords[3] = w; }
	vec4(const vec4 &v){ coords[0] = v.coords[0]; coords[1] = v.coords[1]; coords[2] = v.coords[2]; coords[3] = v.coords[3]; }
	vec4 operator+(const vec4& v) const{
		return vec4(coords[0] + v.coords[0], coords[1] + v.coords[1], coords[2] + v.coords[2], coords[3] + v.coords[3]);
	}
	vec4 operator* (const double scalar) const{
		return vec4(coords[0] * scalar, coords[1] * scalar, coords[2] * scalar, coords[3] * scalar);
	}
	vec4 operator*(const vec4& v)const{
		return vec4(coords[0] * v.coords[0], coords[1] * v.coords[1], coords[2] * v.coords[2], coords[3] * v.coords[3]);
	}
	double& operator[](const int index){
		return coords[index];
	}
	vec4 operator-(const vec4& v){
		return vec4(coords[0] - v.coords[0], coords[1] - v.coords[1], coords[2] - v.coords[2], coords[3] - v.coords[3]);
	}

	vec4 operator-(){
		return (*this)*(-1);
	}

	static vec4 normalize(vec4 v){
		double norm = sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2) + pow(v[3], 2));
		vec4 normalized;
		normalized[0] = v[0] / norm;
		normalized[1] = v[1] / norm;
		normalized[2] = v[2] / norm;
		normalized[3] = v[3] / norm;
		return normalized;
	}

	static vec4 cross(vec4& v1, vec4& v2){
		return vec4(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
	}

	bool isZero(){
		for (int i = 0; i < 3; i++){
			if ((*this)[i] < EPSILON)
				continue;
			else
				return FALSE;
		}
		return TRUE;
	}
    
};
#endif /* vec4_hpp */
