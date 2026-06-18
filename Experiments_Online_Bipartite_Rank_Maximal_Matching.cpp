#include <bits/stdc++.h>

using namespace std;

vector<vector<int>> adj; // Πίνακας γειτνίασης: adj[u][v] = 1 αν υπάρχει rank-1 ακμή
vector<vector<int>> adj_list;
// Αλγόριθμος Online Rank-Maximal Matching
int online_rank_maximal(vector<int> ordl, vector<int> y, vector<int> s) {
    int n = ordl.size();
    int r = 0;
    
    // used: Δείχνει αν μια offline θέση έχει καταληφθεί
    vector<int> used(2 * n + 1, 0); 

    vector<int> pos_y(2 * n + 1, 0);
    for (int i = 0; i < n; i++) {
        pos_y[y[i]] = i;
    }

    int fallback_idx = 0;

    // Οι υποψήφιοι καταφθάνουν online
    for (auto i : ordl) {
        int best_neighbor = -1;
        int min_rank = 1e9; // Αρχικοποίηση με "άπειρο"

        for (auto j : adj_list[i]) {
            if (!used[j]) { // Αν η offline θέση j είναι ακόμα ελεύθερη
                if (pos_y[j] < min_rank) {
                    min_rank = pos_y[j];
                    best_neighbor = j;
                }
            }
        }

        // 1. Στάδιο Rank-1
        if (best_neighbor != -1) {
            used[best_neighbor] = 1;
            r++; // Αυξάνουμε το πλήθος των rank-1 ταιριασμάτων
        } 
        // 2. Στάδιο Fallback
        else {
            while (fallback_idx < n && used[s[fallback_idx]]) {
                fallback_idx++;
            }
            if (fallback_idx < n) {
                used[s[fallback_idx]] = 1;
                fallback_idx++; 
            }
        }
    }
    return r; // Επιστρέφει μόνο το πλήθος των rank-1 ταιριασμάτων (το R της ανάλυσής σου)
}

// Υπολογισμός OPT_1
vector<int> p;
vector<int> vis;
bool try_match(int cur, int n) {
    if (vis[cur]) return false;
    vis[cur] = 1;
    for (int i = 1; i <= n; i++) {
        if (adj[cur][i + n] == 1) { 
            if (p[i] == -1 || try_match(p[i], n)) {
                p[i] = cur;
                return true;
            }
        }
    }
    return false;
}

int bipart(int n) {
    int r = 0;
    p.clear();
    p.resize(n + 1, -1);
    for (int i = 1; i <= n; i++) {
        vis.clear();
        vis.resize(n + 1, 0);
        if (try_match(i, n)) {
            r++;
        }
    }
    return r;
}


void read_graph(int n, int m){
	cin >> m;
	for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        adj[u][v + n] = 0;
        adj[v + n][u] = 0;
    }
}

void generate_random_bipartite_graph(int n, double p, unsigned int graph_seed) {
    
    int edge_count = 0;

    random_device rd;
    mt19937 gen(graph_seed);
    uniform_real_distribution<double> dis(0.0, 1.0);

	adj.assign(2 * n + 1, vector<int>(2 * n + 1, 0));
	adj_list.assign(2 * n + 1, vector<int>());

    for (int u = 1; u <= n; u++) {
        for (int v = 1; v <= n; v++) {
            if (dis(gen) < p) {
                adj[u][v + n] = 1;
                adj[v + n][u] = 1;
                edge_count++;

				adj_list[u].push_back(v + n);
				adj_list[v + n].push_back(u);
            }
        }
    }
	
}

void worst_ranking_family(int n){
    adj.assign(2 * n + 1, vector<int>(2 * n + 1, 0));
	adj_list.assign(2 * n + 1, vector<int>());
     for (int u = 1; u <= n; u++) {
        for (int v = 1; v <= n; v++) {
            if(u==v || (u>n/3 && u<=n/3*2 && v<=n/3) || (u>n/3*2 && v>n/3 && v<=n/3*2)){
                adj[u][v + n] = 1;
                adj[v + n][u] = 1;
                
                adj_list[u].push_back(v + n);
				adj_list[v + n].push_back(u);
            }
        }
    }
}

