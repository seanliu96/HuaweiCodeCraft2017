#include "deploy.h"
#include <stdio.h>
#include <deque>
#include <queue>
#include <vector>
#include <limits.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <cmath>
#include <cstring>
#include <ctime>
#include <utility>
#include <cmath>
using namespace std;

clock_t run_second, last_second = (90 - 1.1) * CLOCKS_PER_SEC;

Fuck fuck;

vector<int> server, best_server, now_server;

int add_node, del_node;
long long now_cost, best_cost;
int flag;

vector<ServerType> Server;	

int node_cost[MAX_V];
vector<vector<int> > node;
vector<int> flow;
bool is_server[MAX_V];

FlowCost flow_cost[10005];
int node_server_id[MAX_V];
int node_customer_id[MAX_V];

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    fuck.readtopo(topo, line_num);
    fuck.spfa();
    XJBS xjbs(fuck);
    int best_index = fuck.customer_num;
    fuck.kmeans(best_index, best_server);
    fuck.add_server(best_server);
    best_cost = fuck.costflow();

    now_cost = best_cost;
    now_server = best_server;
   	flag = 1;
    run_second = last_second * 0.7;
    while (clock() < run_second) {
    	server = now_server;
    	if (flag > 0) {
    		if (add_node == fuck.node_num) 
			{
				fuck.change();
                continue;
			}
			server.push_back(add_node);
			fuck.add_server(server);
            long long cost = fuck.costflow();
			if (cost < now_cost) {
				++flag;
                now_cost = cost;
                now_server = server;
                fuck.update(add_node, del_node, now_server);
				if (now_cost < best_cost) {
					best_cost = now_cost;
					best_server = now_server;
				}
			}
			else {
				fuck.change();
            }
    	}	
		else {
			//cout<<"3"<<endl;
			if (del_node == fuck.node_num) 
			{
                fuck.change();
			}
			for (int i = 0; i < server.size(); ++i) 
				if (server[i] == del_node) {
					server.erase(server.begin() + i);
				}
			fuck.add_server(server);
			//cout<<"4"<<endl;
            long long cost = fuck.costflow();
			//cout<<"5"<<endl;
			if (cost < now_cost) {
				--flag;
                now_cost = cost;
                now_server = server;
                fuck.update(add_node, del_node, now_server);
				if (now_cost < best_cost) {
					best_cost = now_cost;
					best_server = now_server;
				}
			}
            else {
                fuck.change();
            }
        }
        //cout << best_cost << '!'<<flag << endl;
    }


    xjbs.addone(best_server);
    xjbs.initial();
    run_second = last_second;
    while (clock() < run_second) {
        xjbs.run();
    }

    best_server = xjbs.get_best();

    fuck.add_server(best_server);
    //cout << fuck.costflow() << endl;
    fuck.print_flow(node, flow);
    int node_size = node.size();
    char * topo_file = new char[node_size * MAX_V * 5];
    topo_file[0] = '\0';
    char line[MAX_V * 5];
    char tmp[100];
    sprintf(line, "%d\n\n", node_size);
    strcat(topo_file, line);
    for (int i = 0; i < node_size; ++i) {
        line[0] = '\0';
        for (int j = 0; j < node[i].size(); ++j) {
            sprintf(tmp, "%d ", node[i][j]);
            strcat(line, tmp);
        }
        sprintf(tmp, "%d ", node_customer_id[node[i][node[i].size() - 1]]);
        strcat(line, tmp);
        sprintf(tmp, "%d ", flow[i]);
        strcat(line, tmp);
        sprintf(tmp, "%d\n", node_server_id[node[i][0]]);
        strcat(line, tmp);
        strcat(topo_file, line);
    }
    write_result(topo_file, filename);
    delete []topo_file;
}
template <class T>
inline void knuth_shuffle(vector<T> & v) {
    int i = v.size() - 1, j = 0;
    while (i >= 0) {
        j = rand() % (i + 1);
        swap(v[i], v[j]);
        --i;
    }
}

