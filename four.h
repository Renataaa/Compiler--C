//
//  four.h
//  语法分析2
//
//  Created by Renata on 2017/12/6.
//  Copyright © 2017年 Renata. All rights reserved.
//
#include <vector>
#include <string.h>
#ifndef four_h
#define four_h
#include <map>
class FOUR{
public:

    int reg_num = 6;
    int counter = 0 ;
    int counter_l = 0;
    int vec_length = 0;
    string func_name = "";
    int vec_entry[1000]; /////1 是入口 0 不是 否则为-1
    string def[100];
    string use[100];
    int use_count = 0;
    int def_count = 0;
    struct CONTENT{
        string result;
        string op;  //ADD MINUS  MUL DIV BR BNF
        string arg1;
        string arg2;
        string result_type;
    };
    FOUR(){
        }
    FOUR(const FOUR &F){
        map_zhuo = F.map_zhuo;
        register_map = F.register_map;
        reg_num = F.reg_num;
        counter =  F.counter;
        counter_l = F.counter_l;
        vec_length = F.vec_length;
        func_name = F.func_name;
        for(int i=0 ; i<1000; i++){ vec_entry[i] = F.vec_entry[i];}
        for(int i=0; i<100; i++) {def[i] = F.def[i];}
        for(int i=0; i<100; i++) {use[i] = F.use[i];}
        use_count = F.use_count;
        def_count = F.def_count;
        vec = F.vec;
        var_alloc= F.var_alloc;
        map_zhuo = F.map_zhuo;
        register_map = F.register_map;
        for(int i = 0 ; i<1000; i++ ){lva_total[i] = F.lva_total[i];}
        lva_count = F.lva_count;

    }

    vector<CONTENT> vec;
    vector<CONTENT>::iterator it;
    vector<CONTENT>::iterator it2;
    vector<CONTENT>::iterator it3;

    vector<CONTENT>::iterator it4; /////活跃变量的

    vector<string> var_alloc;   //////保存所有 可能被分配全局寄存器的变量
    vector<string> var_alloc_del;

    map<string, map<string,int>> map_zhuo; /////着色图 索引是二维数组

    map<string,map<string,int>> register_map;  /////函数的map 一级索引函数名 二级是变量名

    string to_string_new(int num)
    {
        ostringstream ostr;
        ostr << num;
        string str = ostr.str();
        return str;
    }
    struct LVA{

        vector<int> follow; ////后继
        vector<string> in_lva;
        vector<string> out_lva;
        vector<string> def_lva;
        vector<string> use_lva;
    };

    struct LVA lva_total[1000]; //////1000个基本块
    int lva_count = 0;

    void push(string result,string op, string arg1, string arg2){

        CONTENT content;
        content.result = result;
        content.op = op;
        content.arg1 = arg1;
        content.arg2 = arg2;
        vec.push_back(content);
    }
    string temp_var(){
        string temp = "@t";
        to_string_new(counter);
        temp = temp + to_string_new(counter);
        counter++;
        return temp;
    }
    string temp_var_label(){
        string temp = "label";
        to_string_new(counter_l);
        temp = temp + to_string_new(counter_l);
        counter_l++;
        return temp;
    }
    void set_counter(int a){
        counter =  a;
    }
    void set_counter_l(int a){
        counter_l =  a;
    }

    void split(){ ////划分基本块 返回入口的数组
        vec_length = vec.size();
        for(int k = 0 ; k<1000; k++){ /////初始化为-1
            vec_entry[k] = -1 ;
        }

        for(int i = 0 ; i<vec_length ; i++){
            if(vec.at(i).op == "*label"){
                vec_entry[i] = 1;
            }else if(vec.at(i).result == "func"){
                vec_entry[i] = 1;
            }else if(vec.at(i).op == ">="||vec.at(i).op == "<="||vec.at(i).op == ">"|| vec.at(i).op == "<" || vec.at(i).op == "=="|| vec.at(i).op == "!=" ){
                int tmp = i+1;
                vec_entry[tmp] = 1 ;
                vec_entry[i] = vec_entry[i] == 1 ? 1 : 0;
            }else if(vec.at(i).op == "BR"){
                int tmp = i+1;
                vec_entry[tmp] = 1 ;
                vec_entry[i] = vec_entry[i] == 1 ? 1 : 0;
            }else{
                vec_entry[i] = vec_entry[i] == 1 ? 1 : 0;
            }
        }

    }

