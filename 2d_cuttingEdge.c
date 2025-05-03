#include <stdio.h>
#include <stdlib.h>

#define MAX_TYPES 10
#define MAX_COPIES 100
#define SHEET_DISPLAY_SIZE 20 

typedef struct {
    int length;
    int width;
} PieceType;

typedef struct {
    int type_id;
    int placed;
    int sheet_id;
    int x, y;
    int rotated;
} Piece;

PieceType piece_types[MAX_TYPES];
int piece_demand[MAX_TYPES];
Piece pieces[MAX_COPIES];
int total_pieces = 0;

int sheet_length, sheet_width;

int best_solution = 1e9;
Piece best_arrangement[MAX_COPIES];

int max(int a, int b) {
    return (a > b) ? a : b;
}

int can_place(int piece_id, int sheet_id, int x, int y, int rotated) {
    int l = piece_types[pieces[piece_id].type_id].length;
    int w = piece_types[pieces[piece_id].type_id].width;
    if (rotated) {
        int temp = l;
        l = w;
        w = temp;
    }

    if (x + l > sheet_length || y + w > sheet_width)
        return 0; 

    for (int i = 0; i < total_pieces; i++) {
        if (pieces[i].placed && pieces[i].sheet_id == sheet_id) {
            int x1 = pieces[i].x;
            int y1 = pieces[i].y;
            int l1 = piece_types[pieces[i].type_id].length;
            int w1 = piece_types[pieces[i].type_id].width;
            if (pieces[i].rotated) {
                int temp = l1;
                l1 = w1;
                w1 = temp;
            }

            int x2 = x;
            int y2 = y;

            if (!(x2 + l <= x1 || x2 >= x1 + l1 || y2 + w <= y1 || y2 >= y1 + w1))
                return 0;
        }
    }
    return 1;
}

void branch(int placed_count, int sheets_used) {
    if (placed_count == total_pieces) {
        if (sheets_used < best_solution) {
            best_solution = sheets_used;
            for (int i = 0; i < total_pieces; i++) {
                best_arrangement[i] = pieces[i];
            }
        }
        return;
    }

    if (sheets_used >= best_solution) {
        return;
    }

    for (int i = 0; i < total_pieces; i++) {
        if (!pieces[i].placed) {
            for (int s = 0; s < sheets_used; s++) {
                for (int x = 0; x <= sheet_length; x++) {
                    for (int y = 0; y <= sheet_width; y++) {
                        if (can_place(i, s, x, y, 0)) {
                            pieces[i].placed = 1;
                            pieces[i].sheet_id = s;
                            pieces[i].x = x;
                            pieces[i].y = y;
                            pieces[i].rotated = 0;
                            branch(placed_count + 1, sheets_used);
                            pieces[i].placed = 0;
                        }
                        if (can_place(i, s, x, y, 1)) {
                            pieces[i].placed = 1;
                            pieces[i].sheet_id = s;
                            pieces[i].x = x;
                            pieces[i].y = y;
                            pieces[i].rotated = 1;
                            branch(placed_count + 1, sheets_used);
                            pieces[i].placed = 0;
                        }
                    }
                }
            }

            pieces[i].placed = 1;
            pieces[i].sheet_id = sheets_used;
            pieces[i].x = 0;
            pieces[i].y = 0;
            pieces[i].rotated = 0;
            branch(placed_count + 1, sheets_used + 1);
            pieces[i].placed = 0;

            break;
        }
    }
}

void draw_sheets() {
    printf("\n=== SHEET ARRANGEMENTS ===\n");

    for (int s = 0; s < best_solution; s++) {
        printf("\nSheet %d:\n", s + 1);
        char sheet[SHEET_DISPLAY_SIZE][SHEET_DISPLAY_SIZE];

        for (int i = 0; i < SHEET_DISPLAY_SIZE; i++)
            for (int j = 0; j < SHEET_DISPLAY_SIZE; j++)
                sheet[i][j] = '.';

        for (int i = 0; i < total_pieces; i++) {
            if (best_arrangement[i].sheet_id == s) {
                int l = piece_types[best_arrangement[i].type_id].length;
                int w = piece_types[best_arrangement[i].type_id].width;
                if (best_arrangement[i].rotated) {
                    int temp = l;
                    l = w;
                    w = temp;
                }
                for (int x = 0; x < l && best_arrangement[i].x + x < SHEET_DISPLAY_SIZE; x++) {
                    for (int y = 0; y < w && best_arrangement[i].y + y < SHEET_DISPLAY_SIZE; y++) {
                        sheet[best_arrangement[i].x + x][best_arrangement[i].y + y] = 'A' + best_arrangement[i].type_id;
                    }
                }
            }
        }

        for (int i = 0; i < sheet_length; i++) {
            for (int j = 0; j < sheet_width; j++) {
                printf("%c ", sheet[i][j]);
            }
            printf("\n");
        }
    }
}

int main() {
    printf("Enter sheet length and width: ");
    scanf("%d %d", &sheet_length, &sheet_width);

    int num_types;
    printf("Enter number of piece types: ");
    scanf("%d", &num_types);

    for (int i = 0; i < num_types; i++) {
        printf("Enter length and width for piece type %d: ", i + 1);
        scanf("%d %d", &piece_types[i].length, &piece_types[i].width);
        printf("Enter number of pieces needed for this type: ");
        scanf("%d", &piece_demand[i]);

        for (int j = 0; j < piece_demand[i]; j++) {
            pieces[total_pieces].type_id = i;
            pieces[total_pieces].placed = 0;
            total_pieces++;
        }
    }

    branch(0, 0);

    printf("\nMinimum sheets needed = %d\n", best_solution);

    draw_sheets();

    return 0;
}
