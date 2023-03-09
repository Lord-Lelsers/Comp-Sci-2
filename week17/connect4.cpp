#include <iostream>

#include <climits>
#include <fstream>
#include <tuple>
#include <utility> // pair
#include <vector>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

using std::pair;
using std::tuple;
using std::vector;

const int BOARD_WIDTH = 7;
const int BOARD_HEIGHT = 6;

void clearScreen() { system("clear"); }

enum Spot { Empty, X, O };

char spotToChar(Spot spot, bool writingToFile = false) {
  switch (spot) {
  case Spot::X:
    return 'X';
  case Spot::O:
    return 'O';
  default:
    if (writingToFile) {
      return '+';
    } else {
      return ' ';
    }
  }
}

Spot charToSpot(char c) {
  // for reading from file
  switch (c) {
  case 'X':
    return Spot::X;
  case 'O':
    return Spot::O;
  default:
    return Spot::Empty;
  }
}

Spot nextTurn(Spot spot) {
  switch (spot) {
  case Spot::X:
    return Spot::O;
  case Spot::O:
    return Spot::X;
  default:
    return spot;
  }
}

enum ControlOptions : int { Person = 0, Easy = 1, Hard = 2 };

struct Board {
  Spot grid[BOARD_WIDTH][BOARD_HEIGHT];
  bool notFilledColumn[BOARD_WIDTH];
  Spot turn;
};

int bestMove(Board *board);
int minimax(Board *board, int depth, bool isMaximizing);

void dropSpot(Board *board, int row) {
  // assumes row is not full
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    if (board->grid[row][y] == Spot::Empty) {
      board->grid[row][y] = board->turn;
      break;
    }
  }
}

void undoMove(Board *board, int row) {
  for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
    if (board->grid[row][y] != Spot::Empty) {
      board->grid[row][y] = Spot::Empty;
      break;
    }
  }
}

int getMove(Board *board, ControlOptions player) {
  int col = 0;
  switch (player) {
  case ControlOptions::Person:
    std::cout << "Choose column to place piece: ";
    std::cin >> col;
    col--; // index
    break;
  case ControlOptions::Easy:
    col = rand() % BOARD_WIDTH;
    break;
  case ControlOptions::Hard:
    col = bestMove(board);
    break;
  }

  for (int x = 0; x < BOARD_WIDTH; x++) {
    // pointer arithmetic
    if (x == col && *(board->notFilledColumn + x)) {
      return col;
    }
  }

  std::cout << "Invalid move!" << std::endl;
  return getMove(board, player);
}

void updateFilledColumns(Board *board) {
  for (int x = 0; x < BOARD_WIDTH; x++) {
    board->notFilledColumn[x] = board->grid[x][BOARD_HEIGHT - 1] == Spot::Empty;
  }
}

void move(Board *board, ControlOptions player) {

  int col = getMove(board, player);
  dropSpot(board, col);

  updateFilledColumns(board);
}

bool boardFilled(Board *board) {
  for (int x = 0; x < BOARD_WIDTH; x++) {
    // pointer arithmetic
    if (*(board->notFilledColumn + x)) {
      return false;
    }
  }
  return true;
}

