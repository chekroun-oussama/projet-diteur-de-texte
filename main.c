#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#define _CRT_NONSTDC_NO_WARNINGS
typedef struct Noeud Noeud;
struct Noeud{
    char *ligne;
    Noeud *suivant;
};
typedef struct{
    Noeud *tete;
    Noeud *queue;
}File;
File* creationFile(){
    File *f=(File*)malloc(sizeof(File));
    f->tete=f->queue=NULL;
    return f;
}
int estvid(File *f){
    return f->tete==NULL;
}
void emfilage(File *f,char *l){
    Noeud *p=malloc(sizeof(Noeud));
    p->ligne = strdup(l);//copy la ling l dans le tableau p->ling
    p->suivant=NULL;

    if(estvid(f)){
        f->tete=p;
        f->queue=p;
    }
    else{
        f->queue->suivant=p;
        f->queue=p;
    }
}
typedef struct Element Element;
struct Element{
    int typmodif;
    char *ancien;
    char *nouveau;
    int indice;
    Element *suivant;
};
typedef struct {
    Element *sommet;
}Pile;
Pile* creationPile(){
    Pile *p=(Pile*)malloc(sizeof(Pile));
    p->sommet=NULL;
    return p;
}
void empilage(Pile *p,int type,char *an,char *nv,int ind){
    Element *q=malloc(sizeof(Element));

    q->typmodif=type;
    q->indice=ind;

    q->ancien = an ? strdup(an) : NULL;
    q->nouveau = nv ? strdup(nv) : NULL;

    q->suivant=p->sommet;
    p->sommet=q;

}
int ajoutLine(File *f,int pos ,char *S){
    Noeud *p=f->tete;
    Noeud *nv=malloc(sizeof(Noeud));
    int cmpt=1;
    if(pos==1){
        nv->ligne = strdup(S);
        nv->suivant = f->tete;
        f->tete=nv;
        if(f->queue==NULL)f->queue=nv;
        return 1;
    }
    while(p!=NULL&&cmpt<pos-1){
        p=p->suivant;
        cmpt++;
    }
    if(p==NULL){printf("position invalide\n");free(nv);Sleep(2000);return 0;}
    nv->ligne = strdup(S);
    nv->suivant=p->suivant;
    p->suivant=nv;
    if(nv->suivant==NULL)f->queue=nv;
    return 1;
}
void rechercheMot(File *f){
	char mot[20];
	char *courant;
	char *trouve=NULL;
	int N=1;
	printf("donner l'mot que tu veux cherche:");
	scanf("%s",mot);
	Noeud* p = f->tete;
	while(p!= NULL){
        courant=p->ligne;
		while((trouve = strstr(courant,mot))!= NULL){
			printf("%.*s",(int)(trouve-courant),courant);
            printf("\033[1;33m%s\033[0m",mot);
            courant=trouve+strlen(mot);
            N=0;
		}
        printf("%s\n",courant);
		p=p->suivant;
	}
		if(N)
		    printf("\033[1;31m%s est n'est existe pas !!\n\033[0m", mot);
}
int remplace(File *T,char* motAncien,char* motnouveau){
    Noeud* p = T->tete;
    int found=0;
	while(p!= NULL){
		int nbr = 0;
        char *courant = p->ligne;
        char *trouve = NULL;
		while((trouve = strstr(courant,motAncien))!= NULL){
			nbr++;
			courant = trouve + strlen(motAncien);
		}
		if(nbr == 0){
			p = p->suivant;
			continue;
		}
		found=1;
		int nvTaill=strlen(p->ligne)+nbr*(strlen(motnouveau)-strlen(motAncien))+1;//calculer la taille de nouvelle ligne et (+1) pour '\0'
		char* nvLigne = malloc(nvTaill);
		if (nvLigne==NULL) return 0;

		char *src = p->ligne;
        char *dst = nvLigne;

		while(*src){
			if(strstr(src,motAncien)==src){
				strcpy(dst,motnouveau);
				dst+=strlen(motnouveau);
				src+=strlen(motAncien);
			}else{
				*dst++ = *src++;
			}
		}
		*dst='\0';
		free(p->ligne);
		p->ligne=nvLigne;
		p=p->suivant;
	}
	return found;
}
char* supprimeLigne(File *T,int pos){
    int lpos=1;
    char *Lsupprime=NULL;
    Noeud *p=T->tete;
    Noeud *q;
    while(p!=NULL && lpos<pos){
        lpos++;
        q=p;
        p=p->suivant;
    }
    if( lpos!=pos)
        printf("element  n'exist pas!");
    else{
        Lsupprime = strdup(p->ligne);
        if(pos==1 ){
            T->tete=p->suivant;
            free(p->ligne);
            free(p);
        }else{
            q->suivant=p->suivant;
            free(p->ligne);
            free(p);
        }
    }
    return Lsupprime;
}
void undo(Pile *p,File *f){
    if(p->sommet==NULL)return;
    Element *q=p->sommet;
    switch(q->typmodif){
    case 1://ajout
        {
            char *tmp = supprimeLigne(f,q->indice);
            free(tmp);
        }
        break;
    case 2://supprimer
        ajoutLine(f,q->indice,q->ancien);
        break;
    case 3://remplace
        remplace(f,q->nouveau,q->ancien);
        break;
    }
    p->sommet=q->suivant;
    if(q->ancien) free(q->ancien);
    if(q->nouveau) free(q->nouveau);
    free(q);
}
int affiche_Text(File T){
    int nbr=0;
    Noeud*p=T.tete;
    printf("\t^A:Ajout\t^D:supprime\t^S:search\t^R:replace\t^Z:Undo\t^X:Quite\n");
    printf("______________________________________________________________________________________________________________\n");
    while(p!=NULL){
        puts(p->ligne);
        p=p->suivant;
        nbr++;
    }
    return nbr;
}

