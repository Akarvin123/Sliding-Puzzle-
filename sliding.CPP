#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
using namespace std;
map<vector<vector<int> >,bool> visited;
map<vector<vector<int> >, vector<vector<int> > > parent;
struct position{
    int x,y;
};
position ret_pos(int x,int y){
    position ret;
    ret.x = x;
    ret.y = y;
    return ret;
}
struct compare_pos{
    bool operator()(const position& a, const position& b) const {
        if(a.x == b.x) return a.y < b.y;
        return a.x < b.x;
    }
};
int calculate_distance(position p,position q){
    return (abs(p.x-q.x) + abs(p.y-q.y));
}
int manhattan_distance(vector<vector<int> > matrix,vector<vector<int> > target){
    int rows = matrix.size(), columns = matrix[0].size();
    map<int,position> required_position;
    for(int i = 0;i<rows;i++){
        for(int j = 0;j<columns;j++){
            required_position[target[i][j]] = ret_pos(i,j);
        }
    }
    int total_distance = 0;
    for(int i = 0;i<rows;i++){
        for(int j = 0;j<columns;j++){
            position pos = ret_pos(i,j);
            position target_pos = required_position[matrix[i][j]];
            total_distance += calculate_distance(pos,target_pos);
           
        }
    }
    return total_distance;
}
vector<vector<int> > move(vector<vector<int> > matrix, position initial, position final){
    swap(matrix[initial.x][initial.y],matrix[final.x][final.y]);
    return matrix;
}
position get_currentpos(vector<vector<int> > matrix){
    position pos;
    int rows = matrix.size(), columns = matrix[0].size();
    for(int i = 0;i<rows;i++){
        for(int j = 0;j<columns;j++){
            if(matrix[i][j]==0){
                pos =  ret_pos(i,j); break;
            }
        }
    }
    return pos;
}
int print(vector<vector<int> > matrix){
    for(int i = 0;i<matrix.size();i++){
        for(int j = 0;j<matrix[i].size();j++){
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}


int astar(vector<vector<int> > start, vector<vector<int> > target){
    set<pair<int, vector<vector<int> > > >  nodes;
   
    nodes.insert(make_pair(manhattan_distance(start,target),start));
    int distx[] = {0,0,1,-1}, disty[] = {1,-1,0,0};
    int rows = target.size(), columns = target[0].size();
    map<position,bool,compare_pos> allowed;
    for(int i = 0;i<rows;i++) for(int j = 0;j<columns;j++) allowed[ret_pos(i,j)] = true;
  
    if(manhattan_distance(start,target)==0) return 1;
    while(!nodes.empty()){
        while(visited[nodes.begin()->second]&&!nodes.empty()) nodes.erase(nodes.begin());
        vector<vector<int> > temp = nodes.begin()->second;
        position you = get_currentpos(temp);
        visited[temp] = true;
        for(int d = 0;d<4;d++){
            position change;
            change.x = you.x + distx[d];
            change.y = you.y + disty[d];
            if(!allowed[change]) continue;
            vector<vector<int> > new_move = move(temp,you,change);
            if(visited[new_move]) continue;
            nodes.insert(make_pair(manhattan_distance(new_move,target)+1,new_move));
            parent[new_move] = temp;
            if(manhattan_distance(new_move,target)==0){
                return 1;
            }
        }
    }

    return 0;
}
bool check(vector<vector<int> > a, vector<vector<int> > b){
    for(int i = 0;i<a.size();i++){
        for(int j = 0;j<a[i].size();j++){
            if(a[i][j]!=b[i][j]) return 0;
        }
    }
    return 1;
}
int print_all(vector<vector<int> > start, vector<vector<int> > child){
    if(check(start,child)==1) return 0;
  
    print_all(start,parent[child]);
    cout << endl;
    print(child);
    return 0;
}
int main(){
    vector< vector<int> > start, target;
    int n,m;
    cin >> n >> m;
    start.resize(n), target.resize(m);
    for(int i = 0;i<n;i++){
        start[i].resize(m);
        for(int j = 0;j<m;j++){
            cin >> start[i][j];
        }
    }
    for(int i = 0;i<n;i++){
        target[i].resize(m);
        for(int j = 0;j<m;j++){
            cin >> target[i][j];
        }
    }
    
   if(astar(start,target)==1){
       cout <<"FOUND!";
          print_all(start,target);
   }
   else cout <<"NOT FOUND!";
    return 0;
}
// Sample input 1 :
// 4 4
// 6 12 1 3
// 5 8 4 15
// 10 9 14 13
// 2 11 7 0
// 1 2 3 4 
// 5 6 7 8
// 9 10 11 12
// 13 14 15 0

// Sample Input 2 : 
// 3 3 
// 8 1 3
// 4 0 2
// 7 6 5
// 1 2 3 
// 4 5 6
// 7 8 0