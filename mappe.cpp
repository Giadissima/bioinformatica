#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

/* Creazione della classe nodo: raggruppa tutti i dati relativi al nodo */
class Nodo {
public:
    string etichetta, sequenza;
    Nodo(const string &id, const string &seq) : etichetta(id), sequenza(seq) {}
};

/* Creazione classe arco */
class Arco {
public:
    string direzione, destinazione;
    char overlap;
    Arco(const string &dir, const string &des, char ov) : direzione(dir), destinazione(des), overlap(ov) {}
};

/* Dichiarazioni vettore nodi e lista di adiacenze degli archi */
vector<Nodo> nodi;
using Graph = unordered_map<string, vector<Arco> >;  // Corretto il tipo Graph
// unordered_map<string, vector<Arco>> adj;  // Questa dichiarazione non è necessaria

// Prototipi funzioni
void lettura(const string &filepath, Graph &graph);
void stampa(const Graph &graph);
bool DFSric(Graph &graph, const string &nodo_partenza, unordered_set<string> &visitati, unordered_set<string> &inVisita);
bool DFS(Graph &graph);

/* Funzione che si occupa di leggere il file GFA */
void lettura(const string &filepath, Graph &graph) {
    ifstream file(filepath);
    if (!file) {
        cerr << "Impossibile aprire file" << endl;
        exit(-1);
    }

    string id, seq, linea;
    while (getline(file, linea)) {
        istringstream iss(linea);
        char tipo;
        iss >> tipo;

        /* Lettura dei nodi */
        if (tipo == 'S') {
            iss >> id >> seq;
            nodi.emplace_back(id, seq);

        /* Lettura degli archi */
        } else if (tipo == 'L') {
            string partenza, direzione, destinazione, _;
            char overlap;
            iss >> partenza >> direzione >> destinazione >> _ >> overlap;
            graph[partenza].emplace_back(direzione, destinazione, overlap);
        }
    }
}

/* Funzione che si occupa di stampare il grafo */
void stampa(const Graph &graph) {
    /* Stampa etichetta e sequenza dei nodi */
    for (const auto &nodo : nodi) {
        cout << "Nodo " << nodo.etichetta << " | Sequenza " << nodo.sequenza << endl;
    }
    cout << endl;

    /* Stampa archi connessi ai nodi */
    for (const auto &entry : graph) {
        const auto &nodo = entry.first;
        const auto &arco = entry.second;
        cout << "Nodo " << nodo << ":" << endl;

        /* Iterazione sugli archi */
        for (const auto &arcoItem : arco) {
            cout << "Arco " << arcoItem.direzione << " verso " << arcoItem.destinazione << " | Overlap " << arcoItem.overlap << endl;
        }
    }
}

/* Funzione che si occupa di implementare la DFS su un nodo di partenza */
bool DFSric(Graph &graph, const string &nodo_partenza, unordered_set<string> &visitati, unordered_set<string> &inVisita) {
    if (inVisita.count(nodo_partenza)) {
        cout << "Trovato arco all'indietro: " << nodo_partenza << endl;
        return true;
    }
    if (visitati.count(nodo_partenza)) {
        return false;
    }

    visitati.insert(nodo_partenza);
    inVisita.insert(nodo_partenza);

    auto it = graph.find(nodo_partenza);
    if (it != graph.end()) {
        auto &archi = it->second;
        for (auto edgeIt = archi.begin(); edgeIt != archi.end();) {
            auto &vicino = edgeIt->destinazione;
            if (inVisita.count(vicino)) {
//                cout << "Sto rimuovendo l'arco " << nodo_partenza << " -> " << vicino << endl;
                edgeIt = archi.erase(edgeIt); // Elimina l'elemento corrente e ritorna il prossimo
            } else {
                if (DFSric(graph, vicino, visitati, inVisita)) {
                    edgeIt = archi.erase(edgeIt); // Rimuovi l'arco se si trova un ciclo
                } else {
                    ++edgeIt; // Prosegui con il prossimo arco
                }
            }
        }
    }
    inVisita.erase(nodo_partenza);
    return false;
}

/* Funzione che si occupa di chiamare DFSric su tutti i nodi non visitati */
bool DFS(Graph &graph) {
    unordered_set<string> visitati;
    unordered_set<string> inVisita;

    /* Iterazione su tutti i nodi del grafo */
    for (const auto &nodo : nodi) {
        const auto &etichetta = nodo.etichetta;
        /* Richiama DFSric se il nodo non è stato visitato */
        if (!visitati.count(etichetta)) {
            if (DFSric(graph, etichetta, visitati, inVisita)) {
                return true;
            }
        }
    }
    
    /* Nessun ciclo trovato */
    return false;
}

int main() {
    Graph graph;
    lettura("DRB1-3123_unsorted.gfa", graph);
//    cout << "Grafo originario" << endl;
    // stampa(graph);  // Commentato per non stampare il grafo originale
    cout << endl;
    DFS(graph);
    cout << "Cicli trovati e back edges rimossi. Ecco il grafo aggiornato: " << endl << endl;
    stampa(graph);
    cout << endl;
    return 0;
}