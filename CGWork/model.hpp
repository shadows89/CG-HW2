//
//  model.hpp
//  LList
//
//  Created by Efi Shtain on 25/11/2016.
//  Copyright © 2016 Efi Shtain. All rights reserved.
//

#ifndef model_hpp
#define model_hpp

#include <stdio.h>
#include <vector>
#include "vec4.hpp"
using namespace std;
#include "mat4.hpp"
class Model{
protected:
    virtual ~Model(){}
    void virtual draw()=0;
    std::vector<vec4> Geomtry;
    mat4 mTransform;
};


#endif /* model_hpp */
