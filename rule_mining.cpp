
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <queue>
#include <ctime>
#include <cmath>
#include <map>
using namespace std;
#pragma GCC optimize(3,"Ofast","inline")
std::vector<int> raw_data[900000];
std::unordered_map<std::string, std::vector<int> > hashmap;
typedef struct node {
    int id;
    int number;
    std::string name;
    int father;
    std::vector<int> son;
    int generation;
} Trie_node;
Trie_node Node[99999];

int frequent[1000];
int candi[1000];
double confidence;
map<string,double> Map;
vector<int> string_to_vector(string str){
    vector<int> res;
    int v = 0;
    for(int i = 0; i < str.size(); i++){
        if(str[i] == '_'){
            res.push_back(v);
            v = 0;
            continue;
        }
        int tmp = str[i] - '0';
        v = v * 10 + tmp;
    }
    res.push_back(v);
    return res;
}
string vector_to_string(vector<int> vec){
    string res;
    for(int i = 0; i < vec.size(); i++){
        res += to_string(vec[i]);
        if(i != vec.size()-1)
            res += '_';
    }
    return res;
}

int get_rule(std::string name){
    vector<int> all = string_to_vector(name);
    double all_ratio = hashmap[name].size();
    int len = all.size();
    int res = 0;
    for(int i = 1; i < (1 << len) - 1; i++){
        vector<int> left;
        vector<int> right;
        for(int j = 0;j < len; j++){
            if((i >> j) & 1){
                left.push_back(all[j]);
            }
            else{
                right.push_back(all[j]);
            }
        }
        string left_name = vector_to_string(left);
        string right_name = vector_to_string(right);
        double ratio = all_ratio / hashmap[left_name].size() ;
        
        if(ratio >= confidence){
            res ++ ;
            cout<<left_name<<"->"<<right_name<<" confidence "<<ratio<<endl;
        }
            
//        for(auto it : left){
//            cout<<it<<" ";
//        }
//        cout<<"        ";
//        for(auto it : right){
//            cout<<it<<" ";
//        }
//        cout<<endl;
    }
    return res;
  
}
void read() {
    std::ifstream fin;
    fin.open("C:\\Users\\jc\\Desktop\\DM\\apriori\\retail.dat");

    if (!fin.is_open()) {
        std::cout << "can not open" << std::endl;
        return;
    }
    int max_num = 0;
    for (int i = 0; ; i++) {
        std::string str;
        if (!getline(fin, str))
            break;
        int v = 0;
        for (int j = 0; j < str.size(); j++) {
            if (str[j] == ' ') {
                raw_data[i].push_back(v);
                max_num = std::max(max_num, v);
                std::string str_v = std::to_string(v);
                hashmap[str_v].push_back(i);
                v = 0;
            }
            else
                v = v * 10 + (str[j] - '0');
        }


    }
    //std::cout<<max_num<<std::endl;
}


