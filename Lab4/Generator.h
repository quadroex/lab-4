#pragma once
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>

enum class OperationTypes
{
    WRITE_0, READ_0,
    WRITE_1, READ_1,
    WRITE_2, READ_2,
    STRING_OP
};

struct Probs
{
    double w0, r0;
    double w1, r1;
    double w2, r2;
    double str;
};

const Probs V6_PROBS =
{
    0.20, 0.05,
    0.20, 0.05,
    0.20, 0.05,
    0.25
};

const Probs EQUAL_PROBS =
{
    1.0 / 7.0, 1.0 / 7.0,
    1.0 / 7.0, 1.0 / 7.0,
    1.0 / 7.0, 1.0 / 7.0,
    1.0 / 7.0
};

const Probs STRESS_PROBS =
{
    0.30, 0.01,
    0.30, 0.01,
    0.30, 0.01,
    0.07
};

void generateFile(const std::string& filename, int count, const Probs& p)
{
    std::ofstream file(filename);
    std::random_device rd;
    std::mt19937 gen(rd());

    std::discrete_distribution<> d
        ({
        p.w0, p.r0,
        p.w1, p.r1,
        p.w2, p.r2,
        p.str
        });

    std::uniform_int_distribution<> valDist(0, 100);

    for (int i = 0; i < count; ++i)
    {
        int op = d(gen);
        switch (static_cast<OperationTypes>(op))
        {
        case OperationTypes::WRITE_0: file << "write 0 " << valDist(gen) << "\n"; break;
        case OperationTypes::READ_0:  file << "read 0\n"; break;
        case OperationTypes::WRITE_1: file << "write 1 " << valDist(gen) << "\n"; break;
        case OperationTypes::READ_1:  file << "read 1\n"; break;
        case OperationTypes::WRITE_2: file << "write 2 " << valDist(gen) << "\n"; break;
        case OperationTypes::READ_2:  file << "read 2\n"; break;
        case OperationTypes::STRING_OP: file << "string\n"; break;
        }
    }
}