    bool is_num_four(string str){
        int tmp = 0;
        tmp = (int)str[0];
        if((tmp>= 48 && tmp<=57)||(tmp==43)||(tmp==45)){ /////正负号

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

    void dag(){
        //it - vec.begin()
        for(it = vec.begin(); it!=vec.end();it++){
            string op = "";
            string arg1 = "";
            string arg2 = "";
            string result = "";
            string result2 = "";
            if(it->op == "ADD" ||it->op == "SUB"||it->op == "MUL"||it->op == "DIV" ){ ////当前是一个add操作
                op = it->op;
                arg1 = it->arg1;
                arg2 = it->arg2;
                result = it->result;
                for(it2 = it+1; vec_entry[it2 - vec.begin()]!=1;){   /////往后遍历找三个参数相等到该基本块结束
                    if(it2->result == arg1 || it2->result == arg2){
                        break;
                    }
                    if(it2->op == op && it2->arg1 == arg1 && it2->arg2 == arg2){ /////三个元素都相等
                        result2 = it2->result;

                        for(it3 = it2+1; vec_entry[it3 - vec.begin()]!=1 ; it3++){ /////遍历该元素后边的替换t0 t1
                            if(it3->result == arg1||it3->result == arg2){
                                break;
                            }
                            if(it3->arg1 == result2){
                                it3->arg1 = result;
                            }else if(it3->arg2 == result2){
                                it3->arg2 = result;
                            }else if(it3->result == result2){
                                it3->result = result;
                            }
                        }
                        ///////遍历结束之后删除该四元式
                        it2 = vec.erase(it2);

                    }else{
                        it2++;
                    }
                }
            }
        }
    }
    void into_use(string str){
        if(!is_num_four(str) && str[0]!='@'){ ///不是数字且不是中间变量

            for(int use_tmp = 0 ; use_tmp<use_count ; use_tmp++){
                if(str == use[use_tmp]){
                    return;
                }
            }
            for(int def_tmp = 0; def_tmp<def_count ; def_tmp++){
                if(str == def[def_tmp]){
                    return;
                }
            }

            use[use_count] = str;
            use_count++;

        }
    }
    void into_def(string str){
        if(!is_num_four(str) && str[0]!='@'){ ///不是数字且不是中间变量
            for(int use_tmp = 0 ; use_tmp<use_count ; use_tmp++){
                if(str == use[use_tmp]){
                    return;
                }
            }
            for(int def_tmp = 0; def_tmp<def_count ; def_tmp++){
                if(str == def[def_tmp]){
                    return;
                }
            }

            def[def_count] = str;
            def_count++;

        }
    }
    void lva_born(){ //////生成def 和 use集



        for(it4 = vec.begin() ; it4!=vec.end(); it4++){
            if(vec_entry[it4-vec.begin()]==1 && vec_four(it4-vec.begin())!=0){////进入一个新的基本块 把前边的加进去
                struct LVA lva_tmp;

                for(int p =0 ;p<use_count;p++){
                    lva_tmp.use_lva.push_back(use[p]);
                    use[p]="";
                }
                for(int q =0 ;q<def_count;q++){
                    lva_tmp.def_lva.push_back(def[q]);
                    def[q]="";
                }
                lva_total[lva_count] = lva_tmp;
                lva_count++;
                use_count = 0;
                def_count = 0;
            }
            if (it4->op == "ADD" ||it4->op == "SUB"||it4->op == "MUL"||it4->op == "DIV"||it4->op == "ASSIGN"||
                it4->op == "=="||it4->op == "!="||it4->op == "<"||it4->op == "<="||it4->op == ">"||it4->op == ">="){ ////中间变量不处理 数字不处理
                string arg1 = it4->arg1;
                string arg2 = it4->arg2;
                string result = it4->result;

                if(arg1!="") {into_use(arg1);}
                if(arg2!="") {into_use(arg2);}
                ///////result在判断跳label时result是label不加入def集
                if(result!="" && (it4->op == "ADD" ||it4->op == "SUB"||it4->op == "MUL"||it4->op == "DIV"||it4->op == "ASSIGN")) {into_def(result);}

            }
            if(it4->op == "printf"){
                string arg1 = it4->arg1;
                bool num = is_num_four(arg1);
                bool ischar = arg1[0] == 39;
                bool isstring = arg1[0] == 34;
                if(!num && !ischar && !isstring ){
                    into_use(arg1);
                }
            }
            if(it4->op == "scanf"){
                string arg1 = it4->arg1;
                into_def(arg1);
            }
            if(it4->result =="*const_int"|| it4->result =="*const_char" ||it4->result=="*int" ||it4->result=="*char"){
                string op = it4->op;
                into_def(op);
            }
            if(it4->op == "PUSH"){
                string arg1 = it4->arg1;
                into_def(arg1);
            }
        }

    }
    void lva_in_out(){
        int bianlicishu = 0 ;
        int lva_count_tmp = lva_count; /////5 exit
        bool change = true;
        while(change){
            bianlicishu++;
            change = false;
            for(int lct = lva_count_tmp ; lct>=0 ; lct--){ ////从后向前便利基本块

                for(int r =0; r<lva_total[lct].follow.size();r++){ //////每一个基本块 遍历后继
                    int f_block = lva_total[lct].follow.at(r); /////后继的基本块的位置
                    for(int y = 0 ; y<lva_total[f_block].in_lva.size() ; y++){ ////////某一个后继的基本块的in集
                        bool out_tmp = false;
                        for(int z = 0 ; z <lva_total[lct].out_lva.size();z++){   ///遍历原out集
                            if(lva_total[lct].out_lva.at(z) == lva_total[f_block].in_lva.at(y)){
                                out_tmp = true;
                            }
                        }
                        if(!out_tmp){ lva_total[lct].out_lva.push_back(lva_total[f_block].in_lva.at(y)); change = true;}///////加入}
                    }
                    /////lva_total[lct].in_lva

                    for(int u = 0 ; u<lva_total[lct].use_lva.size(); u++){  //////遍历当前use集
                        bool in_tmp = false;
                        for(int h = 0; h<lva_total[lct].in_lva.size(); h++){  /////便利当前in集
                            if(lva_total[lct].in_lva.at(h) == lva_total[lct].use_lva.at(u)){
                                in_tmp = true;
                            }
                        }
                        if(!in_tmp){lva_total[lct].in_lva.push_back(lva_total[lct].use_lva.at(u)); change = true;}
                    }

                    for(int outt = 0 ; outt<lva_total[lct].out_lva.size(); outt++){  /////遍历当前out集
                        bool del_def = false;
                        for(int deff = 0; deff<lva_total[lct].def_lva.size(); deff++){
                            if(lva_total[lct].out_lva.at(outt) == lva_total[lct].def_lva.at(deff)){
                                del_def = true;
                            }
                        }
                        if(!del_def){ /////////
                            bool in_tmp2 = false;
                            for(int inn = 0; inn<lva_total[lct].in_lva.size(); inn++){ //////遍历当前in集
                                if(lva_total[lct].in_lva.at(inn) == lva_total[lct].out_lva.at(outt)){
                                    in_tmp2 = true;
                                }
                            }
                            if(!in_tmp2){ lva_total[lct].in_lva.push_back(lva_total[lct].out_lva.at(outt));change = true;}
                        }
                    }

                }
            }
        }

    }
    int entry_four(int en){  /////第en个基本块
        int en_count = 0;
        for(int i = 0; i<vec.size(); i++){ //////
            if(vec_entry[i] == 1){

                if(en_count == en){
                    return i;
                }
                en_count++;
            }
        }
        return -1;
    }
    int entry_end(int en){
        int tmp = entry_four(en+1);
        tmp = tmp - 1;   ////下一个基本块的第一句话 的上一句话
        return tmp;
    }
    int vec_four(int i){   ////第i四元式在第几个基本块
        int tmp = -1;
        for(int k = 0; k<=i ; k++){
            if(vec_entry[k] == 1){
                tmp++;
            }
        }
        return tmp;
    }
    void lva_all(){
        int begin = 0;
        int end = 0;
        for(int i = 0 ; i<lva_count ; i++){  /////遍历基本块
            int return_vec = entry_end(i);
            if(vec.at(return_vec).op == "end" ){ //////////////////////////
                end = i;
                lva_func(begin, end);
                begin = i+1;
            }
        }
    }
    void follow(){
        for(int i = 0; i<lva_count; i++){  //////遍历基本块 当前基本块i
            int begin = entry_four(i);
            int end = entry_end(i);
            int con = -1;
            if(vec.at(end).op == "BR"){
                int t1;
                for(t1= 0 ; t1<vec.size(); t1++){
                    if(vec.at(t1).op == "*label" && vec.at(t1).result ==vec.at(end).arg1){
                        break;
                    }
                }
                con = vec_four(t1);
                bool comein = true;
                for(int p = 0 ; p<lva_total[i].follow.size();p++){
                    if(lva_total[i].follow.at(p) ==con){
                        comein=false;
                    }
                }
                if(comein) {lva_total[i].follow.push_back(con);}
            }else if(vec.at(end).op == ">="||vec.at(end).op == "<="||vec.at(end).op == ">"|| vec.at(end).op == "<" || vec.at(end).op == "=="|| vec.at(end).op == "!=" ){
                int t1;
                for(t1= 0 ; t1<vec.size(); t1++){
                    if(vec.at(t1).op == "*label" && vec.at(t1).result ==vec.at(end).result){
                        break;
                    }
                }
                con = vec_four(t1);
                bool comein = true;
                for(int p = 0 ; p<lva_total[i].follow.size();p++){
                    if(lva_total[i].follow.at(p) ==con){
                        comein=false;
                    }
                }
                if(comein) {lva_total[i].follow.push_back(con);}

                comein = true;
                for(int p = 0 ; p<lva_total[i].follow.size();p++){  /////后一个基本块
                    if(lva_total[i].follow.at(p) ==i+1){
                        comein=false;
                    }
                }
                if(comein) {lva_total[i].follow.push_back(i+1);}
            }else{
                bool comein = true;
                for(int p = 0 ; p<lva_total[i].follow.size();p++){  /////后一个基本块
                    if(lva_total[i].follow.at(p) ==i+1){
                        comein=false;
                    }
                }
                if(comein && i+1<lva_count) {lva_total[i].follow.push_back(i+1);}
            }
        }
        for(int i = 0; i<lva_count ; i++){

            for(int j = 0; j<lva_total[i].follow.size(); j++){
                //cout<<"第"<<i<<"几个基本块后继："<<lva_total[i].follow.at(j)<<" "<<endl;
            }
        }
    }
    void lva_func(int begin, int end){

        func_name = vec.at(entry_four(begin)).op;
        for(int b = begin ;b<=end; b++){ /////遍历一个函数的所有基本块


            vector<string> vec_tmp;
            vector<string> repeat;
            for(int ni = 0 ; ni <lva_total[b].in_lva.size()  ; ni++){ /////遍历这个基本块的in集
                /// vector<string> vec_tmp;  /////lva_total[b].in_lva.at(ni)
                /////便利out
                bool flag_in = false;
                for(int ou = 0  ; ou<lva_total[b].out_lva.size()  ; ou++){
                    if(lva_total[b].out_lva.at(ou) ==lva_total[b].in_lva.at(ni)){
                        flag_in = true;
                        break;
                    }
                }
                /////遍历当前
                bool del = false;
                for(int tmp = 0 ; tmp<var_alloc.size() ; tmp++){
                    if(var_alloc.at(tmp) == lva_total[b].in_lva.at(ni)){
                        del = true;
                    }
                }
                repeat.push_back(lva_total[b].in_lva.at(ni));
                if(!del ){vec_tmp.push_back(lva_total[b].in_lva.at(ni)); }
            }

            update_zhuo(vec_tmp,repeat);


        }

        alloc();
        alloc2();
    }
    void update_zhuo(vector<string> t, vector<string> repeat){

        for(int p = 0 ; p<t.size() ; p++){
            for(int q = 0 ;q<var_alloc.size(); q++){
                map_zhuo.at(var_alloc.at(q))[t.at(p)] = 0;
            }
        }

        for(int p = 0; p<t.size(); p++){
            var_alloc.push_back(t.at(p));
        }
        map<string,int> map_tmp;
        for(int p = 0 ; p<var_alloc.size(); p++){
            map_tmp[var_alloc.at(p)] = 0;
        }
        for(int p = 0; p<t.size(); p++){
            map_zhuo[t.at(p)] = map_tmp;
        }

        for(int p = 0 ;p <repeat.size(); p++){
            for(int q = p+1 ; q<repeat.size() ; q++){
                map_zhuo[repeat.at(p)][repeat.at(q)] = 1;
                map_zhuo[repeat.at(q)][repeat.at(p)] = 1;
            }
        }

    }
    void alloc(){
        bool conti = true;
     //   int reg_count = 13;
     //   map<string,int> map_tmp ; /////string 变量名 int分配的寄存器
        bool find_less = false;
        while(conti){
            find_less = false;
            conti = false;
            for(int i = 0 ; i<var_alloc.size(); i++){ //////遍历节点
                int con = 0;
                for(int j = 0 ; j <var_alloc.size(); j ++){    /////连接边数
                    if(map_zhuo[var_alloc.at(i)][var_alloc.at(j)] == 1 ){
                        con++;
                    }
                }
                if(con<reg_num){
                    find_less = true;
             //       map_tmp[var_alloc.at(i)] = reg_count;
               //     reg_count++;
                    var_alloc_del.push_back(var_alloc.at(i));
                    del_var(var_alloc.at(i));
                }
            }
            ////随便删除一个
            if(!find_less){
                if(var_alloc.size()>1){
                    del_var(var_alloc.at(0));
                    conti = true;
                }else{
                    conti = false;
                }
            }else{
                if(var_alloc.size()>=1){
                    conti = true;
                }else{
                    conti = false;
                }
            }
        }

      // register_map[func_name] = map_tmp; ///////
    }
    void alloc2(){

        map<string,int> map_tmp ;
        int register_count = 13;
        for(int z = var_alloc_del.size()-1 ; z>=0 ; z--){ //////从后往前遍历带分配寄存器的变量
            map_tmp[var_alloc_del.at(z)] = register_count;
            register_count++;
            if(register_count == 13 + reg_num ){
                register_count = 13;
            }
            if(z!=var_alloc_del.size()-1){   /////最外边的不会
                for(int i = z+1 ; i<var_alloc_del.size() ; i++){ ////遍历这个和已经出去的
                    if(map_zhuo[var_alloc_del.at(z)][var_alloc_del.at(i)] == 1 && map_tmp[var_alloc_del.at(i)] == map_tmp[var_alloc_del.at(z)]){ //////如果二者冲突
                        map_tmp[var_alloc_del.at(z)] = register_count;
                        register_count++;
                        if(register_count == 13 + reg_num ){
                            register_count = 13;
                        }
                    }
                }
            }
        }
        register_map[func_name] = map_tmp;
    }
    void del_var(string str){
        vector<string>::iterator it5;
        for(it5 = var_alloc.begin(); it5 != var_alloc.end(); it5++){ ///////结点删除
            if(str == *it5){
                var_alloc.erase(it5); ////////
                break;
            }
        }
    }
    void output_four(){
        ofstream fout("/Users/renata/four.txt");
        //ofstream fout("D:\\four.txt");
        for(int i = 0 ; i<vec.size(); i++){
            fout<<vec.at(i).result<<" "<<vec.at(i).op<<" "<<vec.at(i).arg1<<" "<<vec.at(i).arg2<<"\n"<<endl;
        }
    }
    void output_four_youhua(){
        //ofstream fout("/Users/renata/four_youhua.txt");
        ofstream fout("D:\\four_youhua.txt");
        for(int i = 0 ; i<vec.size(); i++){
            fout<<vec.at(i).result<<" "<<vec.at(i).op<<" "<<vec.at(i).arg1<<" "<<vec.at(i).arg2<<"\n"<<endl;
        }
    }
};

#endif /* four_h */
