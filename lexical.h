/*


 */
//
//  lexical.h
//  语法分析2
//
//  Created by Renata on 2017/11/28.
//  Copyright © 2017年 Renata. All rights reserved.
//

#ifndef lexical_h
#define lexical_h
#include <iostream>
#include<sstream>
#include <algorithm>
#include <vector>
#include <stack>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include "lexical.h"
#include <fstream>
using namespace std;
class LEXICAL{
public:
    
    ofstream f_error;
    int source_line = 1;
    char* str;
    char* strtemp;
    char strBox[200];
    const int keynum = 14;
    string syn = "";
    string id_content = "";
    
    int number;  //int的数值
    int char_ascii;
    char char_content;
    string string_content;
    bool line_flag = false;
    bool string_bug = false;
    char* keyword[14] = {
        "while",
        "if",
        "else",
        "char",
        "int",
        "return",
        "const",
        "void",
        "switch",
        "case",
        "default",
        "printf",
        "scanf",
        "main"
    };
    string to_string_new(int num)
    {
        ostringstream ostr;
        ostr << num;
        string str = ostr.str();
        return str;
    }
    
    bool isSpace(char a){
        return a==' ';
    }
    bool isNewline(char a)
    {
        return a=='\n';
    }
    bool isTab(char a)
    {
        return a=='\t';
    }
    bool isPlus(char a)
    {
        return a=='+';
    }
    bool isMinus(char a)
    {
        return a=='-';
    }
    bool isLpar(char a)
    {
        return a=='(';
    }
    bool isRpar(char a)
    {
        return a==')';
    }
    bool isComma(char a)
    {
        return a==',';
    }
    bool isSemi(char a)
    {
        return a==';';
    }
    bool isEqual(char a)
    {
        return a=='=';
    }
    bool isLsquare(char a)
    {
        return a=='[';
    }
    bool isRsquare(char a)
    {
        return a==']';
    }
    bool isLbig(char a)
    {
        return a=='{';
    }
    bool isRbig(char a)
    {
        return a=='}';
    }
    bool isMul(char a)
    {
        return a=='*';
    }
    bool isDiv(char a)
    {
        return a=='/';
    }
    bool isColon(char a)
    {
        return a==':';
    }
    bool isLetter(char a)
    {
        if(a >= 'a' && a <= 'z')
            return true;
        if(a >= 'A' && a <= 'Z')
            return true;
        if(a=='_')
            return true;
        return false;
    }
    bool isDigit(char a)
    {
        if(a >= '0' && a <= '9')
            return true;
        return false;
    }
    bool isChar(char a)
    {
        if(a =='+'|| a =='-'|| a =='*'|| a =='/')
            return true;
        if(isLetter(a))
            return true;
        if(isDigit(a))
            return true;
        
        return false;
    }
    int findKey(char* ch)
    {
        for(int i=0; i<keynum; i++)
        {
            if(0 == strcmp(ch, keyword[i]))
                return i+1;
        }
        return 0;
    }
    void retract(char* &str)
    {
        str--;
    }
    char getChar(char* &str)
    {
        return *str++;
    }
    void error(int para)
    {
        line_flag = false;
        switch (para) {
                
            case 0: ////////////////语法错误 读到分号
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!语法错误，不符合文法规则"<<endl;
                while(syn!="semisy"){
                    scanner();
                }
                
                break;
            case 1:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!重复定义"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 2:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"WRONG!!!NO DEFINE"<<endl;
                while(!line_flag){ scanner(); }
                break;
                
            case 3:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"WRONG!!!Is is not a defined function"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 4:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"WRONG!!!参数不匹配"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 5:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"WRONG!!!不符合文法规则的因子"<<endl;
                while(!line_flag){ scanner(); }
                break;

            case 6:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"WRONG!!!不符合文法规则的项"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 7:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"WRONG!!!不符合文法规则的表达式"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 8:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"WRONG!!!Is is not a defined ARRAY"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 9:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!常量不能赋值"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 10:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WARNING:不接受前导零，已容错"<<endl;
            case 11:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!函数重复定义"<<endl;
                break;
/**********/case 12:/*****************************/
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!函数头部或结构不符合文法规则"<<endl;
                while(!line_flag){ scanner(); }
                
                break;
            case 13:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!return语句不符合文法规则"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 14:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!scanf语句不符合文法规则"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 15:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!printf语句不符合文法规则"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 16:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!赋值语句不符合文法规则"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 17:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!函数调用语句不符合文法规则"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 18:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!!字符串包含非法内容"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 19:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!常量说明语句不符合文法规则"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 20:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!变量说明语句不符合文法规则"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 21:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!函数不符合文法规则"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 22:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!无右小括号"<<endl;
                while(syn!="lbigsy"){ scanner(); }
                break;
            case 23:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!无左大括号"<<endl;
                
                break;
            case 24:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!函数声明部分无}"<<endl;
                while(syn!="voidSY"){ scanner(); }
                break;
            case 25:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!不是可识别的语句"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 26:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!if语句没有else"<<endl;
               // while(!line_flag){ scanner(); }
                break;
            case 27:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!case语句不符合文法要求"<<endl;
                 while(syn!="if"&&syn!="while"&&syn!="switch"&&syn!="return"&&syn!="lbigsy"&&syn!="printf"&&syn!="scanf"&&syn!="IDSY"){ scanner(); }
                break;
            case 28:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!default语句不符合文法要求"<<endl;
                while(syn!="if"&&syn!="while"&&syn!="switch"&&syn!="return"&&syn!="lbigsy"&&syn!="printf"&&syn!="scanf"&&syn!="IDSY"){ scanner(); }
                break;
            case 29:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!数组越界"<<endl;
                 while(!line_flag){ scanner(); }
                break;
            case 30:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!数组不能整体赋值"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 31:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!无右大括号"<<endl;
                while(syn!="void"&&syn!="const"&&syn!="if"&&syn!="while"&&syn!="switch"&&syn!="return"&&syn!="lbigsy"&&syn!="printf"&&syn!="scanf"&&syn!="IDSY"){ scanner(); }
                break;
            case 32:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!存在非法字符"<<endl;
                while(!line_flag){ scanner(); }
                break;
            case 33:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!有返回值函数没有返回值"<<endl;
                break;
            case 34:
                f_error<<"在源程序第"<<to_string_new(source_line)<<"行：WRONG!!switch中case语句的常量判断有重复"<<endl;
                break;
            default:
                break;
        }

 
    }
    
    void scanner(){
        line_flag = false;
        string_bug = false;
        char ch = getChar(str);
        while(ch!='\0'){
            int pos = 0;
            strBox[pos]='\0';
            int num = 0;
            while(isSpace(ch)|| isNewline(ch) ||isTab(ch)) //过滤空格 换行 制表符
            {
                if(isNewline(ch)){
                    source_line ++;
                    line_flag = true;
                }
                ch = getChar(str);
            }
            if(isLetter(ch)){ //处理字符串
                while(isLetter(ch) || isDigit(ch)){
                    strBox[pos++] =ch;
                    ch = getChar(str);
                }
                retract(str);
                strBox[pos]='\0';
                for(int k = 0; k<pos; k++){
                    if(strBox[k]>='A' && strBox[k]<='Z'){
                        strBox[k]=strBox[k]+32;
                    }
                }
                
                int resultvalue = findKey(strBox);
                if(resultvalue == 0)
                {
                    
                    //cout<<'['<<strBox<<','<<"IDSY"<<','<<strBox<<']'<<endl;
                    id_content = strBox;
                    syn = "IDSY";
                    return;
                    
                }else{
                    
                    strBox[pos]='\0';
                    //cout<<'['<<strBox<<','<<strBox<<"SY"<<','<<"NULL"<<']'<<endl;
                    string temp = strBox;
                    syn = temp;
                    return;
                    
                }
                
            }
            else if(isDigit(ch)){ //处理数字串
                int tempp;
                int counter = 0 ;
                tempp = ch - '0';
               
                    while(isDigit(ch))
                    {
                        counter = counter + 1;
                        strBox[pos++]=ch;
                        ch = getChar(str);
                    }
                    if(counter>1){//不止一个数字
                        if(strBox[0]=='0'){
                            error(10);
                        }
                    }
                    retract(str);
                    strBox[pos]='\0'; //字符串数字之后的清空
                    sscanf(strBox, "%d",&num); //字符串转化成数字
                    number = num;

                    //cout<<'['<<strBox<<','<<"INTSY"<<','<<num<<']'<<endl;
                    syn = "INTSY";
                                        return;
                
                
            }else if(ch=='\"'){
                ch = getChar(str);
                while(ch!='\"')
                {
                    if(ch==32||ch==33||(ch>=35 && ch<=126)){
                        strBox[pos++] = ch;
                        ch = getChar(str);
                    }else{
                        error(18);
                        string_bug = true;
                        return;
                    }
                }
                
                strBox[pos]='\0';
                string_content = strBox;
                string_content ="\""+string_content+ "\"";
                //cout<<"[\""<<strBox<<"\","<<"stringsy"<<','<<"NULL"<<']'<<endl;
                syn = "stringsy";
                return;
                
            }else if(ch =='\''){
                char temp;
                
                strBox[pos++]=ch;
                ch = getChar(str);
                temp = ch;
                strBox[pos++]=ch;
                ch = getChar(str);
                strBox[pos++]=ch;
                strBox[pos]='\0';
               
                if(temp =='+' ||temp =='-'||temp =='*'||temp =='/'||isDigit(temp)||isLetter(temp)){
                    
                    //cout<<'['<<strBox<<','<<"char"<<','<<"NULL"<<']'<<endl;
                    syn = "char";
                    char_content = temp;
                    char_ascii = temp;
                    return;
                }else{
                    //cout<<"Illegal!!!!"<<endl;
                    return;
                }
                
            }else if(isPlus(ch)){ //单字符操作符
                //cout<<'['<<ch<<','<<"plussy"<<','<<"NULL"<<']'<<endl;
                syn = "plussy";
                
                return;
                
            }else if(isMinus(ch)){
                //cout<<'['<<ch<<','<<"minussy"<<','<<"NULL"<<']'<<endl;
                syn = "minussy";
                return;
            }else if(isLpar(ch)){
                //cout<<'['<<ch<<','<<"lparsy"<<','<<"NULL"<<']'<<endl;
                syn = "lparsy";
                
                return;
            }else if(isRpar(ch)){
                //cout<<'['<<ch<<','<<"rparsy"<<','<<"NULL"<<']'<<endl;
                syn = "rparsy";
                
                return;
            }else if(isComma(ch)){
                //cout<<'['<<ch<<','<<"commasy"<<','<<"NULL"<<']'<<endl;
                syn = "commasy";
                
                return;
            }else if(isSemi(ch)){
                //cout<<'['<<ch<<','<<"semisy"<<','<<"NULL"<<']'<<endl;
                syn = "semisy";

                return;
            }else if(isEqual(ch)){
                ch = getChar(str);
                if(isEqual(ch)){
                    //cout<<'['<<"=="<<','<<"ifeqlsy"<<','<<"NULL"<<']'<<endl;
                    syn = "ifeqlsy";
                    return;
                }
                else{
                    //cout<<'['<<"="<<','<<"equsy"<<','<<"NULL"<<']'<<endl;
                    syn = "equsy";
                    
                    retract(str);
                    return;
                }
            }else if(isLsquare(ch)){
                //cout<<'['<<ch<<','<<"lsquaresy"<<','<<"NULL"<<']'<<endl;
                syn = "lsquaresy";
                
                return;
            }else if(isRsquare(ch)){
                //cout<<'['<<ch<<','<<"rsquaresy"<<','<<"NULL"<<']'<<endl;
                syn = "rsquaresy";
                
                return;
            }else if(isLbig(ch)){
                //cout<<'['<<ch<<','<<"lbigsy"<<','<<"NULL"<<']'<<endl;
                syn = "lbigsy";
                
                return;
            }else if(isRbig(ch)){
                //cout<<'['<<ch<<','<<"rbigsy"<<','<<"NULL"<<']'<<endl;
                syn = "rbigsy";

                return;
            }else if(isMul(ch)){
                //cout<<'['<<ch<<','<<"mulsy"<<','<<"NULL"<<']'<<endl;
                syn = "mulsy";
                return;
            }else if(isDiv(ch)){
                //cout<<'['<<ch<<','<<"divsy"<<','<<"NULL"<<']'<<endl;
                syn = "divsy";
                
                return;
            }else if(isColon(ch)){
                //cout<<'['<<ch<<','<<"colonsy"<<','<<"NULL"<<']'<<endl;
                syn = "colonsy";
                
                return;
            }else if(ch=='<'){
                ch = getChar(str);
                if(isEqual(ch)){
                    //cout<<'['<<"<="<<','<<"lesseqlsy"<<','<<"NULL"<<']'<<endl;
                    syn = "lesseqlsy";
                    return;
                }
                else{
                    //cout<<'['<<"<"<<','<<"lesssy"<<','<<"NULL"<<']'<<endl;
                    syn = "lesssy";
                    retract(str);
                    return;
                }
            }else if(ch=='>'){
                ch = getChar(str);
                if(isEqual(ch)){
                    //cout<<'['<<">="<<','<<"moreeqlsy"<<','<<"NULL"<<']'<<endl;
                    syn = "moreeqlsy";
                    
                    return;
                }
                else{
                    //cout<<'['<<">"<<','<<"moresy"<<','<<"NULL"<<']'<<endl;
                    syn = "moresy";
                    retract(str);
                    return;
                }
            }else if(ch =='!'){
                ch = getChar(str);
                if(isEqual(ch)){
                    //cout<<'['<<"!="<<','<<"NOTeqlsy"<<','<<"NULL"<<']'<<endl;
                    syn = "NOTeqlsy";
                    return;
                }
            }else{
                //ch = getChar(str);
                error(32);
                return;
            }
        }
    }

};

#endif /* lexical_h */