pair<double, double> calculate_clt_margin(const vector<double>& samples) {
    size_t N = samples.size();
    if (N < 30) return {0.0, 0.0};

    double sum = accumulate(samples.begin(), samples.end(), 0.0);
    double mean = sum / N;

    double variance_sum = 0.0;
    for (double val : samples) {
        variance_sum += (val - mean) * (val - mean);
    }
    double variance = variance_sum / (N - 1);
    double std_dev = sqrt(variance);
    double std_error = std_dev / sqrt(N);

    double z_value = 1.96; // 95% Confidence Level
    double margin_of_error = z_value * std_error;

    return {mean, margin_of_error};
}

int main() {
    // n: πλήθος κορυφών σε κάθε πλευρά
    int n = 200; 
	cin >> n;
    // runs: πλήθος διαφορετικών γραφημάτων
	int runs = 1000;
	random_device rd; 
	mt19937 p_gen(rd()); 
	uniform_real_distribution<double> p_dist(0.0, 5.0/n);

	double valworst = 1;
	pair<double, unsigned int> worst_found = {-1, -1};
    for(int i=0; i<runs; i++){
		// p: πιθανότητα ύπαρξης rank-1 ακμής (a,p)
		double p = p_dist(p_gen);
		
		unsigned int random_graph_seed = rd(); 
		
		
		generate_random_bipartite_graph(n, p, random_graph_seed);
		//read_graph(n,m);
		//worst_ranking_family(n);
		
		vector<int> ordl, y, s;
		for (int i = 1; i <= n; i++) {
			ordl.push_back(i);
			y.push_back(i + n);
			s.push_back(i + n);
		}

		double tot_r = 0;
		int iterations = 1000;
		
		vector<double> current_graph_samples;
		mt19937 g(rd());

        // Υπολογισμός του μέγιστου offline rank-1 ταιριάσματος
        double opt1 = bipart(n);

		// Εκτέλεση προσομοιώσεων με διαφορετικές τυχαίες μεταθέσεις
		for (int i = 0; i < iterations; i++) {
			shuffle(ordl.begin(), ordl.end(), g); // Τυχαία σειρά άφιξης online υποψηφίων
			shuffle(y.begin(), y.end(), g);       // Tυχαία μετάθεση προτεραιότητας y
			shuffle(s.begin(), s.end(), g);       // Tυχαία μετάθεση fallback s
			
			tot_r += online_rank_maximal(ordl, y, s);
            double r_exec = tot_r;
            current_graph_samples.push_back(r_exec / opt1);
		}

		
        
        // Υπολογισμός μέσου όρου και σφάλματος CLT για ΤΟ ΣΥΓΚΕΚΡΙΜΕΝΟ γράφημα
        pair<double, double> clt_stats = calculate_clt_margin(current_graph_samples);
        double avg_ratio = clt_stats.first;
        double margin = clt_stats.second;

        // Έλεγχος αν το σχετικό σφάλμα είναι κάτω από 5% (στατιστικά σημαντικό)
        double relative_error = (avg_ratio > 0) ? (margin / avg_ratio) * 100.0 : 0.0;
        if (relative_error >= 5.0) {
            cout << fixed << setprecision(5);
            cout << "[WARNING] Graph " << i << " (Seed: " << random_graph_seed << ", p: " << p 
                 << ") is NOT statistically significant! Margin: ±" << margin 
                 << " (" << relative_error << "% error)\n";
        }


		double avg_r = tot_r / iterations;
		double empirical_ratio = (opt1 > 0) ? (avg_r / opt1) : 0.0;
		

		if(opt1>0 && empirical_ratio< valworst){
			worst_found = {p, random_graph_seed};
			valworst = empirical_ratio;
		}
		
	}
    
	double theoretical_bound = (1.0 - 1.0 / exp(1.0)) * (double)(n - 1) / (2.0 * n);
    
	cout << "Worst Empirical Ratio: " << valworst << " | Theoretical Bound: " << theoretical_bound << endl;
    cout << "Worst Graph - Chosen p: " << worst_found.first << " | Seed: " << worst_found.second << endl;
	
}
