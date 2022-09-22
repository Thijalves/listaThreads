#include <iostream>
#include <vector>
#include <list>
#include<pthread.h>

#define INFINITY 1000

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void print(vector<int>& A){
    for( auto && a : A){
        cout<<a<<" ";
    }
    cout<<"\n";
}

//Disjoint sets (union find)
class Floresta{
    public:
        int n_arvores;
        vector<int> pai;
        vector<int> rank;
        vector<list<int>> arvore;

        void Floresta_init(int n_nodes){
            n_arvores = n_nodes;
            pai.resize(n_nodes);
            rank.resize(n_nodes);
            arvore.resize(n_nodes);
            for(int i =0; i < n_nodes; i++){
                pai[i] = i;
                rank[i]= 0;
                arvore[i].push_back(i);
            }
        }

        int Find(int node){
            if (pai[node] == node){
                return pai[node];
            }
            
            int raiz = Find(pai[node]);
            pai[node] = raiz;
            return raiz;
        }

        void Union(int node1, int node2){
            int raiz1 = Find(node1);
            int raiz2 = Find(node2);
            
            if (rank[raiz1] > rank[raiz2])
                swap(raiz1,raiz2);
            
            if (rank[raiz1] == rank[raiz2])
                rank[raiz2]++;

            arvore[raiz2].splice(arvore[raiz2].end(), 
                                      arvore[raiz1]);
        
            pai[raiz1] = pai[raiz2];
            this->n_arvores -= 1;
        }     
};

struct aresta{
    int node1;
    int node2;
    int peso;
    aresta(int p){
        peso = p;
    }
    aresta(int n1, int n2, int p){
        node1 = n1;
        node2 = n2;
        peso = p;
    }
};

Floresta f; //criar conjunto de arvores (floresta)
list<int> arvore;
vector<vector<pair<int,int>>> grafo;
vector<aresta> best_edges;

void* find_best_edge(void* args){
    int index  = *((int*) args);

    struct aresta best_edge(INFINITY);

    if(f.pai[index] != index)
        pthread_exit(NULL);
    
    // para cada node na arvore
    for(auto node : f.arvore[index]){
    //olha todas arestas ligadas a este node
        for (auto a : grafo[node]){
            int vizinho = a.first;
            int peso_node_vizinho = a.second;
            
            //apenas arestas que ligam a arvore a outra
            if (f.Find(vizinho) == f.Find(node)) continue;

            if(best_edge.peso > peso_node_vizinho){
                best_edge = aresta(node,vizinho,peso_node_vizinho);
            };
            pthread_mutex_lock(&mutex);
            best_edges.push_back(best_edge);
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}

void find_best_edges(vector<aresta> &mst){
    int n_nodes = grafo.size();
    for (int node = 0; node < n_nodes; node++){

        pthread_t threads[n_nodes];
        for(int i = 0; i < n_nodes; i++){
            pthread_create(&threads[i], NULL, find_best_edge, (void *)&i);
        }

        for(int i = 0; i < n_nodes; i++){
            pthread_join(threads[i], NULL);
        }
        
    }  
}

vector <aresta> boruvka(){
    vector <aresta> mst;
     
    int n_nodes = grafo.size();

    //enquanto numero de arvores  > 1:
    while (f.n_arvores >  1){
        //achar melhor edge para cada arvore
        find_best_edges(mst);
        
        //commit das arestas e uniao das arvores:
        for(auto best_edge : best_edges){
            
            int n1 = best_edge.node1;
            int n2 = best_edge.node2;
            
            if(f.Find(n1) != f.Find(n2)){
                mst.push_back(best_edge);
                f.Union(n1,n2);
            }
        }
    }
    return mst;
}

int main() {

    grafo = {
                                            {make_pair(1,2),make_pair(3,6)},  //0
                                            {make_pair(0,2),make_pair(2,3)},//1
                                            {make_pair(1,3),make_pair(3,8),make_pair(4,7)},//2
                                            {make_pair(0,6),make_pair(2,8),make_pair(4,9)},//3
                                            {make_pair(3,9),make_pair(2,7)}//4
                                          };

    
    f.Floresta_init(grafo.size());

    vector<aresta> mst = boruvka();

    pthread_t * threads = NULL;
    
    for (auto a: mst)
        cout<< a.node1<<" - "<<a.node2<< " " << a.peso<< endl;
        
    
    return 0;
}