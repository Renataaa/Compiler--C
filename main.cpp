//
//  main.cpp
//  语法分析2
//
//  Created by Renata on 2017/11/20.
//  Copyright © 2017年 Renata. All rights reserved.
//
#include <vector>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stack>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include "lexical.h"
#include "symboltable.h"
#include <map>
#include "idtable.h"
#include "four.h"
#include "mips.h"
#define MAX_PARA 10
//////kind
#define VAR_SIM "var_sim"
#define ARRAY "array"
#define FUNC "function"
#define CONST "const"

using namespace std;

char FILE_NAME_tmp[100];
//char* FILE_NAME = "/Users/renata/compile_test/jijiahao.txt";
char* FILE_NAME;
int params[MAX_PARA];
char strBuffer[1026];
char strBuffer2[1026];
int case_judge[100];
int case_counter = 0;
string strtemp = ""; //倒退指针用的
string str = "";
string exp_value = ""; //表达式过程值
int exp_type = -1;// 1为int 2 为char
struct id* id_temp;//函数 标识符对应的符号表内容

LEXICAL l;
void program();
void constant_state();
void constant_define();
void id();
int whole_number();
void is_char();
void var_state();
void var_define();
void returnable();
string declare_head();
void para();
void mul_statement();
void statement_align();
void statement();
void expression();
void main_fuc();


bool factor_bug  = false;
bool term_bug = false;
bool exp_bug = false;
bool value_para_table_bug = false;
bool return_flag = false;

int flag_level = 1;
IDTable map_total;
FOUR four;
MIPS mips;
int addr_total = 0;
const int addr_int = 4;
const int addr_char = 1; //////单位字节
string to_string_new(int num)
{
    ostringstream ostr;
    ostr << num;
    string str = ostr.str();
    return str;
}

void constant_state(){
    if(l.syn!="const"){
        l.error(0);
    }else{
        l.scanner();
        constant_define();
            while(l.syn=="const"){
                l.scanner();
                constant_define();
            }
            //cout<<"it is a constant state"<<endl;
        }
    }


void constant_define(){
    int temp = 0;
    if(l.syn == "int"){
        l.scanner();
        id();
        if(l.syn != "equsy"){
            l.error(19);
            return;

        }else{
            l.scanner();
            temp = whole_number();
        }

            if(map_total.find(l.id_content)!=NULL){ //////当前是否有重复定义
                l.error(1);
                return;
            }else{
                map_total.insert(l.id_content, "int", "const",temp,-1, addr_total);  ///////////const int a
                addr_total  = addr_total +addr_int;
                four.push("*const_int", l.id_content,to_string_new(temp)," "); //// const int a = 1;
            }


        while(l.syn=="commasy"){
            l.scanner();
            id();
            if(l.syn != "equsy"){
                l.error(19);
                return;
            }else{
                l.scanner();
                temp = whole_number();
            }
            if(map_total.find(l.id_content)!=NULL){
                l.error(1);
                return;
            }else{

                    map_total.insert(l.id_content, "int", "const",temp,-1,addr_total); ////////////////////////const int a
                    four.push("*const_int", l.id_content,to_string_new(temp)," "); //// const int a = 1;
                    addr_total = addr_total+addr_int;
            }
        }
        if(l.syn == "semisy"){
            l.scanner();
        }else{
            l.error(19);
            return;
        }

    }else if(l.syn == "char"){
        l.scanner();
        id();
        if(l.syn != "equsy"){
            l.error(19);
            return;
        }else{
            l.scanner();
            is_char();

        }
        if(map_total.find(l.id_content)!=NULL){
            l.error(1);
            return;
        }else{
            map_total.insert(l.id_content, "char", CONST,l.char_ascii,-1,addr_total); /////////////////const char a

            four.push("*const_char", l.id_content,to_string_new(l.char_ascii)," "); //// const char a = 't';
            addr_total = addr_total+addr_char;
        }
        while(l.syn=="commasy"){
            l.scanner();
            id();
            if(l.syn != "equsy"){
                l.error(19);
                return;
            }else{
                l.scanner();
                is_char();
            }
            if(map_total.find(l.id_content)!=NULL){
                l.error(1);
                return;


            }else{
                map_total.insert(l.id_content, "char", CONST,l.char_ascii,-1,addr_total);
                four.push("*const_char", l.id_content,to_string_new(l.char_ascii)," "); //// const char a = 't';
                addr_total = addr_total+addr_char;
            }
        }
        if(l.syn == "semisy"){
            l.scanner();
        }else{
            l.error(19);
            return;
        }

    }else{
        l.error(0);
        return;
    }
}
void id(){
    if(l.syn == "IDSY"){
        l.scanner();
        return;
    }else{
        l.error(0);
        return;
    }
}

int whole_number(){
    if(l.syn=="plussy"){
        l.scanner();
        if(l.number == 0){
            l.error(0);

            return NULL;
        }
        if(l.syn=="INTSY"){
            l.scanner();
            return l.number;

        }
    }else if(l.syn=="minussy"){
        l.scanner();
        if(l.number == 0){
            l.error(0);
            return -1;
        }
        if(l.syn=="INTSY"){
            l.scanner();
            return 0-l.number;

        }
    }else if(l.syn == "INTSY"){
        l.scanner();
        return l.number;

    }
    l.error(0);

    return NULL;
}

