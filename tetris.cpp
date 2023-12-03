#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#if defined(_WIN32)
#include "headers/winstd.h"
#elif defined(_WIN64)
#include "headers/winstd.h"
#else
#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <math.h>
#include <algorithm>
using namespace std;

#include "headers/cube.h"
#include "headers/tetromino.h"

#define PI 3.14159265f

float RED[3] = {1.0f, 0.0f, 0.0f};
float GREEN[3] = {0.0f, 1.0f, 0.0f};
float BLUE[3] = {0.0f, 0.0f, 1.0f};
float MAGENTA[3] = {1.0f, 0.0f, 1.0f};
float CYAN[3] = {0.0f, 1.0f, 1.0f};
float YELLOW[3] = {1.0f, 1.0f, 0.0f};
float GREY[3] = {0.2f, 0.2f, 0.2f};
float LIGHTGREY[3] = {0.6f, 0.6f, 0.6f};
float SKYBLUE[3] = {0.53f, 0.81f, 0.92f};
float WOODBROWN[3] = {0.75f, 0.60f, 0.42f};
float GRASSGREEN[3] = {0.34f, 0.49f, 0.27f};
float BLACK[3] = {0.0f, 0.0f, 0.0f};

/* Material properties */
struct material_t {
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess[1];
};
// For metal (Used for tetromino and game board)
const material_t metal = {
  {0.05, 0.05, 0.05, 1.0},
  {0.1, 0.1, 0.1, 1.0},
  {1.0, 1.0, 1.0, 1.0},
  {50.0}
};
// For wood
const material_t wood = {
  {0.05, 0.05, 0.05, 1.0},
  {0.1, 0.1, 0.1, 0.1},
  {0.0, 0.0, 0.0, 1.0},
  {1.0}
};
// For text
const material_t neon_text = {
  {10.0, 10.0, 10.0, 1.0},
  {10.0, 10.0, 10.0, 1.0},
  {0.0, 0.0, 0.0, 1.0},
  {1.0}
};

// Game Constants
const float BLOCKWIDTH = 3.0f;
const int EASYDELAY = 120;
const int NORMALDELAY = 60;
const int HARDDELAY = 20;
// Game variables
bool isPause = false;
// Graphical variables
bool is3D = true;
int fov = 60;
int boxwidth = 10;
int boxheight = 20;
int sidewidth = 10; // In BLOCKWIDTH units
int far_point = 500;   // In BLOCKWIDTH units
float campos[3] = {boxwidth/2.0f,boxheight/2.0f,(boxwidth+sidewidth*2.0f)*tan((180.0f-fov)*PI/2.0f/180.0f)/2.0f};
// Gameplay variables
int difficulty = 1; // Ranges from 0 to 2, 0 - Easy, 1 - Normal, 2 - Hard
int dropDelay = NORMALDELAY; // Time period for one block drop: Easy - 120, Normal - 60, Hard - 20
int framesElapsed = 1;
int score = 0;
int comboMulti = 2;
int pointsPerRow = 5;
// Selected game environment colours
float *signColour = WOODBROWN;
float *boardColour = WOODBROWN;
float *groundColour = GRASSGREEN;
// Game Objects
static Cube ***gameBoard;
static Tetromino *currBlock;
static Tetromino *blockLA;
static Tetromino *nextBlock;



/* Function Declarations */
void quitGame();
// Declare functions due to circular dependency
void pauseFunc();
void update();


/* Graphical functions */
// Render the passed text
void draw_text(string text){
  glScalef(0.01f, 0.007f, 1.0f);
  for (int i=0; i < text.length(); i++){
    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, text[i]);
  }
}

// Set light parameters
void set_light(){
  float amb_light[] = {0.0f, 0.0f, 0.0f, 0.0f};
  float diff_light[] = {0.5f, 0.5f, 0.5f, 1.0f};
  float spec_light[] = {0.8f, 0.8f, 0.8f, 1.0f};
  float light_pos[] = {boxwidth/2.0f, boxheight/2.0f, 10.0f, 1.0f/BLOCKWIDTH};

  glLightfv(GL_LIGHT0, GL_AMBIENT, amb_light);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diff_light);
  glLightfv(GL_LIGHT0, GL_SPECULAR, spec_light);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
}

