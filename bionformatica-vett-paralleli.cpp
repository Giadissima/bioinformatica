/*
Il progetto riguarda la costruzione e l'analisi di un pangenome graph, che
permette di rappresentare e analizzare variazioni genetiche all'interno di un
insieme di genomi in bioinformatica. Utilizzando il formato GFA (Graphical Fragment
Assembly), il progetto richiede di leggere file GFA e creare un grafo etichettato,
nonché di eseguire diverse operazioni su di esso.

Obiettivi del Progetto

I. Lettura e Creazione del Grafo:
Leggere un file GFA e creare un grafo G etichettato e orientato, utilizzando le
liste di adiacenza (vector in C++).
Etichettare i nodi e gli archi di G in base alle informazioni fornite nel file GFA.

II. Analisi del Grafo:
Verificare se il grafo G sia ciclico: in tal caso effettuare una visita DFS e
rimuovere gli archi all'indietro (back edge) in modo che G diventi un DAG (directed
acyclic graph).
Considerare una sorgente s (grado d'ingresso a zero) a una destinazione t (grado
d'uscita a zero). Se ci sono più sorgenti o destinazioni, selezionarne una specifica
per l'analisi.

III. Stampa dei Cammini del DAG:
Eseguire una ricerca in profondità (DFS) nel DAG G a partire dalla sorgente s per
trovare tutti i cammini che partono da s e arrivano in t.
Attenzione che il numero di tali cammini può essere esponenziale, per cui verificare
questo solo su un DAG piccolo.
Ignorare l'array booleano “visitato” poiché il grafo è aciclico e usare un array S
di caratteri di appoggio: modificare la visita ricorsiva DFS vista a lezione, in
modo da aggiungere e rimuovere la stringa del nodo corrente u nella DFS quando,
rispettivamente, u viene visitato per la prima volta e quando la visita in u termina.
(Nota: quando u=t, la sequenza corrispondente al cammino attuale da s a t si trova in
S.) Stampare le sequenze suddette per i primi 42 cammini.

IV. Facoltativo: Ricerca di Pattern sui Cammini del DAG:
Modificare il codice del punto III in modo che, data una sequenza pattern P di
lunghezza K, verifichi se P è contenuta in una delle sequenze generate come sopra.
In tal caso, P è chiamata K-mer.
Volendo, utilizzare tecniche di rolling hash per calcolare l'hash delle porzioni di
lunghezza K nella sequenza (hash visto a lezione) e confrontarle con l'hash del
pattern P.

Struttura del GFA
Il formato GFA permette di rappresentare graficamente l'assemblaggio di frammenti
genomici. Ogni linea nel file GFA ha un campo specifico:
H (Header): La prima linea che indica il tipo di formato.
S (Segment): Linee che descrivono segmenti di DNA.
L (Link): Linee che descrivono collegamenti tra segmenti.
P (Path): Linee che descrivono cammini attraverso segmenti.
L (Walk): Linee che descrivono cammini attraverso segmenti non sovrapposti.
Utilizzare solo S per i nodi e L per gli archi.
Esempio di Analisi
Consideriamo un grafo con i seguenti nodi e archi rappresentati in formato GFA:

H VN:Z:1.0
S s11 GAT
S s12 T
S s13 A
S s14 CAG
S unused GAA
S s15 A
S s16 T
S s17 TA
L s11 + s12 + *
L s11 + s13 + *
L s12 + s14 + *
L s13 + s14 + *
L s14 + s15 + *
L s14 + s16 + *
L s15 + s17 + *
L s16 + s17 + *
P A s11+,s12+,s14+,s15+,s17+ ,,,
W sample 1 A 0 10 >s11>s12>s14>s15>s17
W sample 2 A 0 10 >s11>s13>s14>s16>s17

In questo grafo G, analizziamo i cammini possibili e verifichiamo se il pattern
P=TTCA è presente in una delle sequenze ottenute.*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
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

class Grafo {
public:
    vector<Nodo> nodi;
    vector<string> adiacenze_origini;
    vector<string> adiacenze_destinazioni;
    vector<string> adiacenze_overlap;
    string id;
    string sequenza;

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
        // Utilizzo di freopen per reindirizzare stdin al file
        FILE* file = freopen(filename.c_str(), "r", stdin);
        if (!file) {
            cerr << "Errore: Impossibile aprire il file " << filename << endl;
            return;
        }

        string linea;
        while (getline(cin, linea)) { // Leggi dalla stdin
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

        fclose(file); // Chiudi il file
    }

    void stampaGrafo()
    {
        if (nodi.size() == 0)
            cout << "non ci sono nodi" << endl; // TODO secondo modo: vettore max di nodi, ciclo e quando trovo una cella non vuota allora la stampo
        // TODO fare un azzeramento del vettore adiacenze. AGG: forse non ce ne è bisogno, provare con il metodo empty
        for (int i = 0; i < nodi.size(); i++) {
            cout << "Nodo " << nodi[i].id << " " << nodi[i].sequenza << " collegato a:" << endl;
            bool trovato = false;
            for (int j = 0; j < adiacenze_origini.size(); j++) {
                if(nodi[i].id==adiacenze_origini[j])
                {
                    cout<<adiacenze_destinazioni[j]<<" con overlap " << adiacenze_overlap[j]<<endl;
                    trovato=true;
                }
            }
            if (!trovato) {
                cout << "non ci sono nodi collegati a " << nodi[i].id << endl;
            }
            
        }
    }
};

int main()
{
    Grafo grafo;
    grafo.leggiGFA("file.gfa"); // Inserisci il nome del file GFA qui
    grafo.stampaGrafo(); // Stampa il grafo per verificare se è corretto
    return 0;
}