void is_char(){
    if(l.syn =="char"){
        l.scanner();
    }else{
        l.error(0);
    }
}
void var_state(){
    char* temp2;
    string temp2_string;
    var_define();

        temp2 = l.str;
        temp2_string = l.syn;

        while(l.syn =="int" || l.syn =="char"){
            l.scanner();
            l.scanner();
            if(l.syn == "lparsy"){
                l.str =temp2;
                l.syn =temp2_string;
                break;
            }else{
                l.str =temp2;
                l.syn =temp2_string;
                var_define();

                temp2 = l.str;
                temp2_string = l.syn;
            }
        }
        //cout<<"it is a var state"<<endl;
        return;


}
void var_define(){
    string temp_ic ;
    temp_ic = l.syn;
    if(l.syn!="int" && l.syn!="char"){
        l.error(20);
        return;
    }else{
        l.scanner();
        if(l.syn=="IDSY"){
            l.scanner();
            if(l.syn=="lsquaresy"){ ////数组
                l.scanner();
                if(l.syn=="INTSY" && l.number!=0){
                    l.scanner();
                    if(l.syn!="rsquaresy"){
                        l.error(20);
                        return;
                    }else{
                        l.scanner();
                    }
                }
                if(map_total.find(l.id_content)!=NULL){
                    l.error(1);
                    return;
                }else{
                    map_total.insert(l.id_content, temp_ic, ARRAY,0,l.number,addr_total); //////int a[1] char a[]
                    addr_total = addr_total+ ((temp_ic == "int")? addr_int:addr_char) * l.number;
                    four.push("*"+temp_ic+"_array", l.id_content, to_string_new(l.number), "");

                }
            }else{
                if(map_total.find(l.id_content)!=NULL){
                    l.error(1);
                    return;
                }else{
                    map_total.insert(l.id_content, temp_ic, VAR_SIM,0,-1,addr_total); ////////int a
                    addr_total = addr_total +((temp_ic == "int")? addr_int:addr_char);
                    four.push("*"+temp_ic, l.id_content, "", "");

                }
            }

            while(l.syn=="commasy"){
                l.scanner();
                if(l.syn=="IDSY"){
                    l.scanner();
                    if(l.syn=="lsquaresy"){
                        l.scanner();
                        if(l.syn=="INTSY" && l.number!=0){
                            l.scanner();
                            if(l.syn=="rsquaresy"){
                                l.scanner();
                            }else{
                                l.error(20);
                                return;
                            }
                        }
                        if(map_total.find(l.id_content)!=NULL){
                            l.error(1);
                            return;
                        }else{
                            map_total.insert(l.id_content, temp_ic, ARRAY,0,l.number,addr_total); //////int a[1]
                            addr_total = addr_total+ ((temp_ic == "int")? addr_int:addr_char) * l.number;
                             four.push("*"+temp_ic+"_array", l.id_content, to_string_new(l.number), "");
                        }
                    }else{
                        if(map_total.find(l.id_content)!=NULL){
                            l.error(1);
                            return;
                        }else{
                            map_total.insert(l.id_content, temp_ic, VAR_SIM,0,-1,addr_total); ////////int a
                             addr_total = addr_total +((temp_ic == "int")? addr_int:addr_char);
                             four.push("*"+temp_ic, l.id_content, "", "");

                        }
                    }
                }
            }
            if(l.syn=="semisy"){
                l.scanner();
            }else{
                l.error(20);
            }

        }else {
            l.error(20);
            return;
        }
    }
}
string declare_head(){
    string return_s = "";
    if(l.syn=="int"){
        l.scanner();
        if(l.syn!="IDSY"){
            l.error(0);
            return "";
        }else{
            return_s = l.id_content;
            l.scanner();

            return return_s;
        }
    }else if(l.syn=="char"){
        l.scanner();
        if(l.syn!="IDSY"){
            l.error(0);
            return "";
        }else{
            return_s = l.id_content;
            l.scanner();
            return return_s;
        }
    }else{
        l.error(0);
        return "";
    }
    return "";
}
void returnable(){
    int addr_begin = addr_total;
    int addr_end = 0;
    int addr_size = 0;

    int register_t_begin = four.counter;
    int register_t_end = 0;
    string func_name = "";
    string func_type = "";
    func_type = l.syn;
    IDMap idmap_temp; ////内部局部的map 还未压入
    int para_array[PARA_MAX];
    for(int i =0; i<PARA_MAX;i++){
        para_array[i] = 0;
    }
    int counter = 0;
    flag_level = 2;
    //map_total.push();

    func_name = declare_head();
    four.push("func", func_name, "", "");

    if(l.syn=="lbigsy"){
        if(map_total.find(func_name)!=NULL){
            l.error(11);
        }else{
            map_total.insert_func(func_name, func_type, FUNC, 0, para_array); /////先把函数名和参数信息压进去 大小在后边
        }
        map_total.push();
        l.scanner();

        return_flag = false;
        mul_statement();
        if(!return_flag){
            l.error(33);
        }
        if(l.syn!="rbigsy"){
            l.error(31);
        }
        l.scanner();
        map_total.pop();

        register_t_end = four.counter;

        addr_total = (register_t_end - register_t_begin)*addr_int +addr_total; ////
        addr_end = addr_total;

        addr_size = addr_end -addr_begin;

      //  map_total.find(func_name)->func_size = addr_size; /////////////无参数函数处理完了 设定大小

        map_total.find(func_name)->register_begin =register_t_begin;
        map_total.find(func_name)->register_end =register_t_end;

        addr_total = addr_begin; //////恢复总地址到刚进入的时候
        four.set_counter(register_t_begin); ////清空该函数中的临时变量
        four.push("", "end", "", "");
        //cout<<"it is a returnable function"<<endl;
        return;
    }else if(l.syn=="lparsy"){
        l.scanner();
       // para();
        string temp_type = "";
        if(l.syn!="int" && l.syn!="char"){
            l.error(0);
        }else{
            temp_type = l.syn;
            l.scanner();
            id();
            four.push("para", temp_type, l.id_content, ""); ///////
            para_array[counter] = (temp_type == "int")?1:2;
            counter++;
            if(idmap_temp.find(l.id_content)!=NULL){
                l.error(1);
            }else{
                idmap_temp.insert(l.id_content, temp_type, VAR_SIM, 0, -1,addr_total);
                addr_total = (temp_type =="int")? (addr_total+addr_int): (addr_total+addr_char);
            }

        }
        while(l.syn=="commasy"){
            l.scanner();
            if(l.syn!="int" && l.syn!="char"){
                l.error(0);
            }else{
                temp_type = l.syn;
                l.scanner();
                id();
                four.push("para", temp_type, l.id_content, "");/////////////
                para_array[counter] = (temp_type == "int")?1:2;
                counter++;
                if(idmap_temp.find(l.id_content)!=NULL){
                    l.error(1);
                }else{
                    idmap_temp.insert(l.id_content, temp_type, VAR_SIM, 0, -1,addr_total);
                    addr_total = (temp_type =="int")? (addr_total+addr_int): (addr_total+addr_char);
                }
            }
        }
        if(map_total.find(func_name)!=NULL){
            l.error(1);
        }else{
            map_total.insert_func(func_name,  func_type, FUNC, counter, para_array);//////////有参数的情况 把参数处理完了 把参数信息push进去
        }
        map_total.push_ex(idmap_temp);  ///////之前的参数信息在一个idmap_temp里边 把它压进去 方便函数内部的继续进

        if(l.syn!="rparsy"){ ///////无有括号 到{
            l.error(22);
        }else{
            l.scanner();
        }

            if(l.syn!="lbigsy"){
                l.error(23);
            }else{
                l.scanner();
            }
            return_flag = false;
            mul_statement();
            if(!return_flag){
                l.error(33);
            }

                if(l.syn!="rbigsy"){
                    l.error(31);
                    return;
                }
                l.scanner();

                map_total.pop();/////////////////////////////////////////////////

                register_t_end = four.counter;

                addr_total = (register_t_end - register_t_begin)*addr_int +addr_total; ////
                addr_end = addr_total;

                addr_size = addr_end -addr_begin;

             //   map_total.find(func_name)->func_size = addr_size; /////////////无参数函数处理完了 设定大小

                map_total.find(func_name)->register_begin =register_t_begin;
                map_total.find(func_name)->register_end =register_t_end;

                addr_total = addr_begin; //////恢复总地址到刚进入的时候
                four.set_counter(register_t_begin); ////清空该函数中的临时变量
                four.push("", "end", "", "");
                //cout<<"it is a returnable(para) function"<<endl;
                return;


    }else{
        l.error(12);
        mul_statement();
        if(l.syn!="rbigsy"){
            l.error(31);
        }
        l.scanner();
        return;
    }
}
void unreturnable(){

    int addr_begin = addr_total;
    int addr_end = 0;
    int addr_size = 0;

    int register_t_begin = four.counter;
    int register_t_end = 0;

    string func_name = "";
    string func_type = l.syn;
    IDMap idmap_temp;

    int para_array[PARA_MAX];
    for(int i =0; i<PARA_MAX;i++){
        para_array[i] = 0;
    }
    int counter = 0 ;


    if(l.syn=="void"){
        l.scanner();
        id();
        func_name = l.id_content;
        four.push("func", func_name, "", "");
        if(l.syn=="lbigsy"){
            if(map_total.find(func_name)!=NULL){
                 l.error(11);
            }else{

                map_total.insert_func(func_name, func_type, FUNC, 0, para_array);
            }
            map_total.push();
            l.scanner();
            mul_statement();
            if(l.syn!="rbigsy"){
                l.error(31);
            }
            l.scanner();
            map_total.pop();
            four.push("", "end", "", "");
            //cout<<"it is a unreturnable(no para) function"<<endl;
            register_t_end = four.counter;

            addr_total = (register_t_end - register_t_begin)*addr_int +addr_total; ////
            addr_end = addr_total;
            addr_size = addr_end -addr_begin;
            if(map_total.find(func_name)==NULL){

            }

            map_total.find(func_name)->register_begin =register_t_begin;
            map_total.find(func_name)->register_end =register_t_end;

            addr_total = addr_begin; //////恢复总地址到刚进入的时候
            four.set_counter(register_t_begin); ////清空该函数中的临时变量

            return;
        }else if(l.syn=="lparsy"){
            l.scanner();
            //para();
            string temp_type ="";
            if(l.syn!="int" && l.syn!="char"){
                l.error(0);
            }else{
                temp_type = l.syn;
                l.scanner();
                id();
                four.push("para", temp_type, l.id_content, "");
                para_array[counter] = (temp_type == "int")?1:2;
                counter++;
                if(idmap_temp.find(l.id_content)!=NULL){
                    l.error(1);
                }else{
                    idmap_temp.insert(l.id_content, temp_type, VAR_SIM, 0, -1,addr_total);
                    addr_total = (temp_type =="int")? (addr_total+addr_int): (addr_total+addr_char);
                }
            }
            while(l.syn=="commasy"){
                l.scanner();
                if(l.syn!="int" && l.syn!="char"){
                    l.error(0);
                }else{
                    temp_type = l.syn;
                    l.scanner();
                    id();
                    four.push("para", temp_type, l.id_content, "");
                    para_array[counter] = (temp_type == "int")?1:2; ///////形容函数参数的数组
                    counter++;
                    if(idmap_temp.find(l.id_content)!=NULL){
                        l.error(1);
                    }else{
                        idmap_temp.insert(l.id_content, temp_type, VAR_SIM, 0, -1,addr_total);
                        addr_total = (temp_type =="int")? (addr_total+addr_int): (addr_total+addr_char);
                    }
                }
            }
            if(map_total.find(func_name)!=NULL){
                l.error(1);
            }else{
                map_total.insert_func(func_name, func_type, FUNC, counter, para_array);
            }

            map_total.push_ex(idmap_temp);

            if(l.syn!="rparsy"){ ///////无有括号 到{
                l.error(22);
            }else{
                l.scanner();
            }

                if(l.syn!="lbigsy"){  ///////
                    l.error(23);
                }else{
                    l.scanner();
                }
                    mul_statement();
                    if(l.syn!="rbigsy"){
                        l.error(31);
                        return;
                    }
                    l.scanner();
                    map_total.pop();

                    register_t_end = four.counter;


                    addr_total = (register_t_end - register_t_begin)*addr_int +addr_total; ////
                    addr_end = addr_total;

                    addr_size = addr_end -addr_begin;  /////2+2+2 6*4 = 24

                    map_total.find(func_name)->func_size = addr_size; /////////////有参数函数处理完了 设定大小
                    map_total.find(func_name)->register_begin =register_t_begin;
                    map_total.find(func_name)->register_end =register_t_end;
                    addr_total = addr_begin; //////恢复总地址到刚进入的时候

                    four.set_counter(register_t_begin); ////清空该函数中的临时变量
                    four.push("", "end", "", "");
                    //cout<<"it is a unreturnable(para) function"<<endl;
                    return;

        }else{
            l.error(12);
            mul_statement();
            if(l.syn!="rbigsy"){
                l.error(31);
            }
            l.scanner();
            return;
        }
    }
    l.error(0);
}