// Apply the given material
void apply_mat(const material_t mat){
  // set the surface properties
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat.shininess);
}


/* Drawing functions */
// Draw the block container
void drawBoard(){
  apply_mat(wood);
  glColor3fv(boardColour);
  // Left side
  glPushMatrix();
    glTranslatef(-0.5f, 0, 0);
    glScalef(0.5f, 1.0f, 1.5f);
    for (int i=0; i < boxheight; i++){
      glCallList(cube_handle);
      glTranslatef(0, 1.0f, 0);
    }
  glPopMatrix();
  // Bottom
  glPushMatrix();
    glTranslatef(-0.5f, -0.5f, 0);
    glScalef(boxwidth+1, 0.5f, 1.5f);
    glCallList(cube_handle);
  glPopMatrix();
  // Right side
  glPushMatrix();
    glTranslatef(boxwidth, 0, 0);
    glScalef(0.5f, boxheight, 1.5f);
    glCallList(cube_handle);
  glPopMatrix();
}

// Draw the status board on the side, showing current score (and next block?)
void drawStatusBoard(){
  string score_str = to_string(score);
  int score_len = int(score_str.length());

  glPushMatrix();
    glTranslatef(-sidewidth/2, -0.5f, 0.0f);
    // If 3D, move back 2 blocks
    if (is3D) glTranslatef(0.0f, 0.0f, -2.0f);

    // Draw signpost
    apply_mat(wood);
    glColor3fv(signColour);
    glPushMatrix();
      glTranslatef(-0.25, 0.0f, -0.5f);
      glScalef(0.5f, 2.0f, 0.5f);
      glCallList(cube_handle);
    glPopMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f);

    // Draw sign board and its text
    glPushMatrix();
      glRotatef(5.0f, 0.0f, 0.0f, 1.0f);  // Skew the sign board (for aesthetics)
      apply_mat(wood);
      glPushMatrix();
        int sign_width = max(5, score_len) + 1;
        glTranslatef(-sign_width/2.0f, 0.0f, -0.5f);
        glScalef(sign_width, 2.3f, 0.5f);
        glCallList(cube_handle);
      glPopMatrix();
      glTranslatef(0.0f, 0.2f, 0.1f);
      // Draw the scoreboard text
      glColor3fv(WHITE);
      apply_mat(neon_text);
      glPushMatrix();
        glTranslatef(-2.5f, 1.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);
        draw_text("Score");
      glPopMatrix();
      glPushMatrix();
        glTranslatef(-score_len/2.0f, 0.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);
        draw_text(score_str);
      glPopMatrix();
    glPopMatrix();
    glTranslatef(0.0f, 2.3f, 0.0f);

    // Draw another signpost
    apply_mat(wood);
    glColor3fv(signColour);
    glPushMatrix();
      glTranslatef(-0.25, 0.0f, -0.5f);
      glScalef(0.5f, 2.0f, 0.5f);
      glCallList(cube_handle);
    glPopMatrix();
    glTranslatef(0.0f, 1.8f, 0.0f);

    // Draw next block sign board and its contents
    glPushMatrix();
      glRotatef(5.0f, 0.0f, 0.0f, -1.0f);  // Skew the sign board (for aesthetics)
      apply_mat(wood);
      glPushMatrix();
        sign_width = 5;
        glTranslatef(-sign_width/2.0f, 0.0f, -0.5f);
        glScalef(sign_width, 6.3f, 0.5f);
        glCallList(cube_handle);
      glPopMatrix();
      glTranslatef(0.0f, 0.2f, 0.1f);
      // Draw the scoreboard text
      glColor3fv(WHITE);
      apply_mat(neon_text);
      glPushMatrix();
        glTranslatef(-2.0f, 5.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);
        draw_text("Next");
      glPopMatrix();
      apply_mat(wood);
      glPushMatrix();
        // Draw the background behind the next block
        glTranslatef(-2.0f, 0.5f, 0.0f);
        glColor3fv(BLACK);
        glBegin(GL_POLYGON);
          glVertex3f(0.0f, 0.0f, 0.0f);
          glVertex3f(4.0f, 0.0f, 0.0f);
          glVertex3f(4.0f, 4.0f, 0.0f);
          glVertex3f(0.0f, 4.0f, 0.0f);
        glEnd();
        // Put next block here
        glTranslatef(-nextBlock->getX(), -nextBlock->getY(), 0.0f); // Since the block is instantiated at the top of the containter
        glTranslatef(2.0f, 2.0f, 0.0f);
        glTranslatef(-nextBlock->getW()/2.0f, -nextBlock->getH()/2.0f, 0.1f);
        apply_mat(metal);
        nextBlock->display();
      glPopMatrix();
    glPopMatrix();
    glTranslatef(0.0f, 6.3f, 0.0f);

  glPopMatrix();
}

