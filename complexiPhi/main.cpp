
#include <stdio.h>

#include <vector>
#include <map>
#include <math.h>
#include <time.h>
#include <iostream>
#include "globalConst.h"
#include "tHMM.h"
#include "tAgent.h"
#include "tGame.h"

#define randDouble ((double)rand()/(double)RAND_MAX)

using namespace std;

//double replacementRate=0.1;
double perSiteMutationRate=0.005;
int update=0;
int repeats=1;
int maxAgent=100;
int totalGenerations=1000;

void init(void);
void display(void);
void reshape(int, int);
void mouse(int, int, int, int);
void myIdleFunction(void);
void renderScene(void);

void computeLOD(FILE *f,FILE *a,tAgent *agent,tGame *game){
	vector<vector<int> > table;
	double R,oldR;
	FILE *df;
	int i,j,k;
	if(agent->ancestor!=NULL)
		computeLOD(f,a,agent->ancestor,game);
	agent->setupPhenotype();
	table=game->analyseGame(agent, NULL);
	R=game->computeR(table,0);
	fprintf(f,"%i	%i	%i	%f",agent->born,agent->correct,agent->incorrect,R);
	for(i=0;i<5;i++){
		vector<vector<int> > subTable;
		subTable=table;
		for(j=0;j<subTable[0].size();j++){
			subTable[0][j]=subTable[0][j]&(1<<i);
			subTable[4][j]=subTable[4][j]&((1<<(16+i))+65535);
		}
		fprintf(f,"	%f",game->computeR(subTable,0));
	}
	fprintf(f,"\n");
	if(a!=NULL)
		for(i=0;i<table[4].size();i++){
			fprintf(a,"%i\n",table[4][i]);
		}
    /*
     for(i=0;i<table.size();i++)
     fprintf(f,"	%f",game->entropy(table[i]));
     fprintf(f,"	%f",oldR);
     for(i=1;i<5;i++)
     fprintf(f,"	%f",game->computeR(table, i));
     fprintf(f,"	%f	%f	%f	%f",game->computeAtomicPhi(table[4], 16),game->predictiveI(table[4]),game->nonPredictiveI(table[4]),game->predictNextInput(table[4]));
     */
	/*
     for(i=0;i<20;i++){
     vector<vector<int> > subTable;
     subTable.clear();
     subTable.resize(table.size());
     for(k=0;k<table.size();k++)
     for(j=0;j<80;j++)
     subTable[k].push_back(table[k][(j*20)+i]);
     fprintf(f,"	%f",game->computeR(subTable, 0));
     }
     fprintf(f,"\n");
	 */
    //	printf("%i	%i	%i	%f\n",agent->born,agent->correct,agent->incorrect,R);
    /*
     df=fopen("testFile.txt","a+t");
     k=0;
     for(i=0;i<table[0].size();i++){
     fprintf(df,"%i",k);
     for(j=0;j<table.size();j++)
     fprintf(df,"	%i",table[j][i]);
     fprintf(df,"\n");
     k++;
     }
     table.clear();
     fclose(df);
     */
}

void doKOonLOD(FILE *f,tAgent *agent,tGame *game){
	int i,j,k;
	if(agent->ancestor!=NULL)
		doKOonLOD(f,agent->ancestor,game);
	game->analyseGame(agent,NULL);
	fprintf(f,"%i	%i",agent->born,agent->correct);
	for(i=0;i<16;i++)
		for(j=0;j<2;j++){
			game->analyseKO(agent,NULL,i,j);
			fprintf(f,"	%i",agent->correct);
		}
	fprintf(f,"\n");
}

