#include<bits/stdc++.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
using namespace std;
vector<vector<int>> distances;
vector<vector<bool>> has_updated;
vector<vector<int>> adj_list;
vector<vector<char>> next_hop;
vector<char> routers;
map<char, int> router_index;
pthread_mutex_t mtx;
pthread_mutex_t display;
const int INF = 1e4;

// read data from filename given
void read_data(string file){
    ifstream fin(file);
    if(!fin.is_open()){
        cerr << "Error opening file" << endl;
        exit(1);
    }
    int n;
    fin >> n;
    distances.resize(n, vector<int>(n, INF));
    has_updated.resize(n, vector<bool>(n, false));
    next_hop.resize(n, vector<char>(n, '-'));
    routers.resize(n, '-');
    adj_list.resize(n, vector<int>());
    for(int i = 0; i < n; i++){
        fin >> routers[i];
        router_index[routers[i]] = i;
    }
    string node1;
    while (fin >> node1 && node1 != "END"){
        string node2;
        fin >> node2;
        int cost;
        fin >> cost;
        char router1 = node1[0];
        char router2 = node2[0];
        distances[router_index[router1]][router_index[router2]] = cost;
        distances[router_index[router2]][router_index[router1]] = cost;
        next_hop[router_index[router1]][router_index[router2]] = router2;
        next_hop[router_index[router2]][router_index[router1]] = router1;
        adj_list[router_index[router1]].push_back(router_index[router2]);
        adj_list[router_index[router2]].push_back(router_index[router1]);
    }
    fin.close();
    for(int i=0; i<n; i++){
        distances[i][i] = 0;
        next_hop[i][i] = routers[i];
    }
}

// display distance vectors
void show_distance_vectors(){
    cout << "Distance vectors:" << endl;
    cout << "  ";
    for(int i = 0; i < routers.size(); i++){
        cout << routers[i] << " ";
    }
    cout << endl;
    for(int i = 0; i < routers.size(); i++){
        cout << routers[i] << " ";
        for(int j = 0; j < routers.size(); j++){
            if(distances[i][j] == INF){
                cout << "∞ ";
            }
            else{
                cout << distances[i][j] << " ";
            }
        }
        cout << endl;
    }
}

// display next hop
void show_next_hop(){
    cout << "Next hop:" << endl;
    cout << "  ";
    for(int i = 0; i < routers.size(); i++){
        cout << routers[i] << " ";
    }
    cout << endl;
    for(int i = 0; i < routers.size(); i++){
        cout << routers[i] << " ";
        for(int j = 0; j < routers.size(); j++){
            cout << next_hop[i][j] << " ";
        }
        cout << endl;
    }
}

// check if the distance graph is connected
bool check_graph_connected(){
    int n = distances.size();
    vector<bool> visited(n, false);
    queue<int> q;
    q.push(0);
    visited[0] = true;
    int count = 1;
    while(!q.empty()){
        int u = q.front();
        q.pop();
        for(int v = 0; v < n; v++){
            if(distances[u][v] != INT_MAX && !visited[v]){
                visited[v] = true;
                q.push(v);
                count++;
            }
        }
    }
    return count == n;
}

// display routing table of a single router along with next hop
void show_routing_table(int router){
    cout << "@" << routers[router] << endl;
    cout << "Destination\t\tCost\t\tNext Hop" << endl;
    for (int i = 0; i < routers.size(); i++){
        if (distances[router][i]==INF)
            cout << routers[i] << "\t\t\t" << "∞ " << "\t\t" << next_hop[router][i] << endl;
        else if(has_updated[router][i])
            cout << routers[i] << "\t\t\t" << distances[router][i] << "*" << "\t\t" << next_hop[router][i] << endl;
        else
            cout << routers[i] << "\t\t\t" << distances[router][i] << "\t\t" << next_hop[router][i] << endl;
    }
    cout << endl;
}

// driver code for a router
void router(void* r){
    int router_index = *((int*)r);
    int iter = 0;
    while(true){
        iter++;
        pthread_mutex_lock(&display);
        cout << "Router: " << router_index << " iteration : " << iter << endl;
        show_routing_table(router_index);
        pthread_mutex_unlock(&display);
        // revert has_updated vector for this router to 'false'
        for(int i = 0; i < routers.size(); i++){
            has_updated[router_index][i] = false;
        }
        // get data from adjacent router's table
        pthread_mutex_lock(&mtx);
        int n = distances.size();
        bool updated = false;
        // access adj routers
        for (auto i : adj_list[router_index]){
            for(int j = 0; j < n; j++){
                if (distances[router_index][j] > distances[i][j] + distances[router_index][i]){
                    distances[router_index][j] = distances[i][j] + distances[router_index][i];
                    if(next_hop[router_index][i] != '-')
                        next_hop[router_index][j] = next_hop[router_index][i];
                    else
                        next_hop[router_index][j] = routers[i];
                    has_updated[router_index][j] = true;
                    updated = true;
                }
            }
        }
        sleep(2);
        pthread_mutex_unlock(&mtx);
        if(!updated){
            // routing table has converged
            break;
        }
    }
}

int main(int argc, char* argv[]){
    if(argc<2){
        cerr<<"Usage: ./dvr <input_file>"<<endl;
        return 0;
    }
    read_data(argv[1]);
    // show_distance_vectors();
    // show_next_hop();
    if (!check_graph_connected()){
        cerr << "The graph is not connected" << endl;
        return 0;
    }
    mtx = PTHREAD_MUTEX_INITIALIZER;
    display = PTHREAD_MUTEX_INITIALIZER;
    vector<pthread_t> threads(routers.size());
    for(int i = 0; i < routers.size(); i++){
        pthread_create(&threads[i], NULL, (void *(*)(void *))router, (void *)&i);
        sleep(1);
    }
    for(int i = 0; i < routers.size(); i++){
        pthread_join(threads[i], NULL);
    }
    cout<<"Routing table of all routers after convergence: "<<endl;
    for(int i = 0; i < routers.size(); i++){
        show_routing_table(i);
    }
    return 0;
}