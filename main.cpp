
#include <bits/stdc++.h>

std::vector<int> raw_data[900000];
std::unordered_map<std::string,std::vector<int> > hashmap;
typedef struct node{
    int id;
    int number;
    std::string name;
    int father;
    std::vector<int> son;
    int generation;
} Trie_node;
Trie_node Node[19999999];

int frequent[1000];
int candi[1000];
void read(){
    std::ifstream fin ;
    fin.open("C:\\Users\\jc\\Desktop\\DM\\apriori\\retail.dat");
    if(!fin.is_open()){
        std::cout<<"can not open"<<std::endl;
        return ;
    }
    int max_num = 0;
    for(int i = 0; ; i++){
        std::string str;
        if(!getline(fin,str))
            break;
        int v = 0;
        for(int j = 0; j < str.size(); j++){
            if(str[j] == ' '){
                raw_data[i].push_back(v);
                max_num = std::max(max_num,v);
                std::string str_v = std::to_string(v);
                hashmap[str_v].push_back(i);
                v = 0;
            }
            else
                v = v*10 + (str[j] - '0');
        }


    }
    //std::cout<<max_num<<std::endl;
}


int main() {

    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    read();

    //std::cout<<(double)(ed-st)/(CLOCKS_PER_SEC);
    //std::cout<<"done";

    int num = 88162;
    candi[1] = num;
    int max_v = 16469;
    float support_ratio;
    std::cin >> support_ratio;

    int support_num = ceil(support_ratio * num);
    Trie_node root;
    root.id = 0;
    root.name = "";
    root.father = 0;
    root.generation = 0;
    int ID = 0;
    std::queue<int> Q;
    Node[0] = root;
    Q.push(0);
    int x = 0;
    int total = 0;

    clock_t st=clock();

    for(int i = 0; i < num; i++){
        for(int j = 0; j < raw_data[i].size() - 1; j++){
            if(hashmap[std::to_string(raw_data[i][j])].size() < support_num)
                continue;
            for(int k = j+1; k < raw_data[i].size(); k++){
                if(hashmap[std::to_string(raw_data[i][k])].size() < support_num)
                    continue;
                std::string name = std::to_string(raw_data[i][j]) + "_" + std::to_string(raw_data[i][k]);

//# pragma omp critical
                hashmap[name].push_back((i));
            }
        }
    }

    while(Q.size()){
        int ft_id = Q.front();

        Q.pop();
        Trie_node &ft_nd = Node[ft_id];

        if(ft_id == 0){
            for(int i = 0; i <= max_v; i++){

                if(hashmap[std::to_string(i)].size() >= support_num){
                    Node[++ID].id = ID;
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


        std::vector<int> candidate;
        for(int i = 0; i < Node[ft_nd.father].son.size(); i++){
            if(Node[Node[ft_nd.father].son[i]].number >  ft_nd.number){

                std::string name = ft_nd.name + "_" + std::to_string(Node[Node[ft_nd.father].son[i]].number);
                bool success = true;
                if(ft_nd.generation >= 2) {
                    x++;
                    std::vector<int> ancestor;
                    int fa = ft_nd.id;
                    while (fa != 0) {
                        ancestor.push_back(Node[fa].number);
                        fa = Node[fa].father;
                    }
                    std::reverse(ancestor.begin(), ancestor.end());
                    for (int j = 0; j < ancestor.size(); j++) {
                        std::string check = "";
                        for (int k = 0; k < ancestor.size(); k++) {

                            if (j == k)
                                continue;
                            check += std::to_string(ancestor[k]) + "_";
                        }
                        check += std::to_string(Node[Node[ft_nd.father].son[i]].number);
                        if (hashmap[check].size() < support_num) {
                            success = false;
                            break;
                        }
                    }
                }
                if(!success)
                    continue;
                candidate.push_back(Node[Node[ft_nd.father].son[i]].number);
                if(ft_nd.generation != 1)
                    candi[ft_nd.generation+1]++;

            }

        }

        if(ft_nd.generation == 1){
            for(int i : candidate){
                std::string name = ft_nd.name + "_" + std::to_string(i);
                if(hashmap[name].size() >= support_num){
                    Node[++ID].id = ID;
                    Node[ID].father = ft_id;
                    Node[ID].number = i;
                    Node[ID].name = ft_nd.name + "_" + std::to_string(i);
                    ft_nd.son.push_back(ID);
                    Node[ID].generation = ft_nd.generation + 1;
                    frequent[Node[ID].generation]++;
                    Q.push(ID);
                    //std::cout<<Node[ID].name<<std::endl;
                    total++;
                }
            }
            continue;
        }
        std::vector<int> ancestor;
        int v = 0;
        for(int i = 0; i < ft_nd.name.size(); i++){
            if(ft_nd.name[i] == '_'){
                ancestor.push_back(v);
                v = 0;
            }
            else{
                v = v*10 + (ft_nd.name[i]-'0');
            }
        }
        ancestor.push_back(v);


        for(int i = 0; i < candidate.size();i++){
            int c = candidate[i];
            std::string name = ft_nd.name + "_" + std::to_string(c);
            if(ft_nd.generation * hashmap[std::to_string(c)].size() < hashmap[ft_nd.name].size()){

                for(int row : hashmap[std::to_string(c)]){
                    bool flag = true;
                    for(int an : ancestor){
                        bool it = std::binary_search(raw_data[row].begin(),raw_data[row].end(),an);
                        if(!it){
                            flag = false;
                            break;
                        }
                    }
                    if(flag){
                        hashmap[name].push_back(row);
                    }
                }
            }
            else{
                for(int row:hashmap[ft_nd.name]){
                    bool it = std::binary_search(raw_data[row].begin(),raw_data[row].end(),c);
                    if(it){
                        hashmap[name].push_back(row);
                    }
                }
            }


            if(hashmap[name].size() >= support_num){

                Node[++ID].id = ID;
                Node[ID].father = ft_id;
                Node[ID].number = c;
                Node[ID].name = name;
                ft_nd.son.push_back(ID);
                Node[ID].generation = ft_nd.generation + 1;
                frequent[Node[ID].generation]++;
                Q.push(ID);
                total++;
            }
        }


    }
    clock_t ed=clock();
    std::cout<<(double)(ed-st)/(CLOCKS_PER_SEC) <<std::endl;
    candi[2] = frequent[1] * (frequent[1]-1) / 2;
    for(int i = 1; i <= 10; i++){
        if(candi[i] == 0)
            break;
        std::cout<<"C["<<i<<"]"<<" = "<<candi[i]<<"   ";
        std::cout<<"L["<<i<<"]"<<" = "<<frequent[i]<<std::endl;
    }
    std::cout  << total <<std::endl;
    std::cout  << x <<std::endl;

    return 0;
}
