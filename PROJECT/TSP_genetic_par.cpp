#include <thread>
#include <mutex>
#include "utilities.h"

using namespace std;


vector<City> cities;
mutex mtx;
int nCities;
int nChromosomes;
int nGeneration;
double mutationRate;
int nThread;

int threadCreation(vector<Chromosome>& chromosomes, int population){
	for(int i = 0; i<population; i++){
		Chromosome myChromosome;
		vector<int> path;
		for(int j = 0; j<nCities; j++){
			path.push_back(j);
		}
		random_shuffle(path.begin(), path.end());
		myChromosome.path = path;
		myChromosome.length = computePathLength(myChromosome.path, cities);
		mtx.lock();
		chromosomes.push_back(myChromosome);
		mtx.unlock();
	}
	return 0;
}

int threadJob(vector<Chromosome> oldGen, vector<Chromosome>& newGen,vector<double> fitness, int population){
	/*
	input:
		oldGen: vector of chromosomes representing previous generation
		newGen: vector of chromosomes where new individual are pushed
		fitness: vector of fitness value
		population: number of individuals that a thread has to create
	*/
	for(int i = 0; i<population; i++){
		int parentA = randomSampling(fitness);
		int parentB = randomSampling(fitness);
		while(parentA == parentB){
			parentB = randomSampling(fitness);
		}
		Chromosome son;
		son.path = reproduce(oldGen[parentA].path, oldGen[parentB].path);
		son.path = mutate(son.path, mutationRate);
		son.length = computePathLength(son.path, cities);
		mtx.lock();
		newGen.push_back(son);
		mtx.unlock();
	}
	return 0;

}




int main(int argc, char **argv){
	auto start = std::chrono::high_resolution_clock::now();
	int xSize;
	int ySize;

	if(argc != 8){
		cout << "Number of Cities,\nNumber of Chomosomes,\nNumber of Generation,\nMutation Rate,\nNumber of Threads,\nMap x size,\nMap y size\n";
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
		nThread = atoi(argv[5]);
		if(nThread <= 0){
			cout << "INFO: nThread given is 0, it has been set to 1\n\n";
			nThread = 1;
		}
		if(nThread > nChromosomes){
			cout << "INFO: nThread given is greater than nChromosomes, it has been set to nChromosomes\n\n";
			nThread = nChromosomes;
		}
		if(mutationRate> 1){
			cout << "INFO: mutation rate is greater than 1, it has been set to 1\n\n";
		}
		xSize = atoi(argv[6]);
		ySize = atoi(argv[7]);
	}


	//Creation of Cities
	srand( time(NULL));
	for (int i = 0; i<nCities; i++){
		City c;
		c.x = rand() % xSize;
		c.y = rand() % ySize;
		cities.push_back(c);
	}



	int integerPart = nChromosomes/nThread;
	int restDivision = nChromosomes%nThread;


	//creation of chromosomes
	vector<Chromosome> chromosomes;
	vector<thread> threads_creation; 
	for(int i = 0; i<restDivision; i++){
		threads_creation.push_back(thread(threadCreation, ref(chromosomes), integerPart +1));
	}
	for(int i = restDivision; i<nThread; i++){
		threads_creation.push_back(thread(threadCreation, ref(chromosomes), integerPart ));
	}

	for(int i = 0; i < nThread; i++){
		threads_creation[i].join();
	}

	vector<double> fitness;
	fitness = computeFitness(chromosomes);

	integerPart = (nChromosomes/2)/nThread;
	restDivision = (nChromosomes/2)%nThread;

	for (int j = 0; j < nGeneration; j++){
		
		//first half of generation obtained by copying previous one
		int index = int(nChromosomes/2)+nChromosomes%2;
		vector<Chromosome> newGen = findNBests(chromosomes,index);
		
		//second half obtained by croosbreeding and mutation
		vector<thread> threads;
		for(int i = 0; i<restDivision; i++){
			threads.push_back(thread(threadJob, chromosomes, ref(newGen), fitness, integerPart +1));
		}
		for(int i = restDivision; i<nThread; i++){
			threads.push_back(thread(threadJob, chromosomes, ref(newGen), fitness, integerPart ));
		}

		for(int i = 0; i < nThread; i++){
			threads[i].join();
		}

		chromosomes = newGen;
		fitness = computeFitness(chromosomes);
		cout<<"Generation: " << j <<"\n";
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