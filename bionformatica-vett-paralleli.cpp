#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// CLASSI
class Nodo {
public:
    string id;
    string sequenza;
    Nodo(string i, string s)
        : id(i)
        , sequenza(s)
    {
    }
};

// DEFINIZIONI GLOBALI
vector<Nodo> nodi;
vector<string> adiacenze_origini;
vector<string> adiacenze_destinazioni;
vector<string> adiacenze_overlap;


// CREAZIONE GRAFO
void aggiungiNodo(const string& id, const string& sequenza)
{
    nodi.push_back(Nodo(id, sequenza));
}

void aggiungiArco(const string& origine, const string& destinazione, string& overlap)
{
    adiacenze_origini.push_back(origine);
    adiacenze_destinazioni.push_back(destinazione);
    adiacenze_overlap.push_back(overlap);
}

void leggiGFA(const string& filename)
{
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


// ANALISI GRAFO
void remove_arco(int i)
{
    adiacenze_destinazioni.erase(adiacenze_destinazioni.begin() + i);
    adiacenze_origini.erase(adiacenze_origini.begin() + i);
    adiacenze_overlap.erase(adiacenze_overlap.begin() + i);
}

bool remove_back_edge(string nodo_partenza, string nodo_destinazione)
{
    for (int j = 0; j < adiacenze_origini.size(); j++) {
        if (nodo_partenza == adiacenze_origini[j] && nodo_destinazione == adiacenze_destinazioni[j]) {
            remove_arco(j);
            return true;
        }
    }
    return false;
}

bool is_visitato(vector<string> visitati, string nodo_k)
{
    auto it = find(visitati.begin(), visitati.end(), nodo_k);
    if (it != visitati.end())
        return true;
    return false;
}

bool DFS_loop(string nodo_attuale, string nodo_partenza, vector<string>& visitati)
{
    visitati.push_back(nodo_attuale);

    for (int j = 0; j < adiacenze_origini.size(); j++) {
        if (adiacenze_origini[j] == nodo_attuale) {
            string prossimo_nodo = adiacenze_destinazioni[j];

            // Se troviamo il nodo di partenza, abbiamo trovato un ciclo
            if (prossimo_nodo == nodo_partenza) {
                remove_arco(j); // Rimuovi l'arco del ciclo
                return true;
            }

            // Se il prossimo nodo non è stato visitato, continua il DFS
            if (!is_visitato(visitati, prossimo_nodo)) {
                if (DFS_loop(prossimo_nodo, nodo_partenza, visitati)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool remove_loop(string nodo_partenza)
{
    vector<string> visitati;

    // Avvia DFS dal nodo di partenza
    return DFS_loop(nodo_partenza, nodo_partenza, visitati);
}

bool DFS()
{

    // se ho trovato un arco da rimuovere, rimuovo subito
    for (int i = 0; i < nodi.size(); i++) {
        for (int j = 0; j < adiacenze_origini.size(); j++) {
            if (nodi[i].id == adiacenze_origini[j]) {
                // caso in cui abbiamo trovato un arco che parte dal nodo oppure un arco che crea un ciclo
                if (remove_back_edge(adiacenze_destinazioni[j], nodi[i].id) || remove_loop(nodi[i].id))
                    return true;
            }
        }
    }
}

void analizza_grafo()
{
    cout << "Inizio a analizzare il grafo, rimuovendo back edges e cicli" << endl;
    // finché la dfs ha rilevato un arco da rimuovere, lo rimuove e rieffettuo il ciclo
    while (DFS()) {
        cout << "rimozione arco eseguita" << endl;
    }
}


// STAMPA GRAFO
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

int main()
{
    leggiGFA("file.gfa"); // Inserisci il nome del file GFA qui
    analizza_grafo(); // Esegui l'analisi del grafo
    stampaGrafo();
    return 0;
}
