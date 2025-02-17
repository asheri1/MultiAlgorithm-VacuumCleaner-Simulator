#include <iostream>
#include <filesystem>
#include <vector>
#include <thread>
#include <atomic>
#include <dlfcn.h>
#include <fstream> 
#include "common/Utils.h"
#include "common/AlgorithmRegistrar.h"
#include "algorithm/AlgorithmRegistration.h"
#include "simulator/MySimulator.h"




class Task {
public:
    std::unique_ptr<AbstractAlgorithm> algorithm;
    std::string algo_name;
    std::string house_path;
    double score;

    Task(std::unique_ptr<AbstractAlgorithm>&& algorithm, const std::string& algo_name, const std::string& house_path)
        : algorithm(std::move(algorithm)), algo_name(algo_name), house_path(house_path), score(0.0) {}
};



std::atomic<long> num_of_tasks(0);
std::vector<Task> tasks;


void thread_function(bool summary_only) {
    while (true) {
        size_t current = num_of_tasks++;
        if (current >= tasks.size()) {
            return;
        }

        Task& task = tasks[current];

        std::cout << "task number " << current << ", algorithm " << task.algo_name << ", house " << task.house_path <<  " tid " << gettid() << std::endl;

        // std::unique_ptr<AbstractAlgorithm> algorithm = std::move(task.algorithm);
        std::string house_path = task.house_path;

        MySimulator simulator;
        if (summary_only)
            simulator.onlySummaryMode();
        simulator.readHouseFile(house_path);
        simulator.setAlgorithm(*task.algorithm);
        simulator.setAlgoName(task.algo_name);
        simulator.run();

        // Store the simulation result in the task
        tasks[current].score = simulator.getScore();
    }
}

bool convertToNumThreads(const std::string& num_threads_str, int& num_threads) {
    // Check if the string is a valid number
    for (char c : num_threads_str) {
        if (!std::isdigit(c)) {
            std::cerr << "Error: num_threads must be a positive integer.\n";
            return false; // Return false if the string contains non-digit characters
        }
    }
    try {
        // Convert the string to an integer
        num_threads = std::stoi(num_threads_str);
        if (num_threads < 1)
        {
            std::cerr << "Error: num_threads must be a positive integer.\n";
            return false;
        }
        
    } 
    catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid argument for num_threads.\n";
        return false; // Return false if std::stoi fails to convert the string
    } 
    return true; 
}


void output_to_csv(AlgorithmRegistrar& registrar, const std::vector<std::string>& house_list) {
    std::ofstream csv_file("summary.csv");
    if (!csv_file.is_open()) {
        std::cerr << "Unable to open summary.csv for writing." << std::endl;
        return;
    }

    // Write header row with house names
    csv_file << "Algorithm/House";
    for (const std::string& house : house_list) {
        csv_file << "," << std::filesystem::path(house).stem().string();
    }
    csv_file << "\n";

    // Write each algorithm's scores for each house
    for (const auto& algo : registrar) {
        csv_file << algo.name();
        for (const std::string& house : house_list) {
            auto it = std::find_if(tasks.begin(), tasks.end(), [&algo, &house](const Task& task) {
                return task.algo_name == algo.name() && task.house_path == house;
            });

            if (it != tasks.end()) {
                csv_file << "," << it->score;
            } else {
                csv_file << ",N/A"; // In case no score was found
            }
        }
        csv_file << "\n";
    }

    csv_file.close();
}

void parse_arguments(int argc, char** argv, std::string& house_path, std::string& algo_path, int& num_threads, bool& summary_only) {
    std::string num_threads_str;

    // assigned to default value.
    summary_only = false;
    num_threads = 10; 
    house_path = "."; 
    algo_path = "."; 

    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg.rfind("-house_path=", 0) == 0) {
            house_path = arg.substr(std::string("-house_path=").size(), arg.size());
        }
        else if (arg.rfind("-algo_path=", 0) == 0) {
            algo_path = arg.substr(std::string("-algo_path=").size(), arg.size());
        }
        else if (arg.rfind("-num_threads=", 0) == 0) {
            num_threads_str = arg.substr(std::string("-num_threads=").size(), arg.size());
        }
        else if (arg == "-summary_only") {
            summary_only = true;
        }
    }

    if (house_path == "") {
        std::cout << "Missing house path." << std::endl;
    }
    if (algo_path == "") {
        std::cout << "Missing algo path." << std::endl;
    }
    if((num_threads_str == "") || !convertToNumThreads(num_threads_str, num_threads)){
        std::cout << "Threads defualt value is 10." << std::endl;
        num_threads = 10;
    }
}


std::vector<void*> load_algorithms(AlgorithmRegistrar& registrar, std::string algo_path) {
    std::vector<void*> so_handlers;

    for (const auto& entry : std::filesystem::directory_iterator(algo_path)) {
        size_t registered_counter = registrar.count(); // counter
        if (entry.path().extension() == ".so") {
            void* so = dlopen(entry.path().c_str(), RTLD_LAZY);
            if (so == NULL) {
                std::cerr << "Failed to load algorithm library: " << entry.path() << " - " << dlerror() << std::endl;
                writeErrorToFile(entry.path().stem().string() + ".error", "Failed to load algorithm library. ");
                continue;
            }

            so_handlers.push_back(so);

            // case the algorithm didn't register
            if (registrar.count() == registered_counter) {
                std::cerr << "Algorithm in " << entry.path() << " failed to register." << std::endl;
                writeErrorToFile(entry.path().stem().string() + ".error", "Algorithm failed to register.");
                dlclose(so);
                so_handlers.pop_back();
                continue;
            }
        }
    }
    
    std::cout << "Number of algorithms in the registrar: " << registrar.count() << std::endl;

    return so_handlers;
}

std::vector<std::string> get_house_files_list(std::string house_path) {
    std::vector<std::string> house_list;
    for (const auto& entry : std::filesystem::directory_iterator(house_path)) {
        if (entry.path().extension() == ".house") {
            MySimulator _simulator; // in order to check the housefiles. 
            if (!_simulator.readHouseFile(entry.path().string())){
                // An error file is generated in the House class operation.
                continue;
            }
            house_list.push_back(entry.path().string());
        }
    }
    return house_list;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv) {
    bool summary_only = false;
    int num_threads = 10; 
    std::string house_path = "."; 
    std::string algo_path = "."; 
    parse_arguments(argc, argv, house_path, algo_path, num_threads, summary_only);
    
    std::vector<std::string> house_list = get_house_files_list(house_path);

    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    // Load algorithm files
    std::vector<void*> so_handlers = load_algorithms(registrar, algo_path);

    // create the tasks (house-algo)
    for (const std::string& house : house_list) {
        // std::cout << "House: " << house << std::endl;
        for (const auto& algo : registrar) {
            //std::cout << "Algorithm: " << algo.name() << "\n" << std::endl;
            tasks.emplace_back(algo.create(), algo.name(), house);
        }
    }
    
    // create threads and run the tasks 
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; i++) {
        threads.push_back(std::thread(thread_function, summary_only));
    }
    for (std::thread& thread : threads) {
        thread.join();
    }

    // write results into csv file.
    output_to_csv(registrar, house_list);

    // clear resources
    registrar.clear();

    for (Task& task : tasks) {
        task.algorithm = nullptr;
    }

    for (void* so : so_handlers) {
        // std::cout << "dlclose" << std::endl;
        dlclose(so);
    }

    return 0;
}