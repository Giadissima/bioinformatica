#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// CLASSI
class Nodo {
public:
    string id;          // Identificatore del nodo
    string sequenza;    // Sequenza associata al nodo

    // Costruttore per inizializzare un nodo con un ID e una sequenza
    Nodo(string i, string s)
        : id(i)
        , sequenza(s)
    {
    }
};

// DEFINIZIONI GLOBALI
vector<Nodo> nodi;                 // Contiene tutti i nodi del grafo
vector<string> adiacenze_origini;  // Nodi di partenza degli archi
vector<string> adiacenze_destinazioni; // Nodi di arrivo degli archi
vector<string> adiacenze_overlap;  // Overlap tra sequenze nei nodi collegati

// CREAZIONE GRAFO
// Funzione per aggiungere un nodo al grafo
void aggiungiNodo(const string id, const string sequenza)
{
    nodi.push_back(Nodo(id, sequenza)); // Aggiungi un nuovo nodo alla lista di nodi
}

// Funzione per aggiungere un arco tra due nodi
void aggiungiArco(const string origine, const string destinazione, string overlap)
{
    adiacenze_origini.push_back(origine);          // Aggiungi il nodo di origine
    adiacenze_destinazioni.push_back(destinazione); // Aggiungi il nodo di destinazione
    adiacenze_overlap.push_back(overlap);          // Aggiungi l'overlap tra i nodi
}

// Funzione per leggere un file GFA e costruire il grafo
void leggiGFA(const string& filename)
{
    // Apri il file specificato per la lettura
    FILE* file = freopen(filename.c_str(), "r", stdin);
    if (!file) {
        cerr << "Errore: Impossibile aprire il file " << filename << endl; // Messaggio di errore se il file non può essere aperto
        exit(1); // Termina il programma in caso di errore
    }

    string linea; // Variabile per memorizzare ogni riga letta dal file
    while (getline(cin, linea)) { // Leggi ogni riga dal file
        istringstream iss(linea); // Usa stringstream per analizzare la riga
        string tipo; // Tipo di riga (S per nodo, L per arco)
        iss >> tipo;

        if (tipo == "S") { // Se la riga rappresenta un nodo
            string id, sequenza;
            iss >> id >> sequenza; // Leggi ID e sequenza
            aggiungiNodo(id, sequenza); // Aggiungi il nodo al grafo
        } else if (tipo == "L") { // Se la riga rappresenta un arco
            string origine, orientamentoOrigine, destinazione, orientamentoDestinazione, overlap;
            iss >> origine >> orientamentoOrigine >> destinazione >> orientamentoDestinazione >> overlap; // Leggi i dettagli dell'arco
            aggiungiArco(origine, destinazione, overlap); // Aggiungi l'arco al grafo
        }
    }

    fclose(file); // Chiudi il file dopo aver finito di leggerlo
}

// ANALISI GRAFO
// Funzione per rimuovere un arco dato il suo indice
void remove_arco(int i)
{
    adiacenze_destinazioni.erase(adiacenze_destinazioni.begin() + i); // Rimuovi il nodo di destinazione
    adiacenze_origini.erase(adiacenze_origini.begin() + i); // Rimuovi il nodo di origine
    adiacenze_overlap.erase(adiacenze_overlap.begin() + i); // Rimuovi l'overlap associato
}

// Funzione per rimuovere un arco che rappresenta un back edge
bool remove_back_edge(string nodo_partenza, string nodo_destinazione)
{
    for (int j = 0; j < adiacenze_origini.size(); j++) {
        // Controlla se l'arco esiste nel grafo
        if (nodo_partenza == adiacenze_origini[j] && nodo_destinazione == adiacenze_destinazioni[j]) {
            remove_arco(j); // Rimuovi l'arco dal grafo
            return true; // Indica che un arco è stato rimosso
        }
    }
    return false; // Nessun arco trovato e rimosso
}

// Funzione per verificare se un nodo è già stato visitato
bool is_visitato(vector<string> visitati, string nodo_k)
{
    auto it = find(visitati.begin(), visitati.end(), nodo_k); // Cerca il nodo nella lista dei visitati
    return it != visitati.end(); // Restituisce true se il nodo è stato visitato, altrimenti false
}