void Fuck::change() {
    //cout<<"1"<<endl;
    if (abs(flag) == 1) {
        int i = 0;
        for (; i < 50; ++i) {
            server = now_server;
            for (int i = 0; i < 3; ++i) {
                int k = rand() % (int)now_server.size(); 
                int u = server[k];
                int v;
                do {
                    v = rand() % fuck.node_num; 
                } while (fuck.d[v][u] > 20);
                server[k] = v;
            }
            sort(server.begin(), server.end());
            server.erase(unique(server.begin(),server.end()),server.end());
            fuck.add_server(server);
            //cout<<now_cost<<'?';
            //for (int  i = 0; i < server.size(); ++i) cout<<server[i]<<' ';
            long long cost = fuck.costflow();
            //cout<<cost<<endl;
            if (cost < best_cost) {
                best_cost = cost;
                best_server = server;
            }
            //cout<<num<<endl;
            if (cost < now_cost)
                break;
        }
        if (i == 50)
            run_second = 0;
        now_server = server;
        now_cost = cost;
        fuck.update(add_node, del_node, now_server);
        flag = 2;
    }
    else if (flag > 0) {
        flag = -1;
    }
    else {
        flag = 1;
    }
    //cout<<"2"<<endl;
}

void Fuck::readtopo(char * topo[MAX_EDGE_NUM], int line_num) { 
    int line = 0;
    int u, v, c, w;
    if (line < line_num)
        sscanf(topo[line], "%d %d %d", &node_num, &edge_num, &customer_num);
    s = node_num;
    t = s + 1;
    psz = 0;
    memset(e, 0, sizeof(e));
    graph.resize(node_num, vector<EdgeInfo>());
    d.resize(node_num, vector<int>(node_num, inf));
    line += 2;
    Max_flow = 0;
    while (true) {
        int len = strlen(topo[line]);
        if (len == 2) break;
        sscanf(topo[line], "%d %d %d", &u, &w, &c);
        Server.push_back(ServerType(w, c));
        Max_flow = max(Max_flow, w);
        ++line;
    }
    for (int i = 0; i <= Max_flow; ++i) {
        flow_cost[i].cost = inf;
        for (int j = 0; j < Server.size(); ++j) 
            if (Server[j].flow >= i && Server[j].cost < flow_cost[i].cost) {
                flow_cost[i].cost = Server[j].cost;
                flow_cost[i].id = j;
            }
    }
    ++line;
    while (true) {
        int len = strlen(topo[line]);
        if (len == 2) break;
        sscanf(topo[line], "%d %d", &u, &c);
        node_cost[u] = c;
        ++line;
    }
    ++line;
    for (int i = 0; i < edge_num; ++i, ++line) {
        sscanf(topo[line], "%d %d %d %d", &u, &v, &w, &c);
        graph[u].emplace_back(v, c);
        graph[v].emplace_back(u, c);
        add_edge(u, v, w, c);
        add_edge(v, u, w, c);
    }
    ++line;
    need_flow = 0;
    for (int i = 0; i < customer_num; ++i, ++line) {
        sscanf(topo[line], "%d %d %d", &u, &v, &w);
        node_customer_id[v] = u;
        customer_nodes.emplace_back(v, w);
        add_edge(v, t, w, 0);
        need_flow += w;
    }
    psz_tmp = psz;
}

void Fuck::spfa() {
    for(int s = 0; s < node_num; ++s) {
        d[s][s] = 0;
        deque<int> q;
        memset(vis,0,sizeof(vis));
        vis[s] = 1;
        q.push_back(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop_front();
            vis[u] = 0;
            for (unsigned int i = 0; i < graph[u].size(); ++i) {
                int v = graph[u][i].v;
                int dis =  d[s][u] + graph[u][i].c;
                if (dis < d[s][v])
                { 
                    d[s][v] = dis;
                    if (!vis[v]) {
                        vis[v] = 1;
                        if (q.size () && d[s][v] < d[s][q[0]])
                            q.push_front(v);
                        else
                            q.push_back(v);
                    }
                }
            }
        }
    }
}

void Fuck::kmeans(int k, vector<int> & clusters) {
    clusters.resize(k);
    memset(vis, -1, sizeof(vis));
    vector<vector<int> > kmean_node(k);
    int min_dist, min_index;
    knuth_shuffle(customer_nodes);
    for (int i = 0; i < k; ++i) {
        clusters[i] = customer_nodes[i].v;
    }
    while (1) {
        for (int i = 0; i < k; ++i)
            kmean_node[i].clear();
        bool update = 0;
        for (int i = 0; i < customer_num; ++i) {
            min_dist = inf;
            min_index = 0;
            for (int j = 0; j < k; ++j) {
                if(d[clusters[j]][customer_nodes[i].v] < min_dist) {
                    min_dist = d[clusters[j]][customer_nodes[i].v];
                    min_index = j;
                }
            }
            if (vis[i] != min_index) {
                update = 1;
                vis[i] = min_index;
            }
            kmean_node[vis[i]].push_back(i);
        }
        if (!update) 
            break;
        for (int j = 0; j < k; ++j) {
            min_dist = inf;
            min_index = 0;
            for (int l = 0; l < node_num; ++l) {
                int dist = 0;
                for (unsigned int i = 0; i < kmean_node[j].size(); ++i) {
                    dist += d[l][customer_nodes[kmean_node[j][i]].v];
                }
                if (dist < min_dist) {
                    min_index = l;
                    min_dist = dist;
                }
            }
            clusters[j] = min_index;
        }
    }
}

