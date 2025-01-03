#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
class Nodo {
public:
    string id;
    string sequenza;
    Nodo(string i, string s)
    {
        id = i;
        sequenza = s;
    }
};

class Arco {
public:
    string origine;
    string destinazione;
    string overlap;
    Arco(string ori, string d, string ov)
    {
        origine = ori;
        destinazione = d;
        overlap = ov;
    }
};
class Grafo {
public:
    vector<Nodo> nodi;
    vector<Arco> adiacenze;

    string id;
    string sequenza;

    void shiftArco(int index)
    {
        // Controlla che l'indice sia valido
        if (index < 0 || index >= adiacenze.size()) {
            cout << "Indice non valido!" << endl;
            return; // Uscita dalla funzione se l'indice non è valido
        }

        // Esegui il left shift a partire dall'indice specificato
        for (int i = index; i < adiacenze.size() - 1; i++) {
            adiacenze[i] = adiacenze[i + 1]; // Sposta l'elemento a sinistra
        }

        adiacenze.pop_back(); // Rimuove l'ultimo elemento
    }

    void azzeraArco(int i)
    {
        adiacenze[i].destinazione = ""; // Rimuovi il nodo di destinazione
        adiacenze[i].origine = ""; // Rimuovi il nodo di destinazione
        adiacenze[i].overlap = ""; // Rimuovi il nodo di destinazione
    }

    // Funzione per verificare se un nodo è già stato visitato
    bool is_visitato(vector<string>& visitati, string nodo_k)
    {
        auto it = find(visitati.begin(), visitati.end(), nodo_k); // Cerca il nodo nella lista dei visitati
        return it != visitati.end(); // Restituisce true se il nodo è stato visitato, altrimenti false
    }

    // Funzione per eseguire una DFS (ricerca in profondità) ricorsiva
    void DFS_loop(string nodo_attuale, string nodo_partenza, vector<string>& visitati)
    {
        visitati.push_back(nodo_attuale); // Aggiungi il nodo attuale alla lista dei visitati

        for (int j = 0; j < adiacenze.size(); j++) {
            // Controlla se ci sono archi uscenti dal nodo attuale
            if (adiacenze[j].origine == nodo_attuale) {
                string prossimo_nodo = adiacenze[j].destinazione; // Ottieni il nodo successivo

                // Se troviamo il nodo di partenza, abbiamo trovato un ciclo
                if (prossimo_nodo == nodo_partenza) {
                    // Aggiungi l'indice dell'arco da rimuovere
                    adiacenze.erase(adiacenze.begin()+j);
                cout << "rimozione arco n."<< j << endl;

                    j--;
                }

                // Se il prossimo nodo non è stato visitato, continua il DFS
                if (!is_visitato(visitati, prossimo_nodo)) {
                    DFS_loop(prossimo_nodo, nodo_partenza, visitati);
                }
            }
        }
        visitati.pop_back();
    }

    void analizzaGrafo()
    {

        cout << "Inizio a analizzare il grafo, rimuovendo back edges e cicli" << endl; // Informativa sull'inizio dell'analisi
        vector<string> visitati; // Lista per tenere traccia dei nodi visitati
        for(int i=0; i<nodi.size(); i++){
            DFS_loop(nodi[i].id, nodi[i].id, visitati);
        }

        // // Rimuovi tutti gli archi raccolti
        // for (int i = 0; i < adiacenze.size(); i++) {
        //     if (adiacenze[i].destinazione == "") {
        //         cout << "rimozione arco..." << endl;
        //         shiftArco(i);
        //         i--;
        //     }
        // }


    }

    void aggiungiNodo(const string& id, const string& sequenza)
    {
        nodi.push_back(Nodo(id, sequenza));
    }

    void aggiungiArco(const string& origine, const string& destinazione, string& overlap)
    {
        adiacenze.push_back(Arco(origine, destinazione, overlap));
    }

/*
    LeggiGFA legge un file di formato gfa e lo carica in un vettore
*/
    void leggiGFA(const string filename)
    {
        // Utilizzo di freopen per reindirizzare stdin al file
        ifstream file;
        file.open(filename);
        if (!file) {
            cerr << "Errore: Impossibile aprire il file " << filename << endl;
            exit(1);
        }
        string linea;
        while (getline(file, linea)) { // Leggi dalla stdin
            istringstream iss(linea);
            string tipo;
            iss >> tipo;

            if (tipo == "S") {
                string id, sequenza;
                iss >> id >> sequenza;
                aggiungiNodo(id, sequenza);
            } else if (tipo == "L") {
                string origine, _, destinazione, overlap;
                iss >> origine >> _ >> destinazione >> _ >> overlap;
                aggiungiArco(origine, destinazione, overlap);
            }
        }

        file.close(); // Chiudi il file
    }

    // definisco una variabile di tipo isstringstream per poter iterare sulla riga come se facessi un cin

    void stampaGrafo()
    {
        if (nodi.size() == 0)
            cout << "non ci sono nodi" << endl;
        for (int i = 0; i < nodi.size(); i++) {
            bool trovato = false;
            for (int j = 0; j < adiacenze.size(); j++) {
                if (nodi[i].id == adiacenze[j].origine) {
                    cout << nodi[i].id <<"->"<< adiacenze[j].destinazione << " " << adiacenze[j].overlap << endl;
                    trovato = true;
                }
            }
            if (!trovato) {
                cout << "non ci sono nodi collegati a " << nodi[i].id;
            }
            cout<<endl;
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
            for (string& nodo : cammino_corrente) {
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
            for (int i = 0; i < adiacenze.size(); i++) {
                if (adiacenze[i].origine == nodo_attuale) {
                    DFS_cammini(adiacenze[i].destinazione, nodo_destinazione, cammino_corrente, contatore_cammini);
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

// TODO rimuovere la funzione e dichiarare il contatore nel main
    void trova_cammini(string sorgente, string destinazione)
    {
        vector<string> cammino_corrente;
        int contatore_cammini = 0;
        DFS_cammini(sorgente, destinazione, cammino_corrente, contatore_cammini);
        cout << "ci sono " << contatore_cammini << " cammini" << endl;
    }
};


int main(int argc, char const *argv[])
{
    Grafo grafo;
    grafo.leggiGFA("test1.gfa"); // Inserisci il nome del file GFA qui
    grafo.analizzaGrafo();
    grafo.stampaGrafo(); // Stampa il grafo per verificare se è corretto
    
    // TODO da tastiera
    string sorgente = "s1"; // Esempio di nodo sorgente
    string destinazione = "s4"; // Esempio di nodo destinazione
    string s;

    grafo.trova_cammini(sorgente, destinazione);
    return 0;
}