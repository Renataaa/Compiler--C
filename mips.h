//
//  mips.h
//  语法分析2
//
//  Created by Renata on 2017/12/9.
//  Copyright © 2017年 Renata. All rights reserved.
//
#include "four.h"
#include <map>
#include "symboltable.h"
#include "idtable.h"
#ifndef mips_h

#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
class MIPS {

public:

    bool return_flag = false;
    IDTable map_total;
    FOUR four;//////全局符号表
    string fuction_n = "";
    struct U{
        int offset;
        int type; ///int 1 char 2;
    };
    bool sp_flag = false;

    map<string,string> map_string;
    string to_string_new(int num)
    {
        ostringstream ostr;
        ostr << num;
        string str = ostr.str();
        return str;
    }

    void insert_2(map<string,U*> map_offset,const string& str,int offset, int type){
        U* u = new U();
        u->offset = offset;
        u->type = type;

        map_offset[str] = u;
    }
    U* find_2(map<string,U*> map_offset,const string& str) const {// 查找一个符号
        auto p = map_offset.find(str);
        if (p != map_offset.end())
            return (*p).second;
        return NULL;
    }

    int counter_str = 0;
    string array_name = "";
    string temp_loc = "";
    int loc = 0;
    void array_split(string str){
        bool flag = false;
        char name[200];
        int tmp = 0;
        char value[200];
        int i;
        for(i = 0 ; i<str.size(); i++){

            tmp = (int)str[i];
            if(tmp == 91){ ///[
                break;
            }
            name[i] =str[i];
        }
        name[i] = '\0';
        array_name = name;

        i++;
        int k = 0;
        for(int c = i; c<str.size();c++){

            tmp = (int)str[c];
            if(tmp == 93){ ////]
                break;
            }
            if(tmp<48 || tmp >57){
                loc = -1;
                flag = true;
            }
            value[k] = str[c];
            k++;
        }
        value[k] = '\0';
        if(flag){
            temp_loc = value;
        }else{
            sscanf(value, "%d",&loc);
        }
    }
    bool is_num(string str){
        int tmp = 0;
        tmp = (int)str[0];
        if((tmp>= 48 && tmp<=57)||(tmp==43)||(tmp==45)){

        }else{
            return false;
        }
        for(int l = 1 ; l<str.size(); l++){
            tmp = (int)str[l];
            if(tmp>= 48 && tmp<=57){
                continue;
            }else{
                return false;
            }

        }
        return true;
    }
    string str_data(){
        string temp = "string";
        temp = temp + to_string_new(counter_str);
        counter_str++;
        return temp;
    }
    void output(){
        //ofstream fout("/Users/renata/mips_out.txt");
        ofstream fout("D:\\mips_out.txt");
        //ofstream fout("C:\\Users\\15780\\Desktop\\mips_out.txt");
        fout<<".data"<<"\n";   //////全局定义的变量和常量j

        map<string,struct id*> map_r = map_total.getAll(0);
        map<string,struct id*>::iterator iterTest;
        for (iterTest = map_r.begin(); iterTest != map_r.end(); iterTest++)
        {
            if(iterTest->second->type == "int" && (iterTest->second->kind =="var_sim"||iterTest->second->kind =="const")){/// int a, const int a ///a: .word 3
                fout<<"\t"<<iterTest->first<<": "<<".word"<<" "<<iterTest->second->value<<"\n";

            }else if(iterTest->second->type == "int" && iterTest->second->kind =="array"){ ////int a[] array2: .space 12 #10-element interger

                fout<<"\t"<<iterTest->first<<": "<<".space"<<" "<<iterTest->second->dimension*4<<"\n";

            }else if(iterTest->second->type == "char" && (iterTest->second->kind =="var_sim"||iterTest->second->kind =="const")){ ///char const char a
              //  char tmp = iterTest->second->value;
                fout<<"\t"<<iterTest->first<<": "<<".word"<<" "<<iterTest->second->value<<"\n"; ///存ascii码

            }else if(iterTest->second->type == "char" && iterTest->second->kind =="array"){ ////char a[]

                fout<<"\t"<<iterTest->first<<": "<<".space"<<" "<<iterTest->second->dimension*4<<"\n";
            }
        }

        //////把字符串存进.data中
        for(int p = 0; p<four.vec.size(); p++){

            if(four.vec.at(p).op=="printf"){

                string arg1 = four.vec.at(p).arg1;
                int tmp = (int)arg1[0];

                if(tmp == 34){ ///字符串

                    string tmp = "";
                    tmp = str_data();
                    fout<<"\t"<<tmp<<": .asciiz "<<arg1<<"\n";
                    map_string[arg1] =  tmp;
                }
            }
        }
        fout<<"\tstring_line: .asciiz \"\\n\"";
        fout<<"\n";
        fout<<"\n";

        fout<<".text"<<"\n";   //////代码部分
        fout<<"\t"<<"j"<<" "<<"main"<<"\n"; ////跳到main函数中

        for(int i = 0; i<four.vec.size(); i++){
            string total_op =four.vec.at(i).op;
            string result = four.vec.at(i).result;
            string op = four.vec.at(i).op;
            string arg1 = four.vec.at(i).arg1;
            string arg2 = four.vec.at(i).arg2;

            if(result =="func"){ //函数声明开始
                map<string,U*> map_offset;

                struct id* func_tmp = map_total.find(op);

                int para_num = func_tmp->func->num;  ///函数中的参数
                int up_offset = 8 + para_num*4;
                int down_offset = 0;
                int down_offset_reserve = 0;
                int register_begin = func_tmp->register_begin;
                int register_end = func_tmp->register_end;

                fout<<op<<":\n";  ///函数标签
                fout<<"\tmove $fp $sp\n";
                fout<<"\n";
                if(op!="main"){
                    fout<<"\tsub $sp $sp 4\n";
                    fout<<"\tsw $ra 0($fp)\n";
                    down_offset = down_offset -4;
                }

                i++;
                while(four.vec.at(i).result == "para"){

                    fout<<"\tsub $sp $sp 4\n";
                    fout<<"\tlw $t1 "<<to_string_new(up_offset)<<"($fp)\n";
                    fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";
                    fout<<"\n";

                    U* u = new U();
                    u->offset = down_offset;
                    u->type = four.vec.at(i).op == "int" ? 1 : 2;
                    map_offset[four.vec.at(i).arg1] = u;

                   // map_offset[four.vec.at(i).arg1]->offset = down_offset;
                   // map_offset[four.vec.at(i).arg1]->type = four.vec.at(i).op == "int" ? 1 : 2;
                    //insert_2(map_offset, four.vec.at(i).arg1, down_offset, four.vec.at(i).op == "int" ? 1 : 2);
                    up_offset = up_offset - 4;
                    down_offset = down_offset -4;
                    i++;
                }

                for(int p = register_begin ; p<register_end ; p++){ ////中间变量

                    fout<<"\tsub $sp $sp 4\n";
                    string tmp = "@t"+to_string_new(p);

               //     map_offset[tmp]->offset = down_offset;
                //    map_offset[four.vec.at(i).arg1]->type = 1;////中间变量暂时都为int
                    U* u = new U();
                    u->offset = down_offset;
                    u->type =  1 ;
                    map_offset[tmp] = u;

                   // insert_2(map_offset, tmp, down_offset, 1);
                    down_offset = down_offset -4;
                    fout<<"\n";
                }
                int j;

                for( j =i ; j<four.vec.size() ;j++){

                    if(four.vec.at(j).op == "MINUS"||four.vec.at(j).op == "ADD"||four.vec.at(j).op == "MUL"||four.vec.at(j).op == "DIV"){
                        int tmp_offset = 0;
                        result = four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;
                        bool bool_left = is_num(arg1); ///左操作数 是数字true
                        bool bool_right = is_num(arg2);
                        if(!bool_left && !bool_right){ //均不是数字
                            struct U* u;
                            if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset,arg1);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";
                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            if(find_2(map_offset,arg2)!=NULL){
                                u = find_2(map_offset,arg2);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                 fout<<"\tla $t0 "<<arg2<<"\n";
                                 fout<<"\tlw $t2 0($t0)\n";
                            }

                            string tmp_op = four.vec.at(j).op == "ADD"? "add":
                                            four.vec.at(j).op == "MINUS"?"sub":
                                            four.vec.at(j).op == "MUL"?"mul":
                                            four.vec.at(j).op == "DIV"?"div":"";

                            fout<<"\t"<<tmp_op<<" $t3 $t1 $t2\n";

                            if(find_2(map_offset,result)!=NULL){
                                u = find_2(map_offset,result);
                                tmp_offset = u->offset;

                                fout<<"\tsw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";
                            }else{

                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t3 0($t0)\n";
                            }

                            fout<<"\n";
                        }else if(!bool_left && bool_right){ ///左边不是数字 右边是数字


                            struct U* u;
                            if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset,arg1);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            string tmp_op = four.vec.at(j).op == "ADD"? "add":
                                            four.vec.at(j).op == "MINUS"?"sub":
                                            four.vec.at(j).op == "MUL"?"mul":
                                            four.vec.at(j).op == "DIV"?"div":"";

                            fout<<"\t"<<tmp_op<<" $t2 $t1 "<<arg2<<"\n"; ////arg2 是数字

                            if(find_2(map_offset,result)!=NULL){
                                u = find_2(map_offset,result);
                                tmp_offset = u->offset;

                                fout<<"\tsw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t2 0($t0)\n";
                            }

                            fout<<"\n";
                        }else if(bool_left && !bool_right){ ///左边是数字

                            struct U* u;

                            fout<<"\tli $t1 "<<arg1<<"\n";

                            if(find_2(map_offset,arg2)!=NULL){
                                u = find_2(map_offset,arg2);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg2<<"\n";
                                fout<<"\tlw $t2 0($t0)\n";
                            }

                            string tmp_op = four.vec.at(j).op == "ADD"? "add":
                                            four.vec.at(j).op == "MINUS"?"sub":
                                            four.vec.at(j).op == "MUL"?"mul":
                                            four.vec.at(j).op == "DIV"?"div":"";
                            fout<<"\t"<<tmp_op<<" $t3 $t1 $t2\n";

                            if(find_2(map_offset,result)!=NULL){
                                u= find_2(map_offset,result);
                                tmp_offset = u->offset;

                                fout<<"\tsw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t3 0($t0)\n";
                            }

                            fout<<"\n";


                        }else if(bool_left && bool_right){ ///左右都是数字
                             fout<<"\tli $t1 "<<arg1<<"\n";
                             fout<<"\tli $t2 "<<arg2<<"\n";

                            string tmp_op = four.vec.at(j).op == "ADD"? "add":
                                            four.vec.at(j).op == "MINUS"?"sub":
                                            four.vec.at(j).op == "MUL"?"mul":
                                            four.vec.at(j).op == "DIV"?"div":"";
                             fout<<"\t"<<tmp_op<<" $t3 $t1 $t2\n";

                             struct U* u;
                            if(find_2(map_offset,result)!=NULL){
                                u= find_2(map_offset,result);
                                tmp_offset = u->offset;

                                fout<<"\tsw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t3 0($t0)\n";
                            }

                             fout<<"\n";

                        }

                    }else if(four.vec.at(j).op == "ASSIGN"){
                        int tmp_offset = 0;
                        result = four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;
                        bool if_ret = arg1 == "RET";
                        bool if_num = is_num(arg1);
                        bool if_array = false;

                        for(int pp =0 ;pp<result.size();pp++){
                            int judge = (int)result[pp];
                            if(judge==91){ ///包含[
                                if_array = true;
                            }
                        }

                        if(if_ret){
                            fout<<"\tmove $t1 $v0\n";
                        }
                        else if(if_num){
                            fout<<"\tli $t1 "<<arg1<<"\n";
                        }else{

                            struct U* u;
                            if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset,arg1 );
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                        }

                        if(if_array){
                            struct U* u;
                            string name_tmp = "";
                            int loc_tmp = 0;

                            array_split(result);
                            name_tmp = array_name;
                            loc_tmp = loc; /////

                            if(loc_tmp==-1){ ///////a[m]

                                if(find_2(map_offset, temp_loc)!=NULL){
                                    u = find_2(map_offset, temp_loc);
                                    tmp_offset = u->offset;
                                    fout<<"\tlw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";
                                    fout<<"\tmul $t3 $t3 4\n";
                                    fout<<"\n";
                                }else{
                                    fout<<"\tla $t0 "<<temp_loc<<"\n";
                                    fout<<"\tlw $t3 0($t0)\n";
                                    fout<<"\tmul $t3 $t3 4\n";
                                    fout<<"\n";
                                }

                            }

                            if(find_2(map_offset,name_tmp)!=NULL){

                                if(loc_tmp!=-1){ /////a[1
                                    u = find_2(map_offset,name_tmp );
                                    tmp_offset = u->offset;
                                    tmp_offset = tmp_offset - 4 * loc_tmp;

                                    fout<<"\tsw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";
                                }else{
                                    u = find_2(map_offset,name_tmp);
                                    tmp_offset = u->offset;
                                    fout<<"\tli $t4 "<<tmp_offset<<"\n";
                                    fout<<"\tsub $t4 $t4 $t3\n";
                                    fout<<"\tadd $t4 $t4 $fp\n";
                                    fout<<"\tsw $t1 0($t4)\n";

                                }
                                fout<<"\n";
                            }else{
                                if(loc_tmp!=-1){
                                    fout<<"\tla $t0 "<<name_tmp<<"\n";
                                    tmp_offset = 4 * loc;
                                    fout<<"\tsw $t1 "<<to_string_new(tmp_offset)<<"($t0)\n";
                                }else{
                                    fout<<"\tla $t0 "<<name_tmp<<"\n";
                                    fout<<"\tadd $t0 $t0 $t3\n";
                                    fout<<"\tsw $t1 0($t0)\n";
                                }
                                fout<<"\n";
                            }


                        }else{
                            struct U* u;
                            if(find_2(map_offset,result)!=NULL){
                                u = find_2(map_offset,result );
                                tmp_offset = u->offset;

                                fout<<"\tsw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t1 0($t0)\n";
                            }

                        }

                    }else if(four.vec.at(j).op == "ret"){ ////////返回空串 判断arg1 是不是空
                        return_flag = true;
                        int tmp_offset = 0;
                        result = four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;
                        bool return_num = is_num(arg1);
                        if(arg1 == ""){ ////return;

                        }else{
                            if(return_num){
                                fout<<"\tli $v0 "<<arg1<<"\n";
                            }else{
                                struct U* u;
                                if(find_2(map_offset,arg1)!=NULL){

                                    u = find_2(map_offset, arg1);

                                    tmp_offset =u->offset;

                                    fout<<"\tlw $v0 "<<to_string_new(tmp_offset)<<"($fp)\n";

                                }else{
                                    fout<<"\tla $t0 "<<arg1<<"\n";
                                    fout<<"\tlw $v0 0($t0)\n";
                                }

                                fout<<"\n";
                            }
                        }
                        if(total_op!="main"){
                            fout<<"\tlw $sp 4($fp)\n";
                            fout<<"\tlw $ra 0($fp)\n";
                            fout<<"\tlw $fp 8($fp)\n";
                            fout<<"\n";
                            fout<<"\tjr $ra\n";
                            fout<<"\n";
                        }else if(total_op=="main"){
                            fout<<"\tj label_end\n";
                        }
                    }else if(four.vec.at(j).result == "*const_int"){

                        fout<<"\tsub $sp $sp 4\n";

                        fout<<"\tli $t1 "<<four.vec.at(j).arg1<<"\n";

                        fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";

                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 1;
                        map_offset[four.vec.at(j).op] = u;


                        down_offset = down_offset - 4;

                        fout<<"\n";
                    }else if(four.vec.at(j).result == "*const_char"){
                         //fout<<"\tsub $sp $sp 4\n";
                        fout<<"\tsub $sp $sp 4\n";
                        fout<<"\tli $t1 "<<four.vec.at(j).arg1<<"\n";

                        fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";
                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 2;
                        map_offset[four.vec.at(j).op] = u;

                        down_offset = down_offset - 4;

                        fout<<"\n";

                    }else if(four.vec.at(j).result == "*int"){
                        fout<<"\tsub $sp $sp 4\n";

                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 1;
                        map_offset[four.vec.at(j).op] = u;


                        down_offset = down_offset - 4;
                        fout<<"\n";

                    }else if(four.vec.at(j).result == "*int_array"){ //sub

                        int length = atoi((char*)four.vec.at(j).arg1.data());
                        int size = length * 4 ;
                        fout<<"\tsub $sp $sp "<<to_string_new(size)<<"\n";

                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 1;
                        map_offset[four.vec.at(j).op] = u;

                        down_offset = down_offset - size;

                    }else if(four.vec.at(j).result == "*char"){
                        fout<<"\tsub $sp $sp 4\n";

                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 2;
                        map_offset[four.vec.at(j).op] = u;

                        down_offset = down_offset - 4;
                        fout<<"\n";

                    }else if(four.vec.at(j).result == "*char_array"){
                        int length = atoi((char*)four.vec.at(j).arg1.data());
                        int size = length * 4 ;
                        fout<<"\tsub $sp $sp "<<to_string_new(size)<<"\n";

                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 2;
                        map_offset[four.vec.at(j).op] = u;

                        down_offset = down_offset - size;

                    }else if(four.vec.at(j).op == "PUSH"){
                        bool push_num = false;
                      //  down_offset_reserve = down_offset;///////////////
                        fout<<"\tmove $s1 $sp\n";
                        sp_flag = true;
                        int k;
                        for(k = j; four.vec.at(k).op=="PUSH";k++){
                            push_num = is_num(four.vec.at(k).arg1);
                            if(push_num){
                                fout<<"\tsub $sp $sp 4\n";

                                fout<<"\tli $t1 "<<four.vec.at(k).arg1<<"\n";

                                fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";
                                down_offset = down_offset - 4;
                                fout<<"\n";

                            }else{
                                fout<<"\tsub $sp $sp 4\n";

                                int tmp_offset = 0 ;

                                struct U* u;

                                if(find_2(map_offset,four.vec.at(k).arg1)!=NULL){

                                    u = find_2(map_offset, four.vec.at(k).arg1);

                                    tmp_offset = u->offset;

                                    fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                                }else{
                                    fout<<"\tla $t0 "<<four.vec.at(k).arg1<<"\n";
                                    fout<<"\tlw $t1 0($t0)\n";
                                }


                                fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";

                                down_offset = down_offset - 4;
                                fout<<"\n";
                            }
                        }
                        j=k-1;

                    }else if(four.vec.at(j).op == "CALL"){
                        if(sp_flag == false){
                            //down_offset_reserve = down_offset;///////////////
                            fout<<"\tmove $s1 $sp\n";
                        }
                        sp_flag = false;
                        fout<<"\tsub $sp $sp 4\n";
                        fout<<"\tsw $fp "<<to_string_new(down_offset)<<"($fp)\n";
                        down_offset = down_offset - 4;

                        fout<<"\tsub $sp $sp 4\n";
                        fout<<"\tsw $sp "<<to_string_new(down_offset)<<"($fp)\n";

                        down_offset = down_offset - 4;
                        fout<<"\n";
                        fout<<"\tjal "<<four.vec.at(j).arg1<<"\n";
                        fout<<"\n";
                       // down_offset = down_offset_reserve;
                        struct id* para;
                        para= map_total.find(four.vec.at(j).arg1);
                        int para_num = para->func->num;
                        for(int pr = 0 ; pr <para_num+2;pr++){

                            down_offset = down_offset + 4;
                            fout<<"\tadd $sp $sp 4\n";
                        }

                    }else if(four.vec.at(j).op == "<"||four.vec.at(j).op == ">"||four.vec.at(j).op == ">="||four.vec.at(j).op == "<="||four.vec.at(j).op == "=="||four.vec.at(j).op == "!="){
                        int tmp_offset = 0;
                        result = four.vec.at(j).result; /////目标label
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;
                        string j_type = four.vec.at(j).op == "<"? "blt":
                                        four.vec.at(j).op == ">"? "bgt":
                                        four.vec.at(j).op == ">="? "bge":
                                        four.vec.at(j).op == "<="? "ble":
                                        four.vec.at(j).op == "=="? "beq":
                                        four.vec.at(j).op == "!="? "bne":"";
                        bool bool_left = is_num(arg1); ///左操作数 是数字true
                        bool bool_right = is_num(arg2);
                        if(!bool_left && !bool_right){
                            struct U* u;

                            if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset, arg1);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            if(find_2(map_offset,arg2)!=NULL){
                                u = find_2(map_offset, arg2);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg2<<"\n";
                                fout<<"\tlw $t2 0($t0)\n";
                            }

                            fout<<"\t"<<j_type<<" $t1 $t2 "<<result<<"\n";

                        }else  if(!bool_left && bool_right){ ///左边不是数字 右边是数字
                            struct U* u;

                            if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset, arg1);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            fout<<"\tli $t2 "<<arg2<<"\n";
                            fout<<"\t"<<j_type<<" $t1 $t2 "<<result<<"\n";

                        }else if(bool_left && !bool_right){ ///左边是数字
                            struct U* u;
                            fout<<"\tli $t1 "<<arg1<<"\n";

                            if(find_2(map_offset,arg2)!=NULL){
                                u = find_2(map_offset, arg2);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg2<<"\n";
                                fout<<"\tlw $t2 0($t0)\n";
                            }

                            fout<<"\t"<<j_type<<" $t1 $t2 "<<result<<"\n";

                        }else if(bool_left && bool_right){ ///左右都是数字

                            fout<<"\tli $t1 "<<arg1<<"\n";
                            fout<<"\tli $t2 "<<arg2<<"\n";

                            fout<<"\t"<<j_type<<" $t1 $t2 "<<result<<"\n";
                        }


