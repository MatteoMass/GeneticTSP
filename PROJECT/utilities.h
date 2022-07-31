#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;


class Chromosome{
	public:
		double length;
		vector<int> path;
};

class City{
	public:
		int x;
		int y;
};


double randomDouble(){
	return ((double)rand()/(double)(RAND_MAX));
}


vector<int> reproduce(vector<int> A, vector<int> B){
	/*
	input:
		path of parent A,
		path of parent B, 
	output:
		path computed by remove a sequence from A, 
		and by adding to this the missing cities,
		in the same order in which they are in B.
		A: 0 1 2 3 4
		B: 4 3 2 1 0
		remove from A: 0 1
		new vector: 2 3 4 1 0
	*/
	int nCities = A.size(); 
	int start = rand()%(nCities-1);
	int end = rand()%(nCities-start) + start;
	vector<int> path;
	for(int i = 0; i<nCities; i++){
		if(i<start or i>end){
			path.push_back(A[i]);
		}
	}
	for(int i = 0; i<nCities; i++){
		if(find(path.begin(), path.end(), B[i]) == path.end()){
			path.push_back(B[i]);
		}
	}
	return path;
}

vector<int> mutate(vector<int> chromosome,double mutationRate){
	/*
	input:
		path of a chromosome
	output:
		mutate path
		if the random double is lower or equal than the mutation rate,
		than swap two random position of the vector.
	*/
	int nCities = chromosome.size();
	double r = randomDouble();
	if(r <= mutationRate){
		int first = rand()%nCities;
		int second = rand()%nCities;
		while(second == first){
			second = rand()%nCities;
		}
		swap(chromosome[first], chromosome[second]);
	}
	return chromosome;
}

int randomSampling(vector<double> fitness){
	/*
	input:
		vector of double containing fitness value of each chromosome
	output:
		the chromosome's index random picked.
	*/
	int nChromosomes = fitness.size();
	double r = randomDouble();
	double count =  0.0;
	for (int i = 0; i<nChromosomes; i++){
		if(r > count && r < count+fitness[i]){
			return i;
		}else{
			count += fitness[i];
		}
	}
	return -1;
}

double distance(City A, City B){
	/*
	input:
		two cities identifyed by their coordinates
	output:
		distance d
		computed by the sqare root of the sum of 
		the sqare of the differences of the value on each axes
	*/
	double d;
	d = sqrt(pow(B.x-A.x,2.0) + pow(B.y-A.y,2.0));
	return d;
}


double computePathLength(vector<int> chromosome, vector<City> cities){
	/*
	input:
		path of a chromosome
	output:
		the length of the path
		computd summing up the distances between each 
		couple of near city in the path
	*/
	int nCities = chromosome.size();
	double d = 0.0;
	for(int i = 0; i<nCities; i++){
		int A = chromosome[i];
		int B = chromosome[i+1]%nCities;
		d += distance(cities[A], cities[B]);
	}
	return d;
}


vector<double> computeFitness(vector<Chromosome> chromosomes){
	/*
	input:
		vector of chromosomes
	output:
		vector containing fitness value of each chromosome,
		computed from each path length, 
		an high fitness is associated with a shortest path
	*/
	int nChromosomes = chromosomes.size();
	double sum = 0.0;
	for(int i = 0; i< nChromosomes; i++){
		sum += chromosomes[i].length;
	}
	vector<double> support;
	double sum2 = 0.0;
	for(int i = 0; i< nChromosomes; i++){
		double fit = sum/chromosomes[i].length;
		support.push_back(fit);
		sum2 += fit;
	}

	vector<double> fitness;
	for(int i = 0; i< nChromosomes; i++){
		double fit = support[i]/sum2;
		fitness.push_back(fit);

	}
	return fitness;
}


Chromosome findBest(vector<Chromosome> chromosomes){
	/*
	input: 
		vector of chromosomes
	output:
		chromosome with the shortest path
		and its length
	*/
	int nChromosomes = chromosomes.size();
	Chromosome best;
	for(int i = 0; i < nChromosomes; i++){
		if(i == 0){
			best.path = chromosomes[i].path;
			best.length = chromosomes[i].length;
		}else{
			if(chromosomes[i].length < best.length){
				best.path = chromosomes[i].path;
				best.length = chromosomes[i].length;
			}
		}
	}
	return best;
}

bool compare(Chromosome A, Chromosome B){
	return A.length < B.length;
}

vector<Chromosome> findNBests(vector<Chromosome> chromosomes, int index){
	/*
	input:
		a vector of chromosomes and an int
	output:
		first *index* position of ordered chromosomes vector
	*/
	sort(chromosomes.begin(), chromosomes.end(), compare);
	chromosomes.resize(index);
	return chromosomes;
}