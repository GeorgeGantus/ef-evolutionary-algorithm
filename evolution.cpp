#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <cstring>

#define MUTATION_RATE 1
#define RANDOM_SEED 32
#define POPULATION_SIZE 3

using namespace std;

float get_rand() {
    return (float) rand()/RAND_MAX;
}

int get_max_index(vector<float> &array) {
    int i = 0;
    float current = array[0];
    for (int j = 0; j < array.size(); j ++){
        if (array[j] > current){
            i = j;
            current = array[j];
        }
    }
    return i;
}

int get_best(vector<vector<int>> &population, vector<float> &results) {
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

// Needs refactor
vector<int> mutate(vector<int> &params) {
    vector<int> mutated;
    for (int i = 0; i < params.size(); i++) {
        mutated.push_back(params[i] + get_single_mutation());
    }
    return mutated;
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

float get_fitness(char *command) {
    FILE *fpipe;
    char ans[256];

    if (!(fpipe = (FILE*)popen(command,"r"))){
        cerr << "Falha ";
        exit(1);
    }

    fgets(ans, sizeof ans, fpipe);

    pclose(fpipe);

    float fitness = stof(ans);
    return fitness;
}

vector<vector<int>> intiialize_population(vector<int> seed, int pop_size){
    vector<vector<int>> population;
    population.push_back(seed);
    for (int i = 0; i < pop_size -1; i++){
        population.push_back(mutate(seed));
    }
    return population;
}

int main () {
    srand(RANDOM_SEED);

    system("g++ test.cpp");

    vector<int> pop_seed = {10,10};
    vector<vector<int>> population;
    population = intiialize_population(pop_seed, POPULATION_SIZE);


    vector<int> best;
    for (int i = 0; i < 200; i++){

        vector<float> results;

        for (int j = 0; j < population.size(); j++){
            string command_s = params_to_command("./a.out", population[j]);
            char command[command_s.length() + 1];
            strcpy(command, command_s.c_str());

            float fitness = get_fitness(command);
            results.push_back(fitness);
        }

        int best_index = get_best(population, results);
        best = population[best_index];

        population = elitism(best,population);
        population = mutate_population(population, best_index);
    }
    cout << "The root is:" << endl;
    print_vector(best);

    

    return 0;
}