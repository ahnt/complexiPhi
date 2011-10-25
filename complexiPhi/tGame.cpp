/*
 *  tGame.cpp
 *  HMMBrain
 *
 *  Created by Arend on 9/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "tGame.h"
#include <math.h>



tGame::tGame(){
}

tGame::~tGame(){
}


void tGame::executeGame(tAgent* agent,FILE *f){
	int spB,w,d,u,p;
	int Ox,Ax;
	int grid[20];
	int i;
	bool hit;
	int correct,incorrect;
	correct=0;
	incorrect=0;
	agent->fitness=1.0;
	agent->convFitness=0.0;
	for(spB=0;spB<20;spB++){
        for(d=0;d<theExperiment.drops();d++){
            for(w=0;w<theExperiment.sizes();w++){
                for(p=0;p<theExperiment.selves();p++){
					Ax=0;
					Ox=spB;
					agent->resetBrain();
					for(u=0;u<20;u++){
						hit=false;
						for(i=0;i<20;i++)
							grid[i]=0;
						for(i=0;i<theExperiment.sizeSequences[w].data[u&7];i++)
							grid[(Ox+i+20)%20]=1;
						for(i=0;i<4+theExperiment.selfSequences[p].data[u&7];i++)
							if(grid[(Ax+i)%20]==1) hit=true;
						agent->states[0]=grid[Ax];
						agent->states[1]=grid[(Ax+1)%20];
						agent->states[2]=grid[(Ax+theExperiment.selfSequences[p].data[u&7]+2)%20];
						agent->states[3]=grid[(Ax+theExperiment.selfSequences[p].data[u&7]+3)%20];
						agent->states[maxNodes-1]=0;
						agent->states[maxNodes-2]=0;
						agent->updateStates();
						int action=(agent->states[maxNodes-1])+(agent->states[maxNodes-2]<<1);
						switch (action) {
							case 0: case 3: break;
							case 1:Ax=(Ax+1)%20;break;
							case 2:Ax=(Ax-1+20)%20;break;
						}
						Ox=(Ox+20+theExperiment.selfSequences[d].data[u&7])%20;
						if(f!=NULL){
							for(i=0;i<20;i++)
								grid[i]=0;
							for(i=0;i<4+theExperiment.selfSequences[p].data[u&7];i++)
								grid[(i+Ax)%20]=1;
							for(i=0;i<20;i++)
								fprintf(f,"%i,	",grid[i]);
							fprintf(f,"\n");
						}
					}
                    if(hit==theExperiment.shouldHit[d][w][p]){
                        agent->fitness*=1.1;
                        agent->convFitness+=1.0;
                        correct++;
                    }
                    else {
                        agent->convFitness-=1.0;
                        agent->fitness/=1.1;
                        incorrect++;
                    }
				}
			}
		}
	}
	agent->correct=correct;
	agent->incorrect=incorrect;
	//cout<<"C: "<<correct<<" I: "<<incorrect<<endl;

}

vector<vector<int> >  tGame::analyseGame(tAgent* agent,FILE *f){
    int spB,w,d,u,p;
	int Ox,Ax;
	int grid[20];
	int i;
	bool hit;
	int correct,incorrect;
    int E,I,H,M,T0,T1,FE;
	vector<vector<int> > retValue;
	vector<int> dummyE,dummyI,dummyH,dummyM,dummyT0,dummyT1,dummyFullE;
    
	retValue.clear();

	correct=0;
	incorrect=0;
	agent->fitness=1.0;
	agent->convFitness=0.0;
	for(spB=0;spB<20;spB++){
        for(d=0;d<theExperiment.drops();d++){
            for(w=0;w<theExperiment.sizes();w++){
                for(p=0;p<theExperiment.selves();p++){
					Ax=0;
					Ox=spB;
					agent->resetBrain();
					for(u=0;u<20;u++){
						hit=false;
						for(i=0;i<20;i++)
							grid[i]=0;
						for(i=0;i<theExperiment.sizeSequences[w].data[u&7];i++)
							grid[(Ox+i+20)%20]=1;
						for(i=0;i<4+theExperiment.selfSequences[p].data[u&7];i++)
							if(grid[(Ax+i)%20]==1) hit=true;
						agent->states[0]=grid[Ax];
						agent->states[1]=grid[(Ax+1)%20];
						agent->states[2]=grid[(Ax+theExperiment.selfSequences[p].data[u&7]+2)%20];
						agent->states[3]=grid[(Ax+theExperiment.selfSequences[p].data[u&7]+3)%20];
						agent->states[maxNodes-1]=0;
						agent->states[maxNodes-2]=0;
                        E=0;
                        I=0;
                        H=0;
                        M=0;
                        FE=d<<6+w<<3+p;
                        if(theExperiment.sizeSequences[w].data[u&7]>=theExperiment.selfSequences[p].data[u&7]) E+=8;
                        if(theExperiment.dropSequences[d].data[u&7]==1) E+=4;
                        int cm=(Ox+(theExperiment.sizeSequences[w].data[u&7]/2))-(Ax+4+theExperiment.selfSequences[p].data[u&7]);
                        if(cm>=0) E+=2;
                        if(hit) E+=1;
                        if(theExperiment.selfSequences[p].data[u&7]<2) E+=16;
                        for(i=0;i<4;i++)
                            I=(I<<1)+agent->states[i];
                        for(i=4;i<14;i++)
                            H=(H<<1)+agent->states[i];
                        for(i=14;i<16;i++)
                            M=(M<<1)+agent->states[i];
                        if(f!=NULL)
                            fprintf(f,"%i	%i	",E,I);
                        dummyE.push_back(E);
                        dummyI.push_back(I);
                        agent->updateStates();
                        H=0;
                        M=0;
                        for(i=4;i<16;i++)
                            H=(H<<1)+agent->states[i];
                        for(i=14;i<16;i++)
                            M=(M<<1)+agent->states[i];
                        if(f!=NULL)
                            fprintf(f,"	%i	%i	%i\n",H,M,(FE<<19)+(E<<14)+(I<<10)+H);
                        dummyH.push_back(H);
                        dummyM.push_back(M);
                        dummyT0.push_back((FE<<21)+(E<<16)+(I<<12)+H);
                        dummyFullE.push_back(FE);
						int action=(agent->states[maxNodes-1])+(agent->states[maxNodes-2]<<1);
						switch (action) {
							case 0: case 3: break;
							case 1:Ax=(Ax+1)%20;break;
							case 2:Ax=(Ax-1+20)%20;break;
						}
						Ox=(Ox+20+theExperiment.selfSequences[d].data[u&7])%20;
						if(f!=NULL){
							for(i=0;i<20;i++)
								grid[i]=0;
							for(i=0;i<4+theExperiment.selfSequences[p].data[u&7];i++)
								grid[(i+Ax)%20]=1;
							for(i=0;i<20;i++)
								fprintf(f,"%i,	",grid[i]);
							fprintf(f,"\n");
						}
					}
                    if(hit==theExperiment.shouldHit[d][w][p]){
                        agent->fitness*=1.1;
                        agent->convFitness+=1.0;
                        correct++;
                    }
                    else {
                        agent->convFitness-=1.0;
                        agent->fitness/=1.1;
                        incorrect++;
                    }
				}
			}
		}
	}
	agent->correct=correct;
	agent->incorrect=incorrect;
    retValue.push_back(dummyE);
	retValue.push_back(dummyI);
	retValue.push_back(dummyH);
	retValue.push_back(dummyM);
	retValue.push_back(dummyT0);
    retValue.push_back(dummyFullE);
	return retValue;
}

void tGame::analyseKO(tAgent* agent,FILE *f,int which, int setTo){
    /*
	int spB,w,d,u;
	int Ox,Ax;
	int grid[20];
	int i;
	bool hit;
	int correct,incorrect,paddleWidth;
	int E,I,H,M,T0,T1;
	correct=0;
	incorrect=0;
	agent->fitness=1.0;
	agent->convFitness=0.0;
	for(spB=0;spB<20;spB++){
		for(w=2;w<6;w+=2){
			for(d=-1;d<3;d+=2){
				Ax=0;
				Ox=spB;
				agent->resetBrain();
				for(u=0;u<20;u++){
					hit=false;
					for(i=0;i<20;i++)
						grid[i]=0;
					for(i=0;i<w;i++)
						grid[(Ox+i+20)%20]=1;
					for(i=0;i<4+w;i++)
						if(grid[(Ax+i)%20]==1) hit=true;
					agent->states[0]=grid[Ax];
					agent->states[1]=grid[(Ax+1)%20];
					agent->states[2]=grid[(Ax+paddleWidth+2)%20];
					agent->states[3]=grid[(Ax+paddleWidth+3)%20];
					agent->states[maxNodes-1]=0;
					agent->states[maxNodes-2]=0;
					agent->states[which]=setTo;
					E=0;
					I=0;
					H=0;
					M=0;
					if(w==4) E+=8;
					if(d==1) E+=4;
					int cm=(Ox+(w/2))-(Ax+4+paddleWidth);
					if(cm>=0) E+=2;
					if(hit) E+=1;
					for(i=0;i<4;i++)
						I=(I<<1)+agent->states[i];
					for(i=4;i<14;i++)
						H=(H<<1)+agent->states[i];
					for(i=14;i<16;i++)
						M=(M<<1)+agent->states[i];
					if(f!=NULL)
						fprintf(f,"%i	%i	",E,I);
					agent->updateStates();
					agent->states[which]=setTo;
					H=0;
					M=0;
					for(i=4;i<14;i++)
						H=(H<<1)+agent->states[i];
					for(i=14;i<16;i++)
						M=(M<<1)+agent->states[i];
					if(f!=NULL)
						fprintf(f,"	%i	%i	%i\n",H,M,(E<<14)+(I<<10)+H);
					int action=(agent->states[maxNodes-1])+(agent->states[maxNodes-2]<<1);
					switch (action) {
						case 0: case 3: break;
						case 1:Ax=(Ax+1)%20;break;
						case 2:Ax=(Ax-1+20)%20;break;
					}
					Ox=(Ox+20+d)%20;
				}
				if(w==2){
					if(hit){
						correct++;
					}
					else {
						incorrect++;
					}
					
				}
				else{
					if(!hit){
						correct++;
					}
					else {
						incorrect++;
					}
				}
			}
		}
	}
	agent->correct=correct;
	agent->incorrect=incorrect;
     */
}

