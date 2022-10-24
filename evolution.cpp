#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <cstring>

#define MUTATION_RATE 1
#define RANDOM_SEED 32
#define POPULATION_SIZE 3

using namespace std;

double get_rand() {
    return (double) rand()/RAND_MAX;
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

pair<vector<float>,int> get_best(vector<vector<float>> &population, vector<float> &results) {
    pair<vector<float>, int> ret;
    int max_index = get_max_index(results);
    ret.first = population[max_index];
    ret.second = max_index;
    return ret;
}

int rand_sign() {
    if (get_rand() > 0.5) {
        return 1;
    } else {
        return -1;
    }
}

float get_single_mutation() {
    return (get_rand()*rand_sign()*MUTATION_RATE);
}

vector<float> mutate(vector<float> &params) {
    vector<float> mutated;
    for (int i = 0; i < params.size(); i++) {
        mutated.push_back(params[i] + get_single_mutation());
    }
    return mutated;
}

vector<vector<float>> mutate_population(vector<vector<float>> &population, int best_index) {
    vector<vector<float>> mutated_population;
    for (int i = 0; i < population.size(); i++){
        vector<float> params = population[i];
        if (i != best_index) {
            params = mutate(population[i]);
        }
        mutated_population.push_back(params);
    }
    return mutated_population;
}

vector<vector<float>> elitism(vector<float> &best, vector<vector<float>> &population) {
    vector<vector<float>> bred_population;

    for (vector<float> ind : population) {
        vector<float> bred_ind;
        for (int i = 0; i < best.size(); i++){
            bred_ind.push_back((best[i] + ind[i]) / 2);
        }
        bred_population.push_back(bred_ind);
    }
    return bred_population;

}

void print_vector (vector<float> test) {
    for (float a : test){
        cout << a << endl;
    }
}
void print_population(vector<vector<float>> population) {
    cout << "### PRINT POPULATION ###" << endl;
    for (vector<float> ind : population){
        cout << "ind" << endl;
        print_vector(ind);
    }
}

string params_to_command(string cmd, vector<float> &params) {
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

vector<vector<float>> intiialize_population(vector<float> seed, int pop_size){
    vector<vector<float>> population;
    population.push_back(seed);
    for (int i = 0; i < pop_size -1; i++){
        population.push_back(mutate(seed));
    }
    return population;
}

int main () {
    srand(RANDOM_SEED);

    system("g++ test.cpp");

    vector<float> pop_seed = {10,10};
    vector<vector<float>> population;
    population = intiialize_population(pop_seed, POPULATION_SIZE);


    vector<float> best;
    for (int i = 0; i < 200; i++){

        vector<float> results;
        for (int j = 0; j < population.size(); j++){
            string command_s = params_to_command("./a.out", population[j]);
            char command[command_s.length() + 1];
            strcpy(command, command_s.c_str());
            float fitness = get_fitness(command);
            results.push_back(fitness);
        }
        pair<vector<float>, int> best_data = get_best(population, results);
        best = best_data.first;
        int best_index = best_data.second;
        /* print_vector(results);
        cout << "BEST_INDEX: " <<best_index << endl;
        cout << "#######" << endl; */
        population = elitism(best,population);
        population = mutate_population(population, best_index);
    }
    cout << "The root is:" << endl;
    print_vector(best);

    

    return 0;
}