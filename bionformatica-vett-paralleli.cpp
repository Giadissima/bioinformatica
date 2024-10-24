#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// CLASSI
class Nodo {
public:
    string id; // Identificatore del nodo
    string sequenza; // Sequenza associata al nodo

    // Costruttore per inizializzare un nodo con un ID e una sequenza
    Nodo(string i, string s)
        : id(i)
        , sequenza(s)
    {
    }
};

// DEFINIZIONI GLOBALI
vector<Nodo> nodi; // Contiene tutti i nodi del grafo
vector<string> adiacenze_origini; // Nodi di partenza degli archi
vector<string> adiacenze_destinazioni; // Nodi di arrivo degli archi
vector<string> adiacenze_overlap; // ? L'overlap è una parte di una sequenza che si sovrappone a un'altra sequenza. Ad esempio, se hai due sequenze:
                                  // ? Sequenza A: ACGTGTA
                                  // ? Sequenza B: GTACG
                                  // ? In questo caso, le sequenze si sovrappongono nel segmento GTA

// CREAZIONE GRAFO
// Funzione per aggiungere un nodo al grafo
void aggiungiNodo(const string id, const string sequenza)
{
    nodi.push_back(Nodo(id, sequenza)); // Aggiungi un nuovo nodo alla lista di nodi
}

// Funzione per aggiungere un arco tra due nodi
void aggiungiArco(const string origine, const string destinazione, string overlap);
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
            string origine, _, destinazione, _, overlap;
            iss >> origine >> _ >> destinazione >> _ >> overlap; // Leggi i dettagli dell'arco
            aggiungiArco(origine, destinazione, overlap); // Aggiungi l'arco al grafo
        }
    }

    fclose(file); // Chiudi il file dopo aver finito di leggerlo
}

// ANALISI GRAFO
// Funzione per rimuovere un arco dato il suo indice
void cancella_arco(int i) // i = cella da rimuovere
{
    adiacenze_destinazioni.erase(adiacenze_destinazioni.begin() + i); // Rimuovi il nodo di destinazione
    adiacenze_origini.erase(adiacenze_origini.begin() + i); // Rimuovi il nodo di origine
    adiacenze_overlap.erase(adiacenze_overlap.begin() + i); // Rimuovi l'overlap associato
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
    bool risultato;
    visitati.push_back(nodo_attuale); // Aggiungi il nodo attuale alla lista dei visitati

    for (int j = 0; j < adiacenze_origini.size(); j++) {
        // Controlla se ci sono archi uscenti dal nodo attuale
        if (adiacenze_origini[j] == nodo_attuale) {
            string prossimo_nodo = adiacenze_destinazioni[j]; // Ottieni il nodo successivo

            // Se troviamo il nodo di partenza, abbiamo trovato un ciclo
            if (prossimo_nodo == nodo_partenza) {
                cancella_arco(j); // Rimuovi l'arco del ciclo
                visitati.pop_back();

                return true; // Indica che un ciclo è stato trovato e rimosso
            }

            // Se il prossimo nodo non è stato visitato, continua il DFS
            if (!is_visitato(visitati, prossimo_nodo)) {
                if (DFS_loop(prossimo_nodo, nodo_partenza, visitati)) {
                    visitati.pop_back();

                    return true; // Propaga il risultato verso l'alto se un ciclo è stato trovato
                }
            }
        }
    }
    visitati.pop_back();
    return false; // Nessun ciclo trovato a partire dal nodo attuale
}

// Funzione principale per analizzare il grafo
void analizza_grafo()
{
    cout << "Inizio a analizzare il grafo, rimuovendo back edges e cicli" << endl; // Informativa sull'inizio dell'analisi
    // Finché la DFS ha rilevato un arco da rimuovere, lo rimuove e rieffettua il ciclo
    int numarchirimossi = 0;
    vector<string> visitati;
    for (int i = 0; i < nodi.size(); i++) {
        while (DFS_loop(nodi[i].id, nodi[i].id, visitati)) {
            cout << "Rimozione arco n." << numarchirimossi + 1 << " eseguita" << endl; // Informa sulla rimozione dell'arco
            numarchirimossi++;
        }
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
        cout << endl
             << "Nodo " << nodi[i].id << " " << nodi[i].sequenza << " collegato a:" << endl;
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

// PUNTO 3
// Funzione per trovare tutti i cammini da s a t
void DFS_cammini(string nodo_attuale, string nodo_destinazione, vector<string>& cammino_corrente, int& contatore_cammini)
{
    // Aggiungi il nodo corrente al cammino
    cammino_corrente.push_back(nodo_attuale);

    // Se siamo arrivati alla destinazione, stampiamo il cammino
    if (nodo_attuale == nodo_destinazione) {
        contatore_cammini++;
        cout << "Cammino " << contatore_cammini << ": ";
        for (const string& nodo : cammino_corrente) {
            cout << nodo << " ";
        }
        cout << endl;
        // Se abbiamo stampato 42 cammini, ci fermiamo
        if (contatore_cammini >= 42) {
            cammino_corrente.pop_back();
            return;
        }
    } else {
        // Altrimenti continuiamo la DFS per i nodi adiacenti
        for (int i = 0; i < adiacenze_origini.size(); i++) {
            if (adiacenze_origini[i] == nodo_attuale) {
                DFS_cammini(adiacenze_destinazioni[i], nodo_destinazione, cammino_corrente, contatore_cammini);
                if (contatore_cammini >= 42) {
                    cammino_corrente.pop_back();
                    return;
                }
            }
        }
    }

    // Rimuoviamo il nodo corrente dal cammino quando torniamo indietro
    cammino_corrente.pop_back();
}

void trova_cammini(string sorgente, string destinazione)
{
    vector<string> cammino_corrente;
    int contatore_cammini = 0;
    DFS_cammini(sorgente, destinazione, cammino_corrente, contatore_cammini);
    cout << "ci sono " << contatore_cammini << " cammini" << endl;
}
// TODO una versione con i for diversi
// Funzione principale del programma
int main()
{
    leggiGFA("test2.gfa"); // Inserisci il nome del file GFA da analizzare
    analizza_grafo(); // Esegui l'analisi del grafo per rimuovere back edges e cicli
    stampaGrafo(); // Stampa il grafo risultante

    // TODO da tastiera
    string sorgente = "s1"; // Esempio di nodo sorgente
    string destinazione = "s10"; // Esempio di nodo destinazione
    trova_cammini(sorgente, destinazione);
    return 0; // Termina il programma
}


// Funzione per aggiungere un arco tra due nodi
void aggiungiArco(const string origine, const string destinazione, string overlap)
{
    adiacenze_origini.push_back(origine); // Aggiungi il nodo di origine
    adiacenze_destinazioni.push_back(destinazione); // Aggiungi il nodo di destinazione
    adiacenze_overlap.push_back(overlap); // Aggiungi l'overlap tra i nodi
}