bool checkWin(Board *board) {
  // check vertical win
  for (int x = 0; x < BOARD_WIDTH; x++) {
    for (int y = 0; y <= BOARD_HEIGHT - 4; y++) {
      Spot startSpot = board->grid[x][y];
      if (startSpot == Spot::Empty) {
        continue;
      }
      bool allMatching = true;
      for (int i = 1; i < 4; i++) {
        if (board->grid[x][y + i] != startSpot) {
          allMatching = false;
        }
      }
      if (allMatching) {
        return true;
      }
    }
  }

  // check horizontal win
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x <= BOARD_WIDTH - 4; x++) {
      Spot startSpot = board->grid[x][y];
      if (startSpot == Spot::Empty) {
        continue;
      }
      bool allMatching = true;
      for (int i = 1; i < 4; i++) {
        if (board->grid[x + i][y] != startSpot) {
          allMatching = false;
        }
      }
      if (allMatching) {
        return true;
      }
    }
  }

  // check diagonal left
  for (int x = 3; x < BOARD_WIDTH; x++) {
    for (int y = 0; y <= BOARD_HEIGHT - 4; y++) {
      Spot startSpot = board->grid[x][y];
      if (startSpot == Spot::Empty) {
        continue;
      }
      bool allMatching = true;
      for (int i = 1; i < 4; i++) {
        if (board->grid[x - i][y + i] != startSpot) {
          allMatching = false;
        }
      }
      if (allMatching) {
        return true;
      }
    }
  }

  // check diagonal right
  for (int x = 0; x <= BOARD_WIDTH - 4; x++) {
    for (int y = 0; y <= BOARD_HEIGHT - 4; y++) {
      Spot startSpot = board->grid[x][y];
      if (startSpot == Spot::Empty) {
        continue;
      }
      bool allMatching = true;
      for (int i = 1; i < 4; i++) {
        if (board->grid[x + i][y + i] != startSpot) {
          allMatching = false;
        }
      }
      if (allMatching) {
        return true;
      }
    }
  }

  return false;
}

void printGame(Board *board) {
  // clearScreen();
  std::cout << std::endl;

  for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
    std::cout << " | ";
    for (int x = 0; x < BOARD_WIDTH; x++) {
      std::cout << spotToChar(board->grid[x][y]) << " ";
    }
    std::cout << "|" << std::endl;
  }

  std::cout << " +---------------+" << std::endl;
  std::cout << " | 1 2 3 4 5 6 7 |" << std::endl;
  if (board->turn != Spot::Empty) {
    std::cout << "Current turn: " << spotToChar(board->turn) << std::endl;
  } else {
    std::cout << "Game over!" << std::endl;
  }
}

ControlOptions setUpPlayer(int playerNum) {
  std::cout << "Player " << playerNum << ": [P]erson, [E]asy AI, [H]ard AI? ";
  char playerChar;
  std::cin >> playerChar;

  switch (playerChar) {
  case 'P':
  case 'p':
    return ControlOptions::Person;
  case 'E':
  case 'e':
    return ControlOptions::Easy;
  case 'H':
  case 'h':
    return ControlOptions::Hard;
  }

  std::cout << "Invalid choice" << std::endl;
  return setUpPlayer(playerNum);
}

pair<ControlOptions, ControlOptions> setUpPlayers() {
  std::cout << "Player 1 starts (and is 'X's)" << std::endl;
  std::cout << "Player 2 is 'O's\n" << std::endl;
  ControlOptions player1 = setUpPlayer(1);
  ControlOptions player2 = setUpPlayer(2);
  return std::make_pair(player1, player2);
}

void saveGameData(Board *board, ControlOptions player1,
                  ControlOptions player2) {
  std::ofstream fout("gameSaves.txt", std::ios::trunc);

  for (int x = 0; x < BOARD_WIDTH; x++) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
      fout << spotToChar(board->grid[x][y], true);
    }
    fout << std::endl;
  }
  fout << player1 << std::endl;
  fout << player2 << std::endl;
  fout.close();
}

