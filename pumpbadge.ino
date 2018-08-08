#include "pumpbadge.h"
#include "deque.h"
#include <SD.h>
#include <Arduino.h>
#include <Print.h>
#include <GFX4d.h>

GFX4d gfx = GFX4d();

File dataFile;
File directory;

static const uint16_t mirccolors[] = {WHITE, BLACK, DARKBLUE, DARKGREEN, RED, BROWN, PURPLE, ORANGE, YELLOW, GREEN, CYAN, LIGHTCYAN, BLUE, MAGENTA, DARKGRAY, GRAY};

Deque canvas;


void setup() {
  Serial.begin(115200);
  gfx.begin();
  gfx.Cls();
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.FillScreen(BLACK);
  gfx.touch_Set(TOUCH_ENABLE);
  canvas = Deque();
  directory = SD.open("/ascii/");
  dataFile = SD.open("/ascii/111pump.txt");
}

void loop() {  
  static uint32_t pixels[720];
  int scrollpos = 0;
  static int count = 0;
  node *curr = canvas.head;

  if(gfx.touch_Update() && gfx.touch_GetPen() == TOUCH_PRESSED){
    //Serial.print("touch received at position: ");Serial.print(gfx.touch_GetX());Serial.print(", ");Serial.println(gfx.touch_GetY());
    if(gfx.touch_GetX() < 120){
      dataFile.seek(0);
    }
    else{
      nextFile();
    }
  }

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
    //Serial.println("Processing line");
    p = strtok((char *)textfile.c_str(),"\x03");
    //create node struct for line
    node *temp = (node *)calloc(1, sizeof(node));
    memset(temp->blocks, BG_COLOR, LINE_WIDTH);
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
    //Serial.println("end of file reached");
    nextFile();


  }
}
void nextFile(){
  //add BLANKS number of blank lines between asskeys
  for(int i = 0; i < BLANKS; i++){
    node *temp = (node *)calloc(1, sizeof(node));
    memset(temp->blocks, BG_COLOR, LINE_WIDTH);
    free(canvas.deleteHead());
    canvas.insertTail(temp);
  }

  dataFile.close();
  dataFile = directory.openNextFile();
  if(dataFile){
    Serial.print("Opening file: ");Serial.println(dataFile.name());
  }
  else{
    directory.rewindDirectory();
    dataFile = directory.openNextFile();
    Serial.print("Starting over with file: ");Serial.println(dataFile.name());
  }
}


