#include<string>
#include <type_traits>

#define GR_H_S 16
#define GR_H_M 32
#define GR_H_L 64

#define GR_W_S 32
#define GR_W_M 64
#define GR_W_L 128

// RASTER_SIDE_LENGTH defined in extern_c.h
typedef char (*RasterRef)[RASTER_SIDE_LENGTH][RASTER_SIDE_LENGTH];

typedef char (*GraphRefS)[GR_H_S][GR_W_S];
typedef char (*GraphRefM)[GR_H_M][GR_W_M];
typedef char (*GraphRefL)[GR_H_L][GR_W_L];
typedef char (*GraphRefHDR)[GR_H_S][RASTER_SIDE_LENGTH];

// TODO:
//  - turn on/off printing char
template <class T>
void updateGraphPriv(RasterRef raster, T patch, int offsetY, int offsetX, int height, int width)
{
    for (int i = height - 1; i > -1; i--) // draws from top to bottom
    {
      int row = i + offsetY; // in raster
      for (int j = 0; j < width; j++)
      {
        int col = j + offsetX; // in raster
        char patchChar = (*patch)[i][j];
        // printf("\n patchChar[%i][%i]: %c", i, j, patchChar);
        if (row > RASTER_SIDE_LENGTH || col > RASTER_SIDE_LENGTH || patchChar == '\0')
        {
          // do nothing
        }
        else if ((*raster)[row][col] != patchChar)
        { // only engage in i/o if we have to
          (*raster)[row][col] = patchChar;
          if (patchChar == '#')
          {
            // Set bg-color to cyan
            printf("\e[48;5;12m");
          }
          else if (patchChar == 'o') // 'o' for over-graph
          {
            // Set bg-color to light blue
            printf("\e[48;5;25m");
          }
          else
          {
            // Set bg-color to dark blue
            printf("\e[48;5;18m");
          }
          printf("\033[%d;%dH", row, col); // move to char location
          printf("%c", ' '); // update char on screen
          
          // Reset text color to default
          printf("\033[0m");
        }
      }
    }
};

// TODO: pass in text color options
template <class T>
void updateTextPriv(RasterRef raster, T patch, int offsetY, int offsetX, int height, int width)
{
    for (int i = height - 1; i > -1; i--) // draws from top to bottom
    {
      int row = i + offsetY; // in raster
      for (int j = 0; j < width; j++)
      {
        int col = j + offsetX; // in raster
        char patchChar = (*patch)[i][j];
        // printf("\n patchChar[%i][%i]: %c", i, j, patchChar);
        if (row > RASTER_SIDE_LENGTH || col > RASTER_SIDE_LENGTH || patchChar == '\0')
        {
          // do nothing
        }
        else if ((*raster)[row][col] != patchChar)
        { // only engage in i/o if we have to
          (*raster)[row][col] = patchChar;
          printf("\033[%d;%dH", row, col); // move to char location
          printf("%c", patchChar); // update char on screen
          
          // Reset text color to default
          // printf("\033[0m");
        }
      }
    }
};

template<class T>
void placeStringPriv(string input, T patch, int offsetY, int offsetX, int height, int width)
{
  if (offsetY > height)
  {
    return;
  }

  int col = 0;
  for (char& c : input) 
  {
    // printf("\nchar %i: %c", col, c);
    int colOffset = col + offsetX;
    if (colOffset > width)
    {
      // do nothing
    }
    else
    {
      (*patch)[offsetY][colOffset] = (char) c;
      // printf("\n patch[offsetY][colOffset]: %c", c);
    }
    
    col += 1;
  }
}

template <typename T>
class GraphRef {
private:
    T patch; // ref to new patch to draw to raster
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;

    GraphRef(string name, T patch, int offsetY, int offsetX)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height{5}
      , width{5}
      {};
    