void mul_statement(){
    if(l.syn=="const"){
        constant_state();
    }
    if(l.syn=="int" || l.syn=="char"){
        var_state();
    }
    while(l.syn!="rbigsy" && l.syn!="void" && l.syn!="int"&&l.syn!="char"){
        statement();
    }
}
void value_para_table(string name){
    struct id* id_temp;
    id_temp = map_total.find_total(name);
    int counter = 0;
    bool flag = false;
    string result = "";
    string op = "";
    string arg1 = "";
    string arg2 = "";
    exp_bug = false;
    expression();
    if(exp_bug){
        value_para_table_bug = true;
        return;
    }
    flag = id_temp->func->params[counter]==exp_type;
    op = "PUSH";

    if(flag){
        four.push(result, op, exp_value, "");
    }else{

        l.error(4);////参数不匹配
        value_para_table_bug = true;
        return;
    }
    counter++;
    while(l.syn=="commasy"){
        l.scanner();

        exp_bug = false;
        expression();
        if(exp_bug){
            value_para_table_bug = true;
            return;
        }

        flag = id_temp->func->params[counter]==exp_type;
        if(flag){
            four.push(result, op, exp_value, "");
        }else{
            l.error(4);////参数不匹配
            value_para_table_bug = true;
            return;
        }
        counter++;
    }
    if(id_temp->func->params[counter]==0){
        //cout<<"it is a value_para_table"<<endl;
    }else{
        l.error(4);
        value_para_table_bug = true;
        return;
    }
}
void factor(){

    if(l.syn=="lparsy"){
        l.scanner();

        exp_bug = false;

        expression();

        if(exp_bug){
            factor_bug = true;
            return;
        }

        if(l.syn!="rparsy"){
            l.error(5);
            factor_bug = true;
            return;
        }
        l.scanner();
        return;
    }else if(l.syn == "char"){ /////'a'
        exp_value= to_string_new(l.char_ascii);
        exp_type = 2;
        l.scanner();
        return;
    }else if(l.syn == "INTSY"||l.syn=="minussy"||l.syn=="plussy"){
        //cout<<l.syn<<endl;
        int tmp_num = whole_number();
        exp_value= to_string_new(tmp_num);
        exp_type = 1;
        return;
    }else if(l.syn == "IDSY"){
        string id_temp_content = l.id_content;
        if(map_total.find_total(l.id_content)==NULL){
            l.error(2);
            factor_bug = true;
            return;

        }

        l.scanner();
        if(l.syn=="lsquaresy"){ ///a[1]数组
            int array_length = 0;
            struct id* id_array;
            string num;
            int use_length = 0 ;
            id_array = map_total.find_total(l.id_content);
            if(id_array->kind!="array"){
                l.error(8);
                factor_bug = true;
                return;
            }
            array_length = id_array->dimension;
            l.scanner();
            exp_bug = false;

            expression();

            if(exp_bug){
                factor_bug = true;
                return;
            }
            num = exp_value;

            if(mips.is_num(num)){/////
                use_length = atoi((char*)num.data());
                if(use_length<0 || use_length>=array_length){
                    l.error(29);
                    factor_bug = true;
                    return;

                }
            }

          //  exp_value =id_temp_content+"["+num+"]";

            exp_type =id_array->type =="int"?1:2;

            string tmpp = four.temp_var();
            four.push(tmpp, "load_array", id_temp_content, num);
            exp_value = tmpp;
            if(l.syn!="rsquaresy"){
                l.error(5);
                factor_bug = true;
                return;
            }else{
                l.scanner();
                return;
            }

        }
        if(l.syn=="lparsy"){ ///有返回值 有参数的函数调用语句a（2） 要查看参数是否符合 找到函数在符号表中的位置后返回参数列表
            value_para_table_bug = false;
            string result ="";
            string op = "";
            string arg1 ="";
            string arg2 = "";
            op = "CALL";
            arg1 = l.id_content;
            id_temp = map_total.find_total(l.id_content);
            string return_type =id_temp->type;
            if(id_temp->kind!="function"){
                l.error(3);
                factor_bug = true;
                return;
            }
            l.scanner();
            value_para_table(id_temp_content);
            if(value_para_table_bug){
                factor_bug = true;
                return;
            }
            if(l.syn=="rparsy"){
                l.scanner();

                four.push(result, op, arg1, arg2);

                result = four.temp_var();///函数返回值给一个临时变量
                op = "ASSIGN";
                arg1 = "RET";
                arg2 ="";
                exp_value = result;
                exp_type = (id_temp->type=="int")?1:2; ///函数的返回值类型int char void 标识符类型 int char

                four.push(result, op, arg1, return_type);
                return;
            }

        }
        /////////////既不是括号 也不是方括号 无参数的函数调用????也可能是一个普通的标识符 a
        if(map_total.find_total(l.id_content)!=NULL){
                        struct id* id_temp2;
            exp_value = l.id_content;
            id_temp2 = map_total.find_total(l.id_content);
            exp_type = id_temp2->type == "int" ? 1:2;

            if(id_temp2->kind=="function"){ ////无参数的函数调用 printf(a)
                string return_type =id_temp2->type;
                string arg1 = l.id_content;
                string result ="";

                four.push("", "CALL", arg1, "");
                result = four.temp_var();///函数返回值给一个临时变量
                four.push(result, "ASSIGN", "RET", return_type);
                exp_value = result;

            }

        }else{
            l.error(2);
            factor_bug = true;
            return;
        }

    }
}
void term(){
    factor_bug = false;
    int type_tmp1 = 0;
    int type_tmp2 = 0;
    string result = "";
    string op = "";
    string arg1 = "";
    string arg2 = "";
    factor();
    if(factor_bug){
        term_bug = true;
        return;
    }
    arg1 = exp_value;
    result = exp_value;

    while(l.syn=="mulsy"||l.syn=="divsy"){
        op = l.syn=="mulsy" ? "MUL" : "DIV";
        l.scanner();
        factor();
        if(factor_bug){
            term_bug = true;
            return;
        }
        type_tmp2 = exp_type;
        if(type_tmp1!=type_tmp2){
           // l.error(5);
        }
        type_tmp1 = type_tmp2;
        result = four.temp_var();
        arg2 = exp_value;
        four.push(result, op, arg1, arg2);
        arg1 = result;
        exp_type = 1; /////////有乘法了一定是一个int了
    }

    exp_value = result;
}
void expression(){
    term_bug = false;
    int type_tmp1 = 0;
    int type_tmp2 = 0;
    string result = "";
    string op = "";
    string arg1 = "";
    string arg2 = "";
    if(l.syn=="plussy"||l.syn=="minussy"){
        op = l.syn=="plussy" ? "ADD" : "MINUS";
        l.scanner();
    }

    term();
    if(term_bug){
        exp_bug = true;
        return;
    }
    type_tmp1 = exp_type;

    if(op == "MINUS"){
        op = "MINUS";
        result = four.temp_var();
        arg2 = exp_value;
        four.push(result, op, "0", arg2);
        exp_value = result;
    }
    arg1 = exp_value;
    result = exp_value;

    while(l.syn=="plussy"||l.syn=="minussy"){
        op = l.syn=="plussy" ? "ADD" : "MINUS";
        l.scanner();
        //cout<<l.syn<<endl;
        term();
        if(term_bug){
            exp_bug = true;
            return;
        }
        type_tmp2 = exp_type;
        if(type_tmp1!=type_tmp2){
            //l.error(5);
        }
        type_tmp1 = type_tmp2;
        result = four.temp_var();
        arg2 = exp_value;
        four.push(result, op, arg1, arg2);
        arg1 = result;

        exp_type = 1;
    }

    exp_value = result;
}

