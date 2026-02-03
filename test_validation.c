#include <stdio.h>
#include <stdbool.h>
#include "src/logic/validation.h"
#include "src/core/tile.h"

void test_validation() {
    printf("--- Testing Rummikub Validation ---\n");

    // Test Group (Serie)
    Tile g1[] = {createTile(5, RED), createTile(5, BLUE), createTile(5, BLACK)};
    printf("Group 5 Red,Blue,Black (3): %s\n", isValidGroup(g1, 3) ? "PASS" : "FAIL");

    Tile g2[] = {createTile(5, RED), createTile(5, BLUE), createJoker()};
    printf("Group 5 Red,Blue,Joker (3): %s\n", isValidGroup(g2, 3) ? "PASS" : "FAIL");

    Tile g3[] = {createTile(5, RED), createJoker(), createJoker()};
    printf("Group 5 Red,Joker,Joker (3): %s\n", isValidGroup(g3, 3) ? "PASS" : "FAIL");

    Tile g4[] = {createTile(5, RED), createTile(5, RED), createTile(5, BLUE)};
    printf("Group 5 Red,Red,Blue (3) [Should Fail]: %s\n", !isValidGroup(g4, 3) ? "PASS" : "FAIL");

    Tile g5[] = {createJoker(), createJoker(), createJoker()};
    printf("Group Joker,Joker,Joker (3) [Should Fail]: %s\n", !isValidGroup(g5, 3) ? "PASS" : "FAIL");

    // Test Run (Suite)
    Tile r1[] = {createTile(1, RED), createTile(2, RED), createTile(3, RED)};
    printf("Run 1,2,3 Red (3): %s\n", isValidRun(r1, 3) ? "PASS" : "FAIL");

    Tile r2[] = {createTile(10, BLUE), createJoker(), createTile(12, BLUE)};
    printf("Run 10,Joker,12 Blue (3): %s\n", isValidRun(r2, 3) ? "PASS" : "FAIL");

    Tile r3[] = {createJoker(), createTile(2, BLACK), createTile(3, BLACK), createJoker()};
    printf("Run Joker,2,3,Joker Black (4): %s\n", isValidRun(r3, 4) ? "PASS" : "FAIL");

    Tile r4[] = {createTile(12, YELLOW), createTile(13, YELLOW), createJoker()};
    printf("Run 12,13,Joker Yellow (3): %s\n", isValidRun(r4, 3) ? "PASS" : "FAIL");

    Tile r5[] = {createTile(1, RED), createTile(2, RED), createTile(4, RED)};
    printf("Run 1,2,4 Red (3) [Should Fail]: %s\n", !isValidRun(r5, 3) ? "PASS" : "FAIL");

    printf("--- End of Validation Testing ---\n");
}

int main() {
    test_validation();
    return 0;
}
