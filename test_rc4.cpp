#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// Same RC4 class as in rc4.cpp
class RC4 {
private:
    vector<unsigned char> s;
public:
    RC4(const string& key) {
        s.resize(256);
        for (int i = 0; i < 256; i++) s[i] = i;
        int j = 0;
        for (int i = 0; i < 256; i++) {
            j = (j + s[i] + (unsigned char)key[i % key.length()]) % 256;
            swap(s[i], s[j]);
        }
    }
    vector<unsigned char> process(const vector<unsigned char>& data) {
        vector<unsigned char> result(data.size());
        vector<unsigned char> box = s;
        int i = 0, j = 0;
        for (size_t n = 0; n < data.size(); n++) {
            i = (i + 1) % 256;
            j = (j + box[i]) % 256;
            swap(box[i], box[j]);
            unsigned char k = box[(box[i] + box[j]) % 256];
            result[n] = data[n] ^ k;
        }
        return result;
    }
};

vector<unsigned char> readFile(const string& name) {
    ifstream f(name, ios::binary);
    if (!f) return {};
    f.seekg(0, ios::end);
    size_t size = f.tellg();
    f.seekg(0, ios::beg);
    vector<unsigned char> data(size);
    f.read(reinterpret_cast<char*>(data.data()), size);
    return data;
}

bool writeFile(const string& name, const vector<unsigned char>& data) {
    ofstream f(name, ios::binary);
    if (!f) return false;
    f.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

int main() {
    // Create a simple test file
    {
        ofstream f("file.txt");
        f << "Hello! This is a test file for RC4 encryption.\n"
          << "It contains text, numbers 12345 and symbols !@#$%.\n";
    }

    string key = "test_key_123";
    vector<unsigned char> original = readFile("file.txt");
    if (original.empty()) {
        cerr << "Failed to create test file\n";
        return 1;
    }

    cout << "Original size: " << original.size() << " bytes\n";

    // Encrypt
    RC4 rc4(key);
    vector<unsigned char> encrypted = rc4.process(original);
    writeFile("file.txt", encrypted);
    cout << "Encrypted size: " << encrypted.size() << " bytes\n";

    // Decrypt
    vector<unsigned char> ciphertext = readFile("file.txt");
    RC4 rc4dec(key);
    vector<unsigned char> decrypted = rc4dec.process(ciphertext);
    writeFile("file.txt", decrypted);
    cout << "Decrypted size: " << decrypted.size() << " bytes\n";

    if (original == decrypted) {
        cout << "Test PASSED – original file restored.\n";
        return 0;
    } else {
        cout << "Test FAILED.\n";
        return 1;
    }
}