void condition(string label){
    int type1 = 0;
    int type2 = 0;
    string result = "";
    string op = "";
    string arg1 = "";
    string arg2 = "";
    expression();
    arg1 = exp_value;
    type1 = exp_type;
    if(l.syn=="lesssy"||l.syn=="moresy"||l.syn=="lesseqlsy"||l.syn=="moreeqlsy"||l.syn=="NOTeqlsy"||l.syn=="ifeqlsy"){
        op = (l.syn=="lesssy")?">=":
            (l.syn=="moresy")?"<=":
            (l.syn=="lesseqlsy")?">":
            (l.syn=="moreeqlsy")?"<":
            (l.syn=="NOTeqlsy")?"==":
            (l.syn=="ifeqlsy")?"!=":"";

        l.scanner();
        expression();
        arg2 = exp_value;
        type2 = exp_type;
      //  result = four.temp_var();
        if(type1!=type2){ // 两个成分不能进行比较
          //  l.error(7);
        }
        four.push(label, op, arg1,  arg2);
        return;
       // return result;
    }
    //////表达式为0条件为假 否则为真 不满足条件跳转 也就是 为0 的时候跳转
    four.push(label, "==", arg1, "0");
    return;

}
void if_statement(){
    string result = "";
    string op = "";
    string arg1 = "";
    string arg2 = "";
    string labelA = "";
    string labelB = "";
    string compare_result ="";
    if(l.syn=="if"){
        l.scanner();
        if(l.syn=="lparsy"){
            l.scanner();

            arg2 = four.temp_var_label();
            condition(arg2);
            labelA = arg2;
           // four.push("", "BNF", arg1, arg2);
            if(l.syn!="rparsy"){
                l.error(22);
            }else{
                l.scanner();
            }
                statement();
                arg1 = four.temp_var_label();
                labelB = arg1;
                four.push("", "BR", arg1, "");
                if(l.syn=="else"){
                    four.push(labelA,"*label","","");
                    l.scanner();
                    statement();
                    four.push(labelB,"*label","","");
                    //cout<<"it is a if_statement"<<endl;
                    return;
                }else{
                    l.error(26);
                    return;
                }

        }

    }
    l.error(0);
}
void while_statement(){
    string result = "";
    string op = "";
    string arg1 ="";
    string arg2 = "";
    string labelA = "";
    string labelB = "";
    labelA = four.temp_var_label();
    four.push(labelA, "*label", "", "");

    if(l.syn=="while"){
        l.scanner();
        if(l.syn=="lparsy"){
            l.scanner();

            labelB = four.temp_var_label();
            condition(labelB);

            if(l.syn!="rparsy"){
                l.error(22);
            }else{
                l.scanner();
            }

                statement();
                four.push("", "BR", labelA, "");
                four.push(labelB, "*label", "", "");
                //cout<<"it is a while_statement"<<endl;
                return;
        }
    }
    l.error(0);
}