// Draw other scene objects
void drawScenery(){
  // The Ground
  glColor3fv(groundColour);
  apply_mat(wood);
  glPushMatrix();
    if (is3D){
      glTranslatef(campos[0], -0.5f, campos[2]+1);
      glScalef(2*far_point*tan(fov/2), 10, far_point);
      glTranslatef(-0.5f, -1.0f, -1.0f);
      glCallList(cube_handle);
      // glScalef(4.0f, 1.0f, 4.0f);
      // glTranslatef(-25, -1.0f, -far);
      // for (int i=0; i < far; i++) {
      //   glPushMatrix();
      //   for (int j=0; j < 50; j++){
      //     glCallList(cube_handle);
      //     glTranslatef(1, 0, 0);
      //   }
      //   glPopMatrix();
      //   glTranslatef(0, 0, 1);
      // }
    } else {
      glTranslatef(0, -0.5f, 0);
      glScalef(2*far_point*tan(fov/2), 10, far_point);
      glTranslatef(-0.5f, -1.0f, -1.0f);
      glCallList(cube_handle);
    }
  glPopMatrix();
}

// Draw cubes that are set onto the board
void drawPlacedCubes(){
  apply_mat(metal);
  for (int i=0; i < (int) boxheight/2; i++){
    for (int j=0; j<(int) boxwidth/2; j++)
      if (gameBoard[i][j] != nullptr)
        gameBoard[i][j]->display(j,i);
    for (int j=boxwidth-1; j>=(int) boxwidth/2; j--)
      if (gameBoard[i][j] != nullptr)
        gameBoard[i][j]->display(j,i);
  }
  for (int i=boxheight-1; i >= (int) boxheight/2; i--){
    for (int j=0; j<(int) boxwidth/2; j++)
      if (gameBoard[i][j] != nullptr)
        gameBoard[i][j]->display(j,i);
    for (int j=boxwidth-1; j>=(int) boxwidth/2; j--)
      if (gameBoard[i][j] != nullptr)
        gameBoard[i][j]->display(j,i);
  }
}

// Draw the paused screen
void drawPaused(){
  glPushMatrix();
    glPushMatrix();
      glColor3fv(BLACK);
      glTranslatef(-100.0f, 100.0f, 0.0f);
      glBegin(GL_POLYGON);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(200.0f, 0.0f, 0.0f);
        glVertex3f(200.0f, -200.0f, 0.0f);
        glVertex3f(0.0f, -200.0f, 0.0f);
      glEnd();
    glPopMatrix();
    glPushMatrix();
      glColor3fv(WHITE);
      glTranslatef(-5.0f, 10.0f, 0.1f);
      glPushMatrix();
        glScalef(2.0f, 2.0f, 1.0f);
        draw_text("Paused");
      glPopMatrix();
      glTranslatef(0.0f, -3.0f, 0.0f);
      draw_text("Press p to unpause");
    glPopMatrix();
  glPopMatrix();
}


/* Game functions */
// Select random colour for tetrimino
float* rand_colour(){
  switch(rand()%6){
    case 0:
      return RED;
    case 1:
      return GREEN;
    case 2:
      return BLUE;
    case 3:
      return MAGENTA;
    case 4:
      return CYAN;
    case 5:
      return YELLOW;
  }
}

