#include <thread>
#include <mutex>
#include "utilities.h"

#include <ff/parallel_for.hpp>


using namespace std;


vector<City> cities;
mutex mtx;
int nCities;
int nChromosomes;
int nGeneration;
double mutationRate;
int nWorker;



int main(int argc, char **argv){
	auto start = std::chrono::high_resolution_clock::now();
	int xSize;
	int ySize;
	if(argc != 8){
		cout << "Number of Cities,\nNumber of Chomosomes,\nNumber of Generation,\nMutation rate,\nNumber of Thread,\nMap x size,\nMap y size\n";
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
		nWorker = atoi(argv[5]);
		if(nWorker <= 0){
			cout << "INFO: nWorker given is 0, it has been set to 1\n\n";
			nWorker = 1;
		}
		if(nWorker > nChromosomes){
			cout << "INFO: nWorker given is greter than nChromosomes, it has been set to nChromosomes\n\n";
			nWorker = nChromosomes;
		}
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
	vector<Chromosome> chromosomes;
	ff::ParallelFor pf_creation(nWorker);
	pf_creation.parallel_for(0, nChromosomes, [&](const int i){
		Chromosome myChromosome;
		vector<int> path;
		for(int j = 0; j<nCities; j++){
			path.push_back(j);
		}
		random_shuffle(path.begin(), path.end());
		myChromosome.path = path;
		myChromosome.length = computePathLength(myChromosome.path,  cities);
		mtx.lock();
		chromosomes.push_back(myChromosome);
		mtx.unlock();
	});


	vector<double> fitness;
	fitness = computeFitness(chromosomes);

	int k = nChromosomes/nWorker;
	int r = nChromosomes%nWorker;


	for (int j = 0; j < nGeneration; j++){
		
		//first half of generation obtained by copying previous one
		int index = int(nChromosomes/2)+nChromosomes%2;
		vector<Chromosome> newGen = findNBests(chromosomes,index);
		
		//second half obtained by croosbreeding and mutation
		ff::ParallelFor pf(nWorker);
		pf.parallel_for(0, chromosomes.size()/2, [&](const int i){
			int parentA = randomSampling(fitness);
			int parentB = randomSampling(fitness);
			while(parentA == parentB){
				parentB = randomSampling(fitness);
			}
			Chromosome son;
			son.path = reproduce(chromosomes[parentA].path, chromosomes[parentB].path);
			son.path = mutate(son.path, mutationRate);
			son.length = computePathLength(son.path, cities);
			mtx.lock();
			newGen.push_back(son);
			mtx.unlock();
		});
		chromosomes = newGen;
		fitness = computeFitness(chromosomes);
		cout << "Generation: " << j<< "\n";
	}
	

	//print cities
	cout << "Coordinates of cities:\n";
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