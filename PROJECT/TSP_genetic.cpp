#include "utilities.h"

using namespace std;

vector<City> cities;
int nCities;
int nGeneration;
int nChromosomes;
double mutationRate;
vector<Chromosome> chromosomes;



vector<Chromosome> nextGeneration(vector<Chromosome> oldGen, vector<double> fitness){
	/*
	input:
		oldGen: vector of chromosomes representing previous generation
		fitness: vector of fitness value
	output:
		vector of chromosomes representing new generation
		computed by choose two random parent from oldGen,
		from they find the son and mutate it. 
		repeat until the newGen has the same size of oldGen.
	*/
	
	//first half of generation obtained by copying previous one
	int index = int(nChromosomes/2)+nChromosomes%2;
	vector<Chromosome> newGen = findNBests(chromosomes,index);
	
	//second half obtained by croosbreeding and mutation
	for(int i = 0; i<nChromosomes/2; i++){
		int parentA = randomSampling(fitness);
		int parentB = randomSampling(fitness);
		while(parentA == parentB){
			parentB = randomSampling(fitness);
		}
		Chromosome son;
		son.path = reproduce(oldGen[parentA].path, oldGen[parentB].path);
		son.path = mutate(son.path, mutationRate);
		son.length = computePathLength(son.path, cities);
		newGen.push_back(son);
	}
	return newGen;
}

int main(int argc, char **argv){
	auto start = std::chrono::high_resolution_clock::now();
	int xSize;
	int ySize;
	if(argc != 7){
		cout << "Number of Cities,\nNumber of Chomosomes,\nNumber of Generation,\nMutation Rate,\nMap x size,\nMap y size\n";
		return 0;
	}else{
		nCities = atoi(argv[1]);
		if(nCities < 2){
			cout << "INFO: Insufficient number of cities\n";
			return 0;
		}
		nChromosomes = atoi(argv[2]);
		nGeneration = atoi(argv[3]);
		mutationRate = atof(argv[4]);
		if(mutationRate> 1){
			cout << "INFO: mutation rate is greater than 1, it has been set to 1\n\n";
		}
		xSize = atoi(argv[5]);
		ySize = atoi(argv[6]);
	}

	//Creation of Cities
	srand( time(NULL));
	for (int i = 0; i<nCities; i++){
		City c;
		c.x = rand() % xSize;
		c.y = rand() % ySize;
		cities.push_back(c);
	}


	//creation of chromosomes
	for(int i = 0; i<nChromosomes; i++){
		Chromosome myChromosome;
		vector<int> path;
		for(int j = 0; j<nCities; j++){
			path.push_back(j);
		}
		random_shuffle(path.begin(), path.end());
		myChromosome.path = path;
		myChromosome.length = computePathLength(myChromosome.path, cities);
		chromosomes.push_back(myChromosome);
	}

	//evolve
	vector<double> fitness;
	fitness = computeFitness(chromosomes);

	for(int i = 0; i<nGeneration; i++){
		chromosomes = nextGeneration(chromosomes, fitness);
		fitness = computeFitness(chromosomes);
		cout <<  "Generation: " << i << "\n";
	}

	//print cities
	cout << "\nCoordinates of cities:\n";
	for(int i = 0; i<nCities; i++){
		cout << cities[i].x << ", " << cities[i].y << "\n";
	}
	//find best path of last generation
	Chromosome best = findBest(chromosomes);
	cout << "\n\nAfter " << nGeneration << " generations the best path found is: \n";
	for(int i = 0; i<nCities; i++){
		cout << best.path[i] << " ";
	}
	cout << "\nWith length of: " << best.length << "\n";
	
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Computed in: "<< std::fixed << elapsed.count()*1000.0 << " ms\n" ;
	return 0;
}