// Randomly select a tetromino
Tetromino &randomBlock(){
  Tetromino *block;
  switch(rand()%7){
    case 0:
      block = new IBlock((int) (boxwidth/2)-1, boxheight, rand_colour());
      break;
    case 1:
      block = new LBlock((int) (boxwidth/2)-1, boxheight, rand_colour());
      break;
    case 2:
      block = new JBlock((int) (boxwidth/2)-1, boxheight, rand_colour());
      break;
    case 3:
      block = new ZBlock((int) (boxwidth/2)-1, boxheight, rand_colour());
      break;
    case 4:
      block = new SBlock((int) (boxwidth/2)-1, boxheight, rand_colour());
      break;
    case 5:
      block = new TBlock((int) (boxwidth/2)-1, boxheight, rand_colour());
      break;
    case 6:
      block = new SqBlock((int) (boxwidth/2)-1, boxheight, rand_colour());
      break;
  }
  return *block;
}

// Updates variables concerning difficulty (Block speed, points & combo)
void updateDifficulty(){
  switch (difficulty) {
    case 0:
      dropDelay = EASYDELAY;
      comboMulti = 1;
      pointsPerRow = 5;
      break;
    case 1:
      dropDelay = NORMALDELAY;
      comboMulti = 2;
      pointsPerRow = 5;
      break;
    case 2:
      dropDelay = HARDDELAY;
      comboMulti = 3;
      pointsPerRow = 10;
      break;
    case 3: // Unfair difficulty, half the box height
      dropDelay = HARDDELAY;
      boxheight = 10;
      comboMulti = 4;
      pointsPerRow = 20;
      break;
  }
}

// When a row is completed
void completeRow(int row){
  for (int j=0; j<boxwidth; j++){
    free(gameBoard[row][j]);
    gameBoard[row][j] = NULL;
  }
  free(gameBoard[row]);
  for (int i=row; i<boxheight-1; i++){
    gameBoard[i] = gameBoard[i+1];
  }
  gameBoard[boxheight-1] = (Cube **) calloc(boxwidth, sizeof(Cube*));
}

// Set the block into the game board
void setBlock(Tetromino &block){
  int x = block.getX();
  int y = block.getY();
  // Place all blocks onto board
  for (int i=0;i<4;i++){
    int *pos = block.getBlock(i);
    gameBoard[y+pos[1]][x+pos[0]] = new Cube(block.colour);
  }
  int points = 0; // Accumulated points for completed rows set by this block
  // Check block's rows for a complete set
  int offset = 0;
  int currentY = -1;
  for (int i=0;i<4;i++){
    int *pos = block.getBlock(i);
    // Skip iteration if row has been scanned before
    if (y+pos[1] != currentY)
      currentY = y+pos[1];
    else
      continue;
    for (int j=0; j<boxwidth; j++){
      if (gameBoard[y+pos[1]-offset][j] == NULL)
        break;
      if (j == boxwidth-1){
        completeRow(y+pos[1]-offset);
        points += (int) pointsPerRow * (pow(comboMulti, offset));
        offset++;
      }
    }
  }
  score += points;
}

// Checks if block collides with cubes in game board
bool isCollide(Tetromino &block){
  int x = block.getX();
  int y = block.getY();
  // Collision if out of left and bottom boundary
  if (y < 0 || x < 0)
    return true;
  for (int i=0;i<4;i++){
    int *pos = block.getBlock(i);
    // If cube is out of right boundary, has collided
    if (x+pos[0] >= boxwidth)
      return true;
    // If block's position occupied, has collided (Check if block is within boundary first, prevent array out of bounds)
    if (y+pos[1] < boxheight && gameBoard[y+pos[1]][x+pos[0]] != NULL)
      return true;
  }
  return false;
}

// Get the lookahead of the block, a ghost copy on top of the cubes below it
Tetromino &getLookahead(Tetromino &block){
  Tetromino &lookahead = *block.clone();
  lookahead.colour = LIGHTGREY;
  do {
    lookahead.translate(0,-1);
  } while(!isCollide(lookahead));
  lookahead.translate(0,1);
  return lookahead;
}

