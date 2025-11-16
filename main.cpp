#include <iostream>
#include <vector>
#include <string>
#include <cctype> // Untuk fungsi tolower
#include <unordered_set> // untuk mengecek kata yang sudah ada
#include <cstdlib> //untuk clearscreen terminal
#include <fstream> // untuk membaca file#include <string>
#include <chrono>   // untuk menghitung waktu

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <cstdlib>
#endif

// ---------- Cross-platform timed input ----------
#include <iostream>
#include <string>
#include <cstdlib>   // Untuk system() dan remove()
#include <fstream>   // Untuk membaca file
#include <sstream>   // Untuk membangun string perintah

using namespace std;

void coutFiglet(const std::string& teks) {
    std::stringstream perintah;
    // 1. Bangun perintah untuk menyimpan output ke file "output.txt"
    perintah << "figlet -w 160 -f larry3d \"" << teks << "\" > output.txt";
    
    // Jalankan perintah
    system(perintah.str().c_str());

    // 2. Buka file yang baru dibuat
    std::ifstream file("output.txt");
    if (file.is_open()) {
        std::string baris;
        // 3. Baca file baris per baris dan cetak dengan cout
        while (std::getline(file, baris)) {
            std::cout << baris << endl;
        }
        file.close();

        // 4. Hapus file temporer setelah selesai
        remove("output.txt");
    } else {
        std::cout << "Gagal membuat output figlet." << std::endl;
    }
}

//fungsi untuk memuat kata dari file sql
void loadKataBakuSQL(const string& filename, unordered_set<string>& kamus) {
  ifstream file(filename);
  if (!file.is_open()) {
    cerr << "Error: Gagal membuka " << filename << endl;
    exit(1);
  }
  string line;
  while (getline(file, line)) {
    size_t start = line.find("('");
    if (start == string::npos) continue;
    start += 2;
    size_t end = line.find("'", start);
    if (end == string::npos) continue;
    string kata = line.substr(start, end - start);
    // Hilangkan spasi di awal/akhir dan konversi ke huruf kecil
    kata.erase(0, kata.find_first_not_of(" \t\n\r"));
    kata.erase(kata.find_last_not_of(" \t\n\r") + 1);
    kamus.insert(kata);
  }
  file.close();
}

std::pair<bool, std::string> timedInput(int timeoutSeconds = 20)
{

#ifdef _WIN32
  std::string raw;
  DWORD start = GetTickCount();
  DWORD deadline = start + static_cast<DWORD>(timeoutSeconds) * 1000;

  while (GetTickCount() < deadline)
  {
    if (_kbhit())
    {
      int ch = _getch();
      if (ch == '\r')
      {
        std::cout << '\n';
        break;
      }
      else if (ch == '\b' || ch == 127)
      {
        if (!raw.empty())
        {
          raw.pop_back();
          std::cout << "\b \b";
          std::cout.flush();
        }
      }
      else if (isprint(static_cast<unsigned char>(ch)))
      {
        raw.push_back(static_cast<char>(ch));
        std::cout << static_cast<char>(ch);
        std::cout.flush();
      }
    }
    Sleep(10);
  }

  if (raw.empty()) return {false, ""};

  std::stringstream ss(raw);
  std::string word;
  ss >> word;
  return {true, word};

#else
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  timeval timeout{};
  timeout.tv_sec = timeoutSeconds;
  timeout.tv_usec = 0;

  int result = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout);

  if (result <= 0)
  {
    if (result == 0)
      std::cout << "\nWaktu habis! Tidak ada input dalam " << timeoutSeconds << " detik.\n";
    else
      std::cout << "\nError occurred during input waiting.\n";
    return {false, ""};
  }
  else
  {
    std::string input;
    std::getline(std::cin, input);
    std::stringstream ss(input);
    std::string word;
    ss >> word;
    return {true, word};
  }
