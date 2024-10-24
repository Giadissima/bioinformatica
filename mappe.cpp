#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;

/* Creazione della classe nodo: raggruppa tutti i dati relativi al nodo */
class Nodo {
public:
    
    /* Attributi */
    string etichetta;
    string sequenza;
    
    /*
     Questo costruttore è una funzione speciale che viene chiamata automaticamente quando viene creato un oggetto della classe Nodo e lo inizializza affinché sia in uno stato valido
     */
    Nodo(string id, string seq){
        etichetta = id;
        sequenza = seq;
    }
};

//Vettore di nodi
vector<Nodo> nodi;

//Mappa che associa ogni nodo ad una lista di coppie con destinazioni e overlap, ovvero nodo->[(destinazione, overlap)], le quali rappresentano gli archi uscenti dal nodo
unordered_map<string, vector<pair<string, string>>> adj;

//Mappa che tiene traccia se un nodo è stato visitato o meno durante la DFS
unordered_map<string, bool> visitato;

/*
 Funzione per leggere il grafo dal file GFA
 */
void leggiGrafo(const string& filepath) {
    FILE *f = freopen(filepath.c_str(), "r", stdin);
    if (!f) {
        cerr << "Impossibile aprire il file " << filepath << endl;
        exit(-1);
    }
    string linea;
    while (getline(cin, linea)) {
        istringstream iss(linea);
        char tipo;
        iss >> tipo;

        if (tipo == 'S') {
            string id, seq;
            iss >> id >> seq;
            nodi.push_back(Nodo(id, seq));
            
            //Inizializzo il nodo come non visitato
            visitato[id] = false;
            
        } else if (tipo == 'L') {
            string partenza, destinazione, overlap, _;
            iss >> partenza >> _ >> destinazione >> _ >> overlap;
            
            //Memorizzazione dell'arco nella lista di adiacenza di partenza con destinazione e overlap
            adj[partenza].emplace_back(destinazione, overlap);
        }
    }
}

/*
 Funzione per stampare il grafo
 */
void stampaGrafo() {
    cout << "Liste di adiacenza:" << endl;
    
    //Iterazione su ogni nodo
    for (int i = 0; i < nodi.size(); i++) {
        auto & nodo = nodi[i].etichetta;
        
        //Stampa il nodo corrente
        cout << "S"<< nodo << ": ";
        
        //Iterazione sulla lista di adiacenza del nodo corrente
        for (int j = 0; j < adj[nodo].size(); j++) {
            
            //Stampa il nodo destinazione corrispondente
            cout << "S"<< adj[nodo][j].first << " ";
        }
        cout << endl;
    }
}

/*
 Funzione DFS ricorsiva
 */

//u rappresenta il nodo corrente su cui sto eseguendo la ricerca
void DFS_ric(const string& u) {
    
    //Contrassegno il nodo u come visitato così ogni nodo verrà esplorato una sola volta
    visitato[u] = true;
    
    //Iterazione attraverso tutti i nodi adiacenti al nodo u
    for (int i = 0; i < adj[u].size(); i++) {
        auto & v = adj[u][i];
        //Stampa dell'arco
        cout << "(" << u << "," << v.first << ") ";
        
        //Verifico che il nodo adiacente non sia già stato visitato e in caso chiamo la DFS su v.first
        if (!visitato[v.first]) {
            DFS_ric(v.first);
        }
    }
}

/*
 Funzione per eseguire DFS su tutto il grafo
 */
void DFS() {
    cout << "DFS tree:" << endl;
    
    //Iterazione su tutti i nodi presenti nel vettore nodi
    for (int i = 0; i < nodi.size(); i++) {
        string s = nodi[i].etichetta;
        
        //Controllo dei nodi visitati
        if (!visitato[s]) {
            DFS_ric(s);
        }
    }
}

/*
 Funzione per rimuovere back edges
 */
