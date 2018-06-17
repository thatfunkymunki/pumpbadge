#include <SD.h>
#include "GFX4d.h"
#include "deque.h"

GFX4d gfx = GFX4d();

File dataFile;
String filename = "testkey.txt";


#define SCROLL_RATE 20
#define BLOCK_DELAY 0
#define BLANKS 0


#define BLUE32 (BLUE<<16|BLUE)
#define GREEN32 (GREEN<<16|GREEN)
#define RED32 (RED<<16|RED)
#define BLACK32 (BLACK<<16|BLACK)
#define WHITE32 (WHITE<<16|WHITE)
#define YELLOW32 (YELLOW<<16|YELLOW)

#define DCOL(x) (x<<16|x)


int colors[54];

uint16_t mirccolors[] = {WHITE, BLACK, BLUE, GREEN, RED, BROWN, PURPLE, ORANGE, YELLOW, LIGHTGREEN, CYAN, LIGHTCYAN, LIGHTBLUE, PINK, DARKGRAY, GRAY};

Deque canvas;

void setup() {
  Serial.begin(115200);
  gfx.begin();
  gfx.Cls();
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.FillScreen(BLACK);
  canvas = Deque();
  dataFile = SD.open(filename);
}

void loop() {  
  static uint32_t pixels[720];
  int scrollpos = 0;
  static int count = 0;
  node *curr = canvas.head;

  for(scrollpos = 0; scrollpos < 318; scrollpos+=6){
    //initialize for writing to GRAM
    gfx.setGRAM(0,scrollpos,240,scrollpos+6);
    
    if(curr == NULL){
      //advanced beyond end of deque
      break;
    }
    //Serial.print("Scrollpos: ");Serial.println(scrollpos,DEC);
    //Serial.print("Color is ");Serial.println(colors[count%60]&0xffff,HEX);

    //need to do this 2 blocks at a time to make it even. 
    //(blocks are 3px wide, display is 240px wide, pixels are 2 per uint32_t)
    //get 2 blocks, put into pixels
    //set first line of blocks
    for(int i = 0, offset = 0; i < LINE_WIDTH; i+=2,offset+=3){
      uint16_t block1 = mirccolors[curr->blocks[i]];
      uint16_t block2 = mirccolors[curr->blocks[i+1]];
      pixels[offset]   = block1 << 16 | block1;
      pixels[offset+1] = block1 << 16 | block2;
      pixels[offset+2] = block2 << 16 | block2;

    }
    
    //now copy this line to the 5 below it
    for(int i = 1; i < 6; i++){
      memcpy(pixels+(120 * i), pixels, 120 * sizeof(uint32_t));
    }
   
    //now write this to GRAM
    //Serial.println("Writing to GRAM");
    gfx.WrGRAMs(pixels,720);
    //advance pointer to next line
    curr = curr->next;
    count++;
    
  }

  //advance to next line in file
  getNextLine();

  //delay(15000);
  delay(SCROLL_RATE);
  //gfx.Cls(WHITE);
}



//this is an ugly as fuck function... reads the next line from the file and does some
//c string magic to extract the mircart blocks out of it.
//i'm not proud of it but this is so far beyond what C was intended for...
void getNextLine(){
  char *p;
  char col1[5];
  char col2[5];
  
  if(dataFile && dataFile.available()){
    String textfile = dataFile.readStringUntil('\n');
    Serial.println("Processing line");
    p = strtok((char *)textfile.c_str(),"\x03");
    //create node struct for line
    node *temp = (node *)calloc(1, sizeof(node));
    int remaining_blocks = LINE_WIDTH;
    while(p!=NULL){
            
      strncpy(col1, p, 2);
      strncpy(col2, p+3, 2);
      uint8_t color = atoi(col2);
      //Serial.print("remaining_blocks for this token: ");Serial.println(remaining_blocks);
    
      //Serial.print("color 1: ");Serial.println(atoi(col1));
     // Serial.print("color 2: ");Serial.println(atoi(col2));

      int numblocks = String(p).substring(6).length()+1;
      //Serial.print("num of chars after colors: ");Serial.println(numblocks);
      
      if(numblocks > LINE_WIDTH ){
        numblocks = LINE_WIDTH;
      }
      //Serial.print("numblocks: ");Serial.println(numblocks);
      for(int i = 0; i < numblocks; i++){
        int offset = LINE_WIDTH - remaining_blocks;
        if(offset >= 80){
          break;
        }
       // Serial.print("offset: ");Serial.println(offset);
        temp->blocks[offset] = color;
        remaining_blocks--;
      //Serial.print("remaining_blocks in the loop: ");Serial.println(remaining_blocks);
      }
      
     // Serial.println(String(p).substring(6));
      p = strtok(NULL,"\x03");
    }
    //add node to deque
    free(canvas.deleteHead());
    canvas.insertTail(temp);
 //this kills performance lol do not uncomment
 //canvas.print();
  }
  else{
    Serial.println("end of file reached");
    //put N blank lines
    for(int i = 0; i < BLANKS ; i++){
      
      node *temp = (node *)calloc(1, sizeof(node));
      memset(temp->blocks, 1, LINE_WIDTH);
      
      free(canvas.deleteHead());
      canvas.insertTail(temp);
    }
    //start the file over
    dataFile.close();
    dataFile = SD.open(filename);

  }
}