                        fout<<"\n";

                    }else if(four.vec.at(j).op == "BNF"){
                        int tmp_offset = 0;
                        result = four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;

                        struct U* u;
                        if(find_2(map_offset,arg1)!=NULL){
                            u = find_2(map_offset, arg1);
                            tmp_offset = u->offset;

                            fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                        }else{
                            fout<<"\tla $t0 "<<arg1<<"\n";
                            fout<<"\tlw $t1 0($t0)\n";
                        }

                        fout<<"\tbeqz $t1 "<<four.vec.at(j).arg2<<"\n"; /// 等于0的时候跳转
                        fout<<"\n";
                    }else if(four.vec.at(j).op == "*label"){
                        fout<<four.vec.at(j).result<<":\n";
                    }else if(four.vec.at(j).op == "BR"){
                        fout<<"\tj "<<four.vec.at(j).arg1<<"\n";
                    }else if(four.vec.at(j).op == "~BNF"){
                        int tmp_offset = 0;
                        result = four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;

                        struct U* u;
                        if(find_2(map_offset,arg1)!=NULL){
                            u = find_2(map_offset, arg1);
                            tmp_offset = u->offset;

                            fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                        }else{
                            fout<<"\tla $t0 "<<arg1<<"\n";
                            fout<<"\tlw $t1 0($t0)\n";
                        }


                        fout<<"\tbnez $t1 "<<four.vec.at(j).arg2<<"\n"; ///不等于0的时候跳转
                        fout<<"\n";

                    }else if(four.vec.at(j).op == "printf"){ /////如果是数字

                        arg1 =four.vec.at(j).arg1;
                        int tmp = (int)arg1[0];
                        bool if_num_2 = is_num(arg1);

                        if(tmp == 34){ ///字符串
                            map<string,string>::iterator it2;
                            string name = "";
                            fout<<"\tli $v0 4\n";
                            it2 = map_string.find(arg1);
                            name = it2->second;
                            fout<<"\tla $a0 "<<name<<"\n";
                            fout<<"\tsyscall\n";
                            fout<<"\n";
                        }else if(if_num_2){
                            fout<<"\tli $v0 1\n";
                            fout<<"\tli $t1 "<<arg1<<"\n";
                            fout<<"\tmove $a0 $t1\n";
                            fout<<"\tsyscall\n";
                            fout<<"\n";

                            fout<<"\tli $v0 4\n";
                            fout<<"\tla $a0 string_line\n";   /////输出换行符
                            fout<<"\tsyscall\n";
                            fout<<"\n";
                        }else{
                            fout<<"\tli $v0 1\n";

                            struct U* u;
                            if(find_2(map_offset,arg1)!=NULL){

                                u = find_2(map_offset, arg1);

                                fout<<"\tlw $t1 "<<u->offset<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            fout<<"\tmove $a0 $t1\n";
                            fout<<"\tsyscall\n";
                            fout<<"\n";

                        }
                        fout<<"\tli $v0 4\n";
                        fout<<"\tla $a0 string_line\n";   /////输出换行符
                        fout<<"\tsyscall\n";
                        fout<<"\n";

                    }else if(four.vec.at(j).op == "printf_c"){ ////‘k' 表达式
                        arg1 =four.vec.at(j).arg1;
                        if(is_num(arg1)){ ////单个字符转成数字了
                            fout<<"\tli $v0 11\n";
                            fout<<"\tli $t1 "<<arg1<<"\n";
                            fout<<"\tmove $a0 $t1\n";
                            fout<<"\tsyscall\n";
                        }else{
                            struct U* u;
                            fout<<"\tli $v0 11\n";

                            if(find_2(map_offset,arg1)!=NULL){

                                u = find_2(map_offset, arg1);

                                fout<<"\tlw $t1 "<<u->offset<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }


                                fout<<"\tmove $a0 $t1\n";
                                fout<<"\tsyscall\n";
                                fout<<"\n";

                        }
                        fout<<"\tli $v0 4\n";
                        fout<<"\tla $a0 string_line\n";   /////输出换行符
                        fout<<"\tsyscall\n";
                        fout<<"\n";
                    }else if(four.vec.at(j).op == "scanf"){
                        bool tmpp = false;

                        string arg1 =four.vec.at(j).arg1;

                        struct U* u;
                        struct id* id_scanf_tmp;
                        int offset_tmp = 0;
                        int type = 0;
                        if(find_2(map_offset, arg1)!=NULL){
                            u = find_2(map_offset, arg1);
                            offset_tmp = u->offset;
                            type = u->type;
                        }else{
                            id_scanf_tmp = map_total.find(arg1);
                            type = id_scanf_tmp->type == "int" ? 1 : 2;
                        }


                        if(type == 1){ ////int型标识符
                            fout<<"\tli $v0 5\n";
                            fout<<"\tsyscall\n";
                            if(find_2(map_offset, arg1)!=NULL){

                                fout<<"\tsw $v0 "<<to_string_new(offset_tmp)<<"($fp)\n";
                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tsw $v0 0($t0)\n";
                            }


                            fout<<"\n";
                        }else if(type == 2){ ////char型标识符
                            fout<<"\tli $v0 12\n";
                            fout<<"\tsyscall\n";

                            if(find_2(map_offset, arg1)!=NULL){

                                fout<<"\tsw $v0 "<<to_string_new(offset_tmp)<<"($fp)\n";
                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tsw $v0 0($t0)\n";
                            }
                        }

                    }else if(four.vec.at(j).op == "load_array"){ /////t1 load a 1

                        result =four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1 ;
                        arg2 =four.vec.at(j).arg2 ;
                        bool arg2_num = is_num(arg2);

                        int tmp_offset;
                        int loc;
                        struct U* u;
                        if(arg2_num){
                            loc = atoi((char*)four.vec.at(j).arg2.data());
                        }else{
                            if(find_2(map_offset, arg2)!=NULL){
                                u = find_2(map_offset, arg2);
                                tmp_offset = u->offset;
                                fout<<"\tlw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";
                                fout<<"\tmul $t3 $t3 4\n";
                            }else{
                                fout<<"\tla $t0 "<<arg2<<"\n";
                                fout<<"\tlw $t3 0($t0)\n";
                                fout<<"\tmul $t3 $t3 4\n";
                            }
                            fout<<"\n";
                        }
                        if(find_2(map_offset, arg1)!=NULL){
                            if(arg2_num){
                                u = find_2(map_offset, arg1);
                                tmp_offset = u->offset;
                                tmp_offset = tmp_offset - 4*loc;
                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";
                            }else{
                                u = find_2(map_offset, arg1);
                                tmp_offset = u->offset;
                                fout<<"\tli $t4 "<<tmp_offset<<"\n";
                                fout<<"\tsub $t4 $t4 $t3\n";
                                fout<<"\tadd $t4 $t4 $fp\n";
                                fout<<"\tlw $t1 0($t4)\n";

                            }
                               fout<<"\n";
                        }else {
                             if(arg2_num){
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                tmp_offset = 4 * loc;
                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($t0)\n";
                             }else{
                                 fout<<"\tla $t0 "<<arg1<<"\n";
                                 fout<<"\tadd $t0 $t0 $t3\n";
                                 fout<<"\tlw $t1 0($t0)\n";
                             }
                            fout<<"\n";
                        }


                        if(find_2(map_offset, result)!=NULL){
                            u= find_2(map_offset, result);
                            tmp_offset = u->offset;
                            fout<<"\tsw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";
                        }else{
                            fout<<"\tla $t0 "<<result<<"\n";
                            fout<<"\tsw $t1 0($t0)\n";
                        }


                    }else if(four.vec.at(j).op == "end"){

                        if(total_op!="main"){

                            fout<<"\tlw $sp 4($fp)\n";
                            fout<<"\tlw $ra 0($fp)\n";
                            fout<<"\tlw $fp 8($fp)\n";
                            fout<<"\n";
                            fout<<"\tjr $ra\n";
                            fout<<"\n";
                        }
                        return_flag = false;
                        break;
                    }

                }
                ////////////一个函数结束了

                i=j;

            }


        }
    }
    void output_youhua(){

      //  ofstream fout("/Users/renata/mips_out_optimize.txt");
        ofstream fout("D:\\mips_out_optimize.txt");

        fout<<".data"<<"\n";   //////全局定义的变量和常量j

        map<string,struct id*> map_r = map_total.getAll(0);
        map<string,struct id*>::iterator iterTest;


        for (iterTest = map_r.begin(); iterTest != map_r.end(); iterTest++)
        {
            if(iterTest->second->type == "int" && (iterTest->second->kind =="var_sim"||iterTest->second->kind =="const")){/// int a, const int a ///a: .word 3
                fout<<"\t"<<iterTest->first<<": "<<".word"<<" "<<iterTest->second->value<<"\n";

            }else if(iterTest->second->type == "int" && iterTest->second->kind =="array"){ ////int a[] array2: .space 12 #10-element interger

                fout<<"\t"<<iterTest->first<<": "<<".space"<<" "<<iterTest->second->dimension*4<<"\n";

            }else if(iterTest->second->type == "char" && (iterTest->second->kind =="var_sim"||iterTest->second->kind =="const")){ ///char const char a
                // char tmp = iterTest->second->value;
                fout<<"\t"<<iterTest->first<<": "<<".word"<<" "<<iterTest->second->value<<"\n"; ///存ascii码

            }else if(iterTest->second->type == "char" && iterTest->second->kind =="array"){ ////char a[]

                fout<<"\t"<<iterTest->first<<": "<<".space"<<" "<<iterTest->second->dimension*4<<"\n";
            }
        }

        //////把字符串存进.data中
        for(int p = 0; p<four.vec.size(); p++){

            if(four.vec.at(p).op=="printf"){

                string arg1 = four.vec.at(p).arg1;
                int tmp = (int)arg1[0];

                if(tmp == 34){ ///字符串

                    string tmp = "";
                    tmp = str_data();
                    fout<<"\t"<<tmp<<": .asciiz "<<arg1<<"\n";
                    map_string[arg1] =  tmp;
                }
            }
        }
        fout<<"\tstring_line: .asciiz \"\\n\"";
        fout<<"\n";
        fout<<"\n";

        fout<<".text"<<"\n";   //////代码部分
        fout<<"\t"<<"j"<<" "<<"main"<<"\n"; ////跳到main函数中

        for(int i = 0; i<four.vec.size(); i++){
            string total_op =four.vec.at(i).op;
            string result = four.vec.at(i).result;
            string op = four.vec.at(i).op;
            string arg1 = four.vec.at(i).arg1;
            string arg2 = four.vec.at(i).arg2;

            if(result =="func"){ //函数声明开始
                fuction_n = op;
                map<string,struct id*> map_p = map_total.getAll(0);
                map<string,struct id*>::iterator it;

                    for(it = map_p.begin() ; it!=map_p.end(); it++){ /////遍历全局变量
                        if(four.register_map[fuction_n][it->first]!=0){
                            four.register_map[fuction_n][it->first]=0;
                        }
                    }



                map<string,U*> map_offset;

                struct id* func_tmp = map_total.find(op);

                int para_num = func_tmp->func->num;  ///函数中的参数
                int up_offset = 13*4 + 8 + para_num*4;
                int down_offset = 0;
                int down_offset_reserve = 0;
                int register_begin = func_tmp->register_begin;
                int register_end = func_tmp->register_end;

                fout<<op<<":\n";  ///函数标签
                fout<<"\tmove $fp $sp\n";
                fout<<"\n";
                if(op!="main"){
                    fout<<"\tsub $sp $sp 4\n";
                    fout<<"\tsw $ra 0($fp)\n";
                    down_offset = down_offset -4;
                }

                i++;
                while(four.vec.at(i).result == "para"){   ///// para int a

                    fout<<"\tsub $sp $sp 4\n";
                    fout<<"\tlw $t1 "<<to_string_new(up_offset)<<"($fp)\n";
                    if(four.register_map[fuction_n][four.vec.at(i).arg1]!=0){
                        fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][four.vec.at(i).arg1])<<" $t1\n";
                    }else{
                        fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";
                    }
                    fout<<"\n";

                    U* u = new U();
                    u->offset = down_offset;
                    u->type = four.vec.at(i).op == "int" ? 1 : 2;
                    map_offset[four.vec.at(i).arg1] = u;


                    up_offset = up_offset - 4;
                    down_offset = down_offset -4;
                    i++;
                }

                for(int p = register_begin ; p<register_end ; p++){ ////中间变量

                    fout<<"\tsub $sp $sp 4\n";
                    string tmp = "@t"+to_string_new(p);

                    //     map_offset[tmp]->offset = down_offset;
                    //    map_offset[four.vec.at(i).arg1]->type = 1;////中间变量暂时都为int
                    U* u = new U();
                    u->offset = down_offset;
                    u->type =  1 ;
                    map_offset[tmp] = u;

                    // insert_2(map_offset, tmp, down_offset, 1);
                    down_offset = down_offset -4;
                    fout<<"\n";
                }
                int j;

                for( j =i ; j<four.vec.size() ;j++){

                    if(four.vec.at(j).op == "MINUS"||four.vec.at(j).op == "ADD"||four.vec.at(j).op == "MUL"||four.vec.at(j).op == "DIV"){
                        int tmp_offset = 0;
                        result = four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;
                        bool bool_left = is_num(arg1); ///左操作数 是数字true
                        bool bool_right = is_num(arg2);
                        if(!bool_left && !bool_right){ //均不是数字
                            struct U* u;

                            if(four.register_map[fuction_n][arg1]!=0){
                                //move $t1 $t9
                                fout<<"\tmove $t1 $"<<to_string_new(four.register_map[fuction_n][arg1])<<"\n";
                            }else if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset,arg1);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";
                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            if(four.register_map[fuction_n][arg2]!=0){
                                //move $t1 $t9
                                fout<<"\tmove $t2 $"<<to_string_new(four.register_map[fuction_n][arg2])<<"\n";
                            }else if(find_2(map_offset,arg2)!=NULL){
                                u = find_2(map_offset,arg2);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg2<<"\n";
                                fout<<"\tlw $t2 0($t0)\n";
                            }

                            string tmp_op = four.vec.at(j).op == "ADD"? "add":
                            four.vec.at(j).op == "MINUS"?"sub":
                            four.vec.at(j).op == "MUL"?"mul":
                            four.vec.at(j).op == "DIV"?"div":"";

                            fout<<"\t"<<tmp_op<<" $t3 $t1 $t2\n";

                            if(four.register_map[fuction_n][result]!=0){

                                fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][result])<<" $t3\n";
                            }else if(find_2(map_offset,result)!=NULL){
                                u = find_2(map_offset,result);
                                tmp_offset = u->offset;

                                fout<<"\tsw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";
                            }else{

                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t3 0($t0)\n";
                            }

                            fout<<"\n";
                        }else if(!bool_left && bool_right){ ///左边不是数字 右边是数字


                            struct U* u;
                            if(four.register_map[fuction_n][arg1]!=0){
                                fout<<"\tmove $t1 $"<<to_string_new(four.register_map[fuction_n][arg1])<<"\n";

                            }else if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset,arg1);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            string tmp_op = four.vec.at(j).op == "ADD"? "add":
                            four.vec.at(j).op == "MINUS"?"sub":
                            four.vec.at(j).op == "MUL"?"mul":
                            four.vec.at(j).op == "DIV"?"div":"";

                            fout<<"\t"<<tmp_op<<" $t2 $t1 "<<arg2<<"\n"; ////arg2 是数字

                            if(four.register_map[fuction_n][result]!=0){
                                fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][result])<<" $t2\n";
                            }else if(find_2(map_offset,result)!=NULL){
                                u = find_2(map_offset,result);
                                tmp_offset = u->offset;

                                fout<<"\tsw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t2 0($t0)\n";
                            }

                            fout<<"\n";
                        }else if(bool_left && !bool_right){ ///左边是数字

                            struct U* u;

                            fout<<"\tli $t1 "<<arg1<<"\n";

                            if(four.register_map[fuction_n][arg2]!=0){
                                //move $t1 $t9
                                fout<<"\tmove $t2 $"<<to_string_new(four.register_map[fuction_n][arg2])<<"\n";
                            }else if(find_2(map_offset,arg2)!=NULL){
                                u = find_2(map_offset,arg2);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg2<<"\n";
                                fout<<"\tlw $t2 0($t0)\n";
                            }

                            string tmp_op = four.vec.at(j).op == "ADD"? "add":
                            four.vec.at(j).op == "MINUS"?"sub":
                            four.vec.at(j).op == "MUL"?"mul":
                            four.vec.at(j).op == "DIV"?"div":"";
                            fout<<"\t"<<tmp_op<<" $t3 $t1 $t2\n";

                            if(four.register_map[fuction_n][result]!=0){

                                fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][result])<<" $t3\n";
                            }else if(find_2(map_offset,result)!=NULL){
                                u= find_2(map_offset,result);
                                tmp_offset = u->offset;

                                fout<<"\tsw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t3 0($t0)\n";
                            }

                            fout<<"\n";


                        }else if(bool_left && bool_right){ ///左右都是数字
                            fout<<"\tli $t1 "<<arg1<<"\n";
                            fout<<"\tli $t2 "<<arg2<<"\n";

                            string tmp_op = four.vec.at(j).op == "ADD"? "add":
                            four.vec.at(j).op == "MINUS"?"sub":
                            four.vec.at(j).op == "MUL"?"mul":
                            four.vec.at(j).op == "DIV"?"div":"";
                            fout<<"\t"<<tmp_op<<" $t3 $t1 $t2\n";

                            struct U* u;
                            if(four.register_map[fuction_n][result]!=0){

                                fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][result])<<" $t3\n";
                            }else if(find_2(map_offset,result)!=NULL){
                                u= find_2(map_offset,result);
                                tmp_offset = u->offset;

                                fout<<"\tsw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t3 0($t0)\n";
                            }

                            fout<<"\n";

                        }

                    }else if(four.vec.at(j).op == "ASSIGN"){
                        int tmp_offset = 0;
                        result = four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;
                        bool if_ret = arg1 == "RET";
                        bool if_num = is_num(arg1);
                        bool if_array = false;

                        for(int pp =0 ;pp<result.size();pp++){
                            int judge = (int)result[pp];
                            if(judge==91){ ///包含[
                                if_array = true;
                            }
                        }

                        if(if_ret){
                            fout<<"\tmove $t1 $v0\n";
                        }else if(if_num){
                            fout<<"\tli $t1 "<<arg1<<"\n";
                        }else{

                            struct U* u;
                            if(four.register_map[fuction_n][arg1]!=0){
                                fout<<"\tmove $t1 $"<<to_string_new(four.register_map[fuction_n][arg1])<<"\n";
                            }else if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset,arg1 );
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                        }

                        if(if_array){
                            struct U* u;
                            string name_tmp = "";
                            int loc_tmp = 0;

                            array_split(result);
                            name_tmp = array_name;
                            loc_tmp = loc; /////

                            if(loc_tmp==-1){ ///////a[m]

                                if(four.register_map[fuction_n][temp_loc]!=0){
                                    fout<<"\tmul $t3 $"<<to_string_new(four.register_map[fuction_n][temp_loc])<<" 4\n";
                                    fout<<"\n";
                                }else if(find_2(map_offset, temp_loc)!=NULL){
                                    u = find_2(map_offset, temp_loc);
                                    tmp_offset = u->offset;
                                    fout<<"\tlw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";
                                    fout<<"\tmul $t3 $t3 4\n";
                                    fout<<"\n";
                                }else{
                                    fout<<"\tla $t0 "<<temp_loc<<"\n";
                                    fout<<"\tlw $t3 0($t0)\n";
                                    fout<<"\tmul $t3 $t3 4\n";
                                    fout<<"\n";
                                }

                            }

                            if(find_2(map_offset,name_tmp)!=NULL){   //////分别去map offset和全局中找数组名

                                if(loc_tmp!=-1){ /////a[1
                                    u = find_2(map_offset,name_tmp );
                                    tmp_offset = u->offset;
                                    tmp_offset = tmp_offset - 4 * loc_tmp;

                                    fout<<"\tsw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";
                                }else{
                                    u = find_2(map_offset,name_tmp);
                                    tmp_offset = u->offset;
                                    fout<<"\tli $t4 "<<tmp_offset<<"\n";
                                    fout<<"\tsub $t4 $t4 $t3\n";
                                    fout<<"\tadd $t4 $t4 $fp\n";
                                    fout<<"\tsw $t1 0($t4)\n";

                                }
                                fout<<"\n";
                            }else{
                                if(loc_tmp!=-1){
                                    fout<<"\tla $t0 "<<name_tmp<<"\n";
                                    tmp_offset = 4 * loc;
                                    fout<<"\tsw $t1 "<<to_string_new(tmp_offset)<<"($t0)\n";
                                }else{
                                    fout<<"\tla $t0 "<<name_tmp<<"\n";
                                    fout<<"\tadd $t0 $t0 $t3\n";
                                    fout<<"\tsw $t1 0($t0)\n";
                                }
                                fout<<"\n";
                            }


                        }else{
                            struct U* u;
                            if(four.register_map[fuction_n][result]!=0){
                                fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][result])<<" $t1\n";
                            }else if(find_2(map_offset,result)!=NULL){
                                u = find_2(map_offset,result );
                                tmp_offset = u->offset;

                                fout<<"\tsw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<result<<"\n";
                                fout<<"\tsw $t1 0($t0)\n";
                            }

                        }

                    }else if(four.vec.at(j).op == "ret"){ ////////返回空串 判断arg1 是不是空
                        return_flag = true;
                        int tmp_offset = 0;
                        result = four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;
                        bool return_num = is_num(arg1);
                        if(arg1 == ""){ ////return;

                        }else{
                            if(return_num){
                                fout<<"\tli $v0 "<<arg1<<"\n";
                            }else{
                                struct U* u;
                                if(four.register_map[fuction_n][arg1]!=0){
                                     fout<<"\tmove $v0 $"<<to_string_new(four.register_map[fuction_n][arg1])<<"\n";
                                }else if(find_2(map_offset,arg1)!=NULL){

                                    u = find_2(map_offset, arg1);

                                    tmp_offset =u->offset;

                                    fout<<"\tlw $v0 "<<to_string_new(tmp_offset)<<"($fp)\n";

                                }else{
                                    fout<<"\tla $t0 "<<arg1<<"\n";
                                    fout<<"\tlw $v0 0($t0)\n";
                                }

                                fout<<"\n";
                            }
                        }
                        if(total_op!="main"){

                            int j = 60;
                            for(int i = 13 ; i<=25; i++){
                                fout<<"\tlw $"<<to_string_new(i)<<" "<<to_string_new(j)<<"($fp)\n";
                                j = j - 4;
                            }
                            fout<<"\tlw $sp 4($fp)\n";
                            fout<<"\tlw $ra 0($fp)\n";
                            fout<<"\tlw $fp 8($fp)\n";
                            fout<<"\n";
                            fout<<"\tjr $ra\n";
                            fout<<"\n";
                        }else if(total_op=="main"){
                            fout<<"\tj label_end\n";
                        }
                    }else if(four.vec.at(j).result == "*const_int"){

                        fout<<"\tsub $sp $sp 4\n";

                        fout<<"\tli $t1 "<<four.vec.at(j).arg1<<"\n";
                        if(four.register_map[fuction_n][four.vec.at(j).op]!=0){
                            fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][four.vec.at(j).op])<<" $t1\n";
                        }else{
                            fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";
                        }
                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 1;
                        map_offset[four.vec.at(j).op] = u;


                        down_offset = down_offset - 4;

                        fout<<"\n";
                    }else if(four.vec.at(j).result == "*const_char"){
                        //fout<<"\tsub $sp $sp 4\n";
                        fout<<"\tsub $sp $sp 4\n";
                        fout<<"\tli $t1 "<<four.vec.at(j).arg1<<"\n";

                        if(four.register_map[fuction_n][four.vec.at(j).op]!=0){
                            fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][four.vec.at(j).op])<<" $t1\n";
                        }else{
                            fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";
                        }
                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 2;
                        map_offset[four.vec.at(j).op] = u;

                        down_offset = down_offset - 4;

                        fout<<"\n";

                    }else if(four.vec.at(j).result == "*int"){
                        fout<<"\tsub $sp $sp 4\n";

                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 1;
                        map_offset[four.vec.at(j).op] = u;

                        down_offset = down_offset - 4;
                        fout<<"\n";

                    }else if(four.vec.at(j).result == "*int_array"){ //sub

                        int length = atoi((char*)four.vec.at(j).arg1.data());
                        int size = length * 4 ;
                        fout<<"\tsub $sp $sp "<<to_string_new(size)<<"\n";

                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 1;
                        map_offset[four.vec.at(j).op] = u;

                        down_offset = down_offset - size;

                    }else if(four.vec.at(j).result == "*char"){
                        fout<<"\tsub $sp $sp 4\n";

                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 2;
                        map_offset[four.vec.at(j).op] = u;

                        down_offset = down_offset - 4;
                        fout<<"\n";

                    }else if(four.vec.at(j).result == "*char_array"){
                        int length = atoi((char*)four.vec.at(j).arg1.data());
                        int size = length * 4 ;
                        fout<<"\tsub $sp $sp "<<to_string_new(size)<<"\n";

                        U* u = new U();
                        u->offset = down_offset;
                        u->type = 2;
                        map_offset[four.vec.at(j).op] = u;

                        down_offset = down_offset - size;

                    }else if(four.vec.at(j).op == "PUSH"){
                        bool push_num = false;
                        //  down_offset_reserve = down_offset;///////////////
                    //    fout<<"\tmove $s1 $sp\n";
                        sp_flag = true;
                        int k;
                        for(k = j; four.vec.at(k).op=="PUSH";k++){
                            push_num = is_num(four.vec.at(k).arg1);
                            if(push_num){
                                fout<<"\tsub $sp $sp 4\n";

                                fout<<"\tli $t1 "<<four.vec.at(k).arg1<<"\n";

                                fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";
                                down_offset = down_offset - 4;
                                fout<<"\n";

                            }else{
                                fout<<"\tsub $sp $sp 4\n";

                                int tmp_offset = 0 ;

                                struct U* u;

                                if(four.register_map[fuction_n][four.vec.at(k).arg1]!=0){
                                    fout<<"\tmove $t1 $"<<to_string_new(four.register_map[fuction_n][four.vec.at(k).arg1])<<"\n";
                                }else if(find_2(map_offset,four.vec.at(k).arg1)!=NULL){

                                    u = find_2(map_offset, four.vec.at(k).arg1);

                                    tmp_offset = u->offset;

                                    fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                                }else{
                                    fout<<"\tla $t0 "<<four.vec.at(k).arg1<<"\n";
                                    fout<<"\tlw $t1 0($t0)\n";
                                }


                                fout<<"\tsw $t1 "<<to_string_new(down_offset)<<"($fp)\n";

                                down_offset = down_offset - 4;
                                fout<<"\n";
                            }
                        }
                        j=k-1;

                    }else if(four.vec.at(j).op == "CALL"){


                        if(sp_flag == false){
                            //down_offset_reserve = down_offset;///////////////
                        //    fout<<"\tmove $s1 $sp\n";
                        }
                        sp_flag = false;

                        for(int i = 13 ; i <=25; i++){
                            fout<<"\tsub $sp $sp 4\n";
                            fout<<"\tsw $"<<to_string_new(i)<<" "<<to_string_new(down_offset)<<"($fp)\n";
                            down_offset = down_offset - 4;
                        }

                        fout<<"\tsub $sp $sp 4\n";
                        fout<<"\tsw $fp "<<to_string_new(down_offset)<<"($fp)\n";
                        down_offset = down_offset - 4;

                        fout<<"\tsub $sp $sp 4\n";
                        fout<<"\tsw $sp "<<to_string_new(down_offset)<<"($fp)\n";
                        down_offset = down_offset - 4;
                        fout<<"\n";



                        fout<<"\tjal "<<four.vec.at(j).arg1<<"\n";
                        fout<<"\n";
                        // down_offset = down_offset_reserve;
                        struct id* para;
                        para= map_total.find(four.vec.at(j).arg1);
                        int para_num = para->func->num;
                        for(int pr = 0 ; pr <para_num+2+13;pr++){

                            down_offset = down_offset + 4;
                            fout<<"\tadd $sp $sp 4\n";
                        }

                    }else if(four.vec.at(j).op == "<"||four.vec.at(j).op == ">"||four.vec.at(j).op == ">="||four.vec.at(j).op == "<="||four.vec.at(j).op == "=="||four.vec.at(j).op == "!="){
                        int tmp_offset = 0;
                        result = four.vec.at(j).result; /////目标label
                        arg1 =four.vec.at(j).arg1;
                        arg2 =four.vec.at(j).arg2;
                        op = four.vec.at(j).op;
                        string j_type = four.vec.at(j).op == "<"? "blt":
                        four.vec.at(j).op == ">"? "bgt":
                        four.vec.at(j).op == ">="? "bge":
                        four.vec.at(j).op == "<="? "ble":
                        four.vec.at(j).op == "=="? "beq":
                        four.vec.at(j).op == "!="? "bne":"";
                        bool bool_left = is_num(arg1); ///左操作数 是数字true
                        bool bool_right = is_num(arg2);
                        if(!bool_left && !bool_right){
                            struct U* u;

                            if(four.register_map[fuction_n][arg1]!=0){
                                //move $t1 $t9
                                fout<<"\tmove $t1 $"<<to_string_new(four.register_map[fuction_n][arg1])<<"\n";
                            }else if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset, arg1);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            if(four.register_map[fuction_n][arg2]!=0){
                                //move $t1 $t9
                                fout<<"\tmove $t2 $"<<to_string_new(four.register_map[fuction_n][arg2])<<"\n";
                            }else if(find_2(map_offset,arg2)!=NULL){
                                u = find_2(map_offset, arg2);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg2<<"\n";
                                fout<<"\tlw $t2 0($t0)\n";
                            }

                            fout<<"\t"<<j_type<<" $t1 $t2 "<<result<<"\n";

                        }else  if(!bool_left && bool_right){ ///左边不是数字 右边是数字
                            struct U* u;

                            if(four.register_map[fuction_n][arg1]!=0){
                                fout<<"\tmove $t1 $"<<to_string_new(four.register_map[fuction_n][arg1])<<"\n";
                            }else if(find_2(map_offset,arg1)!=NULL){
                                u = find_2(map_offset, arg1);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            fout<<"\tli $t2 "<<arg2<<"\n";
                            fout<<"\t"<<j_type<<" $t1 $t2 "<<result<<"\n";

                        }else if(bool_left && !bool_right){ ///左边是数字
                            struct U* u;
                            fout<<"\tli $t1 "<<arg1<<"\n";

                            if(four.register_map[fuction_n][arg2]!=0){
                                //move $t1 $t9
                                fout<<"\tmove $t2 $"<<to_string_new(four.register_map[fuction_n][arg2])<<"\n";

                            }else if(find_2(map_offset,arg2)!=NULL){
                                u = find_2(map_offset, arg2);
                                tmp_offset = u->offset;

                                fout<<"\tlw $t2 "<<to_string_new(tmp_offset)<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg2<<"\n";
                                fout<<"\tlw $t2 0($t0)\n";
                            }

                            fout<<"\t"<<j_type<<" $t1 $t2 "<<result<<"\n";

                        }else if(bool_left && bool_right){ ///左右都是数字

                            fout<<"\tli $t1 "<<arg1<<"\n";
                            fout<<"\tli $t2 "<<arg2<<"\n";

                            fout<<"\t"<<j_type<<" $t1 $t2 "<<result<<"\n";
                        }


                        fout<<"\n";

                    }else if(four.vec.at(j).op == "*label"){
                        fout<<four.vec.at(j).result<<":\n";
                    }else if(four.vec.at(j).op == "BR"){
                        fout<<"\tj "<<four.vec.at(j).arg1<<"\n";
                    }else if(four.vec.at(j).op == "printf"){ /////如果是数字

                        arg1 =four.vec.at(j).arg1;
                        int tmp = (int)arg1[0];
                        bool if_num_2 = is_num(arg1);

                        if(tmp == 34){ ///字符串
                            map<string,string>::iterator it2;
                            string name = "";
                            fout<<"\tli $v0 4\n";
                            it2 = map_string.find(arg1);
                            name = it2->second;
                            fout<<"\tla $a0 "<<name<<"\n";
                            fout<<"\tsyscall\n";
                            fout<<"\n";
                        }else if(if_num_2){
                            fout<<"\tli $v0 1\n";
                            fout<<"\tli $t1 "<<arg1<<"\n";
                            fout<<"\tmove $a0 $t1\n";
                            fout<<"\tsyscall\n";
                            fout<<"\n";

                            fout<<"\tli $v0 4\n";
                            fout<<"\tla $a0 string_line\n";   /////输出换行符
                            fout<<"\tsyscall\n";
                            fout<<"\n";
                        }else{
                            fout<<"\tli $v0 1\n";

                            struct U* u;
                            if(four.register_map[fuction_n][arg1]!=0){
                                fout<<"\tmove $t1 $"<<to_string_new(four.register_map[fuction_n][arg1])<<"\n";
                            }else if(find_2(map_offset,arg1)!=NULL){

                                u = find_2(map_offset, arg1);

                                fout<<"\tlw $t1 "<<u->offset<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }

                            fout<<"\tmove $a0 $t1\n";
                            fout<<"\tsyscall\n";
                            fout<<"\n";

                        }
                        fout<<"\tli $v0 4\n";
                        fout<<"\tla $a0 string_line\n";   /////输出换行符
                        fout<<"\tsyscall\n";
                        fout<<"\n";

                    }else if(four.vec.at(j).op == "printf_c"){ ////‘k' 表达式
                        arg1 =four.vec.at(j).arg1;
                        if(is_num(arg1)){ ////单个字符转成数字了
                            fout<<"\tli $v0 11\n";
                            fout<<"\tli $t1 "<<arg1<<"\n";
                            fout<<"\tmove $a0 $t1\n";
                            fout<<"\tsyscall\n";
                        }else{
                            struct U* u;
                            fout<<"\tli $v0 11\n";

                            if(four.register_map[fuction_n][arg1]!=0){
                                fout<<"\tmove $t1 $"<<to_string_new(four.register_map[fuction_n][arg1])<<"\n";
                            }else if(find_2(map_offset,arg1)!=NULL){

                                u = find_2(map_offset, arg1);

                                fout<<"\tlw $t1 "<<u->offset<<"($fp)\n";

                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }


                            fout<<"\tmove $a0 $t1\n";
                            fout<<"\tsyscall\n";
                            fout<<"\n";

                        }
                        fout<<"\tli $v0 4\n";
                        fout<<"\tla $a0 string_line\n";   /////输出换行符
                        fout<<"\tsyscall\n";
                        fout<<"\n";
                    }else if(four.vec.at(j).op == "scanf"){
                        bool tmpp = false;

                        string arg1 =four.vec.at(j).arg1;

                        struct U* u;
                        struct id* id_scanf_tmp;
                        int offset_tmp = 0;
                        int type = 0;
                        if(find_2(map_offset, arg1)!=NULL){
                            u = find_2(map_offset, arg1);
                            offset_tmp = u->offset;
                            type = u->type;
                        }else{
                            id_scanf_tmp = map_total.find(arg1);
                            type = id_scanf_tmp->type == "int" ? 1 : 2;
                        }


                        if(type == 1){ ////int型标识符
                            fout<<"\tli $v0 5\n";
                            fout<<"\tsyscall\n";
                            if(four.register_map[fuction_n][arg1]!=0){
                                fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][arg1])<<" $v0\n";
                            }else if(find_2(map_offset, arg1)!=NULL){

                                fout<<"\tsw $v0 "<<to_string_new(offset_tmp)<<"($fp)\n";
                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tsw $v0 0($t0)\n";
                            }


                            fout<<"\n";
                        }else if(type == 2){ ////char型标识符
                            fout<<"\tli $v0 12\n";
                            fout<<"\tsyscall\n";

                            if(four.register_map[fuction_n][arg1]!=0){
                                fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][arg1])<<" $v0\n";
                            }else if(find_2(map_offset, arg1)!=NULL){

                                fout<<"\tsw $v0 "<<to_string_new(offset_tmp)<<"($fp)\n";
                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tsw $v0 0($t0)\n";
                            }
                        }

                    }else if(four.vec.at(j).op == "load_array"){ /////t1 load a 1

                        result =four.vec.at(j).result;
                        arg1 =four.vec.at(j).arg1 ;
                        arg2 =four.vec.at(j).arg2 ;
                        bool arg2_num = is_num(arg2);

                        int tmp_offset;
                        int loc;
                        struct U* u;
                        if(arg2_num){
                            loc = atoi((char*)four.vec.at(j).arg2.data());
                        }else{
                            if(four.register_map[fuction_n][arg2]!=0){
                                fout<<"\tmul $t3 $"<<to_string_new(four.register_map[fuction_n][arg2])<<" 4\n";
                                fout<<"\n";
                            }else if(find_2(map_offset, arg2)!=NULL){
                                u = find_2(map_offset, arg2);
                                tmp_offset = u->offset;
                                fout<<"\tlw $t3 "<<to_string_new(tmp_offset)<<"($fp)\n";
                                fout<<"\tmul $t3 $t3 4\n";
                            }else{
                                fout<<"\tla $t0 "<<arg2<<"\n";
                                fout<<"\tlw $t3 0($t0)\n";
                                fout<<"\tmul $t3 $t3 4\n";
                            }
                            fout<<"\n";
                        }
                        if(find_2(map_offset, arg1)!=NULL){
                            if(arg2_num){
                                u = find_2(map_offset, arg1);
                                tmp_offset = u->offset;
                                tmp_offset = tmp_offset - 4*loc;
                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";
                            }else{
                                u = find_2(map_offset, arg1);
                                tmp_offset = u->offset;
                                fout<<"\tli $t4 "<<tmp_offset<<"\n";
                                fout<<"\tsub $t4 $t4 $t3\n";
                                fout<<"\tadd $t4 $t4 $fp\n";
                                fout<<"\tlw $t1 0($t4)\n";

                            }
                            fout<<"\n";
                        }else {
                            if(arg2_num){
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                tmp_offset = 4 * loc;
                                fout<<"\tlw $t1 "<<to_string_new(tmp_offset)<<"($t0)\n";
                            }else{
                                fout<<"\tla $t0 "<<arg1<<"\n";
                                fout<<"\tadd $t0 $t0 $t3\n";
                                fout<<"\tlw $t1 0($t0)\n";
                            }
                            fout<<"\n";
                        }


                        if(four.register_map[fuction_n][result]!=0){
                             fout<<"\tmove $"<<to_string_new(four.register_map[fuction_n][result])<<" $t1\n";
                        }else if(find_2(map_offset, result)!=NULL){
                            u= find_2(map_offset, result);
                            tmp_offset = u->offset;
                            fout<<"\tsw $t1 "<<to_string_new(tmp_offset)<<"($fp)\n";
                        }else{
                            fout<<"\tla $t0 "<<result<<"\n";
                            fout<<"\tsw $t1 0($t0)\n";
                        }


                    }else if(four.vec.at(j).op == "end"){

                        if(total_op!="main"){

                            int j = 60;
                            for(int i = 13 ; i<=25; i++){
                                fout<<"\tlw $"<<to_string_new(i)<<" "<<to_string_new(j)<<"($fp)\n";
                                j = j - 4;
                            }

                            fout<<"\tlw $sp 4($fp)\n";
                            fout<<"\tlw $ra 0($fp)\n";
                            fout<<"\tlw $fp 8($fp)\n";
                            fout<<"\n";
                            fout<<"\tjr $ra\n";
                            fout<<"\n";
                        }
                        return_flag = false;
                        break;
                    }

                }
                ////////////一个函数结束了

                i=j;

            }


        }
    }
};
#endif /* mips_h */