    void placeString(string input, int innerOffsetY, int innerOffsetX); // string, row, col
    void updateGraph(RasterRef raster) {};
    void updateText(RasterRef raster) {};
    
};

template <>
class GraphRef<GraphRefS> {
private:
    GraphRefS patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;

    GraphRef(string name, GraphRefS patch, int offsetY, int offsetX)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_S)
      , width(GR_W_S) 
      {};

    void placeString(string input, int innerOffsetY, int innerOffsetX) // string, row, col
    { // offset string within patch
      placeStringPriv<GraphRefS>(input, patch, innerOffsetY, innerOffsetX, height, width);
    };

    void updateGraph(RasterRef raster)
    {
      updateGraphPriv<GraphRefS>(raster, patch, offsetY, offsetX, height, width);
    };

    void updateText(RasterRef raster)
    {
      updateTextPriv<GraphRefS>(raster, patch, offsetY, offsetX, height, width);
    };

};

template <>
class GraphRef<GraphRefM> {
private:
    GraphRefM patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, GraphRefM patch, int offsetY, int offsetX)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_M)
      , width(GR_W_M) 
      {};

    void placeString(string input, int innerOffsetY, int innerOffsetX) // string, row, col
    { // offset string within patch
      placeStringPriv<GraphRefM>(input, patch, innerOffsetY, innerOffsetX, height, width);
    };

    void updateGraph(RasterRef raster)
    {
      updateGraphPriv<GraphRefM>(raster, patch, offsetY, offsetX, height, width);
    };

    void updateText(RasterRef raster)
    {
      updateTextPriv<GraphRefM>(raster, patch, offsetY, offsetX, height, width);
    };
};

template <>
class GraphRef<GraphRefL> {
private:
    GraphRefL patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, GraphRefL patch, int offsetY, int offsetX)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_L)
      , width(GR_W_L) 
      {};

    void placeString(string input, int innerOffsetY, int innerOffsetX) // string, row, col
    { // offset string within patch
      placeStringPriv<GraphRefL>(input, patch, innerOffsetY, innerOffsetX, height, width);
    };

    void updateGraph(RasterRef raster)
    {
      updateGraphPriv<GraphRefL>(raster, patch, offsetY, offsetX, height, width);
    };

    void updateText(RasterRef raster)
    {
      updateTextPriv<GraphRefL>(raster, patch, offsetY, offsetX, height, width);
    };
};

template <>
class GraphRef<RasterRef> {
private:
    RasterRef patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, RasterRef patch, int offsetY, int offsetX)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(RASTER_SIDE_LENGTH)
      , width(RASTER_SIDE_LENGTH) 
      {};

    void placeString(string input, int innerOffsetY, int innerOffsetX) // string, row, col
    { // offset string within patch
      placeStringPriv<RasterRef>(input, patch, innerOffsetY, innerOffsetX, height, width);
    };

    void updateGraph(RasterRef raster)
    {
      updateGraphPriv<RasterRef>(raster, patch, offsetY, offsetX, height, width);
    };

    void updateText(RasterRef raster)
    {
      updateTextPriv<RasterRef>(raster, patch, offsetY, offsetX, height, width);
    };
};

template <>
class GraphRef<GraphRefHDR> {
private:
    GraphRefHDR patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, GraphRefHDR patch, int offsetY, int offsetX)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_S)
      , width(RASTER_SIDE_LENGTH) 
      {};

    void placeString(string input, int innerOffsetY, int innerOffsetX) // string, row, col
    { // offset string within patch
      placeStringPriv<GraphRefHDR>(input, patch, innerOffsetY, innerOffsetX, height, width);
    };

    void updateGraph(RasterRef raster)
    {
      updateGraphPriv<GraphRefHDR>(raster, patch, offsetY, offsetX, height, width);
    };

    void updateText(RasterRef raster)
    {
      updateTextPriv<GraphRefHDR>(raster, patch, offsetY, offsetX, height, width);
    };
};
