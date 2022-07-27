#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <queue>
#include <ctime>
#include <cmath>
#include <map>
#pragma GCC optimize(3,"Ofast","inline")
using namespace std;
const int N = 1e6 + 10;
int id = 0;
vector<pair<vector<int>, int>> raw_data;
using ll = long long;
int priority[20000];
//按优先级排序
bool cmp(int& a, int& b) {
    //如果优先级相等，按goods_id大小排序
    if (priority[a] == priority[b])
        return a > b;
    return priority[a] > priority[b];
}

// id, count, chilldren_id,father, value
typedef struct {
    int id;
    int count;
    unordered_map<int, int> children_id;
    int father;
    int value;
} node;

node data_node[10000000];


void read() {
    std::ifstream fin;
    fin.open(R"(C:\Users\jc\Desktop\DM\apriori\retail.dat)", ios::in);
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
        vector<int> row;
        for (int j = 0; j < str.size(); j++) {
            if (str[j] == ' ') {
                row.push_back(v);
                max_num = std::max(max_num, v);
                std::string str_v = std::to_string(v);
                v = 0;
            }
            else
                v = v * 10 + (str[j] - '0');
        }
        raw_data.push_back({ row,1 });
    }
}

int fp_growth(vector<int>& frequent_iter, map<vector<int>, int>& raw_data, int& support_num) {

    //如果raw_data只有一行数据，直接结束递归
    //记len为该map的第一个数据（vector）的size(),返回2的len次方减一
    if (raw_data.size() == 1) {
        int len = raw_data.begin()->first.size();
        return pow(2,len) - 1;
    }
    bool is_end = true;
    //data_nodenumber记录头表里的goods_id出现在哪些节点中
    unordered_map<int, vector<int> > data_nodenumber;
    id++;

    //构建root(id,father)
    node root;
    root.id = id;
    root.father = id;
    data_node[id] = root;
    //建树
    for (auto it : raw_data) {

        node father = data_node[root.id];
        //每一行的数据
        auto row = it.first;
        //该行有几个相同的
        int c = it.second;
        //x是行里的数据，即goods_id
        for (auto x : row) {
            //如果father 的孩子有x，就把这个x的节点的count加c,然后把father赋值成该节点

            if (father.children_id.count(x)) {
                int Self_id = father.children_id[x];
                data_node[Self_id].count += c;
                father = data_node[Self_id];
            }

            //如果father 的孩子没有x，首先创建一个节点（id,father,count,value），然后把该节点的id放入对应的data_nodenumber
            //再把father赋值成该节点
            else {
                node Self;
                id++;
                data_nodenumber[x].push_back(id);
                data_node[father.id].children_id[x] = id;
                Self.father = father.id;
                Self.id = id;
                Self.count = it.second;
                Self.value = x;
                data_node[id] = Self;
                father = Self;
            }
        }
    }
    //判断每个树的每个节点是不是只有一个儿子，如果不是就不能结束递归
    for (int i = root.id; i <= id; i++) {
        int children_num = data_node[i].children_id.size();
        if (children_num > 1) {
            is_end = false;
            break;
        }
            
    }
    //如果是单链，返回2的tree_size次方减一
    if (is_end) {
        int tree_size = id - root.id;
        return pow(2, tree_size) - 1;
    }

    //遍历头表frequent_iter的值，继续递归
    int res = 0;
    for (auto it : frequent_iter) {

        //frequent_iter_son是传给下一个递归的头表
        //raw_data_son是原始数据

        vector<int> frequent_iter_son;
        vector<pair<vector<int>, int>> raw_data_son;
        unordered_map<int, int> goods_num;

        //遍历在头表的该值（it）的data_nodenumber存的节点id
        //从下往上遍历，先把原始数据记录下来，顺便记下来每个节点value的数量，注意加的树是就是it的count
        //brother_id是节点编号

        for (auto brother_id : data_nodenumber[it]) {
            int c = data_node[brother_id].count;
            int fa = brother_id;
            vector<int> row;
            //往上遍历，到根节点停止（data_node[fa].id == root.id）
            //先把原始数据放到row里
            //goods_num里该goods_id 加上c
            while (true) {
                fa = data_node[fa].father;
                if (data_node[fa].id == root.id)
                    break;
                int goods_id = data_node[fa].value;
                row.push_back(goods_id);
                goods_num[goods_id] += c;
            }

            //如果row不空才放入raw_data,并且有c个该vector
            if (!row.empty())
                raw_data_son.push_back({ row, c });
        }

        //data_son 存的是清理过的数据
        unordered_map<int, int> has_push;
        map<vector<int>, int> data_son;

        //把goods_num的频繁的单项放入下一个头表中（去重）
        for (auto good : goods_num) {
            if (!has_push.count(good.first) && good.second >= support_num) {
                frequent_iter_son.push_back(good.first );
                has_push[good.first] = 1;
            }
        }

        //遍历原始数据，去掉非频繁的单项，并且按优先级排序，放入清洗过的数据里
        for (auto row : raw_data_son) {
            //row是<vector,nums>的结构

            vector<int> clean_row;
            for (auto x : row.first) {
                //如果x是频繁的,就放到clean_row里，并且设置优先级为x的数量(goods_num[x])
                if (goods_num[x] >= support_num) {
                    clean_row.push_back(x);
                    priority[x] = goods_num[x];
                }
            }
            //如果clean_row不空，就按优先级排序，并且放入data_son里，要把值加上原始row的数量
            if (!clean_row.empty()) {
                sort(clean_row.begin(), clean_row.end(), cmp);
                data_son[clean_row] += row.second;
            }
        }
        //如果头表是空的，就加1
        if (frequent_iter_son.empty())
            res += 1;

        //如果非空，就加递归值在再加一
        else {
            res += fp_growth(frequent_iter_son, data_son, support_num);
            res += 1;
        }

    }
    return res;

}
int main() {
    read();
    float support_ratio;
    std::cin >> support_ratio;
    int support_num = ceil(support_ratio * raw_data.size());
    clock_t st = clock();
    unordered_map<int, int> goods_num;

    //统计每个商品的个数 存入goods_num（goods_id,num）
    for (int i = 0; i < raw_data.size(); i++) {
        for (auto it : raw_data[i].first) {
            goods_num[it]++;
        }
    }
    //unordered_map<string, int> frequent_data;
    //goods_num,goods_id
    vector<int > frequent_iter; 
    unordered_map<int, int> has_push;

    //把频繁的单项集放入frequent_iter中（去重的）
    for (int i = 0; i < raw_data.size(); i++) {
        for (auto it : raw_data[i].first) {
            if (!has_push.count(it) && goods_num[it] >= support_num) {
                frequent_iter.push_back(it);
                has_push[it] = 1;
            }
        }
    }
    map<vector<int>, int> data_son;

    //在原始数据里，把每行的频繁项单项集保留下来，放到clean_row里，并按优先级排序

    for (auto row : raw_data) {
        //clean_row暂时记录去除非频繁单项的一行
        vector<int> clean_row;

        //在原始数据里，把每行的频繁项单项集保留下来，放到clean_row里；并且在priority中记录优先级，即goods_num[goods_id]
        for (auto x : row.first) {
            if (goods_num[x] >= support_num) {
                clean_row.push_back(x);
                priority[x] = goods_num[x];
            }
        }
        //按优先级排序
        //data_son的clean_row 加上clean_row的数量（即row.second)
        if (!clean_row.empty()) {
            sort(clean_row.begin(), clean_row.end(), cmp);
            int nums_of_row = row.second;
            data_son[clean_row] += nums_of_row;
        }
    }
    // frequent_iter  头表
    // data_son   clean_data
    int num_frequent_pattern = fp_growth(frequent_iter, data_son, support_num);
    clock_t ed = clock();
    std::cout << (double)(ed - st) / (CLOCKS_PER_SEC) << std::endl;
    cout << num_frequent_pattern << endl;
    return 0;
}