// Fetch new block (Need to free old blocks to prevent memory leaks)
void newBlock(){
  free(currBlock);
  free(blockLA);
  currBlock = nextBlock;
  nextBlock = &randomBlock();
  blockLA = &getLookahead(*currBlock);
}

// Check if the game is over when there are blocks placed out of bounds above game board
bool isGameOver(){
  for (int i=boxheight; i<boxheight+4;i++){
    for (int j=0; j<boxwidth; j++){
      if (gameBoard[i][j] != NULL){
        return true;
      }
    }
  }
  return false;
}


/* Block movement functions */
// Moves currBlock to the left, returns true if successful, false if not possible (Undoes movement in this case)
bool moveLeft(){
  currBlock->translate(-1,0);
  if (isCollide(*currBlock)){
    currBlock->translate(1,0);
    return false;
  }
  free(blockLA);
  blockLA = &getLookahead(*currBlock);
  return true;
}

// Moves currBlock to the right
bool moveRight(){
  currBlock->translate(1,0);
  if (isCollide(*currBlock)){
    currBlock->translate(-1,0);
    return false;
  }
  free(blockLA);
  blockLA = &getLookahead(*currBlock);
  return true;
}

// Moves currBlock down, returns false if collision occurs
bool moveDown(){
  currBlock->translate(0,-1);
  if (isCollide(*currBlock)){
    currBlock->translate(0,1);
    return false;
  }
  return true;
}

// Drops the block down to where the lookahead is (Specifically, the lookahead replaces the currBlock)
bool dropDown(){
  blockLA->colour = currBlock->colour;
  setBlock(*blockLA);
  newBlock();
  return true;
}

bool rotateLeft(){
  currBlock->rotate(-1);
  if (isCollide(*currBlock)){
    currBlock->rotate(1);
    return false;
  }
  free(blockLA);
  blockLA = &getLookahead(*currBlock);
  return true;
}

bool rotateRight(){
  currBlock->rotate(1);
  if (isCollide(*currBlock)){
    currBlock->rotate(-1);
    return false;
  }
  free(blockLA);
  blockLA = &getLookahead(*currBlock);
  return true;
}


/* Game program functions */
// Display function
void display () {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Clears the colour buffer and z-buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
    glScalef(BLOCKWIDTH, BLOCKWIDTH, BLOCKWIDTH);
    if (is3D)
      gluLookAt(campos[0], campos[1], campos[2],
        boxwidth/2,boxheight/2,0,
        0,1,0
      );
    // Show game when unpaused
    if (!isPause){
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);

      /* Display objects here */
      // Draw scenery
      drawScenery();
      // Draw scoreboard
      drawStatusBoard();
      // Draw game Board
      drawBoard();
      // Draw placed cubes
      drawPlacedCubes();
      // Draw the current block
      apply_mat(metal);
      currBlock->display();
      // Draw lookahead block
      apply_mat(wood);
      blockLA->display();

    // Draw the pause screen when paused (Turn off lighting effects)
    } else {
      glDisable(GL_LIGHT0);
      glDisable(GL_LIGHTING);
      drawPaused();
    }
  glPopMatrix();

  // Flush to display
  glutSwapBuffers();
}

// Initialise program here
void init() {
  srand(time(0));

  /* Init game objects */
  // Init game board
  gameBoard = (Cube ***) calloc(boxheight+4, sizeof(Cube**));
  for (int i=0; i<boxheight+4; i++)
    gameBoard[i] = (Cube **) calloc(boxwidth, sizeof(Cube*));

  currBlock = &randomBlock();
  nextBlock = &randomBlock();
  blockLA = &getLookahead(*currBlock);

  // Init graphical objects and settings
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (is3D)
    gluPerspective(fov, 1.0f, 1, far_point*BLOCKWIDTH);  // For 3D
  else
    gluOrtho2D(-sidewidth*BLOCKWIDTH, (boxwidth+sidewidth)*BLOCKWIDTH, -2*BLOCKWIDTH, (boxheight+2)*BLOCKWIDTH); // For 2D
  glClearColor(SKYBLUE[0], SKYBLUE[1], SKYBLUE[2], 0.0f);
  init_cubes();

  glEnable(GL_DEPTH_TEST);  // Enable z-buffer for depth-testing
  // Turn on lighting effects
  set_light();
  // Setup coloured materials
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_NORMALIZE);
}

