#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <map>
#include <random>

using namespace std;

typedef vector<string> strvec;
typedef pair<string, int> occurance;
typedef vector<occurance> membervec;
typedef map<strvec, membervec> mainmap;

const vector<char> PUNCS = {':', ';', ',', '.', '?', '!', '(', ')', '[', ']', '{', '}', '-', '\'', '"', '\n'};

template <class ITERABLE>
bool in(ITERABLE s, char c)
{
    bool ret = false;
    for (auto i : s)
    {
        if (i == c)
            ret = true;
    }
    return ret;
}

string chr2str(char c) { return string(1, c); }

strvec splitstr(string s, vector<char> delimiters = {' '}, bool use_puncs = true, vector<char> puncs = PUNCS)
{
    string buf = "";
    strvec ret = {}, temp = {};

    // Split input s and store it in vector temp
    for (char i : s)
    {
        if (in(delimiters, i))
        {
            temp.push_back(buf);
            buf = "";
        }
        else if (in(puncs, i) && use_puncs)
        {
            temp.push_back(buf);
            buf = "";
            temp.push_back(chr2str(i));
        }
        else if (in(delimiters, (char)0))
        {
            temp.push_back(chr2str(i));
        }
        else
            buf += i;
    }

    // Remove "" strings in temp
    for (auto i : temp)
    {
        if (i != "")
            ret.push_back(i);
    }

    return ret;
}

string join(strvec segs, string merge)
{
    string ret = "";
    for (int i = 0; i < segs.size() - 1; i++)
    {
        ret += segs[i] + merge;
    }

    ret += segs[segs.size() - 1];

    return ret;
}

strvec preprocess(strvec lines, vector<char> delimiters = {' '}, string merger = "\n",
                  bool use_puncs = true, vector<char> puncs = PUNCS)
{
    return splitstr(join(lines, merger), delimiters, use_puncs, puncs);
}

strvec readfile(string filename)
{
    string line;
    strvec ret;
    ifstream file(filename);

    if (file.is_open())
    {
        while (getline(file, line))
        {
            ret.push_back(line);
        }
    }
    else
    {
        cout << "Error opening file";
    }

    return ret;
}

strvec extend(strvec major, strvec minor)
{
    minor.insert(minor.begin(), major.begin(), major.end());
    return minor;
}

strvec pad(strvec prepad, strvec sv, strvec postpad)
{
    return extend(extend(prepad, sv), postpad);
}

void update(mainmap &table, strvec key, string val)
{
    // cout << (join(key, "|")) << " : " << val << endl;
    auto it = table.find(key);
    if (it == table.end())
    {
        occurance occ(val, 1);
        membervec mem = {occ};
        table.insert({key, mem});
    }
    else
    {
        membervec &mem = it->second;
        bool found = false;

        for (occurance &occ : mem)
        {
            if (occ.first == val)
            {
                occ.second += 1;
                found = true;
            }
        }
        if (!found)
        {
            occurance occ(val, 1);
            mem.push_back(occ);
        }
    }
}

void printMarkovMap(mainmap table)
{
    mainmap::iterator it;
    for (it = table.begin(); it != table.end(); ++it)
    {
        strvec key = it->first;
        membervec mem = it->second;
        cout << join(key, "|") << " --> " << endl;
        for (occurance &occ : mem)
            cout << occ.first << " : " << occ.second << endl;
        cout << endl;
    }
}

mainmap markovmap(strvec preproc, int n)
{
    mainmap table;
    strvec slice;

    preproc = pad({"[SOF]", "[SOF]"}, preproc, {"[EOF]"});
    for (int i = 0; i < preproc.size() - n; ++i)
    {
        slice = strvec(preproc.begin() + i, preproc.begin() + i + n);
        update(table, slice, preproc[i + n]);
    }

    return table;
}

string generate(mainmap table, strvec prompts, int n, int gen_len)
{
    int c = 0, choice;
    string ret = "", add = "";
    default_random_engine randGen;

    prompts = extend({"[SOF]", "[SOF]"}, prompts);

    auto validate = [](int c, int gen_len, string add) {
        if (gen_len > 0)
            return c < gen_len;
        else
            return add != "[EOF]";
    };

    while (validate(c, gen_len, add))
    {
        prompts = strvec(prompts.begin() + prompts.size() - n, prompts.end());
        membervec mem = table.find(prompts)->second;

        vector<int> weights = {};
        for (occurance &i : mem)
        {
            weights.push_back(i.second);
        }

        discrete_distribution<> distro(weights.begin(), weights.end());
        choice = distro(randGen);
        add = mem[choice].first;

        if (add != "\n")
            ret += add + " ";
        else
            ret += add;

        prompts.push_back(add);
        c++;
    }

    return ret;
}

int main()
{
    string filename;
    int n;

    cout << "Enter filename and kernel size: ";
    cin >> filename >> n;

    strvec lines = readfile(filename);

    //strvec preproc = preprocess(lines, {' ', '\n'}, "\n", false); //uncomment if you want to include punctuation as part of the vocab
    strvec preproc = preprocess(lines);

    mainmap table = markovmap(preproc, 2);
    //printMarkovMap(table);

    strvec prompts;
    int gen_len;

    cout << "Enter generation length (non-positive input will make the program run till termination): ";
    cin >> gen_len;
    cout << "Enter prompts ([EOI] to terminate): ";

    string temp;
    do
    {
        cin >> temp;
        prompts.push_back(temp);
    } while (temp != "[EOI]");

    strvec slice = strvec(prompts.begin(), prompts.begin() + prompts.size() - 1);

    string generation = generate(table, slice, n, gen_len);
    cout << generation;
    
    cin >> temp;

    return 0;
}
