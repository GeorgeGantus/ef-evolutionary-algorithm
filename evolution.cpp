#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>

#define MUTATION_RATE 5
#define RANDOM_SEED 32
#define POPULATION_SIZE 3
#define MAX_GENERATIONS 200
#define GENS_TO_BALANCE 30

using namespace std;

float get_rand() {
    return (float) rand()/RAND_MAX;
}

int get_max_index(vector<int> &array) {
    int i = 0;
    int current = array[0];
    for (int j = 0; j < array.size(); j ++){
        if (array[j] > current){
            i = j;
            current = array[j];
        }
    }
    return i;
}

int get_best(vector<vector<int>> &population, vector<int> &results) {
    int max_index = get_max_index(results);
    return max_index;
}

int rand_sign() {
    if (get_rand() > 0.5) {
        return 1;
    } else {
        return -1;
    }
}

int get_single_mutation() {
    return static_cast<int>(get_rand()*rand_sign()*MUTATION_RATE);
}

int get_mutation_amount() {
    float rate = get_rand();

    if (rate < 0.7){
        return 1;
    }else if (rate < 0.95) {
        return 2;
    } else {
        return 3;
    }
}

vector<int> get_genes_to_mutate(int params_size){

    vector<int> indexes;

    int mutate_amount = get_mutation_amount();
    mutate_amount = min(mutate_amount, params_size);
    
    vector<int> genes(params_size);
    for (int i = 0; i < params_size; i++) genes.push_back(i);
    
    random_shuffle(genes.begin(), genes.end());

    for (int i = 0; i < mutate_amount; i++) {
        indexes.push_back(genes[i]);
    }
    return indexes;
}

vector<int> mutate(vector<int> &params) {
    vector<int> mutated;
    for (int index : get_genes_to_mutate(params.size())) {
        params[index] = max(params[index] + get_single_mutation(), 0); // params must be above 0 
    }
    return params;
}

vector<vector<int>> mutate_population(vector<vector<int>> &population, int best_index) {
    vector<vector<int>> mutated_population;
    for (int i = 0; i < population.size(); i++){
        vector<int> params = population[i];
        if (i != best_index) {
            params = mutate(population[i]);
        }
        mutated_population.push_back(params);
    }
    return mutated_population;
}

vector<vector<int>> elitism(vector<int> &best, vector<vector<int>> &population) {
    vector<vector<int>> bred_population;

    for (vector<int> ind : population) {
        vector<int> bred_ind;
        for (int i = 0; i < best.size(); i++){
            bred_ind.push_back((best[i] + ind[i]) / 2);
        }
        bred_population.push_back(bred_ind);
    }
    return bred_population;

}

void print_vector (vector<int> array) {
    for (int a : array){
        cout << a << endl;
    }
}

string params_to_command(string cmd, vector<int> &params) {
    for (int i = 0; i < params.size(); i++){
        cmd += " " + to_string(params[i]);
    }
    return cmd;
}

int get_fitness(vector<int> &individual) {

    string command_s = params_to_command("./a.out", individual);
    char command[command_s.length() + 1];
    strcpy(command, command_s.c_str());

    FILE *fpipe;
    char ans[256];

    if (!(fpipe = (FILE*)popen(command,"r"))){
        cerr << "Falha ao iniciar processo";
        exit(1);
    }

    fgets(ans, sizeof ans, fpipe);
    pclose(fpipe);

    int fitness = stoi(ans);
    return fitness;
}

vector<int> get_population_fitness(vector<vector<int>> &population) {
    vector<int> results;
    for (int j = 0; j < population.size(); j++){
        int fitness = get_fitness(population[j]);
        results.push_back(fitness);
    }
    return results;
}

vector<vector<int>> intiialize_population(vector<int> seed, int pop_size){
    vector<vector<int>> population;
    population.push_back(seed);
    for (int i = 0; i < pop_size -1; i++){
        population.push_back(mutate(seed));
    }
    return population;
}

void write_best_to_file(int index, vector<int> &best, ofstream &myfile, int fitness) {
    myfile << index << " ";
    for (int value : best) {
        myfile << value << " ";
    }
    myfile << fitness;
    /* int pos = myfile.tellp();
    myfile.seekp(pos-1); */
    myfile << "\n";
}


int main () {
    srand(RANDOM_SEED);

    system("g++ test.cpp");

    vector<int> pop_seed = {2, 2};
    vector<vector<int>> population;
    population = intiialize_population(pop_seed, POPULATION_SIZE);
    
    ofstream myfile;
    myfile.open ("output.txt");

    vector<int> best;
    int prev_best_fitness = -1;
    int flag = 0;

    for (int i = 0; i < MAX_GENERATIONS; i++){

        vector<int> results;
        results = get_population_fitness(population);

        int best_index = get_best(population, results);

        if (prev_best_fitness == results[best_index]){
            flag += 1;
            if (flag == GENS_TO_BALANCE) {
                break;
            }
        }else{
            flag = 0;
        }

        prev_best_fitness = results[best_index];
        best = population[best_index];
        write_best_to_file(i, best, myfile, results[best_index]);


        population = elitism(best,population);
        population = mutate_population(population, best_index);
    }

    if (flag == GENS_TO_BALANCE) {
        myfile << "Ended by estabilization.\n";
    } else {
        myfile << "Ended by max generations.\n";
    }


    cout << "The root is:" << endl;
    print_vector(best);
    myfile.close();
    

    return 0;
}