double tGame::mutualInformation(vector<int> A,vector<int>B){
	set<int> nrA,nrB;
	set<int>::iterator aI,bI;
	map<int,map<int,double> > pXY;
	map<int,double> pX,pY;
	int i,j;
	double c=1.0/(double)A.size();
	double I=0.0;
	for(i=0;i<A.size();i++){
		nrA.insert(A[i]);
		nrB.insert(B[i]);
		pX[A[i]]=0.0;
		pY[B[i]]=0.0;
	}
	for(aI=nrA.begin();aI!=nrA.end();aI++)
		for(bI=nrB.begin();bI!=nrB.end();bI++){
			pXY[*aI][*bI]=0.0;
		}
	for(i=0;i<A.size();i++){
		pXY[A[i]][B[i]]+=c;
		pX[A[i]]+=c;
		pY[B[i]]+=c;
	}
	for(aI=nrA.begin();aI!=nrA.end();aI++)
		for(bI=nrB.begin();bI!=nrB.end();bI++)
			if((pX[*aI]!=0.0)&&(pY[*bI]!=0.0)&&(pXY[*aI][*bI]!=0.0))
				I+=pXY[*aI][*bI]*log2(pXY[*aI][*bI]/(pX[*aI]*pY[*bI]));
	return I;
	
}

