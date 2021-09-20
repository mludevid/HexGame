#include <iostream>
#include <string>

using namespace std;

enum GameState {
    ONGOING,
    WHITE_WON,
    BLACK_WON
};

class Hex {
    // 13 x 13
    private:
        bool white_to_move = true;
        bool white[169];
        bool black[169];
        bool white_connected_top[169];
        bool white_connected_bottom[169];
        bool black_connected_left[169];
        bool black_connected_right[169];
        GameState state = ONGOING;
        // TODO: Make this DIM variable
        int DIM = 13;
        
    int count_digits(int num) {
        int count = 0;
        while (num != 0) {
            count++;
            num /= 10;
        }
        return count;
    }
        
    public:
        string to_str() {
            // TODO: Autogenerate letters + separation between squares from DIM
            string ret = " a  b  c  d  e  f  g  h  i  j  k  l  m\n"; // DIM
            for (int i = 0; i < DIM; i++) {
                // add spaces:
                ret += string(i + 1 - count_digits(i + 1), ' ');
                ret += to_string(i + 1);
                ret += "\\";
                
                // add pieces:
                for (int j = 0; j < DIM - 1; j++) {
                    if (white[i * DIM + j] == true) {
                        ret += "W  ";
                    } else if (black[i * DIM + j] == true) {
                        ret += "B  ";
                    } else {
                        ret += ".  ";
                    }
                }
                if (white[i * DIM + DIM - 1] == true) {
                    ret += "W";
                } else if (black[i * DIM + DIM - 1] == true) {
                    ret += "B";
                } else {
                    ret += ".";
                }
                ret += "\\";
                ret += to_string(i + 1);
                ret += "\n";
            }
            ret += string(DIM + 2, ' ');
            ret += "a  b  c  d  e  f  g  h  i  j  k  l  m\n";
            switch (state) {
                case ONGOING:
                    if (white_to_move) {
                        ret += "White to move.\n";
                    } else {
                        ret += "Black to move.\n";
                    }
                    break;
                case WHITE_WON:
                    ret += "White won!";
                    break;
                case BLACK_WON:
                    ret += "Black won!";
                    break;
            }
            return ret;
        }
        
        bool make_move(int row, int col) {
            // Check if row and col is legal
            if (row < 0 || col < 0 || row >= DIM || col >= DIM) {
                return false;
            }
            
            // Check if cell is still empty
            if (white[row * DIM + col] || black[row * DIM + col]) {
                return false;
            }
            
            if (white_to_move) {
                white[row * DIM + col] = true;
                white_to_move = false;
            } else {
                black[row * DIM + col] = true;
                white_to_move = true;
            }
            
            update_connected(row, col);
            update_game_over();
            return true;
        }
        
        bool make_move(string move) {
            if (move.compare("swap-pieces") == 0) {
                // TODO: Implement swap
                return false;
            } else if (move.compare("resign") == 0) {
                // TODO: Implement resign and similar moves.
                return false;
            } else {
                int i = 0;
                // Get col: (letter(s))
                // 'a' = 1 .... 'z' = 26, 'aa' = 27 ...
                // TODO: Test if this conversion actually works
                int col = 0;
                while (i < move.length() && move[i] >= 'a' && move[i] <= 'z') {
                    col *= 26;
                    col += move[i] - 'a' + 1; 
                    i++;
                }
                // Get row: (number(s))
                int row = 0;
                while (i < move.length() && move[i] >= '0' && move[i] <= '9') {
                    row *= 10;
                    row += move[i] - '0';
                    i++;
                }

                // Check if string over
                if (i != move.length()) {
                    return false;
                }

                row--;
                col--;

                return make_move(row, col);
            }
        }
        
        void get_neighbors(int row, int col, int neighbors[6][2]) {
            // above left
            neighbors[0][0] = row - 1;
            neighbors[0][1] = col;
            // above right
            neighbors[1][0] = row - 1;
            neighbors[1][1] = col + 1;
            // left
            neighbors[2][0] = row;
            neighbors[2][1] = col - 1;
            // right
            neighbors[3][0] = row;
            neighbors[3][1] = col + 1;
            // below left
            neighbors[4][0] = row + 1;
            neighbors[4][1] = col - 1;
            // below right
            neighbors[5][0] = row + 1;
            neighbors[5][1] = col;
        }
        
        void depth_search(int row, int col, bool pieces[169], bool connected[169]) {
            connected[row * DIM + col] = true;
            int neighbors[6][2];
            get_neighbors(row, col, neighbors);
            for (int i = 0; i < 6; i++) {
                if (neighbors[i][0] >= 0 && neighbors[i][0] < DIM && neighbors[i][1] >= 0 && neighbors[i][1] < DIM
                        && pieces[neighbors[i][0] * DIM + neighbors[i][1]] && !connected[neighbors[i][0] * DIM + neighbors[i][1]]) {
                    depth_search(neighbors[i][0], neighbors[i][1], pieces, connected);
                }
            }
        }
        
        void update_connected(int move_row, int move_col) {
            // Check if neighbour of move are connected to edge or move is on edge
            // If yes then start depth first search from current square
            if (white[move_row * DIM + move_col]) {
                // WHITE MOVE
                if (move_row == 0) {
                    depth_search(move_row, move_col, white, white_connected_top);
                } else if (move_row == DIM - 1) {
                    depth_search(move_row, move_col, white, white_connected_bottom);
                }
            } else {
                // BLACK MOVE
                if (move_col == 0) {
                    depth_search(move_row, move_col, black, black_connected_left);
                } else if (move_col == DIM - 1) {
                    depth_search(move_row, move_col, black, black_connected_right);
                }
            }
            int neighbors[6][2];
            get_neighbors(move_row, move_col, neighbors);
            for (int i = 0; i < 6; i++) {
                if (neighbors[i][0] >= 0 && neighbors[i][0] < DIM && neighbors[i][1] >= 0 && neighbors[i][1] < DIM) {
                    if (white_connected_top[neighbors[i][0] * DIM + neighbors[i][1]]) {
                        depth_search(move_row, move_col, white, white_connected_top);
                    } else if (white_connected_bottom[neighbors[i][0] * DIM + neighbors[i][1]]) {
                        depth_search(move_row, move_col, white, white_connected_bottom);
                    } else if (black_connected_left[neighbors[i][0] * DIM + neighbors[i][1]]) {
                        depth_search(move_row, move_col, black, black_connected_left);
                    } else if (black_connected_right[neighbors[i][0] * DIM + neighbors[i][1]]) {
                        depth_search(move_row, move_col, black, black_connected_right);
                    }
                }
            }
        }
        
        void update_game_over() {
            // Check if any square is connected to both edges
            if (state != ONGOING) {return;}
            
            for (int i = 0; i < DIM * DIM; i++) {
                if (white_connected_top[i] && white_connected_bottom[i]) {
                    state = WHITE_WON;
                    return;
                }
                if (black_connected_left[i] && black_connected_right[i]) {
                    state = BLACK_WON;
                    return;
                }
            }
        }
};

int main() {
    Hex c = Hex();
    
    while (true) {
        cout << c.to_str() << endl << "Move: ";
        string inp;
        cin >> inp;
        c.make_move(inp);
    }

    return 0;
}