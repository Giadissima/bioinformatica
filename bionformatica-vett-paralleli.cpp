#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class Nodo {
public:
    string id;
    string sequenza;
    Nodo(string i, string s) : id(i), sequenza(s) {}
};

vector<Nodo> nodi;
vector<string> adiacenze_origini;
vector<string> adiacenze_destinazioni;
vector<string> adiacenze_overlap;

// Funzioni per aggiungere nodi e archi
void aggiungiNodo(const string& id, const string& sequenza) {
    nodi.push_back(Nodo(id, sequenza));
}

void aggiungiArco(const string& origine, const string& destinazione, string& overlap) {
    adiacenze_origini.push_back(origine);
    adiacenze_destinazioni.push_back(destinazione);
    adiacenze_overlap.push_back(overlap);
}

// Leggi il file GFA
void leggiGFA(const string& filename) {
    FILE* file = freopen(filename.c_str(), "r", stdin);
    if (!file) {
        cerr << "Errore: Impossibile aprire il file " << filename << endl;
        exit(1);
    }

    string linea;
    while (getline(cin, linea)) {
        istringstream iss(linea);
        string tipo;
        iss >> tipo;

        if (tipo == "S") {
            string id, sequenza;
            iss >> id >> sequenza;
            aggiungiNodo(id, sequenza);
        } else if (tipo == "L") {
            string origine, orientamentoOrigine, destinazione, orientamentoDestinazione, overlap;
            iss >> origine >> orientamentoOrigine >> destinazione >> orientamentoDestinazione >> overlap;
            aggiungiArco(origine, destinazione, overlap);
        }
    }

    fclose(file);
}

// Funzione per trovare l'indice di un nodo dato il suo id
int trovaIndiceNodo(const string& id) {
    for (int i = 0; i < nodi.size(); ++i) {
        if (nodi[i].id == id)
            return i;
    }
    return -1;
}

// DFS per rilevare e rimuovere i back edges
bool DFS(int nodo, vector<bool>& visitato, vector<bool>& inStack, vector<vector<int>>& adjList, vector<pair<int, int>>& archiDaRimuovere) {
    visitato[nodo] = true;
    inStack[nodo] = true;

    // Itera su tutti i vicini del nodo corrente
    for (int vicino : adjList[nodo]) {
        if (!visitato[vicino]) {
            // Esplora in profondità
            if (DFS(vicino, visitato, inStack, adjList, archiDaRimuovere))
                return true;
        } else if (inStack[vicino]) {
            // Back edge trovato (punta a un nodo nello stack)
            archiDaRimuovere.push_back({nodo, vicino});
        }
    }

    inStack[nodo] = false;
    return false;
}

// Rimuove gli archi all'indietro (back edges) e quelli che puntano a nodi precedenti
void rimuoviArchi(vector<pair<int, int>>& archiDaRimuovere, vector<vector<int>>& adjList) {
    for (auto arco : archiDaRimuovere) {
        int u = arco.first;
        int v = arco.second;
        // Trova l'arco u->v e rimuovilo
        for (auto it = adjList[u].begin(); it != adjList[u].end(); ++it) {
            if (*it == v) {
                adjList[u].erase(it);
                break;
            }
        }
    }
}

// Funzione per verificare sorgenti e destinazioni
void trovaSorgenteEDestinazione(const vector<vector<int>>& adjList, vector<int>& gradoEntrata, vector<int>& gradoUscita) {
    for (int i = 0; i < adjList.size(); ++i) {
        for (int vicino : adjList[i]) {
            gradoEntrata[vicino]++;
            gradoUscita[i]++;
        }
    }

    // Trova una sorgente e una destinazione
    int sorgente = -1, destinazione = -1;
    for (int i = 0; i < adjList.size(); ++i) {
        if (gradoEntrata[i] == 0) {
            sorgente = i;
        }
        if (gradoUscita[i] == 0) {
            destinazione = i;
        }
    }

    if (sorgente != -1 && destinazione != -1) {
        cout << "Sorgente: Nodo " << nodi[sorgente].id << endl;
        cout << "Destinazione: Nodo " << nodi[destinazione].id << endl;
    } else {
        cout << "Sorgente o destinazione non trovata." << endl;
    }
}

// Funzione principale di analisi del grafo
void analizzaGrafo() {
    // Costruisci la lista di adiacenza
    vector<vector<int>> adjList(nodi.size());
    for (int i = 0; i < adiacenze_origini.size(); ++i) {
        int u = trovaIndiceNodo(adiacenze_origini[i]);
        int v = trovaIndiceNodo(adiacenze_destinazioni[i]);
        if (u != -1 && v != -1) {
            adjList[u].push_back(v);
        }
    }

    // Inizia DFS per verificare i cicli e rimuovere archi all'indietro
    vector<bool> visitato(nodi.size(), false);
    vector<bool> inStack(nodi.size(), false);
    vector<pair<int, int>> archiDaRimuovere;

    for (int i = 0; i < nodi.size(); ++i) {
        if (!visitato[i]) {
            DFS(i, visitato, inStack, adjList, archiDaRimuovere);
        }
    }

    // Rimuovi gli archi all'indietro e quelli che puntano a nodi precedenti
    if (!archiDaRimuovere.empty()) {
        cout << "Rimuovendo archi all'indietro per ottenere un DAG..." << endl;
        for(int i=0; i<(archiDaRimuovere.size()); i++)
            cout<<"arco da rimuovere "<<archiDaRimuovere[i].first<<archiDaRimuovere[i].second<<endl;
        rimuoviArchi(archiDaRimuovere, adjList);
    } else {
        cout << "Il grafo è già un DAG." << endl;
    }

    // Trova sorgente e destinazione
    vector<int> gradoEntrata(nodi.size(), 0);
    vector<int> gradoUscita(nodi.size(), 0);
    trovaSorgenteEDestinazione(adjList, gradoEntrata, gradoUscita);
}

void stampaGrafo()
{
    if (nodi.size() == 0)
        cout << "non ci sono nodi" << endl;
    for (int i = 0; i < nodi.size(); i++) {
        cout << "Nodo " << nodi[i].id << " " << nodi[i].sequenza << " collegato a:" << endl;
        bool trovato = false;
        for (int j = 0; j < adiacenze_origini.size(); j++) {
            if (nodi[i].id == adiacenze_origini[j]) {
                cout << adiacenze_destinazioni[j] << " con overlap " << adiacenze_overlap[j] << endl;
                trovato = true;
            }
        }
        if (!trovato) {
            cout << "non ci sono nodi collegati a " << nodi[i].id << endl;
        }
    }
}

int main() {
    leggiGFA("file.gfa"); // Inserisci il nome del file GFA qui
    analizzaGrafo(); // Esegui l'analisi del grafo
    stampaGrafo();
    return 0;
}