double tGame::entropy(vector<int> list){
	map<int, double> p;
	map<int,double>::iterator pI;
	int i;
	double H=0.0;
	double c=1.0/(double)list.size();
	for(i=0;i<list.size();i++)
		p[list[i]]+=c;
	for (pI=p.begin();pI!=p.end();pI++) {
			H+=p[pI->first]*log2(p[pI->first]);	
	}
	return -1.0*H;
}

double tGame::ei(vector<int> A,vector<int> B,int theMask){
	set<int> nrA,nrB;
	set<int>::iterator aI,bI;
	map<int,map<int,double> > pXY;
	map<int,double> pX,pY;
	int i,j;
	double c=1.0/(double)A.size();
	double I=0.0;
	for(i=0;i<A.size();i++){
		nrA.insert(A[i]&theMask);
		nrB.insert(B[i]&theMask);
		pX[A[i]&theMask]=0.0;
		pY[B[i]&theMask]=0.0;
	}
	for(aI=nrA.begin();aI!=nrA.end();aI++)
		for(bI=nrB.begin();bI!=nrB.end();bI++){
			pXY[*aI][*bI]=0.0;
		}
	for(i=0;i<A.size();i++){
		pXY[A[i]&theMask][B[i]&theMask]+=c;
		pX[A[i]&theMask]+=c;
		pY[B[i]&theMask]+=c;
	}
	for(aI=nrA.begin();aI!=nrA.end();aI++)
		for(bI=nrB.begin();bI!=nrB.end();bI++)
			if((pX[*aI]!=0.0)&&(pY[*bI]!=0.0)&&(pXY[*aI][*bI]!=0.0))
				I+=pXY[*aI][*bI]*log2(pXY[*aI][*bI]/(pY[*bI]));
	return -I;
}
double tGame::computeAtomicPhi(vector<int>A,int states){
	int i;
	double P,EIsystem;
	vector<int> T0,T1;
	T0=A;
	T1=A;
	T0.erase(T0.begin()+T0.size()-1);
	T1.erase(T1.begin());
	EIsystem=ei(T0,T1,(1<<states)-1);
	P=0.0;
	for(i=0;i<states;i++){
		double EIP=ei(T0,T1,1<<i);
//		cout<<EIP<<endl;
		P+=EIP;
	}
//	cout<<-EIsystem+P<<" "<<EIsystem<<" "<<P<<" "<<T0.size()<<" "<<T1.size()<<endl;
	return -EIsystem+P;
}



