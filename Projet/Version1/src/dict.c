#include "dict.h"

dict_error_t dict_rechercher_mot(dict_t dico, uint8_t* mot, int taille_mot, dict_index_t* resultat, int* taille_index){

	noeud_t noeud_courant = dico->racine;
	int i = 0;
	*taille_index  = 0;
	*resultat = dico->nb_elt;
	// On parcourt toutes les lettres de mot
	while (i < taille_mot) {

		// On parcout la liste des freres jusqu'a atteindre mot[i] 
		while (noeud_courant != NULL && noeud_courant->sym != mot[i] ) {
			noeud_courant = noeud_courant->frere;
		}

		// Si on a bien trouve mot[i] 
		if (noeud_courant != NULL && noeud_courant->sym == mot[i]) {

			// Si on a trouve le mot ENTIER
			if (i == taille_mot - 1) {
				*resultat = noeud_courant->code;
			}
			// Sinon si on est au bout de la chaine des fils
			else if (noeud_courant->fils == NULL) {
				return DICT_NOTFOUND;
			}
			
			// Sinon on continue a parcourir la chaine des fils
			else {
				noeud_courant = noeud_courant->fils;
				*taille_index = *taille_index + 1;
			}
		}

		// Si on a pas trouve le mot
		else{
			return DICT_NOTFOUND;
		}

		i++;
	}
	return DICT_NOERROR ;
}


dict_error_t dict_rechercher_index(dict_t dico, dict_index_t index, uint8_t* resultat){
	int i = 0;
	int j = 1;

	//On cherche le noeud ayant le code correspondant a l'index grace a la map
	for ( i = 0; i < dico->nb_elt && dico->map[i]->code != index; i++) {
	}
	//On verifie que l'on a bien trouver le noeud correspodnant
	if ( i < dico->nb_elt && dico->map[i]->code == index) {
		noeud_t noeud_courant = dico->map[i];
		resultat[0] = noeud_courant->sym;
		//On remonte l'arbre grace au noeud pere jusqu'a arriver a la fin du mot
		while (noeud_courant->pere != NULL) {
			noeud_courant = noeud_courant->pere;
			resultat[j] = noeud_courant->sym;
			j++;
		}
	}else{
		return DICT_NOTFOUND;
	}
	uint8_t temp;
	// Permet de mettre le mot dans le bon ordre 
	if(j != 1){
		for (int k = 0; k < j/2; k++) {
			temp = resultat[k];
			resultat[k] = resultat[j-k-1];
			resultat[j-k-1] = temp;
		}
	}

	return DICT_NOERROR;
}


// Retourne le pointeur du noeud ayant c pour symbole et son frere precedent
// Les deux pointeurs vallent NULL si c n'est pas present
noeud_t rechercher_dans_ligne(noeud_t liste, uint8_t c, noeud_t* frere){

	noeud_t noeud_courant = liste;
	*frere = noeud_courant;

	while(noeud_courant != NULL){

		if(noeud_courant->sym == c){
			return noeud_courant;
		}
		*frere = noeud_courant;
		noeud_courant = noeud_courant->frere;
	}

	return NULL;
}


// Ajoute courant dans dico a la suite de frere avec sym comme symbole
dict_error_t ajouter_dans_ligne(dict_t dico, noeud_t pere, noeud_t frere, uint8_t sym){

	noeud_t courant = malloc(sizeof(struct _node)) ;

	if(dico->nb_elt == TAILLE_MAX - 3){
		dict_reinit(dico);
	}

	dico->nb_elt = dico->nb_elt + 1;
	if(frere != NULL){
		frere->frere = courant ;
	}
	courant->sym = sym ;
	courant->code = dico->nb_elt-1 ;
	courant->fils = NULL ;
	courant->frere = NULL ;
	courant->pere = pere ;
	if(pere != NULL){
		pere->fils = courant ;
	}
	dico->map[dico->nb_elt-1] = courant;

	return DICT_NOERROR ;
}


dict_error_t dict_insert(dict_t dico, uint8_t* mot, int taille_mot){

	dict_index_t index ;

	noeud_t ligne_courante = dico->racine;
	int index_caractere_courant = 0;
	noeud_t noeud_courant = malloc(sizeof(struct _node));
	noeud_t noeud_pere = NULL;
	noeud_t noeud_frere = malloc(sizeof(struct _node));
	noeud_frere = NULL;
	int taille;

	//On cherche premierement si le mot est present ou non
	if (dict_rechercher_mot(dico, mot, taille_mot, &index, &taille) == DICT_NOTFOUND){
		while (index_caractere_courant < taille_mot){
			//On recherche successivement le caractere courant dans les niveaux de l'arbre
			noeud_courant = rechercher_dans_ligne(ligne_courante, mot[index_caractere_courant], &noeud_frere);

			if (noeud_courant != NULL){
				noeud_pere = malloc(sizeof(noeud_t));
				noeud_pere = noeud_courant;
				index_caractere_courant++;
			}
			else{
				//On ajoute alors la derniere lettre du mot que l'on veux ajouter a la bonne place
				ajouter_dans_ligne(dico, noeud_pere, noeud_frere, mot[index_caractere_courant]);
				return DICT_ADDED;
			}

			ligne_courante = noeud_courant->fils;
		}
	}
	return DICT_FOUND ;
}


void dict_print (dict_t dico){

	uint8_t* mot_courant = malloc(sizeof(uint8_t)) ;

	for (int i = 0; i < dico->nb_elt; i++){
		dict_rechercher_index(dico, i, mot_courant);
		printf("Code : %d \t Mot courant : %s\n", i, mot_courant);
	}

}

void ajout_premier(dict_t dico){

	noeud_t courant = malloc(sizeof(struct _node)) ;

	dico->nb_elt = dico->nb_elt + 1;

	courant->sym = 0 ;
	courant->code = dico->nb_elt-1 ;
	courant->fils = NULL ;
	courant->frere = NULL ;
	dico->racine = courant;
	dico->map[dico->nb_elt-1] = courant;
}

//Permet d'ajouter les symbole speciaux dans le dictionnaire
void ajout_speciaux(dict_t dico){

	noeud_t courant = malloc(sizeof(struct _node)) ;

	//EndOfMessage = 256
	dico->nb_elt = dico->nb_elt + 1;

	courant->sym = ' ';
	courant->code = dico->nb_elt-1 ;
	courant->fils = NULL ;
	courant->frere = NULL ;
	dico->map[255]->frere = courant;
	dico->map[256] = courant;

	//AgrandireDico = 257
	dico->nb_elt = dico->nb_elt + 1;

	courant->sym = ' ';
	courant->code = dico->nb_elt-1 ;
	courant->fils = NULL ;
	courant->frere = NULL ;
	dico->map[256]->frere = courant;
	dico->map[257] = courant;
}

dict_t dict_new(){

	dict_t dico = malloc(sizeof(struct _dict)) ;

	dico-> nb_elt = 0 ;

	uint8_t*  mot_courant = malloc(sizeof(uint8_t));

	mot_courant[1] = '\0' ;


	ajout_premier(dico);


	for(int i = 1 ; i < 256 ; i++){
		*mot_courant = (uint8_t) i ;
		dict_insert(dico, mot_courant, 1);
	}


	// Ajout cas speciaux
	// EndOfMessage, ResetDictionnaire, AgrandirDictionnaire
	ajout_speciaux(dico);
	return dico ;
}


void dict_reinit(dict_t dico){

	//On libere toutes les noeuds sauf ceux de longueur 1
	for(int i = 259; i < dico->nb_elt; i++){
		free(dico->map[i]);
	}
	dico->nb_elt = 258;


}
