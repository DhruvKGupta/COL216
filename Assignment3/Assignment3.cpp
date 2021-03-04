#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

int main()
{

    string line;
    ifstream file("code.txt");
    if (file.is_open())
    {
        while (getline(file, line))
        {
            string::iterator end_pos = remove(line.begin(), line.end(), ' ');
            line.erase(end_pos, line.end());
            if (line.empty())
            {
                cout << "Empty";
            }
            else
            {
                cout << line;
            }
        }
    }
    else
    {
        cout << "File does not exist\n";
    }
}