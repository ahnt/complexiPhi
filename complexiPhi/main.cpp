
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
int repeats=10;
int maxAgent=100;
int totalGenerations=5000;

void init(void);
void display(void);
void reshape(int, int);
void mouse(int, int, int, int);
void myIdleFunction(void);
void renderScene(void);

void computeLOD(FILE *f,tAgent *agent,tGame *game){
	vector<vector<int> > table;
	double R,oldR;
	FILE *df;
	int i,j,k;
	if(agent->ancestor!=NULL)
		computeLOD(f,agent->ancestor,game);
	agent->setupPhenotype();
	table=game->analyseGame(agent, 2, NULL);
	R=game->computeR(table,0);
	oldR=game->computeOldR(table);
	fprintf(f,"%i	%i	%i	%f",agent->ID,agent->correct,agent->incorrect,R);
	for(i=0;i<table.size();i++)
		fprintf(f,"	%f",game->entropy(table[i]));
	fprintf(f,"	%f",oldR);
	for(i=1;i<5;i++)
		fprintf(f,"	%f",game->computeR(table, i));
	fprintf(f,"	%f	%f	%f	%f",game->computeAtomicPhi(table[4], 16),game->predictiveI(table[4]),game->nonPredictiveI(table[4]),game->predictNextInput(table[4]));
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
	game->analyseGame(agent,2,NULL);
	fprintf(f,"%i	%i",agent->born,agent->correct);
	for(i=0;i<16;i++)
		for(j=0;j<2;j++){
			game->analyseKO(agent,2,NULL,i,j);
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
	FILE *resFile;
	FILE *LOD;
	FILE *genomeFile;
	
//	resFile=fopen("result.txt","w+t");
	LOD=fopen(argv[2],"w+t");
	genomeFile=fopen(argv[3],"w+t");	
	resFile=fopen(argv[1],"w+t");
//	LOD=fopen(argv[2],"w+t");
//	genomeFile=fopen(argv[3],"w+t");	
	srand(time(NULL));
	agent.resize(maxAgent);
/*	game=new tGame;
	for(i=0;i<100000;i++){
		masterAgent=new tAgent;
		masterAgent->setupRandomAgent(5000);
		masterAgent->setupPhenotype();
		vector<vector<int> > table=game->analyseGame(masterAgent, 2, NULL);
		float R=game->computeR(table,0);
		fprintf(resFile,"%i	%i	%i	%f\n",i,masterAgent->correct,masterAgent->incorrect,R);
		cout<<i<<endl;
//	masterAgent->setupRandomAgent(600);
//	for(i=0;i<masterAgent->genome.size();i++)
//		masterAgent->genome[i]=rand()&255;
//	masterAgent->loadAgent("startGenome.txt");
	//printf("genome length:%i\n",masterAgent->genome.size());
		masterAgent->setupPhenotype();
		
	}
 exit(0);
 */
//	masterAgent->showPhenotype();
//	masterAgent->saveToDot(argv[2]);
	game=new tGame;
	masterAgent=new tAgent;
	masterAgent->setupRandomAgent(5000);
	masterAgent->setupPhenotype();
/*	masterAgent->loadAgent("startGenome.txt");
	masterAgent->saveToDotFullLayout("brainFULL.dot");
	FILE *r=fopen("KO8_0.txt","w+t");
	game->executeGame(masterAgent, 2, r);
	masterAgent->setupPhenotype();
	cout<<masterAgent->correct<<endl;
	masterAgent->showPhenotype();
	FILE *l=fopen("logicTable.txt","w+t");
	masterAgent->saveLogicTable(l);
	exit(0);
 */
	for(i=0;i<agent.size();i++){
		agent[i]=new tAgent;
		agent[i]->inherit(masterAgent,0.5,0);
	}
	nextGen.resize(agent.size());
	masterAgent->nrPointingAtMe--;
	cout<<"setup complete"<<endl;
	while(update<totalGenerations){
		//*
		for(i=0;i<agent.size();i++){
			agent[i]->fitness=0.0;
			agent[i]->fitnesses.clear();
		}
		for(i=0;i<agent.size();i++){
			for(j=0;j<repeats;j++){
//				if((update&31)==0)
//					game->executeGame(agent[i],2,resFile);
//				else
					game->executeGame(agent[i],2,NULL);
				agent[i]->fitnesses.push_back(agent[i]->fitness);
			}
		}
		fflush(resFile);
		maxFitness=0.0;
		
		for(i=0;i<agent.size();i++){
			//agent[i]->fitness=agent[i]->fitnesses[0];
			
			agent[i]->fitness=1.0;
			for(j=0;j<agent[i]->fitnesses.size();j++)
//					agent[i]->fitness+=agent[i]->fitnesses[j];
					agent[i]->fitness*=agent[i]->fitnesses[j];
			agent[i]->fitness=pow(agent[i]->fitness,1.0/(double)agent[i]->fitnesses.size());
	//		agent[i]->fitness/=(double)agent[i]->fitnesses.size();
			 //*/
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
		/*
		for(i=0;i<agent.size();i++)
			if((randDouble<replacementRate)||(agent[i]->fitness==0.0))
			{
				do{ j=rand()%agent.size(); } while((j==i)||(randDouble>(agent[j]->fitness/maxFitness)));
				agent[i]->retire();
				agent[i]->nrPointingAtMe--;										
				if(agent[i]->nrPointingAtMe==0)
					delete agent[i];
				agent[i]=new tAgent;
				agent[i]->inherit(agent[j],perSiteMutationRate,update);
				agent[i]->fitness=0.0;
			}
		 
		if((update&31)==31){
			cout<<"---"<<endl;
			tAgent *a;
			a=agent[0]->findLMRCA();
			if((a!=NULL)&&(a!=agent[0])){
				if(a->ancestor!=NULL){
					cout <<"saving"<<endl;
					a->ancestor->saveFromLMRCAtoNULL(LOD,genomeFile);
				}
			}
			fflush(LOD);
			fflush(genomeFile);
		}
		 */
	}
	
//	agent[0]->saveLOD(LOD,genomeFile);
	computeLOD(resFile,agent[0],game);
	//agent[0]->ancestor->saveLOD(LOD,genomeFile);
	agent[0]->ancestor->ancestor->saveGenome(genomeFile);
	FILE *pF=fopen(argv[4],"w+t");
	game->executeGame(agent[0]->ancestor, 2, pF);
	fclose(pF);
	
//	agent[0]->ancestor->saveToDot(argv[3]);
//	FILE *KOfile=fopen("koResults.txt","w+t");
//	doKOonLOD(KOfile,agent[0]->ancestor,game);
    return 0;
}

