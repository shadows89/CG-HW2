//
//  vec3.hpp
//  LList
//
//  Created by Efi Shtain on 24/11/2016.
//  Copyright © 2016 Efi Shtain. All rights reserved.
//

#ifndef vec3_hpp
#define vec3_hpp

#include <stdio.h>

class vec3{
protected:
    double coords[3];
public:
    vec3():coords{0,0,0}{}
    vec3(double x, double y, double z):coords{x,y,z}{}
    vec3(const vec3 &v):coords{v.coords[0],v.coords[1],v.coords[2]}{}
    vec3 operator+(const vec3& v) const{
        return vec3(coords[0]+v.coords[0],coords[1]+v.coords[1],coords[2]+v.coords[2]);
    }
    vec3 operator* (const double scalar) const{
        return vec3(coords[0]*scalar,coords[1]*scalar,coords[2]*scalar);
    }
    vec3 operator*(const vec3& v)const{
        return vec3(coords[0]*v.coords[0],coords[1]*v.coords[1],coords[2]*v.coords[2]);
    }
};

#endif /* vec3_hpp */
