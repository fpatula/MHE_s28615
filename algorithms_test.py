import psutil, subprocess, statistics as s

NUMBER_OF_UNIQUE_COLORS = "Number of unique colors:"
EXECUTION_TIME = "Execution time (ms):"

def main():
    simulated_annealing_result = simulated_annealing_test()
    print("Best params for simulated annealing: ", simulated_annealing_result[0])
    print("Average colors for simulated annealing: ", simulated_annealing_result[1])
    print("Colors standard deviation for simulated annealing: ", simulated_annealing_result[2])
    print("Average execution time for simulated annealing (in ms): ", simulated_annealing_result[3])
    print("Average memory usage for simulated annealing (in kB): ", simulated_annealing_result[4])

    genetic_algorithm_result = genetic_algorithm_test()
    print("Best params for genetic algorithm: ", genetic_algorithm_result[0])
    print("Average colors for genetic algorithm: ", genetic_algorithm_result[1])
    print("Colors standard deviation for genetic algorithm: ", genetic_algorithm_result[2])
    print("Average execution time for genetic algorithm (in ms): ", genetic_algorithm_result[3])
    print("Average memory usage for genetic algorithm (in kB): ", genetic_algorithm_result[4])


def simulated_annealing_test():
    simulated_annealing_results = []
    for i in range(0, 15):
        temperature = "log"
        max_iterations = 100 + 500 * i
        run_simulated_annealing(max_iterations, simulated_annealing_results, temperature)
    for i in range(0, 15):
        temperature = "standard"
        max_iterations = 100 + 500 * i
        run_simulated_annealing(max_iterations, simulated_annealing_results, temperature)
    simulated_annealing_results.sort(key=lambda x: (x['colors'], x['execution_time']))
    best_annealing_result = simulated_annealing_results[0]
    best_simulated_annealing_results = []
    for i in range(0, 25):
        run_simulated_annealing(best_annealing_result['params']['max_iterations'], best_simulated_annealing_results,
                                best_annealing_result['params']['temperature'])
    return best_annealing_result['params'], s.mean(x['colors'] for x in best_simulated_annealing_results), s.stdev(x['colors'] for x in best_simulated_annealing_results), s.mean(x['execution_time'] for x in best_simulated_annealing_results), s.mean(x['memory'] for x in best_simulated_annealing_results)


def run_simulated_annealing(max_iterations, simulated_annealing_results, temperature):
    result = subprocess.Popen(
        ["./main.exe", "./graph_matrix2.txt", "simulated_annealing", str(max_iterations), temperature],
        stdout=subprocess.PIPE)
    save_results(result, simulated_annealing_results, {"temperature": temperature, "max_iterations": max_iterations})

def genetic_algorithm_test():
    genetic_results = []
    for i in range(0, 3):
        last_generation = 10 + i * 5
        for j in range(0, 4):
            migration_rate = 1 + j
            for k in range(0, 3):
                migration_gap = 3 + k
                for l in range(0, 3):
                    islands_number = 2 + l
                    for n in range(0, 5):
                        max_population_size = 8 + n * 2
                        run_genetic_algorithm(genetic_results,  islands_number, migration_rate, migration_gap, last_generation, max_population_size)
    genetic_results.sort(key=lambda x: (x['colors'], x['execution_time']))
    best_genetic_result = genetic_results[0]
    best_genetic_results = []
    for i in range(0, 25):
        run_genetic_algorithm(best_genetic_results,  best_genetic_result['params']['islands_number'], best_genetic_result['params']['migration_rate'], best_genetic_result['params']['migration_gap'], best_genetic_result['params']['last_generation'], best_genetic_result['params']['max_population_size'])
    return best_genetic_result['params'], s.mean(x['colors'] for x in best_genetic_results), s.stdev(x['colors'] for x in best_genetic_results), s.mean(x['execution_time'] for x in best_genetic_results), s.mean(x['memory'] for x in best_genetic_results)


def run_genetic_algorithm(genetic_results, islands_number, migration_rate, migration_gap, last_generation, max_population_size):
    result = subprocess.Popen(
        ["./main_genetic_island.exe", "./graph_matrix2.txt", str(islands_number), str(migration_rate), str(migration_gap), str(last_generation), str(max_population_size)],
        stdout=subprocess.PIPE)
    save_results(result, genetic_results, {"islands_number": islands_number, "migration_rate": migration_rate, "migration_gap": migration_gap, "last_generation": last_generation, "max_population_size": max_population_size})

def save_results(result, results, params):
    stdout = result.stdout
    process = psutil.Process(result.pid)
    memory = process.memory_info()
    read_stdout = stdout.read().decode()
    colors_index = read_stdout.find(NUMBER_OF_UNIQUE_COLORS) + len(NUMBER_OF_UNIQUE_COLORS) + 1
    colors = read_stdout[colors_index:colors_index + 1]
    execution_time_index = read_stdout.find(EXECUTION_TIME) + len(EXECUTION_TIME) + 1
    execution_time = read_stdout[execution_time_index:]
    results.append({"params": params, "memory": memory.rss >> 10, "colors": int(colors),
                                        "execution_time": int(execution_time.strip())})


if __name__ == "__main__":
    main()