#include <array>
#include <cstring>
#include <iostream>
#include <tuple>

// #include "./level 1/lab-one.cpp"
// #include "./level 2/lab-two.cpp"
#include "./level 3/lab-three.cpp"

// for simplicity
using namespace std;

const char* DEFAULT_PATH_LIN = "./default-lin.txt";
const char* DEFAULT_PATH_REG = "./default-reg.txt";

void printHelpMessage() {
    cout << "\n\n===========================\n"
         << "To run this program arg string should look like this:\n"
         << "./programm.exe -lin ./path/to/your/automaton\n"
         << "OR\n"
         << "./programm.exe -lfsr ./path/to/your/automaton\n"
         << "\n"
         << "Parameters:\n"
         << "-lin [path]: Run linear automaton based on config by given path.\n"
         << "-lfsr [path]: Run LFSR automaton based on config by given path.\n"
         << "-eqClasses: runs search of equvivalent classes in current "
            "automaton\n"
         << "If no path detected, program will use DEFAULT_PATH.\n"
         << "===========================\n"
         << endl;
}

/**
 * recursive function to search all
 * parameters passed in argv[] array
 */
bool hasParameter(int argc, char** argv, string parameter) {
    if (argc == 1) return false;
    if (strcmp(argv[1], parameter.c_str()) == 0) return true;

    return hasParameter(argc - 1, argv + 1, parameter);
}

/**
 * main function
 * executing all lab modules included in "level n" files
 */
int main(int argc, char** argv) {
    printHelpMessage();
    if (argc < 2) return 0;

    /**
     * [0] - lin, [1] - lfsr, ...
     * all false by default
     */
    vector<string> parameters = {"-lin", "-lfsr", "-eqClasses"};
    vector<bool> parametersBool(parameters.size(), false);

    /**
     * iterating all argv parameters passed to main
     */
    {
        for (int i = 0; i < parameters.size(); i++)
            if (hasParameter(argc, argv, parameters[i]))
                parametersBool[i] = true;

        // exit if selected both "-lin" & "-lfsr"
        if (parametersBool[0] && parametersBool[1]) return 0;
    }

    if (parametersBool[0]) {
        ifstream test(argv[argc - 1]);
        const char* filename = !test ? DEFAULT_PATH_LIN : argv[argc - 1];

        ifstream fileObj(filename);
        if (!fileObj.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return -1;
        }

        linearAutomaton lin = initLinear(fileObj);

        // if (!parametersBool[2])
        //     linOperation(&lin);  // run lab one - search next state
        // if (parametersBool[2])
        //     findLinEqClasses(
        //         &lin);  // run lab two - search of equvivalent classes
        graphConnectivityCheck<linearAutomaton, linearAutomaton::VertexType>(
            &lin);
    }

    if (parametersBool[1]) {
        ifstream test(argv[argc - 1]);
        const char* filename = !test ? DEFAULT_PATH_REG : argv[argc - 1];

        ifstream fileObj(filename);
        if (!fileObj.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return -1;
        }

        shiftRegister lfsr = initLFSR(fileObj);

        // if (!parametersBool[2])
        //     LFSROperation(&lfsr);  // run lab one - search next state
        // if (parametersBool[2])
        //     findLFSREqClasses(
        //         &lfsr);  // run lab two - search of equvivalent classes
        graphConnectivityCheck<shiftRegister, shiftRegister::VertexType>(&lfsr);
    }

    return 0;
}