int main(int argc, char *argv[])
{
	vector<tAgent*>agent;
	vector<tAgent*>nextGen;
	tAgent *masterAgent;
	int i,j,k,who=0;
	tGame *game;
	double maxFitness;
	int newBorn=0;
    int scoreMode=atoi(argv[4]);
	FILE *resFile;
	FILE *LOD;
	FILE *genomeFile;
	int A,B,C,D,E,F,G,I,J,K,L;
	int setStates[11]={0,1,2,3,4,6,7,8,12,14,15};
	LOD=fopen(argv[2],"w+t");
	genomeFile=fopen(argv[3],"w+t");	
	resFile=fopen(argv[1],"w+t");
	srand(time(NULL));
	agent.resize(maxAgent);
	game=new tGame;
    game->loadExperiment("basic.txt");
    game->theExperiment.showExperimentProtokoll();
    //exit(0);
	masterAgent=new tAgent;
	// code to compute all non R machines
    /*
     for(A=0;A<3;A++)
     for(B=0;B<3;B++)
     for(C=0;C<3;C++)
     for(D=0;D<3;D++)
     for(E=0;E<3;E++)
     for(F=0;F<3;F++)
     for(G=0;G<3;G++)
     for(I=0;I<3;I++)
     for(J=0;J<3;J++)
     for(K=0;K<3;K++)
     for(L=0;L<3;L++){
     masterAgent->hmmus.clear();
     tHMMU *H=new tHMMU;
     H->ins.clear();
     H->ins.push_back(0);
     H->ins.push_back(1);
     H->ins.push_back(2);
     H->ins.push_back(3);
     H->outs.clear();
     H->outs.push_back(14);
     H->outs.push_back(15);
     H->hmm.resize(16);
     H->sums.resize(16);
     for(i=0;i<16;i++){
     H->hmm[i].resize(4);
     H->sums[i]=255;
     for(j=0;j<4;j++)
     H->hmm[i][j]=0;
     }
     H->hmm[setStates[0]][A]=255;
     H->hmm[setStates[1]][B]=255;
     H->hmm[setStates[2]][C]=255;
     H->hmm[setStates[3]][D]=255;
     H->hmm[setStates[4]][E]=255;
     H->hmm[setStates[5]][F]=255;
     H->hmm[setStates[6]][G]=255;
     H->hmm[setStates[7]][I]=255;
     H->hmm[setStates[8]][J]=255;
     H->hmm[setStates[9]][K]=255;
     H->hmm[setStates[10]][L]=255;
     masterAgent->hmmus.push_back(H);
     vector<vector<int> > table=game->analyseGame(masterAgent, NULL);
     cout<<A<<B<<C<<D<<E<<F<<G<<I<<J<<K<<L<<" "<<masterAgent->correct<<" "<<game->computeR(table, 0)<< endl;
     fprintf(resFile,"%i\n",masterAgent->correct);
     }
     //*/
	
	
	//exit(0);
	masterAgent->setupRandomAgent(5000);
	masterAgent->setupPhenotype();
	for(i=0;i<agent.size();i++){
		agent[i]=new tAgent;
		agent[i]->inherit(masterAgent,0.5,0);
	}
	nextGen.resize(agent.size());
	masterAgent->nrPointingAtMe--;
	cout<<"setup complete"<<endl;
	while(update<totalGenerations){
		for(i=0;i<agent.size();i++){
			agent[i]->fitness=0.0;
			agent[i]->fitnesses.clear();
		}
		for(i=0;i<agent.size();i++){
			for(j=0;j<repeats;j++){
				game->executeGame(agent[i],NULL);
				agent[i]->fitnesses.push_back(agent[i]->fitness);
			}
		}
		fflush(resFile);
		maxFitness=0.0;
		
		for(i=0;i<agent.size();i++){
			agent[i]->fitness=agent[i]->fitnesses[0];
			if(agent[i]->fitness>maxFitness)
				maxFitness=agent[i]->fitness;
		}
		cout<<update<<" "<<(double)maxFitness<<" "<<agent[who]->convFitness<<" "<<agent[who]->correct<<" "<<agent[who]->incorrect<<endl;
		for(i=0;i<agent.size();i++)
		{
			tAgent *d;
			d=new tAgent;
			do{ j=rand()%agent.size(); } while((j==i)||(randDouble>(agent[j]->fitness/maxFitness)));
			d->inherit(agent[j],perSiteMutationRate,update);
			nextGen[i]=d;
		}
		for(i=0;i<agent.size();i++){
			agent[i]->retire();
			agent[i]->nrPointingAtMe--;
			if(agent[i]->nrPointingAtMe==0)
				delete agent[i];
			agent[i]=nextGen[i];
		}
		agent=nextGen;
		update++;
	}
	
	//agent[0]->saveLOD(LOD,genomeFile);
	computeLOD(resFile,NULL,agent[0],game);
	//agent[0]->ancestor->saveLOD(LOD,genomeFile);
	agent[0]->ancestor->ancestor->saveGenome(genomeFile);
    //	FILE *pF=fopen(argv[4],"w+t");
    //	game->executeGame(agent[0]->ancestor, 2, pF);
    //	fclose(pF);
	
    //	agent[0]->ancestor->saveToDot(argv[3]);
    //	FILE *KOfile=fopen("koResults.txt","w+t");
    //	doKOonLOD(KOfile,agent[0]->ancestor,game);
    return 0;
}

