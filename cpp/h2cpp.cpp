#include <iostream>
#include <fstream>
#include <algorithm> 
#include <cctype>
#include <locale>
using namespace std;

// trim from start (in place)
static inline void ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(string &s) {
    rtrim(s);
    ltrim(s);
}

int min(int a, int b) {
    return a<b?a:b;
}

void dealInclude(ofstream & ofs, string fin) {
    string l = "#include \"" + fin + "\"";
    ofs << l << endl;
}

string dealClass(string line) {
    string clss = "class ";
    string name = line.substr(6, line.size());
    size_t ibk = name.find("{");
    name = name.substr(0, ibk);
    trim(name);
    cout << "class name = " << name << endl;
    return name;
}

void dealLine(ofstream & ofs, string line, string cls) {
    //Priority: after last space
    string lp = "(", rp = ")", cm = ",", sp = " ", sc = ";", eq = "=";
    size_t ieq = line.find(eq);
    if ((ieq != string::npos) && (line.find("operator=") == string::npos)) return;
    size_t ilp = line.find(lp), irp = line.find(rp);
    if (ilp == string::npos || irp == string::npos) return;
    size_t isc = line.find(sc);
    line = line.substr(0, isc + 1); //ignore other statements after semicolon
    string head = line.substr(0, ilp);
    string tail = line.substr(ilp, line.size() - ilp + 1);
    if (tail.size() >= 2) {
        if(tail[(size_t)(tail.size() - 2)] != ' ')
            tail.replace((size_t)(tail.size() - 1), 1, " {\n\n}");
        else
            tail.replace((size_t)(tail.size() - 1), 1, "{\n\n}");
    }
    size_t isp_nxt = head.find(sp);
    size_t isp = string::npos;
    while(isp_nxt != string::npos) {
        isp = isp_nxt;
        isp_nxt = head.find(sp, min(isp_nxt+1, line.size() - 1));
    }
    int issp = (isp!=string::npos)?isp:-1;
    string first = head.substr(0, issp + 1);
    string second = head.substr(issp + 1, head.size() - issp);
    string new_head = first + cls + "::" + second;
    ofs << new_head << tail << endl;
}

int main(int argc, char **argv) {
    string fp, fin, fout;
    if (argc != 4 && argc != 3) {
        cout << "Usage: h2cpp path(like \"mp_stickers/src/\") file_in.h file_out.cpp" << endl;
    }
    if (argc == 4) {
        fp = argv[1];
        fin = argv[2];
        fout = argv[3];
    }
    else if (argc == 3) {
        fp = "";
        fin = argv[1];
        fout = argv[2];
    }
    ifstream ifs(fp + fin);
    ofstream ofs(fp + fout);
    dealInclude(ofs, fin);
    bool pubflag = false;
    bool clsflag = false;
    string cls = "";
    for(string line; getline(ifs, line);) {
        trim(line);
        if (pubflag) {
            if (line.find("private:") != string::npos || line.find("};") != string::npos) break;
            ofs << endl;
            dealLine(ofs, line, cls);
            continue;
        }
        if (line[0] == '#') continue;
        if ((!clsflag) && (line.find("class ") == 0)) {
            cls = dealClass(line);
            clsflag = true;
        }
        else if (line.find("public:") != string::npos) pubflag = true;
    }
    return 0;
}