void read_statement(){
    string tmp = "";
    if(l.syn == "scanf"){
        l.scanner();
        if(l.syn == "lparsy"){
            l.scanner();
            id();
            tmp = l.id_content;
            if(map_total.find_total(tmp)==NULL){
                l.error(2);
                return;
            }
               four.push("", "scanf", l.id_content, "");
            while(l.syn=="commasy"){
                l.scanner();
                id();
                tmp = l.id_content;
                if(map_total.find_total(tmp)==NULL){
                    l.error(2);
                    return;
                }
                four.push("", "scanf", l.id_content, "");
            }
            if(l.syn =="rparsy"){
                l.scanner();
                if(l.syn=="semisy"){
                    l.scanner();
                    //cout<<"it is a read_statement"<<endl;
                    return;

                }
            }
        }
    }
    l.error(14);
}
void write_statement(){
    string tmp = "";
    if(l.syn=="printf"){
        l.scanner();
        if(l.syn=="lparsy"){
            l.scanner();
            if(l.syn == "stringsy"){
                if(l.string_bug){
                    return;
                }
                l.scanner();
                tmp = l.string_content;
                if(l.syn=="commasy"){
                    four.push("", "printf", tmp, "");
                    l.scanner();
                    exp_bug = false;
                    expression();
                    if(exp_bug){
                        return;
                    }
                    tmp = exp_value;
                    if(l.syn=="rparsy"){
                        l.scanner();
                        if(l.syn == "semisy"){
                            l.scanner();
                            if(exp_type == 1){
                                four.push("", "printf", tmp, "");
                            }else if(exp_type == 2){
                                four.push("", "printf_c", tmp, "");
                            }
                            //cout<<"it is a write_statement"<<endl; //////两个元素
                            return;
                        }
                    }
                }else if(l.syn=="rparsy"){ ///////only string
                    l.scanner();
                    if(l.syn == "semisy"){
                        l.scanner();
                        four.push("", "printf", tmp, "");
                        //cout<<"it is a write_statement"<<endl;
                        return;
                    }
                }
            }else { /////only expression
                exp_bug = false;
                expression();
                if(exp_bug){
                    return;
                }                tmp = exp_value;

                if(l.syn=="rparsy"){
                    l.scanner();
                    if(l.syn == "semisy"){
                        l.scanner();
                        if(exp_type == 1){
                            four.push("", "printf", tmp, "");
                        }else if(exp_type == 2){
                            four.push("", "printf_c", tmp, "");
                        }
                        //cout<<"it is a write_statement"<<endl;
                        return;
                    }
                }
            }
        }
    }
    l.error(15);
}
void return_statement(){
    if(l.syn=="return"){
        l.scanner();
        if(l.syn=="lparsy"){
            l.scanner();
            exp_bug = false;

            expression();
            if(exp_bug){
                return;
            }
            if(l.syn=="rparsy"){
                l.scanner();
                if(l.syn == "semisy"){
                    l.scanner();
                    four.push("", "ret", exp_value, "");
                    //cout<<"it is a return_statement(para)"<<endl;
                    return_flag=true;
                    return;
                }

            }
        }else if(l.syn == "semisy"){
            l.scanner();
            four.push("", "ret", "", "");
            //cout<<"it is a return_statement(no para)"<<endl;
            return_flag=true;
            return;
        }
    }
    l.error(13);
}
void condition_child(string target,string label_next,string label_final){
    string target2 = "";
    string result = "";
    if(l.syn=="case"){
        l.scanner();
        if(l.syn=="char"){
            l.scanner();
            for(int i = 0; i <case_counter;i++){
                if(l.char_ascii==case_judge[i]){
                   l.error(34);
                }
            }
            case_judge[case_counter] = l.char_ascii;
            target2 = to_string_new(l.char_ascii);
        }else{
            int tt = whole_number();
            for(int i = 0; i <case_counter;i++){
                if(tt==case_judge[i]){
                    l.error(34);
                }
            }
            case_judge[case_counter] = tt;
            target2 = to_string_new(tt);
        }
            case_counter++;

        four.push(label_next, "!=", target, target2);


        if(l.syn=="colonsy"){
            l.scanner();
            statement();
            four.push("","BR",label_final,"");
            //cout<<"it is a condition_child"<<endl;
            return;
        }else{
            l.error(27);
            statement();
            four.push("","BR",label_final,"");
            //cout<<"it is a condition_child"<<endl;
            return;
        }
    }else{
        l.error(27);
        statement();
        four.push("","BR",label_final,"");
        //cout<<"it is a condition_child"<<endl;
        return;
    }
    l.error(0);
}
void condition_table(string target,string label_final){
    string label_tmp = "";
    string label_tmp2 = "";

    label_tmp = four.temp_var_label();
    four.push(label_tmp,"*label", "", "");  ////这个case语句头要写的label

    label_tmp2 = four.temp_var_label();
    condition_child(target,label_tmp2,label_final); /////在子语句中要调到下一个case 的开头
    label_tmp = label_tmp2;

    while(l.syn=="case"){
      four.push(label_tmp,"*label", "", "");  ////这个case语句头要写的label
      label_tmp2 = four.temp_var_label(); ////下个case语句头要写的label
      condition_child(target,label_tmp2,label_final);
      label_tmp = label_tmp2;
    }
    four.push(label_tmp, "*label", "", "");
    //cout<<"it is a condition_table"<<endl;
}
void default_statement(){
    if(l.syn=="default"){
        l.scanner();
        if(l.syn=="colonsy"){
            l.scanner();
            statement();
            //cout<<"it is a default_statement"<<endl;
            return;
        }else{
            l.error(28);
            statement();
            //cout<<"it is a default_statement"<<endl;
            return;
        }
    }else{
        l.error(28);
        statement();
        //cout<<"it is a default_statement"<<endl;
        return;
    }
    l.error(0);
}
void switch_statement(){
    case_counter = 0;
    for(int i = 0; i<100; i++){
        case_judge[i] =0;
    }
    string result = "";
    string op = "";
    string arg1 = "";
    string arg2 = "";
    string label_final ="";
    string target  = "";
    label_final = four.temp_var_label();

    if(l.syn=="switch"){
        l.scanner();
        if(l.syn=="lparsy"){
            l.scanner();
            exp_bug = false;
            expression();
            if(exp_bug){
                condition_table(target,label_final);

                if(l.syn=="default"){
                    default_statement();
                }
                if(l.syn!="rbigsy"){
                    l.error(31);
                    return;
                }
                l.scanner();
                four.push(label_final, "*label", "", "");
                //cout<<"it is a switch_statement"<<endl;
                return;
            }else{
            target = exp_value;

            if(l.syn!="rparsy"){
                l.error(22);
            }else{
                l.scanner();
            }

                if(l.syn!="lbigsy"){
                     l.error(23);
                }else{
                    l.scanner();
                }
                    condition_table(target,label_final);

                    if(l.syn=="default"){
                        default_statement();
                    }
                    if(l.syn!="rbigsy"){
                        l.error(31);
                        return;
                    }
                    l.scanner();
                    four.push(label_final, "*label", "", "");
                    //cout<<"it is a switch_statement"<<endl;
                    return;

            }
        } /////lparsy
    } ///switch
    l.error(0);
}////void
void give_statement(){
    id();
    if(l.syn=="equsy"){
        l.scanner();
        expression();
        //cout<<"it is a give_statement"<<endl;
        return;
    }else if(l.syn == "lsquaresy"){
        l.scanner();
        expression();
        if(l.syn == "rsquaresy"){
            l.scanner();
            if(l.syn == "equsy"){
                l.scanner();
                expression();
                //cout<<"it is a give_statement"<<endl;
                return;
            }
        }
    }
    l.error(0);
}


