#include <bits/stdc++.h>

using namespace std;

#define MAX 2147483640

typedef long long int ll;
typedef pair<int, int> pint;
typedef pair<int, pint> piint;
typedef pair<ll, pint> plint;

class Edge {
public:
   int v, c;

   Edge(int v, int c) {
      this->v = v;
      this->c = c;
   }
};

class Graph {
public:
   int n;
   int* p;
   int* places;
   vector<Edge>* edges;

   Graph(int n) {
      this->n = n;
      p = new int[n];
      edges = new vector<Edge>[n];
      places = new int[n];
      for (int i = 0; i < n; i++)
         places[i] = i;
   }

   void addEdge(int u, int v, int c) {
      edges[u].push_back(Edge(v, c));
      edges[v].push_back(Edge(u, c));
   }
};

class Plan {
public:
   int type, day, family, city;

   Plan(int type, int day, int family, int city) {
      this->type = type;
      this->day = day;
      this->family = family;
      this->city = city;
   }

   void printPlan() {
      printf("%d %d %d %d\n", type, day, family + 1, city + 1);
   }
};


void allPairsShortestPath(ll **d, vector<int> ** path, Graph g) {
   int n = g.n;
   for (int i = 0; i < n; i++) {
      bool * seen = new bool[n];
      for (int j = 0; j < n; j++)
         seen[j] = false;
      int * parent = new int[n];
      priority_queue<plint, vector<plint>, greater<plint>> minHeap;
      minHeap.push(make_pair(0, make_pair(i, i)));
      while (!minHeap.empty()) {
         plint node = minHeap.top();
         minHeap.pop();
         int j = node.second.first;
         if (!seen[j]) {
            parent[j] = node.second.second;
            seen[j] = true;
            for (Edge e : g.edges[j]) {
               if (!seen[e.v])
                  minHeap.push(make_pair(node.first + e.c + g.p[e.v], make_pair(e.v, j)));
            }
            if (j >= i) {
               d[i][j] = node.first;
               d[j][i] = node.first - g.p[j] + g.p[i];
            }
         }
      }
      for (int j = i; j < n; j++) {
         vector<int> best_path;
         best_path.push_back(j);
         int index = parent[j];
         if (index != j) {
            while (parent[index] != index) {
               best_path.push_back(index);
               index = parent[index];
            }
            best_path.push_back(index);
         }
         for (int k = 1; k < best_path.size(); k++) {
            path[j][i].push_back(best_path[k]);
         }
         for (int k = best_path.size() - 2; k >= 0; k--) {
            path[i][j].push_back(best_path[k]);
         }
      }
   }
}