void libererFile(File *f) {
    Noeud *courant = f->tete;
    while (courant != NULL) {
        Noeud *tmp = courant;
        courant = courant->suivant;
        free(tmp->ligne);
        free(tmp);
    }
    free(f);
}

void libererPile(Pile *p) {
    Element *courant = p->sommet;
    while (courant != NULL) {
        Element *tmp = courant;
        courant = courant->suivant;
        if(tmp->ancien) free(tmp->ancien);
        if(tmp->nouveau) free(tmp->nouveau);
        free(tmp);
    }
    free(p);
}

void validerLigneAvantCommande(File *f, char *l, int *i,Pile *P,int *indice){
    if(*i>0){
        l[*i]='\0';
        emfilage(f,l);
        empilage(P,1,NULL,strdup(l),indice);
        *i=0;
        *indice++;
    }
}
int main(){
    int taill=100;
    char *l=malloc(taill);
    char c;
    int i=0;
    int indiceLigne=1;
    File *Text=creationFile();
    Pile *P=creationPile();
    printf("\t^A:Ajout\t^D:supprime\t^S:search\t^R:replace\t^Z:Undo\t^X:Quite\n");
    printf("______________________________________________________________________________________________________________\n");

    while(1){
        c=getch();
        switch(c){
            case 1:   //^A = ajout line

                validerLigneAvantCommande(Text, l, &i,P,&indiceLigne);
                {
                    int pos;
                    char S[200];

                    system("cls");

                    printf("donne la position ou tu veux ajouter la ligne:");
                    scanf("%d",&pos);

                    getchar();//Pour vider le buffer
                    printf("donne votr ling:\n");
                    gets(S);

                    if(ajoutLine(Text,pos,S)){
                        empilage(P,1,NULL,S,pos);//stockes les modifications dans une pile
                    }
                    system("cls");
                    indiceLigne=affiche_Text(*Text);

                }
                break;
            case 4:   //^D = delete line

                validerLigneAvantCommande(Text, l, &i,P,&indiceLigne);
                {
                    int pos;
                    char *Lsupprime;

                    system("cls");

                    printf("donner position de la ligne que tu veux supprimer :");
                    scanf("%d",&pos);

                    Lsupprime=supprimeLigne(Text,pos);
                    if(Lsupprime){
                        empilage(P,2,Lsupprime,NULL,pos);//stockes les modifications dans une pile
                    }else{Sleep(2000);}

                    system("cls");
                    indiceLigne=affiche_Text(*Text);
                }
                break;

            case 18:  //^R = replace

                validerLigneAvantCommande(Text, l, &i,P,&indiceLigne);
                {
                    char motancien[20];
                    char motnouveau[20];
                    int found;

                    system("cls");

                    printf("donne le mot que tu veux changer:");
                    scanf("%s",motancien);

                    printf("donne le nouveau mot :");
                    scanf("%s",motnouveau);

                    found=remplace(Text,motancien,motnouveau);
                    if(found){
                        empilage(P,3,motancien,motnouveau,0);//stockes les modifications dans une pile

                        printf("\n\n le mot a ete change avec succes.\n");
                        Sleep(2000);//attends 2 secondes
                        system("cls");

                        indiceLigne=affiche_Text(*Text);
                    }else{
                        printf("\n\n aucun mot trouve a remplacer !");
                        Sleep(2000);//attends 2 secondes
                        system("cls");

                        indiceLigne=affiche_Text(*Text);
                    }
                }
                break;

            case 19:  //^S = search word

                validerLigneAvantCommande(Text, l, &i,P,&indiceLigne);
                system("cls");

                rechercheMot(Text);

                printf("\n appuyer sur n'importe quelle touche pour revenir.");
                getch();

                system("cls");
                indiceLigne=affiche_Text(*Text);
                break;

            case 26:  //^U = undo

                validerLigneAvantCommande(Text, l, &i,P,&indiceLigne);

                undo(P,Text);

                system("cls");
                indiceLigne=affiche_Text(*Text);
                break;

            case 24://^X
                validerLigneAvantCommande(Text, l, &i,P,&indiceLigne);
                break;

            case'\r': // si on tapait entree

                l[i]='\0';//terminee la line
                i=0;
                emfilage(Text,l);

                printf("\n");
                empilage(P,1,NULL,strdup(l),indiceLigne);
                indiceLigne++;
                break;

            default://on ajout le caracter a la ligne l

                if(i+1>taill){
                    taill+=50;
                    l=realloc(l,taill);
                }
                l[i++]=c;
                printf("%c",c);
        }
        if(c==24)break;
    }
    libererFile(Text);
    libererPile(P);
    free(l);
    return 0;
}