int main() {
    
//    string name = "1_2_3_4_5";
//    get_rule(name);
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    //num:88162
    //max_v:16469
    int num = 88162;
    int max_v = 16469;
    float support_ratio;
    std::cin >> support_ratio;
    cin >> confidence;
    read();
    clock_t st = clock();
    int support_num = ceil(support_ratio * num);
    //建立trie根节点
    Trie_node root;
    root.id = 0;
    root.name = "";
    root.father = 0;
    root.generation = 0;
    int ID = 0;
    int rule_num = 0;
    //用队列宽搜
    std::queue<int> Q;
    Node[0] = root;
    Q.push(0);
    int total = 0;
    candi[1] = max_v;

    //特判2项集情况，每行里枚举j,k，如果j频繁并且k也频繁就把j_k的行号i放到umap里
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < raw_data[i].size() - 1; j++) {
            if (hashmap[std::to_string(raw_data[i][j])].size() < support_num)
                continue;
            for (int k = j + 1; k < raw_data[i].size(); k++) {
                if (hashmap[std::to_string(raw_data[i][k])].size() < support_num)
                    continue;
                std::string name = std::to_string(raw_data[i][j]) + "_" + std::to_string(raw_data[i][k]);

                hashmap[name].push_back((i));
            }
        }
    }

    //宽搜启动
    while (Q.size()) {

        int ft_id = Q.front();
        Q.pop();
        Trie_node& ft_nd = Node[ft_id];

        //如果是根节点，就把频繁的一项集都接上去(id,father,name,number,generation)入栈，总数加一
        if (ft_id == 0) {
            for (int i = 0; i <= max_v; i++) {

                if (hashmap[std::to_string(i)].size() >= support_num) {
                    ID++;
                    Node[ID].id = ID;
                    Node[ID].father = ft_id;
                    Node[ID].name = std::to_string(i);
                    Node[ID].number = i;
                    ft_nd.son.push_back(ID);
                    Node[ID].generation = ft_nd.generation + 1;
                    //std::cout<<Node[ID].name<<std::endl;
                    frequent[Node[ID].generation]++;
                    Q.push(ID);
                    total++;

                }
            }
            continue;
        }

        //找出可以接到该点儿子的候选项
        std::vector<int> candidate;
        for (int i = 0; i < Node[ft_nd.father].son.size(); i++) {
            Trie_node brother = Node[Node[ft_nd.father].son[i]];

            if (brother.number > ft_nd.number) {

                std::string name = ft_nd.name + "_" + std::to_string(brother.number);
                bool success = true;

                //当前代数大于等于2才剪枝
                if (ft_nd.generation >= 2) {

                    std::vector<int> ancestor;
                    int fa = ft_nd.id;
                    //把该条路径的数都存在ancestor里，并且反转ancestor
                    while (fa != 0) {
                        ancestor.push_back(Node[fa].number);
                        fa = Node[fa].father;
                    }
                    std::reverse(ancestor.begin(), ancestor.end());
                    //枚举所有(n-1)的子集，看看是不是频繁的
                    for (int j = 0; j < ancestor.size(); j++) {
                        std::string name = "";
                        for (int k = j+1; k < ancestor.size(); k++){

                            if (j == k)
                                continue;
                            name += std::to_string(ancestor[k]) + "_";
                        }
                        name += std::to_string(brother.number);
                        if (hashmap[name].size() < support_num) {
                            success = false;
                            break;
                        }
                    }
                }
                //有子集不在，直接continue
                if (!success)
                    continue;

                //所有子集都在，加入候选项集，并且ft_nd.generation + 1的候选集加1
                candidate.push_back(brother.number);
                if (ft_nd.generation != 1)
                    candi[ft_nd.generation + 1]++;

            }

        }

        //如果当前是第一代，从candidate中选
        if (ft_nd.generation == 1) {
            //i是候选项集里的数
            for (int i : candidate) {
                std::string name = ft_nd.name + "_" + std::to_string(i);
                //有可能的二项已经存在hashmap中了，直接看size()就行
                //如果是频繁的就新建节点(id,father,number,name,generation)，ft(son)
                //frequent的Node[ID].generation加一，并且入栈，总数加一
                if (hashmap[name].size() >= support_num) {
                    ID++;
                    Node[ID].id = ID;
                    Node[ID].father = ft_id;
                    Node[ID].number = i;
                    Node[ID].name = ft_nd.name + "_" + std::to_string(i);
                    ft_nd.son.push_back(ID);
                    Node[ID].generation = ft_nd.generation + 1;
                    frequent[Node[ID].generation]++;
                    //std::cout << Node[ID].name << std::endl;
                    Q.push(ID);
                    total++;
                    rule_num += get_rule(Node[ID].name);
                }
            }
            continue;
        }


        std::vector<int> ancestor;
        int v = 0;

        //通过name把父节点们都放到ancestor里
        for (int i = 0; i < ft_nd.name.size(); i++) {
            if (ft_nd.name[i] == '_') {
                ancestor.push_back(v);
                v = 0;
            }
            else {
                v = v * 10 + (ft_nd.name[i] - '0');
            }
        }
        ancestor.push_back(v);

        //枚举每一个候选项
        for (int i = 0; i < candidate.size(); i++) {
            int c = candidate[i];
            std::string c_name = std::to_string(c);
            std::string name = ft_nd.name + "_" + c_name;

            int c_size = ft_nd.generation * hashmap[c_name].size();
            int ft_size = hashmap[ft_nd.name].size();
            //如果c_size 比 ft_szie小
            if (c_size < ft_size) {

                //从包含c的行里找所有ancestor,有的话就把行号放入umap
                for (int row : hashmap[c_name]) {
                    bool flag = true;
                    for (int an : ancestor) {
                        bool it = std::binary_search(raw_data[row].begin(), raw_data[row].end(), an);
                        if (!it) {
                            flag = false;
                            break;
                        }
                    }
                    if (flag) {
                        hashmap[name].push_back(row);
                    }
                }
            }
            else {
                //在ft的行里找c               
                for (int row : hashmap[ft_nd.name]) {
                    bool it = std::binary_search(raw_data[row].begin(), raw_data[row].end(), c);
                    if (it) {
                        hashmap[name].push_back(row);
                    }
                }
            }

            //是频繁的就新建节点
            if (hashmap[name].size() >= support_num) {

                Node[++ID].id = ID;
                Node[ID].father = ft_id;
                Node[ID].number = c;
                Node[ID].name = name;
                ft_nd.son.push_back(ID);
                Node[ID].generation = ft_nd.generation + 1;
                frequent[Node[ID].generation]++;
                //std::cout << Node[ID].name << std::endl;
                Q.push(ID);
                total++;
                rule_num += get_rule(Node[ID].name);
            }
        }


    }
    clock_t ed = clock();
    std::cout << (double)(ed - st) / (CLOCKS_PER_SEC) << std::endl;
    //c[2]可以从L[1]直接得出
    candi[2] = frequent[1] * (frequent[1] - 1) / 2;
    for (int i = 1; ; i++) {
        if (candi[i] == 0)
            break;
        std::cout << "C[" << i << "]" << " = " << candi[i] << "   ";
        std::cout << "L[" << i << "]" << " = " << frequent[i] << std::endl;
    }
    std::cout << total << std::endl;
    cout<<rule_num<<endl;
    return 0;
}