void Fuck::add_server(vector<int> & v) {
    if (psz != psz_tmp) {
        psz = psz_tmp;
        for (Edge *j = e[s]; j; j = j->next) {
            int x = j->t;
            e[x] = e[x]->next;
        }
        e[s] = 0;
        Edge *j = epool + psz;
        for (Edge *i = epool; i < j; ++i) {
            i->u = i->U;
            i->c = i->C;
        }
    }
    for (unsigned int i = 0; i < v.size(); ++i) {
        add_edge(s, v[i], Max_flow, 0);
    }
}

inline void Fuck::add_edge(int u, int v, int w, int c) {
    Edge *e1 = epool + psz++, *e2 = epool + psz++;
    *e1 = (Edge){v, w, c, w, c, e[u], e2}, e[u] = e1;
    *e2 = (Edge){u, 0, -c, 0, -c, e[v], e1}, e[v] = e2;
}

void Fuck::print_flow(vector<vector<int> > & node, vector<int> &flow) {
    node.clear();
    flow.clear();
    for (Edge *i = e[s]; i; i = i->next) {
        node_server_id[i->t] = flow_cost[i->U - i->u].id;
    }
    while (1) {
        vector<int> Tmp;
        int u = s;
        int S = inf;
        while (u != t) {
            bool flag=0;
            for (Edge *i = e[u]; i; i = i->next) {
                int v = i->t;
                if (i->U > i->u) {
                    S = min(S, i->U - i->u);
                    u = v;
                    flag = 1;
                    break;
                }
            }
            if (!flag) break;
        }
        if (u != t) break;
        u = s;
        flow.push_back(S);
        while (u != t) {
            for (Edge *i = e[u]; i; i = i->next) {
                int v = i->t;
                if (i->U > i->u) {
                    i->u += S;
                    u = v;
                    break;
                }
            }
            if (u != t) Tmp.push_back(u);
        }
        node.push_back(Tmp);
    }
}

int Fuck::aug(int u, int m) {
    if (u == t)
        return cost += (long long)dist * m, m;
    int d = m;
    vis[u] = 1;
    for (Edge *i = e[u]; i; i = i->next) {
        if (i->u && !i->c && !vis[i->t]) {
            int f = aug(i->t, min(d, i->u));
            i->u -= f;
            i->pair->u += f;
            d -= f;
            if (!d)
                return m;
        }
    }
    return m - d;
}

bool Fuck::modlabel() {
    deque <int> q;
    memset(vis , 0, sizeof(vis));
    memset(D, 0x3f , sizeof(D));
    q.push_back(s);
    D[s] = 0;
    vis[s] = 1;
    while (!q.empty ()) {
        int u = q.front ();
        q.pop_front ();
        for (Edge *i = e[u]; i; i = i->next) {
            int v = i->t;
            int dis = D[u] + i->c;
            if (i->u && dis < D[v]) {
                D[v] = dis;
                if (!vis[v]) {
                    vis[v] = 1;
                    if (q.size () && D[v] < D[q[0]])
                        q.push_front(v);
                    else
                        q.push_back(v);
                }
            }
        }
        vis[u] = 0;
    }
    for (Edge *i = epool; i < epool + psz; ++i) {
        i->c -= D[i->t] - D[i->pair->t];
    }
    dist += D[t];
    return D[t] < inf;
}

long long Fuck::costflow() {
    flow = dist = 0;
    cost = 0;
    while (modlabel()) {
        int tmpf;
        do {
            memset(vis , 0, sizeof(vis));
            tmpf = aug(s, INT_MAX);
            flow += tmpf;
        } while (tmpf);
    }
    for (Edge *i = e[s]; i; i = i->next) {
        cost += node_cost[i->t] + flow_cost[i->U - i->u].cost;
    }
    if (flow != need_flow)
        cost = infll;
    return cost;
}

