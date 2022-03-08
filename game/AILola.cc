#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Lola


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  
  vector<Dir> dirs = {Down, Up, Right, Left};
  
  
  // Funcio BFS que rep un ciutada com a parametre i a partir del qual mirarem tots els objectes
  // que quedin mes a prop. Per no perdre temps nomes fara un recorregut bfs fins a una certa distancia
  // per evitar fer recorreguts a través del tauler que farien perdre temps.
  
  Dir BFS_buscar_objectes(const Citizen& c) {
    
    Pos p = c.pos;
    queue<Pos> cua_pos;               // cua de parell posicio i j
    
    vector<vector<int> > Tauler(board_rows(), vector<int>(board_cols()));
    vector<vector<bool> > visitat(board_rows(), vector<bool>(board_cols(), false));
    
    Tauler[p.i][p.j] = 0;               // a la posicio inicial i, j li donem valor 0 per poder trobar-la despres
    visitat[p.i][p.j] = true;
    
    cua_pos.push(p);
    bool trobat = false;
    
    while (not cua_pos.empty() and not trobat) {
        p = cua_pos.front();
        cua_pos.pop();
        
        if (cell(p).bonus != NoBonus or (c.type == Warrior and cell(p).weapon != NoWeapon)) {
            trobat = true;
            
            Dir ant = Down;
            
            while (Tauler[p.i][p.j] != 0) {         // busco la posicio des d'on he sortit
                if (Tauler[p.i][p.j] == 1) ant = Up;
                else if (Tauler[p.i][p.j] == 2) ant = Down; 
                else if (Tauler[p.i][p.j] == 3) ant = Left; 
                else if (Tauler[p.i][p.j] == 4) ant = Right; 
                
                p = p+ant;          // vaig a la posicio anterior
                
                if (Tauler[p.i][p.j] == 0) {
                    if (ant == Up) return Down;
                    else if (ant == Down) return Up;
                    else if (ant == Left) return Right;
                    else if (ant == Right) return Left;
                }
            }
        }
        
        else {      
            int direccio = 1;               // T[p.i][p.j] <-- {1, 2, 3, 4}: DOWN == 1, UP == 2, RIGHT == 3, LEFT == 4
            for (Dir d : dirs) {
                if (pos_ok(p+d)) {
                    Pos p2(p+d);
                    if (not visitat[p2.i][p2.j] and cell(p2).type != Building and cell(p2).id == -1 and cell(p2).resistance == -1) {
                        visitat[p2.i][p2.j] = true;
                        cua_pos.push(p2);
                        Tauler[p2.i][p2.j] = direccio;  // a cada node que visito li assigno un nombre per poder trobar
                    }                                   // l'origen facilment al trobar algun objecte d'interés
                }
                ++direccio;
            }
        }
    }
    return Down;                            // poso return final per a que el compilador no doni errors
}


 Dir BFS_buscar_enemics(const Citizen& c) {
    Pos p = c.pos;
    queue<Pos> cua_pos;               // cua de parell posicio i j
    
    vector<vector<int> > Tauler(board_rows(), vector<int>(board_cols()));
    vector<vector<bool> > visitat(board_rows(), vector<bool>(board_cols(), false));
    
    Tauler[p.i][p.j] = 0;               // a la posicio inicial i, j li donem valor 0 per poder trobar-la despres
    visitat[p.i][p.j] = true;
    
    cua_pos.push(p);
    bool trobat = false;
    
    while (not cua_pos.empty() and not trobat) {
        p = cua_pos.front();
        cua_pos.pop();
        int id = cell(p).id;            // variable que ens diu si en aquella posicio hi ha una persona
        
        if (id != -1 and citizen(id).player != me() and  (citizen(id).life <= c.life or c.weapon == strongestWeapon(c.weapon, citizen(id).weapon))) {
            trobat = true;
            
            Dir ant = Down;
            
            while (Tauler[p.i][p.j] != 0) {         // busco la posicio des d'on he sortit
                if (Tauler[p.i][p.j] == 1) ant = Up;             // agafo el sentit contrari del que he hagut de
                else if (Tauler[p.i][p.j] == 2) ant = Down;      // fer per arribar a aquella casella anteriorment
                else if (Tauler[p.i][p.j] == 3) ant = Left;      // p.e. si he hagut d'anar cap a DOWN abans per arribar-hi
                else if (Tauler[p.i][p.j] == 4) ant = Right;     // (ant == 1) ara haure d'anar a UP per poder tornar
                
                p = p+ant;          // vaig a la posicio anterior
                
                if (Tauler[p.i][p.j] == 0) {
                    if (ant == Up) return Down;                 // el moviment que hagi hagut de fer per arribar a aquesta
                    else if (ant == Down) return Up;            // casella fare el contrari per a que el ciutada
                    else if (ant == Left) return Right;         // es mogui cap alla
                    else if (ant == Right) return Left;
                }
            }
        }
        
        else {      
            int direccio = 1;               // T[p.i][p.j] <-- {1, 2, 3, 4}: DOWN == 1, UP == 2, RIGHT == 3, LEFT == 4
            for (Dir d : dirs) {
                if (pos_ok(p+d)) {
                    Pos p2(p+d);
                    if (not visitat[p2.i][p2.j] and cell(p2).type != Building) {
                        visitat[p2.i][p2.j] = true;
                        cua_pos.push(p2);
                        Tauler[p2.i][p2.j] = direccio;  // a cada node que visito li assigno un nombre per poder trobar
                    }                                   // l'origen facilment al trobar algun objecte d'interés
                }
                ++direccio;
            }
        }
    }
    return Down;       // poso return per a que el compilador no doni error, pero mai arribarem a aquesta linia
 }
  
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
      
      vector<int> constructors = builders(me());        // obtinc els ids de tots els meus constructors
      vector<int> guerrers = warriors(me());            // idem pels guerrers
      Dir objectiu = Down;                          // li assigno un valor inicial per a que el compilador no doni error
      
      if (is_day()) {
          
          vector<Dir> direc;
          vector<Pos> barrics = barricades(me());
          int tam = barrics.size();
          Pos ant_constr(0, 0);
          
          for (int id : constructors) {                // itero sobre tots els constructors
            int tam = barrics.size();
            Citizen c = citizen(id);
            objectiu = BFS_buscar_objectes(c);        // aplico BFS al constructor per saber que es el que te a prop
            
            if (tam < 3 and cell(c.pos+objectiu).b_owner == -1) {
                Pos dif ((c.pos.i - ant_constr.i), (c.pos.j - ant_constr.j));
                if ((ant_constr.i == 0 and ant_constr.j == 0) or sqrt(dif.i*dif.i + dif.j*dif.j) > 5) {
                    build(id, objectiu);
                    ant_constr = c.pos;
                }
                else move(id, objectiu);
            }
            else move(id, objectiu);
        }
          
          for (int id : guerrers) {
            Citizen c = citizen(id);
            objectiu = BFS_buscar_objectes(c);            // aplico BFS al constructor per saber que es el que te a prop
            move(id, objectiu);
          }
          
      }
      
      if (is_night()) {
          for (int id : constructors) {
            Citizen c = citizen(id);
            objectiu = BFS_buscar_objectes(c);
            move(id, objectiu);
          }
          
          for (int id : guerrers) {
            Citizen c = citizen(id);
            objectiu = BFS_buscar_enemics(c);
            move(id, objectiu);
            
          }
      }
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