bool rimuoviBackEdges(const string& nodo_partenza, const string& nodo_attuale) {
    
    //Contrassegno il nodo come visitato
    visitato[nodo_attuale] = true;
    
    //Iterazione sui nodi adiacenti al nodo attuale, contenuti in adj[nodo_attuale]
    for (int i = 0; i < adj[nodo_attuale].size(); i++) {
        auto & v = adj[nodo_attuale][i];
        
        //Se il nodo adiacente è il nodo di partenza abbiamo trovato un ciclo
        if (v.first == nodo_partenza) {
            cout << "Ciclo trovato: rimuovo arco (" << nodo_attuale << ", " << v.first << ")" << endl;
            
            //Rimozione dell'arco dalla lista di adiacenza di nodo_attuale controllando che la destinazione dell'arco sia uguale al nodo adiacente attuale
            adj[nodo_attuale].erase(remove_if(adj[nodo_attuale].begin(), adj[nodo_attuale].end(),
                [&](const pair<string, string>& p) { return p.first == v.first; }),
                adj[nodo_attuale].end());
            
            //Se un ciclo è stato trovato e rimosso ritorna true
            return true;
            
            //Se il nodo adiacente non è stato visitato
        } else if (!visitato[v.first]) {
            
            //La funzione è richiamata ricorsivamente su di esso
            if (rimuoviBackEdges(nodo_partenza, v.first)) {
                
                //Se la chiamata ricorsiva trova un ciclo ritorna true
                return true;
            }
        }
    }
    
    //Se non trovo un ciclo esaminando tutti i nodi adiacenti ritorna false
    return false;
}

// Funzione per cercare cammini dal nodo sorgente s al nodo destinazione t
void cercaCammini(const string & sorgente, const string & destinazione, vector<string> & cammino_incorso, int & num_cammini) {
    
    //Aggiungo il nodo sorgente al cammino attuale
    cammino_incorso.push_back(sorgente);
    
    //Contrasseggno il nodo sorgente come visitato
    visitato[sorgente] = true;
    
    //Se ho raggiunto il nodo destinazione
    if (sorgente == destinazione) {
        num_cammini++;
        //Limitazione 42 cammini
        if (num_cammini <= 42){
            cout << "Cammino n°" << num_cammini << ": ";
            for (int i = 0; i < cammino_incorso.size(); i++) {
            cout << cammino_incorso[i];
            if (i!=cammino_incorso.size()-1)
            cout << "->";
            }
            cout << endl;
        } else {
            cammino_incorso.pop_back();
            cout << "Sono stati trovati più di 42 cammini" << endl;
            return;
        }
        
        //Se non ho raggiunto il nodo destinazione
    } else {
        
        //Esplora i nodi adiacenti
        for (int i = 0; i < adj[sorgente].size(); i++){
            
            //Ottengo il nodo vicino
            auto & vicino = adj[sorgente][i];
            
            //Se il vicino non è stato visitato continua la dfs
            if (!visitato[vicino.first]) {
                
                //Tolgo il nodo sorgente
                visitato[sorgente] = false;
                
                cercaCammini(vicino.first, destinazione, cammino_incorso, num_cammini); // Ricorsione sul nodo adiacente
                //Stampa i cammini
                if (num_cammini > 42){
                    cammino_incorso.pop_back();
                    return;
                }
            }
        }
    }
    
    //Ripristina il cammino attuale per altre ricerche
    cammino_incorso.pop_back();
    
    //Ripristina il nodo attuale per altre ricerche
    visitato[sorgente] = false;
}

// Funzione principale
int main() {
    leggiGrafo("test1.gfa");
    stampaGrafo();
   DFS();
    
    //Rimuovi gli archi di retrocessione
   for (int i = 0; i < nodi.size(); i++) {
       auto & nodo = nodi[i].etichetta;
       if (!visitato[nodo]) {
           rimuoviBackEdges(nodo, nodo);
       }
   }
    cout << endl;
   vector<string> cammino_incorso;
   int num_cammini;
   cercaCammini("s1", "s5", cammino_incorso, num_cammini);
    return 0;
}