double tGame::computeR(vector<vector<int> > table,int howFarBack){
	double Iwh,Iws,Ish,Hh,Hs,Hw,Hhws,delta,R;
	int i;
	for(i=0;i<howFarBack;i++){
		table[0].erase(table[0].begin());
		table[1].erase(table[1].begin());
		table[2].erase(table[2].begin()+(table[2].size()-1));
	}
	table[4].clear();
	for(i=0;i<table[0].size();i++){
		table[4].push_back((table[0][i]<<14)+(table[1][i]<<10)+table[2][i]);
	}
	Iwh=mutualInformation(table[0],table[2]);
    Iws=mutualInformation(table[0],table[1]);
    Ish=mutualInformation(table[1],table[2]);
    Hh=entropy(table[2]);
    Hs=entropy(table[1]);
    Hw=entropy(table[0]);
    Hhws=entropy(table[4]);
    delta=Hhws+Iwh+Iws+Ish-Hh-Hs-Hw;
    R=Iwh-delta;
  	return R;
}

double tGame::computeOldR(vector<vector<int> > table){
	double Ia,Ib;
	Ia=mutualInformation(table[0], table[2]);
	Ib=mutualInformation(table[1], table[2]);
	return Ib-Ia;
}

double tGame::predictiveI(vector<int>A){
	vector<int> S,I;
	S.clear(); I.clear();
	for(int i=0;i<A.size();i++){
		S.push_back((A[i]>>12)&15);
		I.push_back(A[i]&3);
	}
	return mutualInformation(S, I);
}

