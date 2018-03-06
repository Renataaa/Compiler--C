//
//  symboltable.h
//  语法分析2
//
//  Created by Renata on 2017/12/5.
//  Copyright © 2017年 Renata. All rights reserved.
//

#ifndef symboltable_h
#define symboltable_h
#define PARA_MAX 10
#include <map>
using namespace std;
int a;
enum SymbolType
{
    var_t, type_t, struct_t, enum_t, delegate_t, function_t
};
struct funcInfo{
    int num;
    int params[PARA_MAX]; //int 1 char 2    其余为0
};
struct id {
    int level;
    string type; //类别 int char 函数可能是void
    string kind; //种类 简单变量"var_sim" 数组"array" 函数"function" 过程 常量 const
  
    int value; //int 为数字值 char为ascii码
    int dimension; //数组维数 不是数组的均为-1
    int offset; ////相对偏移量字节来算
    int func_size ;  ///本身的字节
    int register_begin = 0;
    int register_end = 0;
    funcInfo* func = new funcInfo();
    //struct funcInfo *func;
};

//局部符号表
class IDMap
{
private:
    map<string,id*> ID_map;
public:
    IDMap(){
    };
    /*
    ~IDMap(){
        {
            for (auto p : ID_map) {
                id* pid = p.second;
                delete pid;
            }
        }
    };
     */
    id* find(const string& str) const {// 查找一个符号
        auto p = ID_map.find(str);
        if (p != ID_map.end())
            return (*p).second;
        return NULL;
    }
    
    map<string,id*>& getAll() {
        return ID_map;
    }
    
    void insert(const string& str, string type, string kind,int value,int dimension,int offset) {
        id* i = new id();
        i->type = type;
        i->kind = kind;
        i->value = value;
        i->dimension = dimension;
        i->offset = offset;
        ID_map[str] = i;
        
    }
    void insert_func(const string& str, string type, string kind, int num, int para[]){
        id* i = new id();
        i->type = type;
        i->kind = kind;
        i->func->num = num;
        for(int k = 0 ; k<PARA_MAX; k++){
            i->func->params[k] = para[k];
        }
        ID_map[str] = i;
        
    }
};



#endif /* symboltable_h */
