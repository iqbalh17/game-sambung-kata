#include <iostream>
#include <vector>
#include <string>
#include <cctype> // Untuk fungsi tolower
#include <unordered_set> // untuk mengecek kata yang sudah ada
#include <cstdlib> //untuk clearscreen terminal
#include <fstream> // untuk membaca file#include <string>
#include <chrono>   // untuk hitung waktu

using namespace std;

//fungsi untuk memuat kata dari file txt
void kataBaku(unordered_set<std::string>& kamus) {
    ifstream file("katabaku.txt");
    if (!file.is_open()) {
        cerr << "Error: Gagal membuka kamus.txt. Pastikan file ada di folder yang sama." << endl;
        return;
    }
    string kata;
    while (file >> kata) {
        // kata di kamus juga dalam huruf kecil
        for (char &c : kata) {
            c = tolower(c);
        }
        kamus.insert(kata);
    }
    file.close();
    cout << "Kamus berhasil dimuat dengan " << kamus.size() << " kata." << endl;
}

// Fungsi untuk ambil input dengan timer
bool getElapsed(string &out, int &elapsedSeconds) {
    using namespace std::chrono;
    auto start = steady_clock::now();
    string x; cin >> x;
    out = x;
    auto end = steady_clock::now();
    elapsedSeconds = static_cast<int>(duration_cast<seconds>(end - start).count());
    return true;
}

// Fungsi untuk membersihkan layar terminal
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    std::unordered_set<std::string> kamus;
    kataBaku(kamus);

    clearScreen();
    cout << "======================================\n";
    cout << "Selamat Datang di Game Sambung Kata!\n";
    cout << "======================================\n\n";

    // 1. Input jumlah dan nama pemain
    int numPlayers, numChar;
    cout << "Masukkan jumlah pemain: ";
    cin >> numPlayers;

    // Validasi input, jumlah pemain harus lebih dari 1 pemain
    if (numPlayers <= 1) {
        cout << "Permainan membutuhkan minimal 2 pemain." << endl;
        return 1;
    }

    vector<string> players;
    cin.ignore(); // Membersihkan buffer newline setelah cin >> numPlayers

    for (int i = 0; i < numPlayers; ++i) {
        cout << "Masukkan nama Pemain " << i + 1 << ": ";
        string name;
        getline(cin, name);
        players.push_back(name);
    }

    cout << "Masukkan jumlah huruf yang disambung: ";
    cin >> numChar;

    // Inisialisasi variabel permainan
    unordered_set<string> usedWords;
    string currentWord;
    string lastChar = "";
    bool kataPertamaValid = false;
    int currentPlayerIndex = 0;
    int elapsed;
    int timeLimit = 10;

    clearScreen();
    cout << "Permainan Dimulai!\n\n";

    // 2. Input kata pertama dari player 1
    while(!kataPertamaValid){
    cout << "Giliran " << players[currentPlayerIndex] << ", masukkan kata pertama: ";
    cin >> currentWord;
        if (kamus.count(currentWord)) {
            kataPertamaValid = true;
        }else{
            std::cout << "Kata '" << currentWord << "' tidak ditemukan di kamus. Coba lagi.\n";
        }
    }
    
    // Konversi kata ke huruf kecil
    for (char &c : currentWord) {
        c = tolower(c);
    }

    // 3. Simpan kata dan ambil huruf terakhir
    usedWords.insert(currentWord);
    // lastChar = currentWord.back();
    for(int i = numChar - 1; i >= 0; i--){
        lastChar += currentWord[currentWord.length() - 1 - i];
    }

    // Ganti giliran ke player berikutnya
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();

    // Loop utama permainan, berjalan selama pemain lebih dari 1
    while (players.size() > 1) {
        // 4. Tampilkan huruf terakhir
        cout << "\n=========================================\n";
        cout << "Kata berikutnya harus dimulai dengan huruf: '" << lastChar << "'\n";
        
        // 5. Ganti giliran & 6. Minta input dari player
        cout << "Giliran " << players[currentPlayerIndex] << ": ";
        getElapsed(currentWord, elapsed);

        // cout << elapsed << endl;

        // Konversi kata input ke huruf kecil
        string lowerCaseWord = currentWord;
        for (char &c : lowerCaseWord) {
            c = tolower(c);
        }

        string frontWord = "";
        for(int i = 0; i < numChar; i++){
            frontWord += lowerCaseWord[i];
        }

        // 7. Cek kondisi: Sesuai n huruf terakhir dan belum ada di daftar kata yang sudah digunakan
        bool isFirstCharCorrect = (frontWord == lastChar);
        bool isWordNew = (usedWords.find(lowerCaseWord) == usedWords.end());
        bool isWordValid = (kamus.count(currentWord));
        bool isOntime = (elapsed < timeLimit); 

        if (isFirstCharCorrect && isWordNew && isWordValid && isOntime) {
            // -- Ya --> Simpan kata dan lanjut
            usedWords.insert(lowerCaseWord);
            lastChar = "";
            for(int i = numChar - 1; i >= 0; i--){
                lastChar += lowerCaseWord[lowerCaseWord.length() - 1 - i];
            }
            // Pindah ke pemain selanjutnya
            currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
        } else {
            // -- Tidak --> Player kalah
            cout << "\n-----------------------------------------\n";
            cout << "Salah! " << players[currentPlayerIndex] << " kalah dan keluar dari permainan!\n";
            if (!isFirstCharCorrect) {
                cout << "Alasan: Kata tidak dimulai dengan huruf '" << (lastChar) << "'.\n";
            } else if(!isWordNew){
                cout << "Alasan: Kata '" << currentWord << "' sudah pernah digunakan.\n";
            } else if(!isWordValid) {
                cout << "Alasan: Kata '" << currentWord << "' bukanlah bahasa yang baku.\n";
            } else {
                cout << "Alasan: waktu anda habis (" << elapsed << " detik)\n" ;
            }
            cout << "-----------------------------------------\n";

            // Hapus pemain dari daftar
            players.erase(players.begin() + currentPlayerIndex);

            // Jangan increment currentPlayerIndex setelah menghapus
            // karena vector akan bergeser. Cukup pastikan index tetap valid.
            if (!players.empty()) {
                currentPlayerIndex %= players.size();
            }
        }
    }

    // 8. Cek jika pemain sisa 1 -> Pemenang ditemukan
    if (!players.empty()) {
        cout << "\n=====================================\n";
        cout << "Permainan Selesai! Pemenangnya adalah " << players[0] << "!\n";
        cout << "======================================\n";
    }

    return 0;
}