void Fuck::update(int& add_node, int& del_node, vector<int> & v) {
    //cout<<"3"<<endl;
    add_node = node_num;
    del_node = node_num;
    long long Max = 0, Min = infll;
    memset(is_server, 0, sizeof(is_server));
    for (int i = 0; i < v.size(); ++i) {
        is_server[v[i]] = true;
    }
    for (int i = 0; i < node_num; ++i) {
        long long tol_flow = 0;
        for (Edge *j = e[i]; j; j = j->next) 
            if (j -> U > j -> u) {
                tol_flow += j->U - j->u;
            }
        if (is_server[i]) {
            if (tol_flow < Min) {
                Min = tol_flow;
                del_node = i;
            }
        }
        else {
            if (tol_flow > Max) {
                Max = tol_flow;
                add_node = i;
            }
        }
    }
    //cout<<"4"<<endl;
}



Particle::Particle(int length): v(length, 0), v_best(length, 0), vp(length, 0), cost_best(infll), cost(infll) {}

Particle::Particle(int length, vector<int> & vi, Fuck * & fuck): v(length, 0), v_best(length, 0), vp(length, 0) {
    int size = vi.size();
    for (int i = 0; i < size; ++i) {
        v_best[vi[i]] = v[vi[i]] = 1;
    }
    fuck->add_server(vi);
    cost = cost_best = fuck->costflow();
}
bool cmp(const Particle & p1, const Particle & p2) {
    return p1.cost == p2.cost ? p1.cost_best < p2.cost_best : p1.cost < p2.cost;
}

XJBS::XJBS(Fuck & fk) {
    fuck = &fk;
    l = fuck->node_num;
}

inline void XJBS::decode(vector<double> & v) {
    server.clear();
    for (int i = 0; i < l; ++i) {
        if (v[i] > 0.5)
            server.push_back(i);
    }
}

inline void XJBS::addone(vector<int> & v) {
    p.emplace_back(l, v, fuck);
}

inline vector<int> XJBS::get_best() {
    decode(gbest.v_best);
    return server;
}

inline void XJBS::GA_cross(Particle & s1, Particle & s2) {
    //clock_t t1 = clock();
    int r1 = rand() % l, r2 = rand() % l;
    if (r2 < r1)
        swap(r1, r2);
    while (r1 < r2) {
        swap(s1.v[r1], s2.v[r1]);
        ++r1;
    }
    //cout << "GA_cross:" << (double)(clock()- t1) / CLOCKS_PER_SEC << endl;
}


inline void XJBS::OBMA(Particle & s) {
    //clock_t t1 = clock();
    int r1, r2;
    do {
        r1 = rand() % l;
    } while (s.v[r1] < 0.5);
    do {
        r2 = rand() % l;
    } while (s.v[r2] > 0.5 || fuck->d[r1][r2] > 20);
    swap(s.v[r1], s.v[r2]);

    //cout << "OBMA:" << (double)(clock()- t1) / CLOCKS_PER_SEC << endl;
}

inline void XJBS::PSO_update(Particle & s) {
    //clock_t t1 = clock();
    for (int i = 0; i < l; ++i) {
        s.vp[i] = PSO_w * s.vp[i] + PSO_c1 * rand() / RAND_MAX * (s.v_best[i] - s.v[i]) + PSO_c2 * rand() / RAND_MAX * (gbest.v_best[i] - s.v[i]); 
        s.v[i] = (1 / (1 + exp(100*(0.5-(s.v[i] + s.vp[i])))));
    }
    //cout << "PSO:" << (double)(clock()- t1) / CLOCKS_PER_SEC << endl;
}

inline void XJBS::updateone(Particle & s) {
    decode(s.v);
    fuck->add_server(server);
    s.cost = fuck->costflow();
    if (s.cost < s.cost_best) {
        s.v_best = s.v;
        s.cost_best = s.cost;
        if (s.cost_best < gbest.cost_best) {
            gbest.v_best = s.v_best;
            gbest.cost_best = s.cost_best;
        }
    }
}

void XJBS::run() {
    int i = 0;
    int j = max_p_size - 1;
    sort(p.begin(), p.end(), cmp);
    for (; i < j; ++i, --j) {
        OBMA(p[i]);
        PSO_update(p[j]);
        //OBMA(p[j]);
        GA_cross(p[i], p[j]);
        updateone(p[i]);
        updateone(p[j]);
    }
    //cout << "run " << gbest.cost_best << endl;
}

void XJBS::initial() {
    PSO_c1 = c1;
    PSO_c2 = c2;
    PSO_w = w;
    gbest = p[0];
    max_p_size = 6;
    for (int i = 1; i < max_p_size; ++i) {
        p.push_back(p[0]);
    }
}





