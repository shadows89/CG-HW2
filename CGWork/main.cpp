//
//  main.cpp
//  LList
//
//  Created by Efi Shtain on 22/11/2016.
//  Copyright © 2016 Efi Shtain. All rights reserved.
//

#include <iostream>

#include "mat4.hpp"


//struct Node{
//    Node* next;
//    double coords[3];
//};
//
//class PolyList{
//private:
//    Node* first;
//    Node* last;
//public:
//    PolyList(){
//        first = NULL;
//        last = NULL;
//    }
//    
//    void Insert(double coords[3]){
//        if(first==NULL){
//            first = new Node;
//            first->next = NULL;
//            first->coords[0] = coords[0];
//            first->coords[1] = coords[1];
//            first->coords[2] = coords[2];
//            last = first;
//        }else{
//            last->next = new Node;
//            last = last->next;
//            last->coords[0] = coords[0];
//            last->coords[1] = coords[1];
//            last->coords[2] = coords[2];
//        }
//    }
//    
//    const Node* GetList()const {
//        return first;
//    }
//    
//};



int main(int argc, const char * argv[]) {
    vec4 v1(5,0,0,0);
    vec4 v2(0,0,3,0);
    vec4 v3(0,1,3,0);
    vec4 v4(1,0,0,1);

    mat4 m(v1,v2,v3,v4);
    mat4 inv = m.inv_minor();
    m.print();
    inv.print();
    (m*inv).print();

    
    return 0;
}