void statement_align(){

    while(l.syn!="rbigsy"){
            statement();
        }
        l.scanner();
        //cout<<"it is a statement_align"<<endl;
        return;

}
void statement(){
    if(l.syn=="if"){
        if_statement();
    }else if(l.syn=="while"){
        while_statement();
    }else if(l.syn=="switch"){
        switch_statement();
    }else if(l.syn=="return"){
        return_statement();
        return;
    }else if(l.syn=="lbigsy"){
        l.scanner();
        statement_align();
    }else if(l.syn=="printf"){
        write_statement();

    }else if(l.syn=="scanf"){
        read_statement();

    }else if(l.syn == "IDSY"){

        string tmp_given = l.id_content;

        struct id* id_given;
        if(map_total.find_total(l.id_content)==NULL){
            l.error(2);
            return;
        }
        id_given = map_total.find_total(l.id_content);

        l.scanner();
        if(l.syn == "equsy"){ //////赋值语句

            if(id_given->kind == "const"){
                l.error(9);
                return;
            }else if(id_given->kind == "array"){
                l.error(30);
                return;
            }

            int type_given = id_given->type=="int"?1:2;
            int type_give = 0;
            string result = l.id_content;
            string op = "ASSIGN";
            string arg1 = "";
            string arg2 = "";

            l.scanner();
            exp_bug = false;
            expression();
            if(exp_bug){
                return;
            }
            type_give = exp_type;
            if(type_give!=type_given){
               // l.error(6);
            }
            arg1 = exp_value;
            if(l.syn!="semisy"){
                l.error(16);
                return;
            }else{
                l.scanner();
                if(map_total.find_total(result)==NULL){
                    l.error(2);
                    return;
                }else{
                    four.push(result, op, arg1, arg2);
                }
                //cout<<"it is a give_statement"<<endl;
            }

        }else if(l.syn=="lsquaresy"){ /////赋值语句
            int array_length = id_given->dimension;
            int use_length = 0;
            int type_given = id_given->type=="int"?1:2;
            int type_give = 0;

            string result = l.id_content; ///a
            string op = "ASSIGN";
            string arg1 = "";
            string arg2 = "";

            l.scanner();

            exp_bug = false;
            expression();
            if(exp_bug){
                return;
            }
            if(mips.is_num(exp_value)){/////
                use_length = atoi((char*)exp_value.data());
                if(use_length<0 || use_length>=array_length){
                    l.error(29);
                    return;

                }
            }

            string result_tmp = exp_value;
            result = result+"["+result_tmp+"]";

            if(l.syn == "rsquaresy"){
                l.scanner();
                if(l.syn=="equsy"){
                    l.scanner();
                    exp_bug = false;
                    expression();
                    if(exp_bug){
                        return;
                    }
                    arg1 = exp_value;
                    if(l.syn=="semisy"){
                        l.scanner();
                        four.push(result, op, arg1, arg2);
                        //cout<<"it is a give_statement"<<endl;
                        return;
                    }

                }
            }
            l.error(16);
        }else if(l.syn == "lparsy" ){
            l.scanner();
            value_para_table(tmp_given);
            if(l.syn == "rparsy"){
                l.scanner();
                if(l.syn=="semisy"){
                    l.scanner();
                    //cout<<"it is a para-use"<<endl; //////a(b,c);
                    four.push("", "CALL", tmp_given, "");
                    return;
                }
            }

            l.error(17);
            return;

        }else if(l.syn=="semisy"){
            l.scanner();

            //cout<<"it is a none-para-use"<<endl; ////////// a; 函数调用
            four.push("", "CALL", tmp_given, "");
            return;
        }
    }else if(l.syn == "semisy"){
        l.scanner();
        return;
    }else{
        l.error(25);

    }
}
void para(){
    if(l.syn!="int" && l.syn!="char"){
        l.error(0);
    }else{
        l.scanner();
        id();
    }
    while(l.syn=="commasy"){
        l.scanner();
        if(l.syn!="int" && l.syn!="char"){
            l.error(0);
        }else{
            l.scanner();
            id();
        }
    }

}
void program(){
    string temp;
    if(l.syn=="const"){
        constant_state();
    }
    l.strtemp = l.str;
    temp = l.syn;
    if(l.syn=="int" ||l.syn=="char"){
        l.scanner();
        if(l.syn=="IDSY"){
            l.scanner();
            if(l.syn=="commasy" || l.syn=="lsquaresy" ||l.syn=="semisy"){
                l.str = l.strtemp;
                l.syn = temp;
                var_state();
            }else{
                l.str = l.strtemp;
                l.syn = temp;
            }
        }
    }
        l.strtemp = l.str;
        temp  = l.syn;

        l.scanner();
        while(l.syn=="IDSY"){

            l.str=l.strtemp;
            l.syn = temp;

            if(l.syn=="int"||l.syn=="char"){
                returnable();
            }else if(l.syn=="void"){
                unreturnable();
            }
            l.strtemp = l.str;
            temp  = l.syn;
            l.scanner();

        }
        l.str=l.strtemp;
        l.syn = temp;
        main_fuc();
        //cout<<"it is a perfect program"<<endl;
    }

