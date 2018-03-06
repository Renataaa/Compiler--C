//
//  idtable.h
//  语法分析2
//
//  Created by Renata on 2017/12/5.
//  Copyright © 2017年 Renata. All rights reserved.
//

#ifndef idtable_h
#define idtable_h

/**
 * @brief 简易栈式符号表，需要在进入函数后手动压栈等操作
 */
class IDTable
{
public:
    IDTable(){
        ID_stack.push_back(IDMap());
    }
    
    /**
     * @brief 查找元素是否在符号表中
     * @details 查找元素是否在当前符号表中，若当前符号栈不只一层，还好向上层符号表进行查找
     *
     * @param str 要查找的符号名
     * @return 若找到，返回id结构体的指针，无需释放内存。未找到返回NULL
     */
    id* find(const string& idname) const{
      /*  for (auto p = ID_stack.rbegin(); p != ID_stack.rend(); ++p) {
            const IDMap& imap = *p;
            id* pid = imap.find(idname);
            if (pid != NULL) return pid;
        }
        return NULL;*/
        auto p =ID_stack.rbegin();
        const IDMap& imap = *p;
        id* pid = imap.find(idname);
        if (pid != NULL) return pid;
        return NULL;
    }
    id* find_total(const string& idname) const{
        for (auto p = ID_stack.rbegin(); p != ID_stack.rend(); ++p) {
            const IDMap& imap = *p;
            id* pid = imap.find(idname);
            if (pid != NULL) return pid;
        }
        return NULL;
    }
    
    /**
     * @brief 返回一层的全部元素
     *
     * @param level 返回一层的全部元素，level是传入的层级
     * @return 返回的元素map
     */
    map<string,id*>& getAll(int level){ //level 0 应该是第一级
         return ID_stack[level].getAll();
    }
    

    void insert(const string& str, string type, string kind,int value,int dimension,int offset){
        IDMap& imap = ID_stack.back();
        imap.insert(str,type, kind,value,dimension,offset);
    }
    void insert_func(const string& str,string type, string kind, int num, int para[] ){
         IDMap& imap = ID_stack.back();
        imap.insert_func(str,  type, kind, num, para);
        
    }

    void push(){
         ID_stack.push_back(IDMap());
    }
    
    void push_ex(IDMap idmap){
        ID_stack.push_back(idmap);
    }
    

    void pop(){
        ID_stack.pop_back();
    }
    

    int getLevel(){
        return ID_stack.size()-1;
    }
    
private:
    deque<IDMap> ID_stack;
};

#endif /* idtable_h */
