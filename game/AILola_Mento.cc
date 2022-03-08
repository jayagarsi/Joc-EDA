#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Lola_Mento


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
  // que quedin mes a prop. Per al cas de que estem en la nit tambe retornara un valor amb la "distancia" (nivells de l'arbre BFS)
  // a la que es troba el ciutada de l'objecte o barricada per sospesar si surt a compte anar-hi o no
  
  Dir BFS_buscar_objectes(const Citizen& c, int& nivells) {
    
    Pos p = c.pos;
    queue<Pos> cua_pos;               // cua de parell posicio i j
    
    vector<vector<int> > Tauler(board_rows(), vector<int>(board_cols()));
    vector<vector<bool> > visitat(board_rows(), vector<bool>(board_cols(), false));
    
    Tauler[p.i][p.j] = 0;               // a la posicio inicial i, j li donem valor 0 per poder trobar-la despres
    visitat[p.i][p.j] = true;
    
    cua_pos.push(p);
    bool trobat = false;
    
    while (not cua_pos.empty() and not trobat) {        // mentre no hi hagi posicions o  no hagi trobat res busco
        p = cua_pos.front();
        cua_pos.pop();
        
        // si la cel.la conte diners o som un guerrer i trobem un arma o tenim poca vida i trobem menjar ens movem cap alla
        if (cell(p).bonus == Money or (c.type == Warrior and cell(p).weapon != NoWeapon) or (cell(p).bonus == Food and c.life < 30)) {
            trobat = true;
            
            Dir ant = Down;                         // dono una posicio inicial a ant per a que el compilador no doni error
            
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
                    if (not visitat[p2.i][p2.j] and cell(p2).type == Street and cell(p2).id == -1 and cell(p2).b_owner != me()) {        // si no he visitat la posicio i no tenim cap element que ens bloquegi ens guardem aquest lloc
                        visitat[p2.i][p2.j] = true;
                        cua_pos.push(p2);
                        Tauler[p2.i][p2.j] = direccio;  // a cada node que visito li assigno un nombre per poder trobar
                    }                                   // l'origen facilment al trobar algun objecte d'interés
                }
                ++direccio;
            }
            ++nivells;
        }
    }
    return Down;                            // poso return final per a que el compilador no doni errors
}