void findBestCost(Graph g) {
   int n = g.n;
   vector<Plan> plans1, plans2;
   ll cost = 0;
   int *places = g.places;
   ll **d = new ll*[n];
   for (int i = 0; i < n; i++) {
      d[i] = new ll[n];
   }
   vector<int> **path = new vector<int>*[n];
   for (int i = 0; i < n; i++) {
      path[i] = new vector<int>[n];
   }
   allPairsShortestPath(d, path, g);
//     for (int i = 0; i < n; i++) {
//        for (int j = 0; j < n; j++) {
//           cout << i << ", " << j << ": ";
//           for (int k : path[i][j]) {
//              cout << k << " - > ";
//           }
//           cout << "\n";
//        }
//     }
   int count = 0, day = 1;
   priority_queue<plint, vector<plint>, greater<plint>> heap;
   for (int u = 0; u < n; u++) {
      for (int v = 0; v < n; v++) {
         if (u == v) continue;
         ll pairCost = d[places[u]][v] + d[places[v]][v];
         if (path[places[u]][v].size() >= path[places[v]][v].size())
            pairCost += (path[places[u]][v].size() - path[places[v]][v].size()) * g.p[v];
         else
            pairCost += (path[places[v]][v].size() - path[places[u]][v].size()) * g.p[v];
         heap.push(make_pair(pairCost, make_pair(u, v)));
      }
   }
   int minPlace = 0;
   int minP = MAX;
   for (int i = 0; i < n; i++) {
      if (g.p[i] < minP) {
         minP = g.p[i];
         minPlace = i;
      }
   }
   vector<piint> *waitNodePairs = new vector<piint>[n];
   vector<piint> *endWaitNode = new vector<piint>[n];
   int *waitNode = new int[n];
   for (int i = 0; i < n; i++)
		waitNode[i] = 0;
   while (count < n * (n - 1)) {
      int max_day = 0;
      vector<plint> selected;
      vector<plint> toPush;
      vector<pint> toMove;
      bool *used = new bool[n];
      bool *usedInPlan = new bool[n];
      int *dest = new int[n];
      for (int i = 0; i < n; i++) {
         used[i] = false;
         usedInPlan[i] = false;
         dest[i] = -1;
      }
      while (!heap.empty()) {
         plint pair = heap.top();
         heap.pop();
         int u = pair.second.first, v = pair.second.second;
         if (!used[u] && !used[v]) {
            used[u] = true; used[v] = true;
            dest[u] = v; dest[v] = v;
            selected.push_back(pair);
         }
         else {
            if (used[v] && !used[u] && dest[v] == v) {
               toMove.push_back(make_pair(u, v));
               used[u] = true;
            } else {
               toPush.push_back(pair);
            }
         }
      }
      for (int i = 0; i < n; i++) {
         if (!used[i]) {
            waitNode[i]++;
         }
         else {
			if (waitNode[i] > 1)
				waitNodePairs[i].push_back(make_pair(places[i], make_pair(day - waitNode[i], waitNode[i])));
			waitNode[i] = 0;
		 }
	 }
      for (plint pair : selected) {
         int u = pair.second.first, v = pair.second.second;
         if (places[u] != v) {
            int newPlace = path[places[u]][v][0];
            plans1.push_back(Plan(1, day, u, newPlace));
            cost += d[places[u]][v] - d[newPlace][v] - g.p[newPlace];
            places[u] = newPlace;
         }
         cost += g.p[places[u]];
         if (places[v] != v) {
            int newPlace = path[places[v]][v][0];
            plans1.push_back(Plan(1, day, v, newPlace));
            cost += d[places[v]][v] - d[newPlace][v] - g.p[newPlace];
            places[v] = newPlace;
         }
         cost += g.p[places[v]];
         if (places[v] == v && places[u] == v && !usedInPlan[v] && !usedInPlan[u]) {
            count++;
            usedInPlan[u] = true;
            usedInPlan[v] = true;
            plans2.push_back(Plan(2, day, u, v));
         } else {
            ll pairCost = d[places[u]][v] + d[places[v]][v];
            if (path[places[u]][v].size() >= path[places[v]][v].size())
               pairCost += (path[places[u]][v].size() - path[places[v]][v].size()) * g.p[v];
            else
               pairCost += (path[places[v]][v].size() - path[places[u]][v].size()) * g.p[v];
            pair.first = pairCost;
            heap.push(pair);
         }
      }
      for (pint pair : toMove) {
         int v = pair.second;
         int u = pair.first;
         if (places[u] != v) {
            int newPlace = path[places[u]][v][0];
            plans1.push_back(Plan(1, day, u, newPlace));
            cost += d[places[u]][v] - d[newPlace][v] - g.p[newPlace];
            places[u] = newPlace;
         }
         cost += g.p[places[u]];
         if (places[v] == v && places[u] == v && !usedInPlan[v] && !usedInPlan[u]) {
            count++;
            usedInPlan[u] = true;
            usedInPlan[v] = true;
            plans2.push_back(Plan(2, day, u, v));
         } else {
            ll pairCost = d[places[u]][v] + d[places[v]][v];
            if (path[places[u]][v].size() >= path[places[v]][v].size())
               pairCost += (path[places[u]][v].size() - path[places[v]][v].size()) * g.p[v];
            else
               pairCost += (path[places[v]][v].size() - path[places[u]][v].size()) * g.p[v];
            heap.push(make_pair(pairCost, make_pair(u, v)));
         }
      }
      for (plint pair : toPush) {
         int u = pair.second.first, v = pair.second.second;
         if (places[v] == v && places[u] == v && !usedInPlan[v] && !usedInPlan[u]) {
            count++;
            usedInPlan[u] = true;
            usedInPlan[v] = true;
            plans2.push_back(Plan(2, day, u, v));
            if (waitNode[u] > 1)
                waitNodePairs[u].push_back(make_pair(places[u], make_pair(day - waitNode[u] + 1, waitNode[u] - 1)));
            waitNode[u] = 0;
            if (waitNode[v] > 1)
                waitNodePairs[v].push_back(make_pair(places[v], make_pair(day - waitNode[v] + 1, waitNode[v] - 1)));
            waitNode[v] = 0;
            continue;
         }
         ll pairCost = d[places[u]][v] + d[places[v]][v];
         if (path[places[u]][v].size() >= path[places[v]][v].size())
            pairCost += (path[places[u]][v].size() - path[places[v]][v].size()) * g.p[v];
         else
            pairCost += (path[places[v]][v].size() - path[places[u]][v].size()) * g.p[v];
         pair.first = pairCost;
         heap.push(pair);
      }
      if (count == n * (n - 1)) {
            for (int i = 0; i < n; i++) {
                if (waitNode[i] > 0)
                    endWaitNode[i].push_back(make_pair(places[i], make_pair(day - waitNode[i] + 1, waitNode[i])));
             }
      }
      day++;
   }

//   for (int i = 0; i < n; i++) {
//        cout << i << "\n";
//        for (piint wait : waitNodePairs[i]) {
//            cout << "\tplace, day, time : " << wait.first << ", " << wait.second.first << ", " << wait.second.second << "\n";
//        }
//   }
//
//   for (int i = 0; i < n; i++) {
//        cout << i << "\n";
//        for (piint wait : endWaitNode[i]) {
//            cout << "\tplace, day, time : " << wait.first << ", " << wait.second.first << ", " << wait.second.second << "\n";
//        }
//   }

   for (int i = 0; i < n; i++) {
        for (piint wait : waitNodePairs[i]) {
            int bestPlace = wait.first;
            int bestPathSize = 0;
            ll bestCost = (ll)g.p[bestPlace] * wait.second.second;
            for (int j = 0; j < n; j++) {
                int pathSize = path[wait.first][j].size() * 2;
                if (pathSize <= wait.second.second) {
                    ll loopCost = d[wait.first][j] + d[j][wait.first];
                    loopCost += (ll)g.p[j] * (wait.second.second - pathSize);
                    if (loopCost < bestCost) {
                        bestCost = loopCost;
                        bestPlace = j;
                        bestPathSize = pathSize;
                    }
                }
            }
            int currentDay = wait.second.first;
            for (int node : path[wait.first][bestPlace]) {
                plans1.push_back(Plan(1, currentDay, i, node));
                currentDay++;
            }
            currentDay += wait.second.second - bestPathSize;
            for (int node : path[bestPlace][wait.first]) {
                plans1.push_back(Plan(1, currentDay, i, node));
                currentDay++;
            }
        }
   }
   for (int i = 0; i < n; i++) {
        for (piint wait : endWaitNode[i]) {
            int bestPlace = wait.first;
            ll bestCost = g.p[bestPlace] * wait.second.second;
            int bestPathSize = 0;
            for (int j = 0; j < n; j++) {
                int pathSize = path[wait.first][j].size();
                if (pathSize <= wait.second.second) {
                    ll loopCost = d[wait.first][j];
                    loopCost += g.p[j] * (wait.second.second - pathSize);
                    if (loopCost < bestCost) {
                        bestCost = loopCost;
                        bestPlace = j;
                        bestPathSize = pathSize;
                    }
                }
            }
            int currentDay = wait.second.first;
            for (int node : path[wait.first][bestPlace]) {
                plans1.push_back(Plan(1, currentDay, i, node));
                currentDay++;
            }
        }
   }
   sort(plans1.begin( ), plans1.end( ), [ ]( const Plan& lhs, const Plan& rhs )
   {
      return lhs.day < rhs.day;
   });
   cout << (plans1.size() + plans2.size()) << endl;
   int planIndex1 = 0, planIndex2 = 0;
   for (int i = 1; i <= day; i++) {
      while(planIndex1 < plans1.size() && plans1[planIndex1].day <= i) {
         plans1[planIndex1].printPlan();
         planIndex1++;
      }
      while(planIndex2 < plans2.size() && plans2[planIndex2].day <= i) {
         plans2[planIndex2].printPlan();
         planIndex2++;
      }
   }
}


int main() {

   int n, m;
   scanf("%d %d", &n, &m);
   Graph g = Graph(n);
   for (int i = 0; i < n; i++)
      scanf("%d", (g.p + i));
   for (int i = 0; i < m; i++) {
      int u, v, c;
      scanf("%d %d %d", &u, &v, &c);
      g.addEdge(u - 1, v - 1, c);
   }

   findBestCost(g);

   return 0;
}
