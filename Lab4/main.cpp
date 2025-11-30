#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>
#include <functional>
#include <barrier> 

#include "SafeData.h"
#include "Generator.h"

using namespace std;

struct Command
{
    int index;
    int value;
    string type;
};


vector<Command> loadCommands(const string& filename)
{
    vector<Command> commands;
    ifstream file(filename);

    string t;
    while (file >> t)
    {
        Command cmd;
        cmd.type = t;

        if (t == "write") file >> cmd.index >> cmd.value;
        else if (t == "read") file >> cmd.index;

        commands.push_back(cmd);
    }

    return commands;
}

void worker(SafeData& data, const vector<Command>& commands)
{
    for (const auto& cmd : commands) 
    {
        if (cmd.type == "write") data.write(cmd.index, cmd.value);

        else if (cmd.type == "read")
        {
            volatile int res = data.read(cmd.index);
            (void)res;
        }

        else if (cmd.type == "string")
        {
            volatile string s = static_cast<string>(data);
            (void)s;
        }
    }
}

void runTest(const string& testName, int numThreads, const vector<string>& files)
{
    SafeData sharedData;
    vector<vector<Command>> allThreadCommands(numThreads);

    for (int i = 0; i < numThreads; ++i) allThreadCommands[i] = loadCommands(files[i]);

    vector<jthread> threads;
    threads.reserve(numThreads);

    cout << "Test: " << testName << " [" << numThreads << " threads]... ";

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; ++i) threads.emplace_back(worker, ref(sharedData), ref(allThreadCommands[i]));

    for (auto& t : threads) if (t.joinable()) t.join();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;

    cout << "Time: " << diff.count() << "s" << endl;
}

int main()
{
    const int CMD_COUNT = 100000;

    cout << "Generating files..." << endl;

    generateFile("v6_1.txt", CMD_COUNT, V6_PROBS);
    generateFile("v6_2.txt", CMD_COUNT, V6_PROBS);
    generateFile("v6_3.txt", CMD_COUNT, V6_PROBS);

    generateFile("eq_1.txt", CMD_COUNT, EQUAL_PROBS);
    generateFile("eq_2.txt", CMD_COUNT, EQUAL_PROBS);
    generateFile("eq_3.txt", CMD_COUNT, EQUAL_PROBS);

    generateFile("bad_1.txt", CMD_COUNT, STRESS_PROBS);
    generateFile("bad_2.txt", CMD_COUNT, STRESS_PROBS);
    generateFile("bad_3.txt", CMD_COUNT, STRESS_PROBS);

    cout << "------------------------------------------------" << endl;

    runTest("Variant 6", 1, { "v6_1.txt" });
    runTest("Variant 6", 2, { "v6_1.txt", "v6_2.txt" });
    runTest("Variant 6", 3, { "v6_1.txt", "v6_2.txt", "v6_3.txt" });
    cout << "------------------------------------------------" << endl;

    runTest("Equal Prob", 1, { "eq_1.txt" });
    runTest("Equal Prob", 2, { "eq_1.txt", "eq_2.txt" });
    runTest("Equal Prob", 3, { "eq_1.txt", "eq_2.txt", "eq_3.txt" });
    cout << "------------------------------------------------" << endl;

    runTest("Stress Prob", 1, { "bad_1.txt" });
    runTest("Stress Prob", 2, { "bad_1.txt", "bad_2.txt" });
    runTest("Stress Prob", 3, { "bad_1.txt", "bad_2.txt", "bad_3.txt" });

    return 0;
}