double tGame::nonPredictiveI(vector<int>A){
	vector<int> S,I;
	S.clear(); I.clear();
	for(int i=0;i<A.size();i++){
		S.push_back((A[i]>>12)&15);
		I.push_back(A[i]&3);
	}
	return entropy(I)-mutualInformation(S, I);
}
double tGame::predictNextInput(vector<int>A){
	vector<int> S,I;
	S.clear(); I.clear();
	for(int i=0;i<A.size();i++){
		S.push_back((A[i]>>12)&15);
		I.push_back(A[i]&3);
	}
	S.erase(S.begin());
	I.erase(I.begin()+I.size()-1);
	return mutualInformation(S, I);
}

void tGame::loadExperiment(char *filename){
    theExperiment.loadExperiment(filename);
}


//** tOctuplet implementation
void tOctuplet::loadOctuplet(FILE *f){
    int i,IN;
    data.clear();
    data.resize(8);
    for(i=0;i<8;i++){
        fscanf(f,"  %i",&IN);
        data[i]=IN;
    }
}

//** tEperiment class implementations
void tExperiment::loadExperiment(char *filename){
    FILE *f=fopen(filename,"r+t");
    int i,j,k;
    fscanf(f,"%i:",&j);
    dropSequences.resize(j);
    for(i=0;i<dropSequences.size();i++)
        dropSequences[i].loadOctuplet(f);
    fscanf(f,"%i:",&j);
    sizeSequences.resize(j);
    for(i=0;i<sizeSequences.size();i++)
        sizeSequences[i].loadOctuplet(f);
    fscanf(f,"%i:",&j);
    selfSequences.resize(j);
    for(i=0;i<selfSequences.size();i++)
        selfSequences[i].loadOctuplet(f);
    shouldHit.resize(drops());
    for(i=0;i<shouldHit.size();i++){
        shouldHit[i].resize(sizes());
        for(j=0;j<shouldHit[i].size();j++){
            shouldHit[i][j].resize(selves());
            for(k=0;k<shouldHit[i][j].size();k++){
                int l;
                fscanf(f,"%i\n",&l);
                if(l==1)
                    shouldHit[i][j][k]=true;
                else
                    shouldHit[i][j][k]=false;
            }
        }
    }
    fclose(f);
}

void tExperiment::showExperimentProtokoll(void){
    int i,j,k;
    printf("drop directions: %i\n",drops());
    for(i=0;i<drops();i++){
        printf("%i:",i);
        for(j=0;j<8;j++)
            printf("    %i",dropSequences[i].data[j]);
        printf("\n");
    }
    printf("drop sizes: %i\n",sizes());
    for(i=0;i<sizes();i++){
        printf("%i:",i);
        for(j=0;j<8;j++)
            printf("    %i",sizeSequences[i].data[j]);
        printf("\n");
    }
    printf("self sizes: %i\n",selves());
    for(i=0;i<selves();i++){
        printf("%i:",i);
        for(j=0;j<8;j++)
            printf("    %i",selfSequences[i].data[j]);
        printf("\n");
    }
    printf("should hit\n%i means true\nD  B   S   catch\n",(int)true);
    for(i=0;i<shouldHit.size();i++)
        for(j=0;j<shouldHit[i].size();j++)
            for(k=0;k<shouldHit[i][j].size();k++)
                printf("%i  %i  %i  %i\n",i,j,k,(int)shouldHit[i][j][k]);
}

int tExperiment::drops(void){
    return (int) dropSequences.size();
}

int tExperiment::sizes(void){
    return (int) sizeSequences.size();
}

int tExperiment::selves(void){
    return (int) selfSequences.size();
    
}