// Funzione per eseguire una DFS (ricerca in profondità) ricorsiva
bool DFS_loop(string nodo_attuale, string nodo_partenza, vector<string>& visitati)
{
    visitati.push_back(nodo_attuale); // Aggiungi il nodo attuale alla lista dei visitati

    for (int j = 0; j < adiacenze_origini.size(); j++) {
        // Controlla se ci sono archi uscenti dal nodo attuale
        if (adiacenze_origini[j] == nodo_attuale) {
            string prossimo_nodo = adiacenze_destinazioni[j]; // Ottieni il nodo successivo

            // Se troviamo il nodo di partenza, abbiamo trovato un ciclo
            if (prossimo_nodo == nodo_partenza) {
                remove_arco(j); // Rimuovi l'arco del ciclo
                return true; // Indica che un ciclo è stato trovato e rimosso
            }

            // Se il prossimo nodo non è stato visitato, continua il DFS
            if (!is_visitato(visitati, prossimo_nodo)) {
                if (DFS_loop(prossimo_nodo, nodo_partenza, visitati)) {
                    return true; // Propaga il risultato verso l'alto se un ciclo è stato trovato
                }
            }
        }
    }

    return false; // Nessun ciclo trovato a partire dal nodo attuale
}

// Funzione per avviare la rimozione dei cicli
bool remove_loop(string nodo_partenza)
{
    vector<string> visitati; // Lista per tenere traccia dei nodi visitati

    // Avvia DFS dal nodo di partenza
    return DFS_loop(nodo_partenza, nodo_partenza, visitati); // Restituisce true se un ciclo è stato trovato
}

// Funzione per eseguire la ricerca in profondità su tutti i nodi
bool DFS()
{
    // Se ho trovato un arco da rimuovere, rimuovo subito
    for (int i = 0; i < nodi.size(); i++) {
        for (int j = 0; j < adiacenze_origini.size(); j++) {
            if (nodi[i].id == adiacenze_origini[j]) {
                // Controlla se un arco crea un back edge o un ciclo
                if (remove_back_edge(adiacenze_destinazioni[j], nodi[i].id) || remove_loop(nodi[i].id))
                    return true; // Indica che un arco è stato rimosso
            }
        }
    }
    return false; // Nessun arco da rimuovere trovato
}

// Funzione principale per analizzare il grafo
void analizza_grafo()
{
    cout << "Inizio a analizzare il grafo, rimuovendo back edges e cicli" << endl; // Informativa sull'inizio dell'analisi
    // Finché la DFS ha rilevato un arco da rimuovere, lo rimuove e rieffettua il ciclo
    while (DFS()) {
        cout << "Rimozione arco eseguita" << endl; // Informativa sulla rimozione dell'arco
    }
}

// STAMPA GRAFO
// Funzione per stampare il grafo e i suoi nodi
void stampaGrafo()
{
    if (nodi.size() == 0) // Controlla se non ci sono nodi
        cout << "Non ci sono nodi nel grafo." << endl; // Messaggio informativo
    for (int i = 0; i < nodi.size(); i++) {
        // Stampa informazioni su ogni nodo
        cout << "Nodo " << nodi[i].id << " " << nodi[i].sequenza << " collegato a:" << endl;
        bool trovato = false; // Flag per controllare se ci sono archi collegati
        for (int j = 0; j < adiacenze_origini.size(); j++) {
            // Controlla se il nodo attuale ha archi uscenti
            if (nodi[i].id == adiacenze_origini[j]) {
                // Stampa i dettagli dell'arco
                cout << adiacenze_destinazioni[j] << " con overlap " << adiacenze_overlap[j] << endl;
                trovato = true; // Marca che un arco è stato trovato
            }
        }
        if (!trovato) {
            cout << "Non ci sono nodi collegati a " << nodi[i].id << endl; // Messaggio se non ci sono collegamenti
        }
    }
}

// Funzione principale del programma
int main()
{
    leggiGFA("file.gfa"); // Inserisci il nome del file GFA da analizzare
    analizza_grafo(); // Esegui l'analisi del grafo per rimuovere back edges e cicli
    stampaGrafo(); // Stampa il grafo risultante
    return 0; // Termina il programma
}
