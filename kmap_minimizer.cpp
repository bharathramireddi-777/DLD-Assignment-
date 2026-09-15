
#include <bits/stdc++.h>
using namespace std;

struct Group
{
    string pattern;
    vector<int> minterms;
};

int n;
vector<int> ones;
vector<Group> primeGroups;

int gray(int x)
{
    return x ^ (x >> 1);
}

bool isValid(string pattern)
{
    vector<int> covered;

    for (int m = 0; m < (1 << n); m++)
    {
        string binary = "";

        for (int i = n - 1; i >= 0; i--)
        {
            binary += char('0' + ((m >> i) & 1));
        }

        bool match = true;

        for (int i = 0; i < n; i++)
        {
            if (pattern[i] != '-' && pattern[i] != binary[i])
            {
                match = false;
                break;
            }
        }

        if (match)
            covered.push_back(m);
    }

    if (covered.empty())
        return false;

    for (int m : covered)
    {
        if (find(ones.begin(), ones.end(), m) == ones.end())
            return false;
    }

    return true;
}

bool isBigger(string a, string b)
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] != '-' && a[i] != b[i])
            return false;
    }

    return a != b;
}

vector<int> getMinterms(string pattern)
{
    vector<int> result;

    for (int m = 0; m < (1 << n); m++)
    {
        bool match = true;

        for (int i = 0; i < n; i++)
        {
            int bit = (m >> (n - 1 - i)) & 1;

            if (pattern[i] != '-' &&
                pattern[i] - '0' != bit)
            {
                match = false;
                break;
            }
        }

        if (match)
            result.push_back(m);
    }

    return result;
}

string makeExpression(string pattern)
{
    char variable[] = {'a', 'b', 'c', 'd'};

    string answer = "";

    for (int i = 0; i < n; i++)
    {
        if (pattern[i] == '-')
            continue;

        answer += variable[i];

        if (pattern[i] == '0')
            answer += "'";
    }

    return answer;
}

void findPrimeGroups()
{
    vector<string> validGroups;

    int total = 1;

    for (int i = 0; i < n; i++)
        total *= 3;

    for (int number = 0; number < total; number++)
    {
        int x = number;
        string pattern = "";

        for (int i = 0; i < n; i++)
        {
            int value = x % 3;
            x = x / 3;

            if (value == 0)
                pattern += '0';
            else if (value == 1)
                pattern += '1';
            else
                pattern += '-';
        }

        reverse(pattern.begin(), pattern.end());

        if (isValid(pattern))
            validGroups.push_back(pattern);
    }

    for (string p : validGroups)
    {
        bool prime = true;

        for (string q : validGroups)
        {
            if (p != q && isBigger(q, p))
            {
                prime = false;
                break;
            }
        }

        if (prime)
        {
            Group g;

            g.pattern = p;
            g.minterms = getMinterms(p);

            primeGroups.push_back(g);
        }
    }
}

int main()
{
    ifstream input("input.txt");

    if (!input)
    {
        cout << "Error: input.txt not found.\n";
        return 0;
    }

    input >> n;

    if (n < 1 || n > 4)
    {
        cout << "Number of variables must be between 1 and 4.\n";
        return 0;
    }

    int rowVariables = n / 2;
    int columnVariables = n - rowVariables;

    int rows = 1 << rowVariables;
    int columns = 1 << columnVariables;

    vector<vector<int>> kmap(rows,
                              vector<int>(columns));

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            input >> kmap[i][j];
        }
    }

    input.close();

    /*
       IMPORTANT:

       Professor's K-map format:

       Columns = AB
       Rows    = CD

       Both are in Gray code order:
       00 01 11 10
    */

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (kmap[i][j] == 1)
            {
                int rowValue = gray(i);
                int columnValue = gray(j);

                // AB are columns and CD are rows
                int minterm =
                    (columnValue << rowVariables) | rowValue;

                ones.push_back(minterm);
            }
        }
    }

    if (ones.empty())
    {
        cout << "Minimized expression: 0\n";
        return 0;
    }

    if (ones.size() == (1 << n))
    {
        cout << "Minimized expression: 1\n";
        return 0;
    }

    findPrimeGroups();

    int numberOfPrimeGroups = primeGroups.size();

    int bestTerms = 100;
    int bestLiterals = 100;

    vector<vector<int>> answers;

    for (int mask = 1;
         mask < (1 << numberOfPrimeGroups);
         mask++)
    {
        vector<int> selected;
        vector<bool> covered(ones.size(), false);

        int terms = 0;
        int literals = 0;

        for (int i = 0; i < numberOfPrimeGroups; i++)
        {
            if (mask & (1 << i))
            {
                selected.push_back(i);
                terms++;

                for (char c : primeGroups[i].pattern)
                {
                    if (c != '-')
                        literals++;
                }

                for (int j = 0; j < (int)ones.size(); j++)
                {
                    for (int m : primeGroups[i].minterms)
                    {
                        if (ones[j] == m)
                            covered[j] = true;
                    }
                }
            }
        }

        bool allCovered = true;

        for (bool x : covered)
        {
            if (!x)
            {
                allCovered = false;
                break;
            }
        }

        if (!allCovered)
            continue;

        if (terms < bestTerms ||
            (terms == bestTerms &&
             literals < bestLiterals))
        {
            bestTerms = terms;
            bestLiterals = literals;

            answers.clear();
            answers.push_back(selected);
        }
        else if (terms == bestTerms &&
                 literals == bestLiterals)
        {
            answers.push_back(selected);
        }
    }

    cout << "\nPossible minimized Boolean expressions:\n\n";

    for (int i = 0; i < (int)answers.size(); i++)
    {
        cout << i + 1 << ". ";

        for (int j = 0;
             j < (int)answers[i].size();
             j++)
        {
            if (j > 0)
                cout << " + ";

            cout << makeExpression(
                primeGroups[answers[i][j]].pattern);
        }

        cout << "\n";
    }

    return 0;
}
