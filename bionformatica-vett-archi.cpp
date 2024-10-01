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

    void aggiungiNodo(const string& id, const string& sequenza)
    {
        nodi.push_back(Nodo(id, sequenza));
    }

    void aggiungiArco(const string& origine, const string& destinazione, string& overlap)
    {
        adiacenze.push_back(Arco(origine, destinazione, overlap));
    }

    void leggiGFA(const string& filename)
    {
        // Utilizzo di freopen per reindirizzare stdin al file
        FILE* file = freopen(filename.c_str(), "r", stdin);
        if (!file) {
            cerr << "Errore: Impossibile aprire il file " << filename << endl;
            exit(1);
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
            cout << "non ci sono nodi" << endl;
        for (int i = 0; i < nodi.size(); i++) {
            cout << "Nodo " << nodi[i].id << " " << nodi[i].sequenza << " collegato a:" << endl;
            bool trovato = false;
            for (int j = 0; j < adiacenze.size(); j++) {
                if (nodi[i].id == adiacenze[j].origine) {
                    cout << adiacenze[j].destinazione << " con overlap " << adiacenze[j].overlap << endl;
                    trovato = true;
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