void main_fuc(){
    int register_begin = four.counter;
    int register_end = 0;

    four.push("func", "main", "", "");
    string func_name = "main";
    string func_type = "void";
    int para_array[PARA_MAX];
    for(int i =0; i<PARA_MAX;i++){
        para_array[i] = 0;
    }

    if(l.syn=="void"){
        l.scanner();
        if(l.syn=="main"){
            l.scanner();
            if(map_total.find(func_name)!=NULL){
                l.error(1);
            }else{
                map_total.insert_func(func_name, func_type, FUNC, 0, para_array);
            }
            map_total.push();
            if(l.syn=="lparsy"){
                l.scanner();
                if(l.syn!="rparsy"){
                    l.error(22);
                }else{
                    l.scanner();
                }

                if(l.syn!="lbigsy"){  ///////
                    l.error(23);
                }else{
                    l.scanner();
                }

                mul_statement();
                if(l.syn!="rbigsy"){
                    l.error(31);
                    return;
                }
                l.scanner();
                map_total.pop();
                four.push("label_end","*label","","");
                four.push("", "end", "", "");
                //cout<<"it is a main_fuc"<<endl;
                register_end = four.counter;
                map_total.find(func_name)->register_begin =register_begin;
                map_total.find(func_name)->register_end =register_end;

                four.set_counter(register_begin);
                return;



            }
        }

    }
}

int main(int argc, const char * argv[]) {
    string f="";
    cin>>FILE_NAME_tmp;
    f=FILE_NAME_tmp;
    FILE_NAME = (char*)f.data();
    //l.f_error.open("/Users/renata/error_record.txt");
    l.f_error.open("D:\\error_record.txt");
    // l.f_error.open("C:\\Users\\15780\\Desktop\\error_record.txt");
    FILE* file = fopen(FILE_NAME, "r");

    while(NULL != fgets(strBuffer, 1024, file))//strBuffer 是char[] str是char*
    {

        strtemp = strBuffer;

        str = str + strtemp;
    }

    l.str = (char*)str.data();

    l.scanner();
    program();

    four.push("label_end2", "*label", "", "");



    four.split();
    four.output_four();
    four.dag();
    four.output_four_youhua();

    four.lva_born();  /////def use
    four.follow();
    four.lva_in_out(); /////

    four.lva_all();

    mips.map_total = map_total;
    mips.four = four;
    mips.output();
    mips.output_youhua();
    return 0;
}