// Mateixa funcio que abans amb la diferencia que ara el BFS no busca objectes si no que busca ciutadans enemics
 Dir BFS_buscar_enemics(const Citizen& c, int& nivells) {
    Pos p = c.pos;
    queue<Pos> cua_pos;               // cua de parell posicio i j
    
    vector<vector<int> > Tauler(board_rows(), vector<int>(board_cols()));
    vector<vector<bool> > visitat(board_rows(), vector<bool>(board_cols(), false));
    
    Tauler[p.i][p.j] = 0;               // a la posicio inicial i, j li donem valor 0 per poder trobar-la despres
    visitat[p.i][p.j] = true;
    
    cua_pos.push(p);
    bool trobat = false;                // boolea per saber si hem trobat el ciutada enemic o no
    //bool fuig = false;                  // boolea per saber si es millor que el ciutada fugi de l'enemic o no
    
    while (not cua_pos.empty() and not trobat) {
        p = cua_pos.front();
        cua_pos.pop();
        int id = cell(p).id;            // variable que ens diu si en aquella posicio hi ha una persona
    
        if (id != -1 and citizen(id).player != me()) {    // si el ciutada trobat no soc jo i no es cap dels meus jugadors
            
            // el guerrer atacara si l'enemic te menor vida que ell o si te un arma millor
            if (c.type == Warrior and (citizen(id).life <= c.life or c.weapon == strongestWeapon(c.weapon, citizen(id).weapon))) 
                trobat = true;
            
            else if (c.type == Builder and citizen(id).type == Builder and citizen(id).life < c.life) trobat = true;
            
        }
            
        if (trobat) {
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
                    if (not visitat[p2.i][p2.j] and cell(p2).type == Street and cell(p2).b_owner != me()) {
                        visitat[p2.i][p2.j] = true;
                        cua_pos.push(p2);
                        Tauler[p2.i][p2.j] = direccio;  // a cada node que visito li assigno un nombre per poder trobar
                    }                                   // l'origen facilment al trobar algun objecte d'interés
                }
                ++direccio;
            }
            ++nivells;
        }
    }
    return Down;       // poso return per a que el compilador no doni error, pero mai arribarem a aquesta linia
 }

  Dir BFS_buscar_barricades(const Citizen& c, int& nivells) {
    Pos p = c.pos;
    queue<Pos> cua_pos;               // cua de parell posicio i j
    
    vector<vector<int> > Tauler(board_rows(), vector<int>(board_cols()));
    vector<vector<bool> > visitat(board_rows(), vector<bool>(board_cols(), false));
    
    Tauler[p.i][p.j] = 0;               // a la posicio inicial i, j li donem valor 0 per poder trobar-la despres
    visitat[p.i][p.j] = true;
    
    cua_pos.push(p);
    bool trobat = false;

    while (not cua_pos.empty() and not trobat) {        // mentre no hi hagi posicions o  no hagi trobat res busco
        p = cua_pos.front();
        cua_pos.pop();
        
        // si la cel.la conte diners o som un guerrer i trobem un arma o tenim poca vida i trobem menjar ens movem cap alla
        if (cell(p).b_owner == me() and cell(p).id == -1) {
            trobat = true;
            
            Dir ant = Down;                         // dono una posicio inicial a ant per a que el compilador no doni error
            
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
                    if (not visitat[p2.i][p2.j] and cell(p2).type == Street and cell(p2).id == -1) {        // si no he visitat la posicio i no tenim cap element que ens bloquegi ens guardem aquest lloc
                        visitat[p2.i][p2.j] = true;
                        cua_pos.push(p2);
                        Tauler[p2.i][p2.j] = direccio;  // a cada node que visito li assigno un nombre per poder trobar
                    }                                   // l'origen facilment al trobar algun objecte d'interés
                }
                ++direccio;
            }
            ++nivells;
        }
    }
    return Down;                            // poso return final per a que el compilador no doni errors
}
  
  
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
      
      int nivells = 0;
      vector<int> constructors = builders(me());        // obtinc els ids de tots els meus constructors
      vector<int> guerrers = warriors(me());            // idem pels guerrers
      Dir objectiu = Down;                          // li assigno un valor inicial per a que el compilador no doni error
      
      if (is_day()) {
          
          vector<Pos> barrics = barricades(me());
          int tam = barrics.size();
          Pos ant_constr(0, 0);
          
          
          // els constructors de dia poden construir o moures per buscar objectes hem d'escollir quina de les dues fem
          
          for (int id : constructors) {                // itero sobre tots els constructors
            int tam = barrics.size();
            Citizen c = citizen(id);
            objectiu = BFS_buscar_objectes(c, nivells);        // aplico BFS al constructor per saber que es el que te a prop
            
            Dir contraria = Down;
            
            if (objectiu == Up) contraria = Down;
            else if (objectiu == Down) contraria = Up;
            else if (objectiu == Right) contraria = Left;
            else if (objectiu == Left) contraria = Right;
            
            
            // si hi han menys de 3 barricades i la cel·la contraria es buida i correcta consstrueixo         
            if ((tam < 3 and pos_ok(c.pos+contraria) and (cell(c.pos+contraria).b_owner == -1)) or (cell(c.pos+contraria).resistance > -1 and cell(c.pos+contraria).resistance < barricade_max_resistance()))
                    build(id, contraria);           // construeixo en la direccio contraria a la que he trobtat l'objecte
            else move(id, objectiu);            // si tenim el maxim nombre de barricades que es mogui
          }
          
          
          // els guerrers de dia nomes busquen objectes no fan res mes
          
          for (int id : guerrers) {
            Citizen c = citizen(id);
            objectiu = BFS_buscar_objectes(c, nivells);
            move(id, objectiu);
          }
          
      }
      
      else if (is_night()) {
          
          Dir objecte_proper = Up;
          int dist_perill =  0;
          
          // els constructors de nit eviten l'enfrontament i busquen escapar o amagarse en una barricada
          for (int id : constructors) {
            Citizen c = citizen(id);
            
            objecte_proper = BFS_buscar_objectes(c, nivells);            // busco si hi ha objectes a prop
            objectiu = BFS_buscar_barricades(c, dist_perill);               // busco si hi ha enemics a prop
            
            if (c.life < 30) move(id, objectiu);                    // si te poca vida que vagi a una barricada 
            else move (id, objecte_proper);                         // si te sucificient vida que busqui objectes
            
            if (cell(c.pos).b_owner == me()) {
                for (Dir d : dirs) {
                    int id_enem = cell(c.pos+d).id;
                    if (id_enem != -1 and citizen(id).player != me())       // miro si hi ha algun enemic a prop de la barricada
                        move(id, d);                                        // i si hi es em moc cap a ell per ataca-lo
                }
            }
          }
          
          // els guerrers de nit busquen matar, pero nomes mataran si tenen mes vida que el contrincant o si tenen millor arma
          for (int id : guerrers) {
            Citizen c = citizen(id);
            
            if (c.life < 30) {
                Dir barricada_propera = BFS_buscar_barricades(c, nivells);
                move(id, barricada_propera);  // si tenim poca vida busquem una barricada
            }
            
            else {
                objectiu = BFS_buscar_enemics(c, dist_perill);       // buscar enemics i matar-los
                move (id, objectiu);       // si no tenim poca vida busquem victimes
            }
            
            if (cell(c.pos).b_owner == me()) {              // si es troba en una barricada
                for (Dir d : dirs) {
                    int id_enem = cell(c.pos+d).id;
                    if (id_enem != -1 and citizen(id).player != me())   // i te un enemic aprop que l'ataqui
                        move(id, d);
                }
            }
          
          }
      }
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