tuple<Board, ControlOptions, ControlOptions> readSaveGameData() {
  std::ifstream saveReader("gameSaves.txt");
  Board board = {{}, {}, Spot::X};
  std::string line;

  int x = 0;
  while (saveReader.good() && x < BOARD_WIDTH) {
    std::getline(saveReader, line);
    for (int y = 0; y < BOARD_HEIGHT; y++) {
      board.grid[x][y] = charToSpot(line[y]);
    }
    x++;
  }

  updateFilledColumns(&board);

  bool gameOver = checkWin(&board) || boardFilled(&board);

  ControlOptions player1, player2;
  if (saveReader.good() && x == BOARD_WIDTH && !gameOver) {
    std::getline(saveReader, line);
    player1 = ControlOptions(std::stoi(line));

    std::getline(saveReader, line);
    player2 = ControlOptions(std::stoi(line));

    Spot currentTurn = Spot::X;
    for (int x = 0; x < BOARD_WIDTH; x++) {
      for (int y = 0; y < BOARD_HEIGHT; y++) {
        Spot boardSpot = board.grid[x][y];
        if (boardSpot != Spot::Empty) {
          currentTurn = nextTurn(currentTurn);
        }
      }
    }
    board.turn = currentTurn;
  } else {
    pair<ControlOptions, ControlOptions> players = setUpPlayers();
    player1 = players.first;
    player2 = players.second;

    // default blank board
    // Spot::Empty is first and is used as the default value for Board.grid
    board = {{}, {true, true, true, true, true, true, true}, Spot::X};
  }

  saveReader.close();

  return std::make_tuple(board, player1, player2);
}

vector<int> openMoves(Board *board) {
  vector<int> validMoves = vector<int>();
  for (int i = 0; i < BOARD_WIDTH; i++) {
    if (board->notFilledColumn[i]) {
      validMoves.push_back(i);
    }
  }
  return validMoves;
}

int bestMove(Board *board) {

  int bestScore = INT_MIN;
  int bestCol = 0;

  vector<int> validMoves = openMoves(board);
  for (int col : validMoves) {
    dropSpot(board, col);

    bool isMaximizing = board->turn != Spot::O;
    int score = minimax(board, 0, isMaximizing);

    undoMove(board, col);
    if (score > bestScore) {
      bestScore = score;
      std::cout << bestScore << std::endl;
      bestCol = col;
    }
  }

  return bestCol;
}

int minimax(Board *board, int depth, bool isMaximizing) {
  std::cout << "Depth: " << depth << std::endl;
  updateFilledColumns(board);

  bool gameWon = checkWin(board);
  if (gameWon && board->turn == Spot::X) {
    return 10;
  } else if (gameWon && board->turn == Spot::O) {
    return -10;
  }

  bool tied = boardFilled(board);
  if (tied) {
    return 0;
  }

  if (isMaximizing) {
    int bestScore = INT_MIN;
    vector<int> validMoves = openMoves(board);
    for (int col : validMoves) {
      dropSpot(board, col);

      board->turn = nextTurn(board->turn);
      int score = minimax(board, depth + 1, false);
      board->turn = nextTurn(board->turn);

      undoMove(board, col);

      bestScore = MAX(score, bestScore);
    }
    return bestScore;
  } else {
    int bestScore = INT_MAX;
    vector<int> validMoves = openMoves(board);
    for (int col : validMoves) {
      dropSpot(board, col);

      board->turn = nextTurn(board->turn);
      int score = minimax(board, depth + 1, true);
      board->turn = nextTurn(board->turn);

      undoMove(board, col);

      bestScore = MIN(score, bestScore);
    }
    return bestScore;
  }
}

int main() {
  srand(time(NULL));

  tuple<Board, ControlOptions, ControlOptions> saveData = readSaveGameData();
  Board board = std::get<0>(saveData);
  ControlOptions player1 = std::get<1>(saveData);
  ControlOptions player2 = std::get<2>(saveData);

  while (true) { // break when game is over or board is filled
    printGame(&board);

    bool won = checkWin(&board);
    bool isBoardFilled = boardFilled(&board);

    saveGameData(&board, player1, player2);

    if (won || isBoardFilled) {
      if (won) {
        char winner = spotToChar(nextTurn(board.turn));
        std::cout << "The winner is: " << winner << std::endl;
      } else {
        std::cout << "Tie" << std::endl;
      }
      break; // skip running move()
    }

    ControlOptions player;
    switch (board.turn) {
    case Spot::X:
      player = player1;
      break;
    case Spot::O:
      player = player2;
      break;
    default:
      continue;
    }

    move(&board, player);
    board.turn = nextTurn(board.turn);
  }
}