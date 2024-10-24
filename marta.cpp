// CLASSI
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// Variabili globali
string id, seq;
int s;


// Facciamo la classe NODO
class Nodo {
public:
    string etichetta;
    string sequenza;
    Nodo (string id, string seq)
    {
        etichetta=id;
        sequenza=seq;
    }
};

//classe arco con nodo di origine, di destinazione e overlap
class Arco {
public:
    string origine;
    string destinazione;
    string overlap;
    Arco (string ori, string des, string ov)
    {
        origine=ori;
        destinazione=des;
        overlap=ov;
    }
};


class Grafo{
public:
    vector<Nodo> nodi;
    vector<Arco> archi;
    
    void carica(string filename) {
        FILE *f = freopen(filename.c_str(), "r", stdin); //ci permette di leggere il file e di prendere il cin da quel file. Con filename lo aggiungo dopo, è un parametro. c_str() ce lo converte in array di char.
        
        if (f== NULL){
            cout<<"impossibile aprire il file"<<endl;
            exit(1);
        }
        
        string linea;
        
        while(getline(cin, linea)){
            istringstream iss(linea);
            string tipo;
            iss>>tipo;        //fa il cin del tipo dal nostro file, quello che trova per primo
            if(tipo == "S"){
                string id, seq; //se è di tipo nodo, allora prendiamo l'id e la sequenza
                iss>>id>>seq;
                nodi.push_back(Nodo(id, seq));
            }
            else if( tipo== "L") { //se è di tipo arco prende origine, destinazione e overlap con i vari orientamenti
                string origine, orientamento_p, destinazione, orientamento_d, overlap;
                iss>> origine>>orientamento_p>>destinazione>>orientamento_d>>overlap;
                archi.push_back(Arco(origine, destinazione, overlap));
            }
        }
        fclose(f);
    }
    
    // Funzione stampa
    void stampa(){
        for (int i = 0; i < nodi.size(); i++) {
            cout << nodi[i].etichetta << " " << nodi[i].sequenza << endl;
            for(int j=0; j<archi.size(); j++){
                if(nodi[i].etichetta == archi[j].origine){
                    cout<<archi[j].origine<<"->"<<archi[j].destinazione<<" con overlap: "<<archi[j].overlap<<endl;
                }
            }
            
        }
    }
    
    //funzione visitato
    bool is_visitato(vector<string> visitati, string nodo_dacercare){
        for(int i=0; i<visitati.size(); i++){
            if(visitati[i] == nodo_dacercare){
                return true; //ritorna true se lo trova tra il vettore trovato
            }
        }
        return false; //non c'è da nessuna parte il nodo_dacercare
    }
    
    //funzione per elimina arco (non lo eliminiamo veramente, ma ci diamo un contenuto nullo)
    void azzera_arco(int i){
        archi[i].origine="";
        archi[i].destinazione="";
        archi[i].overlap="";
    }
    
    
    //definizione di visita in profondità
    bool DFS_loop(string nodo_attuale, string nodo_partenza, vector<string>& visitati)
    {
        visitati.push_back(nodo_attuale); // Aggiungi il nodo attuale alla lista dei visitati
        
        for (int j = 0; j < archi.size(); j++) {
            // Controlla se ci sono archi uscenti dal nodo attuale
            if (archi[j].origine == nodo_attuale) {
                string prossimo_nodo = archi[j].destinazione; // Ottieni il nodo successivo
                
                // Se troviamo il nodo di partenza, abbiamo trovato un ciclo
                if (prossimo_nodo == nodo_partenza) {
                    azzera_arco(j); // Rimuovi l'arco del ciclo
                    visitati.pop_back();
                    return true; // Indica che un ciclo è stato trovato e rimosso
                }
                
                // Se il prossimo nodo non è stato visitato, continua la DFS
                if (!is_visitato(visitati, prossimo_nodo)) {
                    if (DFS_loop(prossimo_nodo, nodo_partenza, visitati)==true) {//faccio un altro ciclo if perchè in questo modo, appena trovo un nodo che mi ritorna true, mi si scatena una catena di true
                        visitati.pop_back();
                        return true; // così il risultato si propaga per tutti i nodi
                    }
                }
            }
        }
        visitati.pop_back();
        //è una funzione ricorsiva: ha richiamato se stessa
        return false; // Nessun ciclo trovato a partire dal nodo di partenza
    }
    
    /*La dfs_loop opera eliminando solo un arco. Questa funzione va richiamata finchè ritorna true (true=c'è stato un arco eliminato) e va ciclata ulteriormente per ogni nodo (con un for). Adesso però, la DFS scritta si concentra solo sul nodo di partenza. Scriviamo un'altra funzione che ci faccia analizzare ogni nodo (analizza grafo): quindi per ogni nodo (ciclo for) fai la DFS fin quando mi ritorna true (ciclo while). LA DFS TOGLIE UN ARCO ALLA VOLTA
     */
    
    void analizza_grafo(){
        cout<<"inizio ad analizzare grafobackedges e cicli" << endl; // Informativa sull'inizio dell'analisi
        // Finché la DFS ha rilevato un arco da rimuovere, lo rimuove e rieffettua il ciclo
        int numero_archi_rimossi=0;
        vector<string> visitati;
        for (int i = 0; i < nodi.size(); i++) {
            while (DFS_loop(nodi[i].etichetta, nodi[i].etichetta, visitati)) {
                cout << "Rimozione arco numero "<<numero_archi_rimossi+1<< " eseguita" << endl; // Informa sulla rimozio"<<endl;
                numero_archi_rimossi++;
            }
        }
    }
   
    
    //PUNTO 3
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
            for (int i = 0; i < archi.size(); i++) {
                if (archi[i].origine == nodo_attuale) {
                    DFS_cammini(archi[i].destinazione, nodo_destinazione, cammino_corrente, contatore_cammini);
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

};



int main(){
    Grafo g;//creo la "variabile" grafo, come int x
    g.carica("test1.gfa");  //funzione carica su grafo, per cambiare file cambia solo qui
    g.stampa();  // Chiamata alla funzione stampa
    g.analizza_grafo();
   g.trova_cammini("s1","s2");
    return 0;
}