// Set as idle function when pausing game
void pauseFunc(){
  // Continue game if not paused
  if (!isPause)
    glutIdleFunc(update);
}

// Updates game in 60 ticks per second
void update(){
  if (isGameOver()){
    quitGame();
  }
  if (framesElapsed%dropDelay == 0){
    if (currBlock->getY() != blockLA->getY()){
      currBlock->translate(0,-1);
    } else {  // When block needs to be placed
      setBlock(*currBlock);
      newBlock();
    }
  }
  glutPostRedisplay();
  usleep(1000/60);
  framesElapsed++;

  // Stop updates when game is paused
  if (isPause)
    glutIdleFunc(pauseFunc);
}

void visible_handle(int vis){
  // Pause game when window is not visible and not on first frame
  if (vis != GLUT_VISIBLE && framesElapsed > 1)
    isPause = true;
}

// Input handler
void keyHandle(unsigned char key, int, int){
  switch(key){
    case 'A':
    case 'a':
      moveLeft();
      break;
    case 'S':
    case 's':
      if (!moveDown()){
        setBlock(*currBlock);
        newBlock();
      }
      break;
    case 'D':
    case 'd':
      moveRight();
      break;
    case 'W':
    case 'w':
      dropDown();
      break;
    case 'Q':
    case 'q':
      rotateLeft();
      break;
    case 'E':
    case 'e':
      rotateRight();
      break;
    case 'P':
    case 'p':
      isPause = !isPause;
      break;
  }
  glutPostRedisplay();
}

// Cleanup game program, specifically free up allocated memory
void cleanup(){
  for (int i=0; i<boxheight+4; i++){
    for (int j=0; j<boxwidth; j++)
      free(gameBoard[i][j]);
    free(gameBoard[i]);
  }
  free(gameBoard);
  free(currBlock);
  free(nextBlock);
  free(blockLA);
};

void quitGame(){
  cleanup();
  cout << "Final Score: " << score << endl;
  exit(0);
}

// Main program function
int main (int argc, char *argv[]) {
  // Parse arguments
  if (argc < 2){
    cerr << "Usage: ./tetris <difficulty> [2dmode] [width] [height]" << endl
    << "Difficulty: 0 - Easy, 1 - Normal, 2 - Hard, 3 - Unfair" << endl;
    exit(1);
  }
  // Difficulty required
  difficulty = *argv[1]%'0';
  if (difficulty < 0 || difficulty > 3) {
    cerr << "Difficulty: 0 - Easy, 1 - Normal, 2 - Hard, 3 - Unfair" << endl;
    exit(1);
  }
  updateDifficulty();

  // Parse 2dmode argument
  if (argc >= 3){
    if (*argv[2]%'0' != 0 && *argv[2]%'0' != 1){
      cerr << "2dmode arg takes 0 or 1" << endl;
      exit(2);
    }
    if (*argv[2]%'0' == 0)
      is3D = true;
    else
      is3D = false;
  }

  // Parse width
  if (argc >= 4){
    if (atoi(argv[3]) < 0){
      cerr << "Width cannot be negative" << endl;
      exit(3);
    }
    boxwidth = atoi(argv[3]);
  }

  // Parse height
  if (argc == 5){
    if (atoi(argv[4]) < 0){
      cerr << "Height cannot be negative" << endl;
      exit(4);
    }
    boxheight = atoi(argv[4]);
  }


  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(500, 500);

  glutCreateWindow("Tetris 0.3");

  glutDisplayFunc(display);
  glutIdleFunc(update);
  glutKeyboardFunc(keyHandle);
  glutVisibilityFunc(visible_handle);
  init();
  glutMainLoop();
  cleanup();
}