#endif
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
    // Muat semua kosakata yang ada pada file sql kemudian simpan unordered_set<std::string> kamus;
    string play = "y";
    while(play == "y"){
      unordered_set<std::string> kamus;
    loadKataBakuSQL("kbbi-1-10000.sql" ,kamus);
    loadKataBakuSQL("kbbi-10001-30000.sql" ,kamus);
    loadKataBakuSQL("kbbi-30001-90000.sql" ,kamus);

    //bersihkan layar terminal dan tampilan game dimulai
    clearScreen();
    cout << "============================================================================================================================================================================\n";
    coutFiglet("Selamat Datang di Game Sambung Kata!");
    cout << "============================================================================================================================================================================\n\n";

    // Input jumlah dan nama pemain
    int numPlayers, numChar;
    cout << "Masukkan jumlah pemain: ";
    cin >> numPlayers;

    // Validasi input, jumlah pemain harus lebih dari 1 pemain
    if (numPlayers <= 1) {
        cout << "Permainan membutuhkan minimal 2 pemain." << endl;
        return 1;
    }

    vector<string> players;
    cin.ignore(); // Untuk membersihkan buffer newline setelah cin >> numPlayers

    // Loop sebanyak N pemain dan input nama masing-masing pemain
    for (int i = 0; i < numPlayers; ++i) {
        cout << "Masukkan nama Pemain " << i + 1 << ": ";
        string name;
        getline(cin, name);
        players.push_back(name);
    }

    // Input jumlah huruf yang digunakan sebagai acuan kata selanjutnya
    cout << "Masukkan jumlah huruf yang akan disambungkan: ";
    cin >> numChar;
    while(numChar < 1 && numChar > 3){
        cout << "Masukkan jumlah huruf yang akan disambungkan (maks 3): ";
        cin >> numChar;
    }
    

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

    // Input kata pertama dari player 1
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

    // Simpan kata dan ambil n huruf terakhir
    usedWords.insert(currentWord);
    for(int i = numChar - 1; i >= 0; i--){
        lastChar += currentWord[currentWord.length() - 1 - i];
    }

    // Ganti giliran ke player berikutnya
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();

    // Loop utama permainan, berjalan selama pemain lebih dari 1
    while (players.size() > 1) {
        // Tampilkan huruf terakhir
        cout << "\n=========================================\n";
        cout << "Kata berikutnya harus dimulai dengan huruf: '" << lastChar << "'\n";
        
        // Ganti giliran & Minta input dari player
        cout << "Giliran " << players[currentPlayerIndex] << ": ";
        // getElapsed(currentWord, elapsed);
        auto result = timedInput();

        if(result.first){
            currentWord = result.second;
            string lowerCaseWord = currentWord;
            for (char &c : lowerCaseWord) {
                c = tolower(c);
            }

            // Ambil n huruf terdepan
            string frontWord = "";
            for(int i = 0; i < numChar; i++){
                frontWord += lowerCaseWord[i];
            }

            // Cek kondisi: 
            // Sesuai n huruf terakhir
            bool isFirstCharCorrect = (frontWord == lastChar);
            // Belum ada di daftar kata yang sudah digunakan
            bool isWordNew = (usedWords.find(lowerCaseWord) == usedWords.end());
            // Termasuk kata yang ada pada KBBI
            bool isWordValid = (kamus.count(currentWord));

            if (isFirstCharCorrect && isWordNew && isWordValid) {
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
                } else {
                    cout << "Alasan: Kata '" << currentWord << "' bukanlah bahasa yang baku.\n";
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
        }else{
            cout << "\n-----------------------------------------\n";
            cout << "Salah! " << players[currentPlayerIndex] << " kalah dan keluar dari permainan!\n";
            cout << "Alasan: waktu anda habis\n" ;
            players.erase(players.begin() + currentPlayerIndex);

            // Jangan increment currentPlayerIndex setelah menghapus
            // karena vector akan bergeser. Cukup pastikan index tetap valid.
            if (!players.empty()) {
                currentPlayerIndex %= players.size();
            }
        }
    }

    // Cek jika pemain sisa 1 -> Pemenang ditemukan
    if (!players.empty()) {
        cout << "\n===============================================\n";
        cout << "Permainan Selesai! Pemenangnya adalah " << players[0] << "!\n";
        cout << "===============================================\n";
    }

    cout << "Apakah ingin bermain lagi [y/n]?";
    cin >> play;
    }
    
    cout << "Terima kasih sudah bermain game sambung kata" << endl;
    return 0;
}