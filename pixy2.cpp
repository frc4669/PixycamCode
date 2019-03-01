/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <cmath>

#include <networktables/NetworkTableInstance.h>
#include <wpi/StringRef.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

#include <signal.h>
#include "libpixyusb2.h"

#define PI 3.14159265

Pixy2 pixycamF;
Pixy2 pixycamR;
static bool  run_flag = true;
const int uidF = -619097246;
const int uidR = 316060495;

void correctPixies(Pixy2& pixyF, Pixy2& pixyR){
  if(pixyF.m_link.callChirp("getUID")==uidF)
    return;
  else{
    std::swap(pixyF,pixyR);
  }
}

void handle_SIGINT(int unused)
{
  // On CTRL+C - abort! //

  run_flag = false;
}

void  get_line_features(Pixy2 pixy)
{
  int  Element_Index;

  // Query Pixy for line features //
  pixy.line.getAllFeatures();

  // Were vectors detected? //
  if (pixy.line.numVectors)
  {
    // Blocks detected - print them! //

    printf ("Detected %d vectors(s)\n", pixy.line.numVectors);

    for (Element_Index = 0; Element_Index < pixy.line.numVectors; ++Element_Index)
    {
      printf ("  Vector %d: ", Element_Index + 1);
      pixy.line.vectors[Element_Index].print();
    }
  }

  // Were intersections detected? //
  if (pixy.line.numIntersections)
  {
    // Intersections detected - print them! //

    printf ("Detected %d intersections(s)\n", pixy.line.numIntersections);

    for (Element_Index = 0; Element_Index < pixy.line.numIntersections; ++Element_Index)
    {
      printf ("  ");
      pixy.line.intersections[Element_Index].print();
    }
  }

  // Were barcodes detected? //
  if (pixy.line.numBarcodes)
  {
    // Barcodes detected - print them! //

    printf ("Detected %d barcodes(s)\n", pixy.line.numBarcodes);

    for (Element_Index = 0; Element_Index < pixy.line.numBarcodes; ++Element_Index)
    {
      printf ("  Barcode %d: ", Element_Index + 1);
      pixy.line.barcodes[Element_Index].print();
    }
  }
}

int main(int argc, char* argv[]) {
  
  // start NetworkTables
  
  auto ntinst = nt::NetworkTableInstance::GetDefault();
  auto table = ntinst.GetTable("DataTable");
  ntinst.StartClientTeam(4669); 

  int  Result;

  // Catch CTRL+C (SIGINT) signals //
  signal (SIGINT, handle_SIGINT);
  printf ("Connecting to Pixy2...");

  // Initialize Pixy2 Connection //
  {
    Result = pixycamF.init();

    if (Result != 0)
    {
      printf ("Error\n");
      printf ("pixy.init() returned %d\n", Result);
      return Result;
    }

    printf ("Success\n");
  }

  // Initialize Pixy2 Connection //
  {
    Result = pixycamR.init();

    if (Result != 0)
    {
      printf ("Error\n");
      printf ("pixy.init() returned %d\n", Result);
      return Result;
    }

    printf ("Success\n");
  }

  // Set Pixy2 to line feature identification program //
  pixycamF.changeProg("line");
  pixycamR.changeProg("line");

  
  while(1)
  {
    get_line_features(pixycamF);
    if (pixycamF.line.numVectors) {
      for (int index = 0; index<pixycamF.line.numVectors; index++)
      {
        int yDifference = pixycamF.line.vectors[index].m_y1-pixycamF.line.vectors[index].m_y0;
        int xDifference = pixycamF.line.vectors[index].m_x1-pixycamF.line.vectors[index].m_x0;
        double distance = hypot(yDifference,xDifference);
        double angle = asin(xDifference/distance)*180/PI;
                        table->PutNumber("LineX0F", pixycamF.line.vectors[index].m_x0);
                        table->PutNumber("LineY0F", pixycamF.line.vectors[index].m_y0);
                        table->PutNumber("LineX1F", pixycamF.line.vectors[index].m_x1);
                        table->PutNumber("LineY1F", pixycamF.line.vectors[index].m_y1);
                        table->PutNumber("LineAngleF", angle);
      }
    }
    get_line_features(pixycamR);
    if (pixycamR.line.numVectors) {
      for (int index = 0; index<pixycamR.line.numVectors; index++)
      {
        int yDifference = pixycamR.line.vectors[index].m_y1-pixycamR.line.vectors[index].m_y0;
        int xDifference = pixycamR.line.vectors[index].m_x1-pixycamR.line.vectors[index].m_x0;
        double distance = hypot(yDifference,xDifference);
        double angle = asin(xDifference/distance)*180/PI;
                        table->PutNumber("LineX0R", pixycamR.line.vectors[index].m_x0);
                        table->PutNumber("LineY0R", pixycamR.line.vectors[index].m_y0);
                        table->PutNumber("LineX1R", pixycamR.line.vectors[index].m_x1);
                        table->PutNumber("LineY1R", pixycamR.line.vectors[index].m_y1);
                        table->PutNumber("LineAngleR", angle);
      }
    }
    if (run_flag == false)
    {
      // Exit program loop //
      break;
    }
    
  }  
}
