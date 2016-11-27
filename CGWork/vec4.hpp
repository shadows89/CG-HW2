//
//  vec4.hpp
//  LList
//
//  Created by Efi Shtain on 24/11/2016.
//  Copyright © 2016 Efi Shtain. All rights reserved.
//

#ifndef vec4_hpp
#define vec4_hpp

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
        return vec4(coords[0]+v.coords[0],coords[1]+v.coords[1],coords[2]+v.coords[2], coords[3]+v.coords[3]);
    }
    vec4 operator* (const double scalar) const{
        return vec4(coords[0]*scalar,coords[1]*scalar,coords[2]*scalar, coords[3]*scalar);
    }
    vec4 operator*(const vec4& v)const{
        return vec4(coords[0]*v.coords[0],coords[1]*v.coords[1],coords[2]*v.coords[2], coords[3]*v.coords[3]);
    }
    double& operator[](const int index){
        return coords[index];
    }
    
};
#endif /* vec4_hpp */
