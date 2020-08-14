#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <chrono> 
#include <climits>
using namespace std;
//visited - map : Takes care of all the expanded states
map<vector<vector<int> >,bool> visited;
//Parent - map; Stores the parent of each and every state, Used in ---> printing all the moves that led to the solution[solved state]
map<vector<vector<int> >, vector<vector<int> > > parent; 
//position of a cell in (x,y) coordinates
struct position{
    int x,y;
};
position ret_pos(int x,int y){
    position ret;
    ret.x = x;
    ret.y = y;
    return ret;
}
//modified comparator function for map<position,[any type]>
struct compare_pos{
    bool operator()(const position& a, const position& b) const {
        if(a.x == b.x) return a.y < b.y;
        return a.x < b.x;
    }
};
//Formula for calculating manhattan distance between two (x,y) co-ordinates
int manhattan_distance(position p,position q){
    return (abs(p.x-q.x) + abs(p.y-q.y));
}
//Sum Of manhattan_distance of all cells
//Manhattan_distance of a cell, is no. of moves it has to make to reach its goal state.
int calculate_distance(vector<vector<int> > matrix,vector<vector<int> > target){
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
            total_distance += manhattan_distance(pos,target_pos);
           
        }
    }
    return total_distance;
}
//Given initial and final position, move the "empty" cell from its initial position to final position
//Returns the modified matrix
vector<vector<int> > move(vector<vector<int> > matrix, position initial, position final){
    if(final.x==initial.x){
        if(final.y>initial.y){
             for(int i = initial.y;i<final.y;i++)
                swap(matrix[initial.x][i],matrix[initial.x][i+1]);
        }
        else{
            for(int i = initial.y;i>final.y;i--)    
             swap(matrix[initial.x][i],matrix[initial.x][i-1]);
        }
    }
    else{
        if(final.x>initial.x){
            for(int i = initial.x;i<final.x;i++)
                swap(matrix[i][initial.y],matrix[i+1][initial.y]);
        }
        else{
            for(int i = initial.x;i>final.x;i--)
                swap(matrix[i][initial.y],matrix[i-1][initial.y]);
        }
    }
    return matrix;
}
//Returns the position of the empty cell in a matrix;
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
//Prints the matrix
int print(vector<vector<int> > matrix){
    for(int i = 0;i<matrix.size();i++){
        for(int j = 0;j<matrix[i].size();j++){
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}

//A* Algorithm[Unsolved Puzzle, Solved instance of the Puzzle]
//Movement cost -> 1 to max(n,m); where n is no. of rows, and m is no. of columns.
//Estimated cost to arrive at the solved[goal] state -> manhattan distance
//total cost = movement cost + estimated cost;
int astar(vector<vector<int> > start, vector<vector<int> > target){
    //Sort the nodes by its manhattan distance, 
    set<pair<int, vector<vector<int> > > >  nodes;
    int rows = target.size(), columns = target[0].size();
    nodes.insert(make_pair(calculate_distance(start,target),start));
    vector<int> distx, disty;
    //distx = [0,0,-1,1,0,0,-2,2....] disty = [1,-1,0,0,-2,2,0,0];
    //Why are we using distx, disty? 
    //Explanation -> let the position of the empty block be (x,y)
    //For the sake of simplicity, assume the empty block can move only in 1 Dimension[Either Row or Column]
    //then, (x,y) can be -> (x+1,y) (x+2,y)...(n,y) (or) (x,y+1), (x,y+2) .... (x,m) (or) (x-1,y) (x-2,y) .... (0,y) (or) (x,y-1)..(x,0);
    // we observe that only one coordinate can change at a time, while the other coordinate remains constant. 
    //that is, new_x = x + distx[i]; new_y = y + disty[i]; if distx[i]!=0 then disty[i] = 0, vice versa
    for(int i = 1;i<max(rows,columns);i++){ //Fancier way of writing distx = [0,0,-1,1,0,0,-2,2....] disty = [1,-1,0,0,-2,2,0,0];
        for(int j = 0;j<4;j++){ 
            if(j<2){ distx.push_back(0); disty.push_back(i*(-1 + 2*(j%2==0)));}
            else{disty.push_back(0); distx.push_back(i*(-1 + 2*(j%2==0))); }
        }
    }
    //This map is not necessary, it is just used to check if 0<=new_x<n && 0<=new_y<m [Basically, reduces bunch of If and else statements]
    map<position,bool,compare_pos> allowed;
    for(int i = 0;i<rows;i++) for(int j = 0;j<columns;j++) allowed[ret_pos(i,j)] = true;

    //if start == target, that is, if the puzzle is already solved, we just return 1;
    if(calculate_distance(start,target)==0) return 1;
    while(!nodes.empty()){
       //nodes.begin()->first points to manhattan distance
       //nodes.begin()->second points to a matrix.
        vector<vector<int> > temp = nodes.begin()->second; //Gets the matrix with the smallest manhattan distance
        nodes.erase(nodes.begin()); //Since we are expanding the matrix, we will not require it again. 
       //Get the position of the empty cell 
        position you = get_currentpos(temp);
       //Mark the matrix[state] as visited, as you should not visit the same node/state again.
        visited[temp] = true;
       //store all the possible states 
        for(int d = 0;d<distx.size();d++){
            position change;
            change.x = you.x + distx[d];
            change.y = you.y + disty[d];
            // check if 0<=change.x<n && 0<=change.y<m
            if(!allowed[change]) continue; 
            // if it conforms to the constraints, we get the modified matrix;
            vector<vector<int> > new_move = move(temp,you,change);
            //check if the matrix has already been visited, if yes, do not insert, else insert the node/state into the set (manhattan_distance,matrix)
            if(visited[new_move]) continue;
            nodes.insert(make_pair(calculate_distance(new_move,target)+max(abs(distx[d]),abs(disty[d])),new_move));
            parent[new_move] = temp;
            //check if the new matrix/state is solved or not, if solved return 1;
            if(calculate_distance(new_move,target)==0){
                return 1;
            }
        }
    }
    //if solution is not found after exhausting all the possible options, return 0;
    return 0;
}
//check if two matrices are equal or not
//Note: Check can be replaced with manhattan_distance, as manhattan_distance of two equal matrices will be zero
bool check(vector<vector<int> > a, vector<vector<int> > b){
    for(int i = 0;i<a.size();i++){
        for(int j = 0;j<a[i].size();j++){
            if(a[i][j]!=b[i][j]) return 0;
        }
    }
    return 1;
}
//if we arrive at the solution, we print all the states that led to the solution, recursively.
int print_all(vector<vector<int> > start, vector<vector<int> > child){
    if(check(start,child)==1) return 0;
    print_all(start,parent[child]);
    cout << endl;
    print(child);
    return 0;
}
//This is to check running time of A* algorithm
int calculate_time(vector<vector<int> > start, vector<vector<int> > target){
    auto start_time = chrono::high_resolution_clock::now();
    astar(start,target);
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time-start_time).count();
    cout << duration;
    exit(0);
}
int main(){
    //Start - unsolved Puzzle
    //Target - solved instance of the Puzzle.
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
   // calculate_time(start,target);
    if(astar(start,target)==1){
        cout <<"FOUND!";
        print_all(start,target);
    }
    else cout <<"NOT FOUND!";
    return 0;
}
//Look ahead : 1 move
//Time Taken : 4x4 = 28~ seconds(Input 1), 4x4 = 0.4 seconds (Input 3), 3x3 = 0.008437 seconds(input 2)
//Look ahead : max (rows,columns) moves;
//Time Taken : 4x4 = 2.5~ seconds(Input 1), 4x4 = 0.4~ seconds (Input 3), 3x3 = 0.01 seconds(Input 2)
// Sample Input 1 :
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

// Sample Input 3 : 
// 4 4
// 1 2 6 3
// 4 9 5 7
// 8 13 11 15
// 12 14 0 10
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15

