
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

vector<Nodo> nodi;
vector<string> adiacenze_origini;
vector<string> adiacenze_destinazioni;
vector<string> adiacenze_overlap;


void aggiungiNodo(const string& id, const string& sequenza){ nodi.push_back(Nodo(id, sequenza)); }

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
    stampaGrafo(); // Stampa il grafo per verificare se è corretto
    return 0;
}