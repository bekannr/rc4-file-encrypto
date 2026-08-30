#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

class RC4 {
private:
    vector<unsigned char> s;

public:
    RC4(const string& key) {
        s.resize(256);
        for (int i = 0; i < 256; i++)
            s[i] = i;

        int j = 0;
        for (int i = 0; i < 256; i++) {
            j = (j + s[i] + (unsigned char)key[i % key.length()]) % 256;
            swap(s[i], s[j]);
        }
    }

    // RC4 is symmetric – same function encrypts and decrypts
    vector<unsigned char> process(const vector<unsigned char>& data) {
        vector<unsigned char> result(data.size());
        vector<unsigned char> box = s;   // work on a copy

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

vector<unsigned char> readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: cannot open " << filename << endl;
        return {};
    }

    file.seekg(0, ios::end);
    size_t size = file.tellg();
    file.seekg(0, ios::beg);

    vector<unsigned char> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    return data;
}

bool writeFile(const string& filename, const vector<unsigned char>& data) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: cannot write to " << filename << endl;
        return false;
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

void printHex(const vector<unsigned char>& data, size_t maxBytes = 16) {
    for (size_t i = 0; i < min(data.size(), maxBytes); i++)
        cout << hex << setw(2) << setfill('0') << (int)data[i] << " ";
    if (data.size() > maxBytes) cout << "...";
    cout << dec << endl;
}

bool encryptFile(const string& filename, const string& key) {
    vector<unsigned char> data = readFile(filename);
    if (data.empty()) return false;

    cout << "Original size: " << data.size() << " bytes\n";
    cout << "First bytes: ";
    printHex(data);

    RC4 rc4(key);
    vector<unsigned char> encrypted = rc4.process(data);

    if (!writeFile(filename, encrypted)) return false;

    cout << "Encrypted size: " << encrypted.size() << " bytes\n";
    cout << "Encrypted bytes: ";
    printHex(encrypted);
    cout << "Encryption done.\n";
    return true;
}

bool decryptFile(const string& filename, const string& key) {
    vector<unsigned char> data = readFile(filename);
    if (data.empty()) return false;

    cout << "Encrypted size: " << data.size() << " bytes\n";
    cout << "First bytes: ";
    printHex(data);

    RC4 rc4(key);
    vector<unsigned char> decrypted = rc4.process(data);

    if (!writeFile(filename, decrypted)) return false;

    cout << "Decrypted size: " << decrypted.size() << " bytes\n";
    cout << "First bytes: ";
    printHex(decrypted);
    cout << "Decryption done.\n";
    return true;
}

int main() {
    string filename = "file.txt";
    string key = "my_secret_password";   // change if needed

    cout << "RC4 File Encryptor\n";
    cout << "File: " << filename << "\n";
    cout << "Key:  " << key << "\n\n";

    while (true) {
        cout << "1. Encrypt\n";
        cout << "2. Decrypt\n";
        cout << "3. Encrypt then decrypt (test)\n";
        cout << "4. Exit\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            encryptFile(filename, key);
        }
        else if (choice == 2) {
            decryptFile(filename, key);
        }
        else if (choice == 3) {
            vector<unsigned char> original = readFile(filename);
            if (original.empty()) continue;

            if (!encryptFile(filename, key)) continue;
            if (!decryptFile(filename, key)) continue;

            vector<unsigned char> restored = readFile(filename);
            if (restored == original)
                cout << "\nSuccess – file restored correctly.\n";
            else
                cout << "\nError – file does not match original.\n";
        }
        else if (choice == 4) {
            break;
        }
        else {
            cout << "Invalid choice.\n";
        }
        cout << endl;
    }
    return 0;
}
