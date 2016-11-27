//
//  mat4.hpp
//  LList
//
//  Created by Efi Shtain on 24/11/2016.
//  Copyright © 2016 Efi Shtain. All rights reserved.
//

#ifndef mat4_hpp
#define mat4_hpp

#include <stdio.h>
#include "vec4.hpp"

class mat4{
    vec4 rows[4];
public:
    mat4(){};
    
    mat4(const vec4& v1,const vec4& v2,const vec4& v3,const vec4& v4 ){
        rows[0]=v1;
        rows[1]=v2;
        rows[2]=v3;
        rows[3]=v4;
    }
    mat4(const mat4& m){
         //mat4(m.rows[0],m.rows[1],m.rows[2],m.rows[3]);
        rows[0]=m.rows[0];
        rows[1]=m.rows[1];
        rows[2]=m.rows[2];
        rows[3]=m.rows[3];
        
    }
    mat4 operator*(const double scalar){
        return mat4(rows[0]*scalar,rows[1]*scalar,rows[2]*scalar,rows[3]*scalar);
    }
    vec4& operator[](const int index){
        return rows[index];
    }
    
    mat4 operator*( mat4& rhs){
        mat4 m;
        for (int i=0; i<4; ++i) {
            for(int j=0;j<4;++j){
                for(int k=0;k<4;++k){
                    m[i][j] += rows[i][k]*rhs[k][j];
                }
            }
        }
        return m;
    }
    
    mat4 operator+(mat4& rhs){
        return mat4(rows[0]+rhs[0],rows[1]+rhs[1],rows[2]+rhs[2],rows[3]+rhs[3]);
    }

	static mat4 scale(const double scalar){
		mat4 s = eye();
		s[0][0] = s[0][0] * scalar;
		s[1][1] = s[1][1] * scalar;
		s[2][2] = s[2][2] * scalar;
		return s;
	}

	static mat4 scale(vec4& v){
		mat4 s = eye();
		s[0][0] = s[0][0] * v[0];
		s[1][1] = s[1][1] * v[1];
		s[2][2] = s[2][2] * v[2];
		return s;
	}
    
	void updateScale(vec4& v){
		//mat4 s = eye();
		(*this)[0][0] += v[0];
		(*this)[1][1] += v[1];
		(*this)[2][2] += v[2];
		//return s;
	}

    vec4 operator*( vec4& v){
        vec4 nv;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                nv[i]+=rows[i][j]*v[j];
            }
        }
        return nv;
    }



	static mat4 translate(vec4& v){
		mat4 translate = eye();
		translate[0][3] = translate[0][3] + v[0];
		translate[1][3] = translate[1][3] + v[1];
		translate[2][3] = translate[2][3] + v[2];
		translate[3][3] = 1;
		return translate;

	}

	void updateTranslate(vec4& v){
		//mat4 translate = eye();
		(*this)[0][3] += v[0];
		(*this)[1][3] += v[1];
		(*this)[2][3] += v[2];
		//return translate;

	}
    
	static mat4 rotateX(double theta){
		mat4 r = eye();
		r[1][1] = cos(theta);
		r[1][2] = -sin(theta);
		r[2][1] = sin(theta);
		r[2][2] = cos(theta);
		return r;
	}

	static mat4 rotateY(double theta){
		mat4 r = eye();
		r[0][0] = cos(theta);
		r[0][2] = -sin(theta);
		r[2][0] = sin(theta);
		r[2][2] = cos(theta);
		return r;
	}

	static mat4 rotateZ(double theta){
		mat4 r = eye();
		r[0][0] = cos(theta);
		r[0][1] = -sin(theta);
		r[1][0] = sin(theta);
		r[1][1] = cos(theta);
		return r;
	}

	static mat4 rotate(double thetaX, double thetaY, double thetaZ){
		return rotateX(thetaX)*rotateY(thetaY)*rotateZ(thetaZ);
	}

    static mat4 eye(){
        mat4 m;
        m[0][0]=1.0;
        m[1][1]=1.0;
        m[2][2]=1.0;
        m[3][3]=1.0;
        return m;
    }
    
    
    
    //Calculated using kramers minors rule, not very efficient, but works like a charm
    mat4 inverse(){
        double inv[16], det;
        double m[16];
        int index=0;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                m[index++] = rows[i][j];
            }
        }
        
        inv[0] = m[5]  * m[10] * m[15] -
        m[5]  * m[11] * m[14] -
        m[9]  * m[6]  * m[15] +
        m[9]  * m[7]  * m[14] +
        m[13] * m[6]  * m[11] -
        m[13] * m[7]  * m[10];
        
        inv[4] = -m[4]  * m[10] * m[15] +
        m[4]  * m[11] * m[14] +
        m[8]  * m[6]  * m[15] -
        m[8]  * m[7]  * m[14] -
        m[12] * m[6]  * m[11] +
        m[12] * m[7]  * m[10];
        
        inv[8] = m[4]  * m[9] * m[15] -
        m[4]  * m[11] * m[13] -
        m[8]  * m[5] * m[15] +
        m[8]  * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];
        
        inv[12] = -m[4]  * m[9] * m[14] +
        m[4]  * m[10] * m[13] +
        m[8]  * m[5] * m[14] -
        m[8]  * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];
        
        inv[1] = -m[1]  * m[10] * m[15] +
        m[1]  * m[11] * m[14] +
        m[9]  * m[2] * m[15] -
        m[9]  * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];
        
        inv[5] = m[0]  * m[10] * m[15] -
        m[0]  * m[11] * m[14] -
        m[8]  * m[2] * m[15] +
        m[8]  * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];
        
        inv[9] = -m[0]  * m[9] * m[15] +
        m[0]  * m[11] * m[13] +
        m[8]  * m[1] * m[15] -
        m[8]  * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];
        
        inv[13] = m[0]  * m[9] * m[14] -
        m[0]  * m[10] * m[13] -
        m[8]  * m[1] * m[14] +
        m[8]  * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];
        
        inv[2] = m[1]  * m[6] * m[15] -
        m[1]  * m[7] * m[14] -
        m[5]  * m[2] * m[15] +
        m[5]  * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];
        
        inv[6] = -m[0]  * m[6] * m[15] +
        m[0]  * m[7] * m[14] +
        m[4]  * m[2] * m[15] -
        m[4]  * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];
        
        inv[10] = m[0]  * m[5] * m[15] -
        m[0]  * m[7] * m[13] -
        m[4]  * m[1] * m[15] +
        m[4]  * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];
        
        inv[14] = -m[0]  * m[5] * m[14] +
        m[0]  * m[6] * m[13] +
        m[4]  * m[1] * m[14] -
        m[4]  * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];
        
        inv[3] = -m[1] * m[6] * m[11] +
        m[1] * m[7] * m[10] +
        m[5] * m[2] * m[11] -
        m[5] * m[3] * m[10] -
        m[9] * m[2] * m[7] +
        m[9] * m[3] * m[6];
        
        inv[7] = m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];
        
        inv[11] = -m[0] * m[5] * m[11] +
        m[0] * m[7] * m[9] +
        m[4] * m[1] * m[11] -
        m[4] * m[3] * m[9] -
        m[8] * m[1] * m[7] +
        m[8] * m[3] * m[5];
        
        inv[15] = m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];
        
        det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
        
        if (det == 0)
            throw "invalid";
        
        det = 1.0 / det;
        mat4 inverted;
        index=0;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                inverted[i][j] = inv[index++]*det;
            }
        }
        
        return inverted;
        
        
    }
    
    //for now unstable, without choosing best pivot
    
  /*  void print(){
        std::cout<<"******Matrix******"<<std::endl;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                std::cout<<rows[i][j]<<" ";
            }
            std::cout<<std::endl;
        }
        std::cout<<"*******DONE*******"<<std::endl;
    }*/
};

#endif /* mat4_hpp */
